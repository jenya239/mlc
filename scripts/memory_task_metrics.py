#!/usr/bin/env python3
"""Count tool calls and file reads per agent turn from transcripts + TURNLOG."""
import argparse
import json
import os
import sys
from collections import defaultdict
from datetime import datetime, timezone
from glob import glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools", "mcp"))
from mlc_memory_server import db_connect, now_iso  # noqa: E402

TRANSCRIPTS_GLOB = os.path.expanduser(
    os.environ.get(
        "AGENT_TRANSCRIPTS_GLOB",
        "~/.cursor/projects/home-jenya-workspaces-current-mlc/agent-transcripts/*/*.jsonl",
    )
)
TURNLOG_PATH = os.environ.get("TURNLOG_PATH", os.path.join(ROOT, "docs", "agent", "TURNLOG.jsonl"))

METRICS_SCHEMA = """
CREATE TABLE IF NOT EXISTS task_metrics (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  prompt_key TEXT NOT NULL,
  role TEXT,
  step TEXT,
  track TEXT,
  tool_calls INTEGER NOT NULL,
  files_read INTEGER NOT NULL,
  read_tool_calls INTEGER NOT NULL,
  tools_json TEXT NOT NULL DEFAULT '{}',
  transcript_path TEXT,
  recorded_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_task_metrics_prompt ON task_metrics(prompt_key);
CREATE INDEX IF NOT EXISTS idx_task_metrics_recorded ON task_metrics(recorded_at);
"""


def ensure_metrics_schema(conn):
    conn.executescript(METRICS_SCHEMA)


def extract_user_text(event: dict) -> str:
    message = event.get("message") or {}
    content = message.get("content")
    if isinstance(content, str):
        return content
    if isinstance(content, list):
        parts = []
        for block in content:
            if isinstance(block, dict) and block.get("type") == "text":
                parts.append(block.get("text") or "")
        return "\n".join(parts)
    return ""


def count_assistant_tools(content_blocks: list) -> tuple[int, int, set[str], dict[str, int]]:
    tool_calls = 0
    read_tool_calls = 0
    files_read: set[str] = set()
    tools: dict[str, int] = defaultdict(int)
    if not isinstance(content_blocks, list):
        return tool_calls, read_tool_calls, files_read, tools
    for block in content_blocks:
        if block.get("type") != "tool_use":
            continue
        tool_name = block.get("name") or "unknown"
        tools[tool_name] += 1
        tool_calls += 1
        if tool_name == "Read":
            read_tool_calls += 1
            file_path = (block.get("input") or {}).get("path")
            if file_path:
                files_read.add(file_path)
    return tool_calls, read_tool_calls, files_read, tools


def count_transcript(transcript_path: str) -> dict:
    tool_calls = 0
    read_tool_calls = 0
    files_read: set[str] = set()
    tools: dict[str, int] = defaultdict(int)
    with open(transcript_path, encoding="utf-8") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue
            try:
                event = json.loads(line)
            except json.JSONDecodeError:
                continue
            if event.get("role") != "assistant":
                continue
            message = event.get("message") or {}
            block_tools, block_reads, block_files, block_names = count_assistant_tools(
                message.get("content")
            )
            tool_calls += block_tools
            read_tool_calls += block_reads
            files_read |= block_files
            for name, count in block_names.items():
                tools[name] += count
    return {
        "transcript_path": transcript_path,
        "tool_calls": tool_calls,
        "read_tool_calls": read_tool_calls,
        "files_read": len(files_read),
        "tools": dict(tools),
    }


def count_turn_slice(transcript_path: str, turn: dict) -> dict:
    step = str(turn.get("step") or "")
    role = turn.get("role") or ""
    track_token = (turn.get("track") or "").replace(".md", "")
    tool_calls = 0
    read_tool_calls = 0
    files_read: set[str] = set()
    tools: dict[str, int] = defaultdict(int)
    active = False
    with open(transcript_path, encoding="utf-8") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue
            try:
                event = json.loads(line)
            except json.JSONDecodeError:
                continue
            if event.get("role") == "user":
                user_text = extract_user_text(event)
                if active and "AGENT_TOKEN=" in user_text and f"STEP={step}" not in user_text:
                    break
                if (
                    f"STEP={step}" in user_text
                    and f"ROLE={role}" in user_text
                    and (not track_token or track_token in user_text)
                ):
                    active = True
                continue
            if not active or event.get("role") != "assistant":
                continue
            message = event.get("message") or {}
            block_tools, block_reads, block_files, block_names = count_assistant_tools(
                message.get("content")
            )
            tool_calls += block_tools
            read_tool_calls += block_reads
            files_read |= block_files
            for name, count in block_names.items():
                tools[name] += count
    return {
        "transcript_path": transcript_path,
        "tool_calls": tool_calls,
        "read_tool_calls": read_tool_calls,
        "files_read": len(files_read),
        "tools": dict(tools),
    }


