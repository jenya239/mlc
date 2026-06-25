# Memory policy (mlc)

## Layers

| Layer | Tool | Role |
|-------|------|------|
| Structural code | `codebase-memory` MCP | symbols, callers, impact, tests, routes |
| Temporal decisions | `mlc-memory` MCP (`project_memories`) | decisions, conventions, bugs, fixes, failed_attempts |
| Turn audit | `docs/agent/TURNLOG.jsonl` (git) | who/when/why per turn � guard only |
| Human summary | `docs/agent/SESSION.md` (git) | done/verify/issues per turn |

Do not use vector RAG instead of structural graph for code navigation.

## Hard rules

- No secrets, tokens, `.env`, cookies, raw stack traces.
- No unverified hypotheses as `status=active`.
- No full source files in long-term memory.
- No per-turn log in mlc-memory � use TURNLOG.
- Memory must not override current git/code.

## Memory types (`project_memories.type`)

`decision`, `convention`, `bug`, `fix`, `failed_attempt`, `command`, `architecture`, `dependency`, `test_result`, `risk`, `todo`, `obsolete`

## Required fields

`type`, `scope`, `status` (`active`|`obsolete`|`unverified`), `confidence` (`low`|`medium`|`high`), `content`, `evidence` (JSON: files, commands, commits), `group_id` (`project:mlc`)

## Write when

- architectural decision confirmed
- repeatable bug or verified fix
- failed path confirmed by test/command
- user rule or repo convention

## Do not write

- intermediate thoughts, obvious one-file facts, generated output, temporary noise

## Task cycle

**Before:** Graphiti/mlc-memory search ? codebase-memory symbols/impact ? read minimal files ? plan.

**During:** check callers/callees; targeted tests; record only verified failures.

**After:** short `fix`/`decision` memory; reindex codebase-memory if structure changed; mark obsolete contradictions.

## mlc commands (evidence)

```bash
compiler/tests/build_tests.sh && compiler/out/run_tests
compiler/build.sh && compiler/build_bin.sh
compiler/tests/run_architecture_lint.sh
```

## Reindex

```bash
scripts/memory_reindex.sh
```

## Obsolete sync (after refactors)

```bash
python3 scripts/memory_sync_obsolete.py          # dry-run
python3 scripts/memory_sync_obsolete.py --apply  # mark obsolete
```

MCP: `memory_sync_obsolete` with `apply: true`.

## Task metrics

```bash
python3 scripts/memory_task_metrics.py --report --limit 10
python3 scripts/memory_task_metrics.py --record --limit 5
```

MCP: `memory_task_metrics`. Per-turn `tool_calls` and unique `Read` paths from agent transcripts + TURNLOG.
