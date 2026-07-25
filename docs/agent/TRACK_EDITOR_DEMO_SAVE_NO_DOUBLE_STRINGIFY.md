# Track: Editor demo save avoids double stringify

Parent: [../PLAN.md](../PLAN.md) §89.
Residual of [TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY.md)
(§88) / #1d. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + demo_live + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `aaa6a995` | Decision: one flatten per tab in demo save |
| 1 | `a1ca5785` | Red: ≥2 `document_to_string(tab.buffer.document)`; stable stub |
| 2 | (this) | Bind `text` once; green stable + demo compile |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §88, `save_demo_session` in `demo_live.mlc` still does `document_to_string(tab.buffer.document)` twice per tab — once for `dump_tab_file`, again for `tab_set_open`. Honest #1d residual after §88 |
| Strategy (v1) | One flatten per tab: bind `text = document_to_string(...)` once and reuse for dump + open, **or** have dump return `OpenBuffer` and `tab_set_open_buffer`. Out of scope: frame_snapshot; edit/save/clipboard; tree_hit `open_text`; incremental lexer |
| Primary gate | Red: no green `run_ux_demo_save_no_double_stringify_stable.sh`. Green: `ux_ok demo_save_no_double_stringify` — ≤1 `document_to_string` in `save_demo_session` path; `demo_live` compile stays green; auto-discovered by `run_ux_gate` if scenario script present |
| Module touch | `demo_live.mlc`; scripts; small scenario optional |
| REG | no |
| Out of scope | frame_snapshot; edit/save/clipboard; tree_hit; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: demo save double-stringifies | **done** |
| 2 | Single flatten (or open_buffer); green | **done** |
| 3 | Critic: stable + demo_live + `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit/save/clipboard; tree_hit open_text; incremental/diff lexer.
