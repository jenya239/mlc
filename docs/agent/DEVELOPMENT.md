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
3. If **`compiler/**` touched:** `compiler/build.sh` → self-host `diff -rq`.
4. Step **14** in TRACK: mandatory self-host before closing visitor batch.

## Current priority

[TRACK_PLAN.md](TRACK_PLAN.md) — see **Next step** section at bottom of that file. Parser `ref mut` = step 15, separate branch.

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

**mlc-memory:** blockers (`known_limitations`), Planner/Backlog notes (`notes`).
