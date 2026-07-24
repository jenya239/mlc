# Track: Editor wrap hit-test respects scroll_x

Parent: [../../PLAN.md](../../PLAN.md) §58.
Residual of [TRACK_EDITOR_HORIZONTAL_SCROLL](TRACK_EDITOR_HORIZONTAL_SCROLL.md)
(§46 #17) — exposed after §53 pixel hit API still omitted `scroll_offset_x`.
Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `wrap_hit_scroll_x_stable` ×2 + `proportional_hit_test_stable` +
`shift_wheel_scrolls_horizontal` + `demo_live` compile + HEAD `run_ux_gate` (73 scenarios).
Anti-false-done: `e00cc359`…`c35bbb67` (STEP=0–2); `scroll_offset_x` on
`wrap_offset_at_point_pixel`/`wrap_offset_at_point`; demo click wire;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: horizontal scrollbar UI still OOS; glyph-draw shift under wrap
may still lag (Decision OOS here); `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_wrap_hit_scroll_x_stable.sh` | `ux_ok wrap_hit_scroll_x_stable` EXIT=0 (×2) |
| `run_ux_proportional_hit_test_stable.sh` | `ux_ok proportional_hit_test_stable` EXIT=0 |
| `run_ux_shift_wheel_scrolls_horizontal.sh` | `ux_ok shift_wheel_scrolls_horizontal` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (73 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `e00cc359` | Decision: wrap hit scroll_x |
| 1 | `3a61d451` | Red/stable stub |
| 2 | `c35bbb67` | API + demo + green stable |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Live wrap click uses `wrap_offset_at_point_pixel` with `scroll_offset_y` only; `content_x` ignores horizontal scroll. Non-wrap `nav_offset_at_point` already adds `metrics.scroll_offset_x`. Click/caret wrong when `scroll_offset_x > 0` under wrap |
| Strategy (v1) | Add `scroll_offset_x` to `wrap_offset_at_point_pixel` (`content_x += scroll_offset_x`); wire `demo_live` click; keep mono `wrap_offset_at_point` in sync |
| Primary gate | Green: `ux_ok wrap_hit_scroll_x_stable` — same screen x → different byte when scroll_x differs; auto-discovered by `run_ux_gate` |
| Module touch | `layout/word_wrap.mlc`, `demo_live` click site, scripts/scenario |
| REG | no |
| Out of scope | Horizontal scrollbar UI; ICU; SCRIPT_VM; MIR Epic 5; full-frame idle reshape (§57 OOS) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: hit API ignores scroll_x | **done** |
| 2 | Add scroll_x to pixel hit; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; horizontal scrollbar chrome.
