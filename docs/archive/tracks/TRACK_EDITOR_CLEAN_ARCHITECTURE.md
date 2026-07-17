# Track: Editor development discipline — TDD, Clean Architecture, UX backlog

Parent: [../../PLAN.md](../../PLAN.md) §45.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=4):** Checklist:
1. `GUI_UX_TESTING.md` §"Standing discipline" present (`caf412f4`); Critic
   fixed stale "named phase functions" wording → orchestrator calls existing
   `ux/*` / §46 `#1 EDITOR_DEMO_ORCHESTRATOR`.
2. `TRACK_EDITOR_UX_BACKLOG` opened (`0c03bfe2`); order starts
   `#1 EDITOR_DEMO_ORCHESTRATOR` then `#1b`/`#1c`/`#1d`/`#2`…
3. STEP=1/2 reframed (not silently dropped) — table marks superseded /
   folded into §46 #1.
No `lib/mlc/` / `compiler/` → REG/self-host N/A. **reopen: none**.

## Next step

**closed** — Critic OK. Queue → Planner (open §46 #1 Decision).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `caf412f4` | Standing discipline + track open |
| 3 | `0c03bfe2` | Opus review → §46 backlog |
| 1–2 | reframed | execution = §46 #1, not this track |
| 4 | this Critic | close + archive |

## Delivered (this track)

- Standing TDD-scenario-first rule for all `misc/editor/**` tracks.
- Framing of `demo_live` god-file debt as §46 `#1 EDITOR_DEMO_ORCHESTRATOR`.
- Explicit UX backlog (§46).

## Non-goals (unchanged)

No editor feature code here; no `compiler/` changes.
