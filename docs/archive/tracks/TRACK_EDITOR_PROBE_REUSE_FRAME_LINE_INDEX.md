# Track: Editor probe reuses frame LineIndex

Parent: [../../PLAN.md](../../PLAN.md) §85.
Residual of [TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX](TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX.md)
(§84) / #1d. Size **M**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `probe_reuses_frame_line_index_stable` ×2 +
`probe_unit` + `goto_line_scrolls_caret` + `newline_caret` +
`large_file_no_full_stringify` + `demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `f866a137`…`121ebb52` (STEP=0–2); `editor_ux_snapshot` takes
`LineIndex` (no `line_index_from_document` in `ux/probe.mlc`); production
`misc/editor/ux/**` has zero `line_index_from_document`;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); snapshot
`document_to_string` + incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_probe_reuses_frame_line_index_stable.sh` | `ux_ok probe_reuses_frame_line_index` EXIT=0 (×2) |
| `run_ux_probe_unit.sh` | `probe_unit ok` EXIT=0 |
| `run_ux_goto_line_scrolls_caret.sh` | `ux_ok goto_line_scrolls_caret` EXIT=0 |
| `run_ux_newline_caret.sh` | `ux_ok newline_caret_and_spaces` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (100 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `f866a137` | Decision: probe snapshot reuses frame `LineIndex` |
| 1 | `9b1d31cc` | Red harness + stable stub `not implemented` |
| 2 | `121ebb52` | Pass `LineIndex` into `editor_ux_snapshot`; wire callers; green |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §80–§84 paint/scroll/selection/goto/overflow reuse frame `LineIndex`, `editor_ux_snapshot` (`ux/probe.mlc`) still re-flattens via `line_index_from_document` for caret line/column. Last production UX flatten of #1d (scenarios/tests may still build indexes for harness). Incremental/diff lexer remains OOS (Size L) |
| Strategy (v1) | Pass `LineIndex` into `editor_ux_snapshot`; wire all scenario/unit callers (use `line_index_from_string` / existing index). Drop `line_index_from_document` inside `probe.mlc`. Out of scope: `document_to_string` in snapshot; incremental lexer |
| Primary gate | Red: no green `run_ux_probe_reuses_frame_line_index_stable.sh`. Green: `ux_ok probe_reuses_frame_line_index` — `editor_ux_snapshot` takes/reuses `LineIndex` (no `line_index_from_document` in `ux/probe.mlc`); a representative caret scenario + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/probe.mlc`, scenario/unit callers; scripts |
| REG | no |
| Out of scope | `document_to_string` in snapshot; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: probe still re-stringifies | **done** |
| 2 | Pass frame LineIndex; wire callers; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; incremental/diff lexer; snapshot `document_to_string`.
