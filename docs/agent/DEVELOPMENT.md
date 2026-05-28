# Development (after CONTINUITY block in queued prompt)

Re-read [CONTINUITY.md](CONTINUITY.md) every turn — rules apply without restart.

## Principles

- **One sub-step per prompt** — one concern; bisect-friendly.
- **One layer per sub-step** — `compiler/` XOR `lib/mlc/`, not both (except tiny import fix).
- **Verify before next queue:** tests green, then self-host if `compiler/**` changed.
- **Plans:** [PLAN.md](../PLAN.md) north star; [TRACK_*.md](.) active work; **Planner** extends TRACK, **Driver** executes.

## Roles (see CONTINUITY)

- **Driver** — code + verify (most turns).
- **Planner** — TRACK rows from PLAN; no `compiler/`.
- **Backlog** — TRACK vs git hygiene; no `compiler/`.

## Verification ladder (compiler changes)

1. `compiler/tests/build_tests.sh` → `run_tests` (expect 473+). Abort if > 10 min silent.
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

## Current priority

[TRACK_DIAGNOSTICS.md](TRACK_DIAGNOSTICS.md) **closed** (`f6863fe`). [TRACK_SECURITY.md](TRACK_SECURITY.md) **closed** (`a035c3d`). [TRACK_PERFORMANCE.md](TRACK_PERFORMANCE.md) **closed** (`0671422`). [TRACK_SAFETY.md](TRACK_SAFETY.md) **closed** (`32f8335`). [TRACK_PHASE1.md](TRACK_PHASE1.md) **closed** (`fd42eab`). Parser `ref mut` = deferred separate branch. **Planner:** `STEP=plan-refresh`.

## Step sizing (good vs bad)

| Good | Bad |
|------|-----|
| Step 9: one arm + tests | «Migrate all expr_eval» |
| Planner adds steps 10–12 | Driver invents scope without TRACK |
| Backlog flags uncommitted > 15 | Silent TRACK drift |

## Code rules

- No abbreviations (`.cursor/rules/no-abbreviations.mdc`).
- Minimal diff; match surrounding style.

## MCP (`user-cr-cursor`)

Token-only — [CONTINUITY.md](CONTINUITY.md).

После изменений в `cr`: `npm run build` в `~/workspaces/current/cr`, Reload MCP в Cursor.

| Check | Command / tool |
|-------|----------------|
| CDP | `cursor_cdp_status` → `ok: true` |
| Composer health | `cursor_session({ token })` |
| Stuck queue / modal | `npm run watchdog:start` в `cr` (см. CONTINUITY) |
| UI / ручная отладка | `npm run dev` в `cr` → http://127.0.0.1:3847 |

**mlc-memory:** blockers (`known_limitations`), Planner/Backlog notes (`notes`).
