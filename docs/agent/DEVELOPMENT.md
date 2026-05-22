# Development (after CONTINUITY block in queued prompt)

## Principles

- **Small verifiable steps** — one concern per commit/prompt; bisect-friendly.
- **Verify before next queue:** Ruby/compiler tests, then self-host if `compiler/**` changed.
- **Re-check is normal** — return to tests after fixes; do not skip because “already ran once”.
- **Plans:** [PLAN.md](../PLAN.md) is north star; [TRACK_*.md](.) hold active work; adjust tasks when facts change — update TRACK, not silent drift.

## Verification ladder (compiler changes)

1. `compiler/tests/build_tests.sh` → `run_tests` (expect 463+).
2. `compiler/build.sh` → fresh `compiler/out/mlcc`.
3. Self-host: `mlcc → mlcc2`, `diff -rq` output dirs (see `.cursor/rules/mlcc-self-host-verification.mdc`).
4. Optional: `benchmarks/profile/run_profile.sh`, `compare_baseline.sh` (soft gate).

## Current priority

1. **Finish [TRACK_OPTIMIZATION.md](TRACK_OPTIMIZATION.md)** — uncommitted optimization batch: commit-worthy after full verify.
2. **Then** next items from [PLAN.md](../PLAN.md) § phases (parser `ref mut` = separate branch; MLC IR / C++ AST later).

## Step sizing (good vs bad)

| Good | Bad |
|------|-----|
| Fix trait maps + run tests | “Optimize everything” |
| One TRACK checkbox + verify | Multi-file refactor without tests |
| Update TRACK + queue next | Stop without `cursor_enqueue_send` |

## Code rules

- No abbreviations in identifiers (`.cursor/rules/no-abbreviations.mdc`).
- Minimal diff; match surrounding style.
- Do not edit `docs/agent/CONTINUITY.md` workflow without user ask.

## MCP (`user-cr-cursor`)

Только через **token** — см. [CONTINUITY.md](CONTINUITY.md):

`register` → `resolve` → `cursor_enqueue_send({ token, text })` с `AGENT_TOKEN=…` в text.

Self-методов нет.

**mlc-memory:** decisions / notes между сессиями.
