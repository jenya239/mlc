# Track: MCP memory layers

Parent: [../PLAN.md](../PLAN.md); [.agent/memory-policy.md](../../.agent/memory-policy.md)

## Status: **closed** (STEP=4 done)

| Step | Item | Status |
|------|------|--------|
| 1 | codebase-memory index + policy + mlc-memory typed table | done |
| 2 | Graphiti MCP (FalkorDB) — optional upgrade from SQLite | done |
| 3 | Sync obsolete memories after major refactors | done |
| 4 | Metrics: files read / tool calls per task | done |

## Architecture

```text
Agent
 ├── codebase-memory-mcp   → structural graph (symbols, callers, impact)
 ├── mlc-memory MCP        → project_memories (decisions, bugs, conventions)
 ├── graphiti-memory (opt) → temporal KG when FalkorDB local
 ├── docs/archive/TURNLOG.jsonl (git) → turn audit, archived (cr guard discontinued)
 └── SESSION.md (git)      → human turn summary
```

SQLite `mlc-memory` remains default; Graphiti is optional via `scripts/graphiti_setup.sh`.

## Setup

1. Copy [mcp.example.json](mcp.example.json) entries into `~/.cursor/mcp.json`
2. Reload MCP in Cursor
3. `bash scripts/memory_reindex.sh`
4. `python3 scripts/memory_bootstrap.py`

## Agent cycle

**Before task:** `memory_search` → `search_graph` / `query_graph` → minimal file reads.

**After verified change:** `memory_write` type=fix|decision; `memory_sync_obsolete --apply` if paths moved; reindex if structure changed.

**Never:** secrets, per-turn rows in mlc-memory, unverified active memories.

## Gate

```bash
bash scripts/memory_gate.sh
```

Optional Graphiti health (Docker running):

```bash
GRAPHITI_GATE=1 bash scripts/memory_gate.sh
```

## Task metrics (STEP 4)

```bash
python3 scripts/memory_task_metrics.py --report --limit 10
python3 scripts/memory_task_metrics.py --record --limit 5
```

MCP: `memory_task_metrics` with optional `record: true`.

Counts `Read` tool unique paths and total `tool_use` blocks from agent transcripts; correlates with `docs/archive/TURNLOG.jsonl` `turn_done` rows (historical, cr guard discontinued).

## Obsolete sync (STEP 3)

After refactors that move/delete cited files:

```bash
python3 scripts/memory_sync_obsolete.py          # dry-run
python3 scripts/memory_sync_obsolete.py --apply
```

MCP: `memory_sync_obsolete` with `{"apply": true}`.

## Graphiti (STEP 2, optional)

```bash
export OPENAI_API_KEY=...
bash scripts/graphiti_setup.sh
```

`group_id: project:mlc`. Migrate: `python3 scripts/memory_export_graphiti.py`.
