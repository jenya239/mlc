# Track: Editor idle multi-caret paint overlay

Parent: [../../PLAN.md](../../PLAN.md) §57.
Residual of [TRACK_EDITOR_MULTI_CARET_DRAW](TRACK_EDITOR_MULTI_CARET_DRAW.md)
(§56 Decision OOS). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `idle_multi_caret_overlay_stable` ×2 + `multi_caret_draw_stable` +
`multi_selection_visual_stable` + `demo_live` compile + HEAD `run_ux_gate` (72 scenarios).
Anti-false-done: `22339446`…`f61f51e3` (STEP=0–2); `last_caret_rects` + idle `idle_multi_caret_overlay` loop;
`ux/caret_overlay.mlc`; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: none material (Decision OOS full-frame idle reshape stays deferred).
`*_red` post-green fails (use stable only).
Legacy note in §56 archive residual text superseded by this close.

| Gate | Result |
|------|--------|
| `run_ux_idle_multi_caret_overlay_stable.sh` | `ux_ok idle_multi_caret_overlay_stable` EXIT=0 (×2) |
| `run_ux_multi_caret_draw_stable.sh` | `ux_ok multi_caret_draw_stable` EXIT=0 |
| `run_ux_multi_selection_visual_stable.sh` | `ux_ok multi_selection_visual_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (72 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `22339446` | Decision: idle multi-caret overlay |
| 1 | `3b58a3e4` | Red/stable stub |
| 2 | `f61f51e3` | last_caret_rects + caret_overlay |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Full-frame draw paints all multi carets, but idle paint-only blink path overlays only primary `last_caret_*` — secondary carets freeze/vanish on idle |
| Strategy (v1) | Cache caret rects for all multi selections at full draw; idle paint-only loop erase/draw each rect on blink (no reshape / no wrap recompute); keep §51 CPU budget |
| Primary gate | Green: `ux_ok idle_multi_caret_overlay_stable` — ≥2 cached caret rects with distinct x; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` idle paint-only path (+ `ux/caret_overlay.mlc`), scripts/scenario |
| REG | no |
| Out of scope | ICU; SCRIPT_VM; MIR Epic 5; reopen §56 numbered STEPs; full-frame redraw on idle |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: idle overlay primary-only | **done** |
| 2 | Implement multi-caret idle overlay; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; idle full-frame reshape.
