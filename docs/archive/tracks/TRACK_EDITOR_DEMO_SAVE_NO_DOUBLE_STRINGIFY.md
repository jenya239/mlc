# Track: Editor demo save avoids double stringify

Parent: [../../PLAN.md](../../PLAN.md) §89.
Residual of [TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY](TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY.md)
(§88) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `demo_save_no_double_stringify_stable` ×2
(includes `demo_live` compile) + HEAD `run_ux_gate`.
Anti-false-done: `aaa6a995`…`a7695b8b` (STEP=0–2); exactly one
`document_to_string(tab.buffer.document)` bound as `text` and reused for
`dump_tab_file` + `tab_set_open`; `misc/editor/**` + scripts → REG skip;
no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); `frame_snapshot` /
edit/save/clipboard/tree_hit stringifies + incremental lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_demo_save_no_double_stringify_stable.sh` | `ux_ok demo_save_no_double_stringify` + `demo_live_fs_compile_ok` EXIT=0 (×2) |
| HEAD `run_ux_gate.sh` (104 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `aaa6a995` | Decision: one flatten per tab in demo save |
| 1 | `a1ca5785` | Red harness + stable stub `not implemented` |
| 2 | `a7695b8b` | Bind `text` once; green stable + demo compile |
| 3 | (this) | Critic OK; archive |

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
| 3 | Critic: stable + demo_live + `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit/save/clipboard; tree_hit open_text; incremental/diff lexer.