def load_turnlog_events(turnlog_path: str) -> list[dict]:
    if not os.path.isfile(turnlog_path):
        return []
    events = []
    with open(turnlog_path, encoding="utf-8") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue
            try:
                events.append(json.loads(line))
            except json.JSONDecodeError:
                continue
    return events


def completed_turns(events: list[dict], limit: int) -> list[dict]:
    turns = [
        event
        for event in events
        if event.get("event") == "turn_done" and event.get("prompt_key")
    ]
    return turns[-limit:]


def newest_transcripts(limit: int) -> list[str]:
    paths = glob(TRANSCRIPTS_GLOB)
    paths.sort(key=lambda path: os.path.getmtime(path), reverse=True)
    return paths[:limit]


def build_turn_report(turn: dict, transcript_metrics: dict | None) -> dict:
    metrics = transcript_metrics or {
        "tool_calls": 0,
        "read_tool_calls": 0,
        "files_read": 0,
        "tools": {},
        "transcript_path": None,
    }
    return {
        "prompt_key": turn.get("prompt_key"),
        "role": turn.get("role"),
        "step": turn.get("step"),
        "track": turn.get("track"),
        "turn_timestamp": turn.get("ts"),
        "tool_calls": metrics["tool_calls"],
        "read_tool_calls": metrics["read_tool_calls"],
        "files_read": metrics["files_read"],
        "tools": metrics["tools"],
        "transcript_path": metrics.get("transcript_path"),
    }


def report_task_metrics(limit: int = 10) -> dict:
    events = load_turnlog_events(TURNLOG_PATH)
    turns = completed_turns(events, limit)
    transcripts = newest_transcripts(3)
    primary_transcript = transcripts[0] if transcripts else None
    rows = []
    for turn in turns:
        metrics = None
        if primary_transcript:
            metrics = count_turn_slice(primary_transcript, turn)
        rows.append(build_turn_report(turn, metrics))
    totals = {
        "tool_calls": sum(row["tool_calls"] for row in rows),
        "files_read": sum(row["files_read"] for row in rows),
        "turns": len(rows),
    }
    return {
        "turns": rows,
        "totals": totals,
        "transcript_files_scanned": len(transcripts),
        "primary_transcript": primary_transcript,
    }


def record_task_metrics(limit: int = 5) -> dict:
    report = report_task_metrics(limit=limit)
    conn = db_connect()
    ensure_metrics_schema(conn)
    inserted = 0
    timestamp = now_iso()
    for row in report["turns"]:
        existing = conn.execute(
            "SELECT 1 FROM task_metrics WHERE prompt_key = ? AND recorded_at >= ? LIMIT 1",
            (row["prompt_key"], row.get("turn_timestamp") or ""),
        ).fetchone()
        if existing:
            continue
        conn.execute(
            """INSERT INTO task_metrics
               (prompt_key, role, step, track, tool_calls, files_read, read_tool_calls,
                tools_json, transcript_path, recorded_at)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
            (
                row["prompt_key"],
                row.get("role"),
                str(row.get("step")) if row.get("step") is not None else None,
                row.get("track"),
                row["tool_calls"],
                row["files_read"],
                row["read_tool_calls"],
                json.dumps(row.get("tools") or {}, ensure_ascii=False),
                row.get("transcript_path"),
                timestamp,
            ),
        )
        inserted += 1
    conn.commit()
    conn.close()
    return {"recorded": inserted, "report": report}


def main():
    parser = argparse.ArgumentParser(description="Task metrics: files read and tool calls")
    parser.add_argument("--report", action="store_true", help="Print metrics report JSON")
    parser.add_argument("--record", action="store_true", help="Persist report rows to task_metrics")
    parser.add_argument("--limit", type=int, default=5, help="Recent turns / transcripts")
    arguments = parser.parse_args()
    if arguments.record:
        result = record_task_metrics(limit=arguments.limit)
    else:
        result = report_task_metrics(limit=arguments.limit)
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
