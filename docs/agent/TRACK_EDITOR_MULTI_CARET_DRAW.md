# Track: Editor multi-caret draw + blink

Parent: [../PLAN.md](../PLAN.md) §56.
Residual of [TRACK_EDITOR_MULTI_SELECTION_VISUAL](../archive/tracks/TRACK_EDITOR_MULTI_SELECTION_VISUAL.md)
(§55 Decision OOS). Size **S**.

## Status: **active** (2026-07-24) — STEP=2 done → Critic STEP=3

## Next step

**STEP=3** — Critic: stable + full `run_ux_gate`.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | `demo_live` draws/blinks only the primary `selection` caret (`last_caret_*`); secondary carets in `EditorMultiSelection` are invisible |
| Strategy (v1) | For each range in `multi`, compute caret pixel via existing wrap/advance helpers; draw all carets when blink-on; paint-only idle path overlays primary only remains OK for v1 (full multi overlay optional residual) |
| Primary gate | Red: no `run_ux_multi_caret_draw_stable.sh`. Green: `ux_ok multi_caret_draw_stable` — two collapsed carets → two distinct caret x positions; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` caret draw (+ thin helper if needed), `scripts/run_ux_multi_caret_draw_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Paint-only idle multi-overlay rewrite; ICU; SCRIPT_VM; MIR Epic 5; reopen §55 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no multi-caret draw script | **done** |
| 2 | Implement multi-caret draw; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves stable script missing or documents gap -->
<!-- STEP=2: loop multi carets → draw; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; full idle paint-only multi overlay.
