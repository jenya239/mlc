# Track: Editor demo save avoids double stringify

Parent: [../PLAN.md](../PLAN.md) §89.
Residual of [TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY.md)
(§88) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert `save_demo_session` still calls `document_to_string` twice per tab; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: one flatten per tab in demo save |

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
| 1 | Red: demo save double-stringifies | pending |
| 2 | Single flatten (or open_buffer); green | pending |
| 3 | Critic: stable + demo_live + `run_ux_gate` | pending |

<!-- STEP=1 sub-steps: 1) red asserts ≥2 document_to_string in save_demo_session; 2) stable stub; 3) docs -->
<!-- STEP=2 sub-steps: 1) bind text once or tab_set_open_buffer; 2) green stable; 3) demo_live compile -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit/save/clipboard; tree_hit open_text; incremental/diff lexer.
