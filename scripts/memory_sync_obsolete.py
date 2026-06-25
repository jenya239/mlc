#!/usr/bin/env python3
"""Mark project_memories obsolete when evidence files are missing or invalidates chain."""
import argparse
import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools", "mcp"))
from mlc_memory_server import db_connect, now_iso  # noqa: E402


def missing_evidence_files(repository_root: str, evidence: dict) -> list[str]:
    paths = evidence.get("files") or []
    if not paths:
        return []
    missing = []
    for relative_path in paths:
        full_path = os.path.join(repository_root, relative_path)
        if not os.path.isfile(full_path):
            missing.append(relative_path)
    return missing


def collect_obsolete_candidates(conn, repository_root: str) -> list[dict]:
    candidates: list[dict] = []
    seen_ids: set[int] = set()

    cursor = conn.execute(
        "SELECT id, evidence, content FROM project_memories WHERE status = 'active'"
    )
    for memory_id, evidence_text, content in cursor.fetchall():
        evidence = json.loads(evidence_text or "{}")
        missing = missing_evidence_files(repository_root, evidence)
        if missing:
            candidates.append({
                "id": memory_id,
                "reason": "missing_evidence_files",
                "missing_files": missing,
                "content_preview": content[:80],
            })
            seen_ids.add(memory_id)

    cursor = conn.execute(
        """SELECT invalidates, id, content FROM project_memories
           WHERE status = 'active' AND invalidates IS NOT NULL"""
    )
    for target_id, source_id, content in cursor.fetchall():
        if target_id in seen_ids:
            continue
        target = conn.execute(
            "SELECT status FROM project_memories WHERE id = ?",
            (target_id,),
        ).fetchone()
        if target and target[0] == "active":
            candidates.append({
                "id": target_id,
                "reason": "invalidated_by",
                "invalidated_by": source_id,
                "content_preview": content[:80],
            })
            seen_ids.add(target_id)

    return candidates


def apply_obsolete(conn, candidates: list[dict]) -> int:
    timestamp = now_iso()
    updated = 0
    for candidate in candidates:
        conn.execute(
            "UPDATE project_memories SET status = 'obsolete', updated_at = ? WHERE id = ? AND status = 'active'",
            (timestamp, candidate["id"]),
        )
        if conn.total_changes:
            updated += 1
    conn.commit()
    return updated


def sync_obsolete_memories(repository_root: str, apply: bool = False) -> dict:
    conn = db_connect()
    try:
        candidates = collect_obsolete_candidates(conn, repository_root)
        updated = apply_obsolete(conn, candidates) if apply and candidates else 0
        return {
            "repository_root": repository_root,
            "apply": apply,
            "candidates": candidates,
            "updated": updated,
        }
    finally:
        conn.close()


def main():
    parser = argparse.ArgumentParser(description="Sync obsolete project_memories after refactors")
    parser.add_argument(
        "--root",
        default=ROOT,
        help="Repository root for evidence file checks",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Set status=obsolete for matched rows (default: dry-run report)",
    )
    arguments = parser.parse_args()
    report = sync_obsolete_memories(arguments.root, apply=arguments.apply)
    print(json.dumps(report, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
