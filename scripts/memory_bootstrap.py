#!/usr/bin/env python3
"""One-time bootstrap for project_memories. Idempotent by content hash."""
import json
import os
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools", "mcp"))
from mlc_memory_server import db_connect, handle_memory_write  # noqa: E402

BOOTSTRAP = [
    {
        "type": "architecture",
        "scope": "repo",
        "confidence": "high",
        "content": "mlc: self-hosting compiler (Ruby bootstrap + mlcc). Layers: lib/mlc Ruby, compiler/*.mlc, runtime/. Modular compile via build.sh/build_bin.sh.",
        "evidence": {"files": ["compiler/main.mlc", "compiler/build.sh", "docs/ARCHITECTURE.md"]},
        "entities": ["mlcc", "build_bin.sh"],
    },
    {
        "type": "command",
        "scope": "repo",
        "confidence": "high",
        "content": "Tests: compiler/tests/build_tests.sh && compiler/out/run_tests. Self-host after compiler/** changes.",
        "evidence": {"files": ["compiler/tests/build_tests.sh"], "commands": ["compiler/tests/build_tests.sh && compiler/out/run_tests"]},
    },
    {
        "type": "command",
        "scope": "repo",
        "confidence": "high",
        "content": "Build speed: use build_bin.sh (ccache/parallel), not bare g++ link.",
        "evidence": {"files": ["compiler/build_bin.sh", "docs/agent/TRACK_BUILD_SPEED.md"]},
    },
    {
        "type": "convention",
        "scope": "repo",
        "confidence": "high",
        "content": "Clean compiler architecture = IR layers + passes + verifiers + data-oriented core; not enterprise OOP layers.",
        "evidence": {"files": ["docs/ARCHITECTURE.md", "docs/agent/TRACK_CLEAN_ARCHITECTURE.md"]},
    },
    {
        "type": "convention",
        "scope": "repo",
        "confidence": "high",
        "content": "Turn audit in git TURNLOG.jsonl (guard). mlc-memory only cross-cutting decisions/limitations, not per-turn.",
        "evidence": {"files": ["docs/agent/TRACK_TURN_AUDIT.md", "docs/agent/CONTINUITY.md"]},
    },
    {
        "type": "convention",
        "scope": "repo",
        "confidence": "high",
        "content": "Before code changes: codebase-memory symbol/impact query; mlc-memory active decisions; read minimal files.",
        "evidence": {"files": [".agent/memory-policy.md"]},
    },
]


def exists(conn, content: str) -> bool:
    cur = conn.execute("SELECT 1 FROM project_memories WHERE content = ? LIMIT 1", (content,))
    return cur.fetchone() is not None


def main():
    conn = db_connect()
    added = 0
    for item in BOOTSTRAP:
        if exists(conn, item["content"]):
            continue
        handle_memory_write(item)
        added += 1
    conn.close()
    print(json.dumps({"bootstrapped": added, "total_defs": len(BOOTSTRAP)}))


if __name__ == "__main__":
    main()
