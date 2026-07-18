# Track: Editor drag-past-edge autoscroll

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#9**.
Text-selection drag past the viewport top/bottom does not scroll — cannot
extend selection beyond visible lines. Review gate: `drag_past_edge_autoscrolls`
(L1.5). Size **M**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1.5 + compile. Anti-false-done:
`a375f640`…`6d4840e3` (STEP=0–2). Wire present: `editor_ux_drag_text_autoscroll`
(past-edge ±1 line_height + clamp hit Y + `editor_ux_drag_text`);
`editor_app_drag_text_autoscroll`; demo_live selecting_text path. **reopen: none**.

Honest residual: live drag uses non-wrap `nav_drag` (replaced wrap_offset path);
horizontal autoscroll out of scope (#17); soft margin out of scope; L1.5 exercises
ux API not GLFW inject; viewport = editor panel (not gutter/text_rect inset).

| Gate | Result |
|------|--------|
| `run_ux_drag_past_edge_autoscrolls.sh` | `ux_ok drag_past_edge_autoscrolls` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#10 EDITOR_AUTO_INDENT`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `a375f640` | Decision freeze + open |
| 1 | `4ea9c197` | L1.5 red harness |
| 2 | `6d4840e3` | autoscroll + demo_live wire |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | During click-drag text selection, pointer past viewport top/bottom does not auto-scroll; selection stuck to visible range |
| Trigger | Active text-selection drag only (`selecting_text` / ux drag path). Not divider drag, not hover-without-button |
| Edge | **Past edge** (pointer Y &lt; viewport top → scroll up; Y ≥ viewport bottom → scroll down). No soft margin zone this track |
| Per-frame | While held past edge: scroll by **1 line_height** toward the edge (clamped); map hit-test Y to the **clamped viewport edge** (top or bottom-1); extend selection via existing `nav_drag` / `editor_ux_drag_text` semantics |
| Multi-frame | Stateful across frames: each call while past edge advances scroll + selection. Scenario drives ≥2 frames with pointer held below/above edge |
| Horizontal | **Out of scope** (`#17 EDITOR_HORIZONTAL_SCROLL`) |
| API | `editor_ux_drag_text_autoscroll(state, metrics, pixel_x, pixel_y) -> EditorUxState` (or extend `editor_ux_drag_text` with same signature — prefer dedicated name for gate clarity). Inside viewport → same as plain drag (no scroll change from this helper) |
| Wire | `demo_live` selection-drag branch calls autoscroll helper instead of bare offset hit-test |
| Scenarios | L1.5: `drag_past_edge_autoscrolls` — tall doc, short viewport; start drag; hold pointer past bottom ≥2 frames; assert `scroll_offset_y` increased and selection focus moved to a later line |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux drag/scroll + `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok drag_past_edge_autoscrolls` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1.5 scenario first (`drag_past_edge_autoscrolls`) | **done** (red: `ux_fail drag autoscroll scroll_offset_y`) |
| 2 | autoscroll helper + demo_live wire | **done** (`ux_ok`; `demo_live_fs_compile_ok`) |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Horizontal autoscroll (#17); tab reorder drag (#15); soft edge margin; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
