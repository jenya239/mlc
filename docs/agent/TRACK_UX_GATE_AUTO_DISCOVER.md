# Track: UX gate auto-discover full suite

Parent: [../PLAN.md](../PLAN.md) §48.
Size **S**. Source: uncommitted interactive WIP + `GUI_UX_TESTING.md` Standing
discipline item 5 (frozen 2026-07-20, user request).

## Status: **active** (2026-07-22) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Land auto-discover `run_ux_gate.sh` + `GUI_UX_TESTING.md` discipline;
full gate EXIT=0 with reported scenario count (baseline ≥ 60, excludes `*_red`).

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | HEAD `scripts/run_ux_gate.sh` is a **hand-maintained 14-script subset**; new tracks' `run_ux_*_stable.sh` (e.g. idle-frame, minimap-clip) are **not** regression-gated. Coverage silently drops when scripts are added without editing the list. §46/#37 Critic had to run HEAD gate separately from track L2 |
| Strategy (v1) | Auto-discover `scripts/run_ux_*.sh`; skip `run_ux_gate.sh` and `*_red.sh` (STEP=1 red scaffolding intentionally fails post-green). Print scenario count on success/fail. Document Standing discipline in `docs/GUI_UX_TESTING.md` (item 5 already drafted in WIP): Critic runs full gate **twice** back-to-back; record count in SESSION |
| Primary gate | `bash scripts/run_ux_gate.sh` EXIT=0 and stdout/stderr includes `all ok (N scenarios)` with **N ≥ 60** (current tree: 63 non-red scripts). Optional STEP=2: second back-to-back run (determinism) |
| Module touch | `scripts/run_ux_gate.sh`, `docs/GUI_UX_TESTING.md` only |
| REG | no |
| Out of scope | fixing individual red/flaky scenarios (quarantine separately); `compiler/`; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Commit auto-discover + docs; full gate green | open |
| 2 | Critic: gate ×2; archive | open |

<!-- STEP=1: land WIP; run_ux_gate all ok (N≥60) -->
<!-- STEP=2: Critic runs gate twice; archive -->

## Out of scope

MIR_VM Epic 5; SCRIPT_VM; LANG_AUTO_CYCLE; new editor features.
