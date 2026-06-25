#!/usr/bin/env python3
"""Export project_memories as Graphiti episode JSONL (import via add_memory MCP)."""
import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools", "mcp"))
from mlc_memory_server import db_connect  # noqa: E402

GROUP_ID = os.environ.get("GRAPHITI_GROUP_ID", "project:mlc")


def episode_from_row(row: dict) -> dict:
    evidence = json.loads(row.get("evidence") or "{}")
    entities = json.loads(row.get("entities") or "[]")
    body = {
        "memory_id": row["id"],
        "type": row["type"],
        "scope": row["scope"],
        "status": row["status"],
        "confidence": row["confidence"],
        "content": row["content"],
        "evidence": evidence,
        "entities": entities,
        "created_at": row["created_at"],
        "updated_at": row["updated_at"],
    }
    name = f"{row['type']}:{row['scope']}:{row['id']}"
    return {
        "name": name[:120],
        "episode_body": json.dumps(body, ensure_ascii=False),
        "group_id": row.get("group_id") or GROUP_ID,
        "source": "json",
        "source_description": "mlc project_memories export",
    }


def main():
    status = os.environ.get("EXPORT_STATUS", "active")
    conn = db_connect()
    cur = conn.execute(
        "SELECT * FROM project_memories WHERE status = ? ORDER BY id",
        (status,),
    )
    cols = [d[0] for d in cur.description]
    for record in cur.fetchall():
        row = dict(zip(cols, record))
        print(json.dumps(episode_from_row(row), ensure_ascii=False))
    conn.close()


if __name__ == "__main__":
    main()
