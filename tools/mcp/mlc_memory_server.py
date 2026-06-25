#!/usr/bin/env python3
"""mlc-memory MCP: legacy tables + typed project_memories (Graphiti-compatible subset)."""
import json
import os
import sqlite3
import sys
from datetime import datetime, timezone

DB_PATH = os.environ.get(
    "SQLITE_DB_PATH",
    os.path.join(os.path.dirname(__file__), "..", "..", ".cursor", "memory.db"),
)

SCHEMA = """
CREATE TABLE IF NOT EXISTS project_memories (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  type TEXT NOT NULL,
  scope TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'active',
  confidence TEXT NOT NULL DEFAULT 'medium',
  group_id TEXT NOT NULL DEFAULT 'project:mlc',
  content TEXT NOT NULL,
  evidence TEXT NOT NULL DEFAULT '{}',
  entities TEXT NOT NULL DEFAULT '[]',
  invalidates INTEGER,
  created_at TEXT NOT NULL,
  updated_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_pm_status ON project_memories(status);
CREATE INDEX IF NOT EXISTS idx_pm_type ON project_memories(type);
CREATE INDEX IF NOT EXISTS idx_pm_scope ON project_memories(scope);
"""

TOOLS = [
    {
        "name": "read_query",
        "description": "Execute SELECT against SQLite (legacy tables)",
        "inputSchema": {
            "type": "object",
            "properties": {"query": {"type": "string"}},
            "required": ["query"],
        },
    },
    {
        "name": "write_query",
        "description": "Execute INSERT/UPDATE/DELETE (legacy tables)",
        "inputSchema": {
            "type": "object",
            "properties": {"query": {"type": "string"}},
            "required": ["query"],
        },
    },
    {
        "name": "list_tables",
        "description": "List SQLite tables",
        "inputSchema": {"type": "object", "properties": {}},
    },
    {
        "name": "describe_table",
        "description": "Describe table columns",
        "inputSchema": {
            "type": "object",
            "properties": {"table_name": {"type": "string"}},
            "required": ["table_name"],
        },
    },
    {
        "name": "memory_search",
        "description": "Search typed project memories (decisions, bugs, conventions, etc.)",
        "inputSchema": {
            "type": "object",
            "properties": {
                "query": {"type": "string"},
                "types": {"type": "array", "items": {"type": "string"}},
                "status": {"type": "string", "default": "active"},
                "scope": {"type": "string"},
                "limit": {"type": "integer", "default": 10},
            },
        },
    },
    {
        "name": "memory_write",
        "description": "Write typed project memory with evidence",
        "inputSchema": {
            "type": "object",
            "properties": {
                "type": {"type": "string"},
                "scope": {"type": "string"},
                "content": {"type": "string"},
                "status": {"type": "string", "default": "active"},
                "confidence": {"type": "string", "default": "medium"},
                "group_id": {"type": "string", "default": "project:mlc"},
                "evidence": {"type": "object"},
                "entities": {"type": "array", "items": {"type": "string"}},
                "invalidates": {"type": "integer"},
            },
            "required": ["type", "scope", "content"],
        },
    },
    {
        "name": "memory_sync_obsolete",
        "description": "Mark active memories obsolete when evidence files are missing or invalidates chain",
        "inputSchema": {
            "type": "object",
            "properties": {
                "apply": {"type": "boolean", "default": False},
            },
        },
    },
    {
        "name": "memory_task_metrics",
        "description": "Report or record per-turn tool_calls and files_read from agent transcripts",
        "inputSchema": {
            "type": "object",
            "properties": {
                "record": {"type": "boolean", "default": False},
                "limit": {"type": "integer", "default": 5},
            },
        },
    },
]


def now_iso():
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def db_connect():
    conn = sqlite3.connect(DB_PATH)
    conn.executescript(SCHEMA)
    return conn


def handle_read_query(query):
    conn = db_connect()
    try:
        cur = conn.execute(query)
        cols = [d[0] for d in cur.description] if cur.description else []
        return json.dumps([dict(zip(cols, r)) for r in cur.fetchall()], ensure_ascii=False, indent=2)
    finally:
        conn.close()


def handle_write_query(query):
    conn = db_connect()
    try:
        cur = conn.execute(query)
        conn.commit()
        return f"OK: {cur.rowcount} rows affected"
    finally:
        conn.close()


def handle_list_tables():
    conn = db_connect()
    try:
        cur = conn.execute("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")
        return "\n".join(r[0] for r in cur.fetchall())
    finally:
        conn.close()


def handle_describe_table(table_name):
    conn = db_connect()
    try:
        cur = conn.execute(f"PRAGMA table_info({table_name})")
        return "\n".join(f"{c[1]} {c[2]}" + (" NOT NULL" if c[3] else "") for c in cur.fetchall())
    finally:
        conn.close()


