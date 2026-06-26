# Development (after CONTINUITY block in queued prompt)

Re-read [CONTINUITY.md](CONTINUITY.md) every turn — rules apply without restart.

## Principles

- **One sub-step per prompt** — one concern; bisect-friendly.
- **One layer per sub-step** — `compiler/` XOR `lib/mlc/`, not both (except tiny import fix).
- **Verify before next queue:** Tier A green every turn; Tier B before commit / TRACK close (see ladder).
- **Plans:** [PLAN.md](../PLAN.md) north star; [TRACK_*.md](.) active work; **Planner** extends TRACK, **Driver** executes.

## Roles (see CONTINUITY)

- **Driver** — code + verify (most turns).
- **Planner** — TRACK rows from PLAN; no `compiler/`.
- **Backlog** — TRACK vs git hygiene; no `compiler/`.

## Verification ladder (compiler changes)

**Tier A — every Driver turn (target &lt;5 min warm):**
```bash
bash scripts/dev_gate_fast.sh
```
`run_tests` (reuse `compiler/out/tests/run_tests` if tests unchanged) + `mlcc --check-only main.mlc` if `out/mlcc` exists + arch lint. No fuzz / LSP / differential.

**Tier B — before commit, TRACK close, or after `compiler/**` / fuzz / LSP edits:**
1. `compiler/tests/build_tests.sh` → full gate (~30 min cold). Abort if &gt; 10 min silent.
2. If **`lib/mlc/**` touched:** `bundle exec rake test_mlc`.
3. If **`compiler/**` touched:** `compiler/build.sh` → self-host diff:
   ```bash
   compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
   compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
   .tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
   diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
   ```
   Use `build_bin.sh`, not bare `g++` — parallel + ccache.
4. Step **14** in TRACK: mandatory self-host before closing visitor batch.

**Tier C — CI / release:** `.github/workflows/build-mlcc-once.yml` or manual full Tier B on clean tree.

## Current priority

**TRACK_BOOTSTRAP_LINK** STEP=1 — fresh bootstrap emit g++ link (`MLCC_BOOTSTRAP=1` regressed). Stability first.

## Step sizing (good vs bad)

| Good | Bad |
|------|-----|
| Step 9: one arm + tests | «Migrate all expr_eval» |
| Planner adds steps 10–12 | Driver invents scope without TRACK |
| Backlog flags uncommitted > 15 | Silent TRACK drift |

## Code rules

- No abbreviations (`.cursor/rules/no-abbreviations.mdc`).
- Minimal diff; match surrounding style.

## MCP (`agent-loop`)

Token-only — [CONTINUITY.md](CONTINUITY.md).

После изменений в `cr`: `npm run build` в `~/workspaces/current/cr`, Reload MCP в Cursor.

| Check | Command / tool |
|-------|----------------|
| CDP | `cursor_cdp_status` → `ok: true` |
| Composer health | `cursor_session({ token })` |
| Stuck queue / modal | `npm run watchdog:start` в `cr` (см. CONTINUITY) |
| UI / ручная отладка | `npm run dev` в `cr` → http://127.0.0.1:3847 |

**mlc-memory:** `memory_search` / `memory_write` (`project_memories`) — decisions, bugs, conventions, failed_attempts. Legacy: `known_limitations`, `notes`. Не дублировать TURNLOG.

**codebase-memory:** перед чтением многих файлов — `search_graph`, `query_graph`, impact/callers. Reindex: `bash scripts/memory_reindex.sh`.
