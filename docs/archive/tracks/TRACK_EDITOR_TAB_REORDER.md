# Track: Editor Tab Reorder (drag strip)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#15**.
Tab strip click activates / closes; no drag-to-reorder. Review gate:
`drag_reorder_tabs` (L1). Size **M**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1 + tab_strip + demo compile.
Anti-false-done: `dccfb086`…`ba252355` (STEP=0–2). Wire present:
`tab_set_move` (active follows); `editor_ux_tab_strip_drag_to`; demo_live
`dragging_tab_index` (title press, not close) + release move; suppress
text/divider drag while reordering. **reopen: none**.

Honest residual: no animated slide; no drag threshold/pixels (index-only);
no L1 GLFW multi-frame inject (API + live wire); strip overflow scroll out
of scope.

| Gate | Result |
|------|--------|
| `run_ux_drag_reorder_tabs.sh` | `ux_ok drag_reorder_tabs` EXIT=0 |
| `run_ux_tab_strip_click.sh` | `ux_ok tab_strip_click_activates` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#16 EDITOR_FONT_ZOOM`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `dccfb086` | Decision freeze + open |
| 1 | `56527c1b` | L1 red harness + stubs |
| 2 | `ba252355` | `tab_set_move` + demo_live drag |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Cannot drag-reorder tabs; order fixed by open order only |
| Model | Add `tab_set_move(tabs, from_index, to_index) -> TabSet`: remove tab at `from`, insert at `to` (clamp); **active follows the moved tab** (new index). Same index / invalid → no-op |
| Hit | Press on **title** slot (`tab_strip_hit_index`), not close rect. Drop target = hit index under pointer on release |
| Gesture | Press tab A → drag → release on different slot B → `tab_set_move(A, B)`. No threshold pixels required in L1 (index change is enough). Click-without-index-change stays activate-only |
| Close | Close hit unchanged; tab-drag must not fire close. If press was on close → close path, not reorder |
| Live | `demo_live`: while mouse down on strip title, track `dragging_tab_index`; on release if hit ≠ drag source → move; suppress text-selection / splitter drag while tab-drag active |
| API surface | Prefer `tab_set_move` + thin ux helper `editor_ux_tab_strip_drag_to(...)` (or equivalent) callable from L1 without GLFW |
| Scenarios | L1: `drag_reorder_tabs` — ≥3 tabs with distinct paths; move index 0 → 2; assert path order + `active_index` follows moved tab; token `ux_ok drag_reorder_tabs` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` `workspace/tabs`, `ux/tab_strip`, `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok drag_reorder_tabs` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | existing tab strip / tabs unit if present (no new compiler gate) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1 scenario first (`drag_reorder_tabs`) | **done** (2026-07-18) — stubs red: `ux_fail drag_reorder order first` |
| 2 | `tab_set_move` + strip drag wire + demo_live | **done** (2026-07-18) — `ux_ok drag_reorder_tabs` |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Animated tab slide; overflow scroll of strip; pin/unpin; middle-click reorder; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
