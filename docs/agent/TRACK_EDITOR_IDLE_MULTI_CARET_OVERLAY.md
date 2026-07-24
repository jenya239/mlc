# Track: Editor idle multi-caret paint overlay

Parent: [../PLAN.md](../PLAN.md) §57.
Residual of [TRACK_EDITOR_MULTI_CARET_DRAW](../archive/tracks/TRACK_EDITOR_MULTI_CARET_DRAW.md)
(§56 Decision OOS). Size **S**.

## Status: **active** (2026-07-24) — STEP=2 done → Critic STEP=3

## Next step

**STEP=3** — Critic: stable + full `run_ux_gate`.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Full-frame draw paints all multi carets, but idle paint-only blink path overlays only primary `last_caret_*` — secondary carets freeze/vanish on idle |
| Strategy (v1) | Cache caret rects for all multi selections at full draw; idle paint-only loop erase/draw each rect on blink (no reshape / no wrap recompute); keep §51 CPU budget |
| Primary gate | Red: no `run_ux_idle_multi_caret_overlay_stable.sh`. Green: `ux_ok idle_multi_caret_overlay_stable` — ≥2 cached caret rects with distinct x; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` idle paint-only path (+ thin caret-rect cache helper if needed), `scripts/run_ux_idle_multi_caret_overlay_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | ICU; SCRIPT_VM; MIR Epic 5; reopen §56 numbered STEPs; full-frame redraw on idle |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: idle overlay primary-only | **done** |
| 2 | Implement multi-caret idle overlay; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves paint-only uses single last_caret_* / no multi cache -->
<!-- STEP=2: cache N rects at full draw; idle loop all; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; idle full-frame reshape.