def handle_memory_search(args):
    q = (args.get("query") or "").strip()
    status = args.get("status") or "active"
    scope = args.get("scope")
    types = args.get("types") or []
    limit = min(int(args.get("limit") or 10), 50)
    sql = "SELECT * FROM project_memories WHERE status = ?"
    params: list = [status]
    if types:
        sql += f" AND type IN ({','.join('?' * len(types))})"
        params.extend(types)
    if scope:
        sql += " AND scope LIKE ?"
        params.append(f"%{scope}%")
    if q:
        sql += " AND (content LIKE ? OR scope LIKE ? OR evidence LIKE ? OR entities LIKE ?)"
        like = f"%{q}%"
        params.extend([like, like, like, like])
    sql += " ORDER BY updated_at DESC LIMIT ?"
    params.append(limit)
    conn = db_connect()
    try:
        cur = conn.execute(sql, params)
        cols = [d[0] for d in cur.description]
        return json.dumps([dict(zip(cols, r)) for r in cur.fetchall()], ensure_ascii=False, indent=2)
    finally:
        conn.close()


def handle_memory_sync_obsolete(args):
    scripts_directory = os.path.join(os.path.dirname(__file__), "..", "..", "scripts")
    if scripts_directory not in sys.path:
        sys.path.insert(0, scripts_directory)
    import memory_sync_obsolete  # noqa: E402

    repository_root = os.path.join(os.path.dirname(__file__), "..", "..")
    report = memory_sync_obsolete.sync_obsolete_memories(
        repository_root,
        apply=bool(args.get("apply")),
    )
    return json.dumps(report, ensure_ascii=False, indent=2)


def handle_memory_task_metrics(args):
    scripts_directory = os.path.join(os.path.dirname(__file__), "..", "..", "scripts")
    if scripts_directory not in sys.path:
        sys.path.insert(0, scripts_directory)
    import memory_task_metrics  # noqa: E402

    limit = int(args.get("limit") or 5)
    if args.get("record"):
        result = memory_task_metrics.record_task_metrics(limit=limit)
    else:
        result = memory_task_metrics.report_task_metrics(limit=limit)
    return json.dumps(result, ensure_ascii=False, indent=2)


def handle_memory_write(args):
    ts = now_iso()
    evidence = json.dumps(args.get("evidence") or {}, ensure_ascii=False)
    entities = json.dumps(args.get("entities") or [], ensure_ascii=False)
    conn = db_connect()
    try:
        cur = conn.execute(
            """INSERT INTO project_memories
               (type, scope, status, confidence, group_id, content, evidence, entities, invalidates, created_at, updated_at)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
            (
                args["type"],
                args["scope"],
                args.get("status") or "active",
                args.get("confidence") or "medium",
                args.get("group_id") or "project:mlc",
                args["content"],
                evidence,
                entities,
                args.get("invalidates"),
                ts,
                ts,
            ),
        )
        conn.commit()
        return json.dumps({"id": cur.lastrowid, "created_at": ts}, ensure_ascii=False)
    finally:
        conn.close()


def call_tool(name, arguments):
    if name == "read_query":
        return handle_read_query(arguments["query"])
    if name == "write_query":
        return handle_write_query(arguments["query"])
    if name == "list_tables":
        return handle_list_tables()
    if name == "describe_table":
        return handle_describe_table(arguments["table_name"])
    if name == "memory_search":
        return handle_memory_search(arguments)
    if name == "memory_write":
        return handle_memory_write(arguments)
    if name == "memory_sync_obsolete":
        return handle_memory_sync_obsolete(arguments)
    if name == "memory_task_metrics":
        return handle_memory_task_metrics(arguments)
    raise ValueError(f"Unknown tool: {name}")


def send(message):
    sys.stdout.write(json.dumps(message, ensure_ascii=False) + "\n")
    sys.stdout.flush()


def handle_request(request):
    rid = request.get("id")
    method = request.get("method")
    if method == "initialize":
        send({
            "jsonrpc": "2.0",
            "id": rid,
            "result": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "serverInfo": {"name": "mlc-memory", "version": "2.0.0"},
            },
        })
    elif method == "notifications/initialized":
        pass
    elif method == "tools/list":
        send({"jsonrpc": "2.0", "id": rid, "result": {"tools": TOOLS}})
    elif method == "tools/call":
        params = request.get("params", {})
        try:
            text = call_tool(params.get("name"), params.get("arguments") or {})
            send({"jsonrpc": "2.0", "id": rid, "result": {"content": [{"type": "text", "text": text}]}})
        except Exception as e:
            send({
                "jsonrpc": "2.0",
                "id": rid,
                "result": {"content": [{"type": "text", "text": f"Error: {e}"}], "isError": True},
            })
    elif rid is not None:
        send({"jsonrpc": "2.0", "id": rid, "error": {"code": -32601, "message": f"Method not found: {method}"}})


def main():
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            handle_request(json.loads(line))
        except json.JSONDecodeError:
            pass


if __name__ == "__main__":
    main()
