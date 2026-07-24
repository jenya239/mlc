# Track: Editor probe reuses frame LineIndex

Parent: [../PLAN.md](../PLAN.md) §85.
Residual of [TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX](../archive/tracks/TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX.md)
(§84) / #1d. Size **M**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + related + `demo_live_fs_compile` + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `f866a137` | Decision: probe snapshot reuses frame `LineIndex` |
| 1 | `9b1d31cc` | Red: ≥1 `line_index_from_document` in `probe.mlc`; stable stub |
| 2 | (this) | Pass `LineIndex` into `editor_ux_snapshot`; wire callers; green |

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
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; incremental/diff lexer; snapshot `document_to_string`.
