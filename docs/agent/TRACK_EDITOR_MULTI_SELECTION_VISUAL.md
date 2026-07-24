# Track: Editor multi-cursor selection visual fills

Parent: [../PLAN.md](../PLAN.md) §55.
Residual of [TRACK_EDITOR_WRAP_SELECTION_VISUAL](../archive/tracks/TRACK_EDITOR_WRAP_SELECTION_VISUAL.md)
(§54 Decision OOS). Size **S**.

## Status: **active** (2026-07-24) — STEP=1 done → Driver STEP=2

## Next step

**STEP=2** — Implement multi-range visual fills; green stable.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | `demo_live` draws selection fill only for primary `selection`; secondary ranges in `EditorMultiSelection` get no visual fill (carets may exist; fills do not) |
| Strategy (v1) | For each range in `multi`, call `selection_fill_visual_rows` and draw spans (reuse §54 helper); primary path unchanged when `multi_selection_count == 1` |
| Primary gate | Red: no `run_ux_multi_selection_visual_stable.sh`. Green: `ux_ok multi_selection_visual_stable` — two non-empty ranges → ≥2 fill span groups; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` selection draw (+ thin helper if needed), `scripts/run_ux_multi_selection_visual_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Multi-caret blink polish; find-highlight; ICU; SCRIPT_VM; MIR Epic 5; reopen §54 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no multi-selection visual script | **done** |
| 2 | Implement multi-range visual fills; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves stable script missing or documents gap -->
<!-- STEP=2: loop multi ranges → selection_fill_visual_rows; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; multi-caret chrome redesign.
