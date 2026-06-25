#!/usr/bin/env python3
"""Lint latest SESSION turn block: requires concrete done + verify/result."""
import argparse
import json
import re
import sys

ROOT = __import__("os").path.dirname(__import__("os").path.dirname(__import__("os").path.abspath(__file__)))
SESSION_PATH = __import__("os").path.join(ROOT, "docs", "agent", "SESSION.md")

PLACEHOLDER_PATTERNS = (
    r"^<",
    r"^TBD$",
    r"^pending$",
    r"^none yet$",
    r"^\.\.\.$",
)


def parse_turn_table(block: str) -> dict[str, str]:
    fields: dict[str, str] = {}
    for line in block.splitlines():
        match = re.match(r"^\| (\w+)\s+\| (.+) \|$", line.strip())
        if match:
            fields[match.group(1)] = match.group(2).strip()
    return fields


def latest_turn_block(session_text: str) -> tuple[str, dict[str, str]] | tuple[None, None]:
    match = re.search(
        r"(### Turn[^\n]+\n\n\| field\s+\| value \|[\s\S]*?\| next\s+\|[^\n]+\n)",
        session_text,
    )
    if not match:
        return None, None
    block = match.group(1)
    return block, parse_turn_table(block)


def is_placeholder(value: str) -> bool:
    lowered = value.strip().lower()
    if not lowered:
        return True
    for pattern in PLACEHOLDER_PATTERNS:
        if re.search(pattern, lowered, re.IGNORECASE):
            return True
    return False


def lint_session_turn(session_path: str) -> dict:
    with open(session_path, encoding="utf-8") as handle:
        session_text = handle.read()
    block, fields = latest_turn_block(session_text)
    errors: list[str] = []
    if not block:
        errors.append("no ### Turn block found near top of SESSION.md")
        return {"ok": False, "errors": errors, "fields": {}}
    done_value = fields.get("done", "")
    if is_placeholder(done_value) or len(done_value) < 8:
        errors.append("latest turn missing concrete | done | (min 8 chars, no placeholders)")
    if not fields.get("verify") and not fields.get("result"):
        errors.append("latest turn needs | verify | or | result |")
    if not fields.get("role"):
        errors.append("latest turn missing | role |")
    if not fields.get("step"):
        errors.append("latest turn missing | step |")
    return {
        "ok": not errors,
        "errors": errors,
        "fields": fields,
        "turn_heading": block.splitlines()[0] if block else None,
    }


def main():
    parser = argparse.ArgumentParser(description="Lint latest SESSION turn")
    parser.add_argument("--session", default=SESSION_PATH)
    arguments = parser.parse_args()
    report = lint_session_turn(arguments.session)
    print(json.dumps(report, ensure_ascii=False, indent=2))
    return 0 if report["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
