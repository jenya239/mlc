# Track: Editor multi-caret draw + blink

Parent: [../../PLAN.md](../../PLAN.md) §56.
Residual of [TRACK_EDITOR_MULTI_SELECTION_VISUAL](TRACK_EDITOR_MULTI_SELECTION_VISUAL.md)
(§55 Decision OOS). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `multi_caret_draw_stable` ×2 + `multi_selection_visual_stable` +
`wrap_selection_visual_stable` + `proportional_hit_test_stable` + `demo_live` compile + HEAD `run_ux_gate` (71 scenarios).
Anti-false-done: `728d014a`…`a163d58f` (STEP=0–2); `multi_caret_draw` loop over `multi_selection_count`;
`caret_pixel_in_visual_rows`; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual (Decision OOS at close time; **absorbed by §57** 2026-07-24):
paint-only idle path was primary `last_caret_*` only → fixed in
[TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY](TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY.md).
`*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_multi_caret_draw_stable.sh` | `ux_ok multi_caret_draw_stable` EXIT=0 (×2) |
| `run_ux_multi_selection_visual_stable.sh` | `ux_ok multi_selection_visual_stable` EXIT=0 |
| `run_ux_wrap_selection_visual_stable.sh` | `ux_ok wrap_selection_visual_stable` EXIT=0 |
| `run_ux_proportional_hit_test_stable.sh` | `ux_ok proportional_hit_test_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (71 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `728d014a` | Decision: multi-caret draw |
| 1 | `79062774` | Red/stable stub |
| 2 | `a163d58f` | multi_caret_draw + caret_pixel helper |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | `demo_live` draws/blinks only the primary `selection` caret (`last_caret_*`); secondary carets in `EditorMultiSelection` are invisible |
| Strategy (v1) | For each range in `multi`, compute caret pixel via existing wrap/advance helpers; draw all carets when blink-on; paint-only idle path overlays primary only remains OK for v1 (full multi overlay optional residual) |
| Primary gate | Green: `ux_ok multi_caret_draw_stable` — two collapsed carets → two distinct caret x; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` caret draw (+ thin helper), `scripts/run_ux_multi_caret_draw_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Paint-only idle multi-overlay rewrite; ICU; SCRIPT_VM; MIR Epic 5; reopen §55 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no multi-caret draw script | **done** |
| 2 | Implement multi-caret draw; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; full idle paint-only multi overlay.
