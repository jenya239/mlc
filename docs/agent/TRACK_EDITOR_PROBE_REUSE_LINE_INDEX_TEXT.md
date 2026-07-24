# Track: Editor probe reuses LineIndex text

Parent: [../PLAN.md](../PLAN.md) §86.
Residual of [TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX](../archive/tracks/TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX.md)
(§85) / #1d. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + related + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `cdd07cdc` | Decision: snapshot `document_text` from `line_index.text` |
| 1 | `123e40ad` | Red: ≥1 `document_to_string` in `probe.mlc`; stable stub |
| 2 | (this) | `document_text = line_index.text`; drop import; green stable |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §85 `editor_ux_snapshot` takes frame `LineIndex`, it still full-flattens via `document_to_string(active.buffer.document)` for `document_text` while `line_index.text` already holds the same buffer string. Honest #1d residual after §85 |
| Strategy (v1) | Set `document_text = line_index.text` (when active tab); drop `document_to_string` import/use from `ux/probe.mlc`. Out of scope: `drop_file` stringify; incremental lexer |
| Primary gate | Red: no green `run_ux_probe_reuses_line_index_text_stable.sh`. Green: `ux_ok probe_reuses_line_index_text` — `editor_ux_snapshot` uses `line_index.text` (no `document_to_string` in `ux/probe.mlc`); `probe_reuses_frame_line_index` + `probe_unit` + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/probe.mlc`; scripts; small scenario |
| REG | no |
| Out of scope | drop_file; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: probe still stringifies document | **done** |
| 2 | Use `line_index.text`; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; drop_file stringify; incremental/diff lexer.
