# Track: Editor text draw respects scroll_x

Parent: [../../PLAN.md](../../PLAN.md) §59.
Residual of [TRACK_EDITOR_WRAP_HIT_SCROLL_X](TRACK_EDITOR_WRAP_HIT_SCROLL_X.md)
(§58) / [TRACK_EDITOR_HORIZONTAL_SCROLL](TRACK_EDITOR_HORIZONTAL_SCROLL.md)
(§46 #17). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `text_draw_scroll_x_stable` ×2 + `wrap_hit_scroll_x_stable` +
`shift_wheel_scrolls_horizontal` + `demo_live` compile + HEAD `run_ux_gate` (74 scenarios).
Anti-false-done: `b950bc84`…`836e0e1f` (STEP=0–2); `text_pen_x_scrolled` on
find/fill/caret/syntax; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: horizontal scrollbar UI still OOS; `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_text_draw_scroll_x_stable.sh` | `ux_ok text_draw_scroll_x_stable` EXIT=0 (×2) |
| `run_ux_wrap_hit_scroll_x_stable.sh` | `ux_ok wrap_hit_scroll_x_stable` EXIT=0 |
| `run_ux_shift_wheel_scrolls_horizontal.sh` | `ux_ok shift_wheel_scrolls_horizontal` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (74 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `b950bc84` | Decision: text draw scroll_x |
| 1 | `79dc9526` | Red/stable stub |
| 2 | `836e0e1f` | helper + demo + green stable |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | After §58, wrap hit adds `scroll_offset_x`, but live draw still places syntax glyphs / selection fills / carets at `text_rect.x + 4` with no `- scroll_offset_x`. Visual text does not shift; click maps as if it did |
| Strategy (v1) | Subtract `app.ux.scroll_offset_x` from text pen_x, selection fill x, and caret x in `demo_live` (same origin as hit); thin helper optional |
| Primary gate | Red: no `run_ux_text_draw_scroll_x_stable.sh`. Green: `ux_ok text_draw_scroll_x_stable` — draw origin with scroll_x differs from origin without; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` draw sites (+ thin helper if needed), `scripts/run_ux_text_draw_scroll_x_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Horizontal scrollbar UI; ICU; SCRIPT_VM; MIR Epic 5; reopen §58 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: draw ignores scroll_x | **done** |
| 2 | Subtract scroll_x on draw; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; horizontal scrollbar chrome.
