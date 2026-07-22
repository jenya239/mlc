# Track: UX gate auto-discover full suite

Parent: [PLAN.md](../../PLAN.md) §48.
Size **S**. Source: uncommitted interactive WIP + `GUI_UX_TESTING.md` Standing
discipline item 5 (frozen 2026-07-20, user request).

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=2):** Re-ran full `run_ux_gate.sh` **twice** back-to-back.
Both: `[ux gate] all ok (63 scenarios)` EXIT=0. Anti-false-done: `6422b6c4`…`8d8d154e`
(STEP=0–1); auto-discover + `*_red.sh` exclude; `GUI_UX_TESTING.md` item 5; no
`compiler/`/`lib/mlc/` → REG skip. Coverage 14 → **63**. **reopen: none**.

| Gate | Result |
|------|--------|
| `run_ux_gate.sh` run 1 | `all ok (63 scenarios)` EXIT=0 |
| `run_ux_gate.sh` run 2 | `all ok (63 scenarios)` EXIT=0 |
| REG / self-host | N/A (scripts + docs only) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; SCRIPT_VM / MIR Epic 5 / LANG_AUTO_CYCLE gated).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `6422b6c4` | Decision: auto-discover, N≥60 |
| 1 | `8d8d154e` | Land gate + GUI_UX_TESTING; 63 green |
| 2 | (this) | Critic gate ×2; archive |

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | Hand-maintained 14-script subset missed new track stables |
| Strategy (v1) | Auto-discover `run_ux_*.sh`, exclude `*_red.sh`; print count |
| Primary gate | `all ok (N scenarios)` with N ≥ 60; Critic ×2 |
| Module touch | `scripts/run_ux_gate.sh`, `docs/GUI_UX_TESTING.md` |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Commit auto-discover + docs; full gate green | **done** |
| 2 | Critic: gate ×2; archive | **done** |

## Out of scope

MIR_VM Epic 5; SCRIPT_VM; LANG_AUTO_CYCLE; new editor features.
