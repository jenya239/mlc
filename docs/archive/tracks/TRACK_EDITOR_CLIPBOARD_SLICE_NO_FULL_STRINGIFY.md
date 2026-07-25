# Track: Editor clipboard slice without full stringify

Parent: [../../PLAN.md](../../PLAN.md) §92.
Residual of [TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY](TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY.md)
(§91) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `clipboard_slice_no_full_stringify_stable` ×2
(+ clipboard_unit + copy_paste) + HEAD `run_ux_gate`.
Anti-false-done: `eb759388`…`79e1a78c` (STEP=0–2); `document_byte_slice` on
`piece_table`; `clipboard_edit` has zero `document_to_string`;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); `frame_snapshot`
single flatten / `edit.mlc` flattens / save stringify + incremental lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_clipboard_slice_no_full_stringify_stable.sh` | `ux_ok clipboard_slice_no_full_stringify` EXIT=0 (×2) |
| `run_editor_clipboard_unit.sh` | EXIT=0 |
| `run_ux_copy_paste.sh` | EXIT=0 |
| HEAD `run_ux_gate.sh` (107 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `eb759388` | Decision: piece-range slice for clipboard copy/cut |
| 1 | `dc0ac679` | Red: `document_to_string` in slice; stable stub |
| 2 | `79e1a78c` | `document_byte_slice` + clipboard wire; green stable |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §91, `clipboard_edit.mlc` `document_slice` still does `document_to_string(document)` then `substring` for Ctrl+C/X — full flatten even for a small selection. Honest #1d residual after §91 |
| Strategy (v1) | Export `document_byte_slice` (or equivalent) on `piece_table` that walks only pieces overlapping `[byte_start, byte_end)`; use it in `clipboard_edit.document_slice`. Out of scope: `frame_snapshot` single flatten; `edit.mlc` autoclose/newline flattens; save stringify; incremental lexer |
| Primary gate | Red: no green `run_ux_clipboard_slice_no_full_stringify_stable.sh`. Green: `ux_ok clipboard_slice_no_full_stringify` — `clipboard_edit` has no `document_to_string`; clipboard unit / copy-paste scenario stay green |
| Module touch | `document/piece_table.mlc`; `document/clipboard_edit.mlc`; scripts; optional scenario |
| REG | no |
| Out of scope | frame_snapshot; edit.mlc flattens; save; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: clipboard slice still full-flattens | **done** |
| 2 | `document_byte_slice` + wire clipboard; green | **done** |
| 3 | Critic: stable + related + `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit.mlc flattens; save; incremental/diff lexer.
