# Track: Editor CRLF/LF preserve on open+save

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#12**.
Open loads raw bytes; edits insert `"\n"`; save writes `document_to_string`
as-is — CRLF files can become LF/mixed. Review gate: `crlf_preserved_on_save`
(L0). Size **M**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L0 + save_unit + compile. Anti-false-done:
`84e3ab53`…`2c786f3a` (STEP=0–2). Wire present: `detect_line_ending` +
`normalize_newlines_to_lf` on `open_buffer_from_path`; `OpenBuffer.line_ending`;
`encode_for_save` CRLF on save. **reopen: none**.

Honest residual: no UI line-ending picker; `open_buffer_from_text` stays LF
default (no detect); mixed per-line not preserved (whole-file mode); encoding/BOM
→ #13.

| Gate | Result |
|------|--------|
| `run_editor_crlf_preserved_on_save.sh` | `[mlc-editor] crlf_preserved_on_save ok` EXIT=0 |
| `run_editor_save_unit.sh` | `[mlc-editor] save_unit ok` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#13 EDITOR_ENCODING_GUARD`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `84e3ab53` | Decision freeze + open |
| 1 | `8709d489` | L0 red harness |
| 2 | `2c786f3a` | detect/normalize + save convert |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | No line-ending policy: CRLF on disk is not preserved across open→edit→save |
| Internal model | **Normalize buffer to LF** (`\n` only) after open. Strip `\r` that precedes `\n`; lone `\r` → `\n`. Edits keep inserting `"\n"` |
| Detect on open | Scan raw text: if any `\r\n` → mode **CRLF**; else → mode **LF**. Empty / no newline → **LF** (default). Mixed: prefer CRLF if any `\r\n` present |
| Persist mode | Add `line_ending: i32` on `OpenBuffer` (`0`=LF, `1`=CRLF). Propagate through `open_buffer_*` helpers; default LF for `open_buffer_from_text` / new buffers |
| Save | `document_to_string` then if CRLF mode: replace each `\n` with `\r\n` before `file_write_string`. LF mode: write as-is |
| Reload | Re-detect from disk (same as open) |
| Scenarios | L0: `crlf_preserved_on_save` — write fixture `"a\r\nb\r\n"`; open; optional dirty no-op or tiny edit that keeps line count; save; re-read bytes assert still CRLF (`\r\n` present, not LF-only). Companion: LF fixture stays LF |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/document/save.mlc` (+ small line-ending helper if needed; L0 test + script). Wire existing save path only. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `[mlc-editor] crlf_preserved_on_save ok` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_editor_save_unit.sh` → `[mlc-editor] save_unit ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L0 scenario first (`crlf_preserved_on_save`) | **done** (red: `crlf_fail not normalized to LF`) |
| 2 | detect + normalize + OpenBuffer.line_ending + save convert | **done** (`crlf_preserved_on_save ok`; save_unit; demo compile) |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Encoding/BOM (#13); UI line-ending picker; convert-on-demand command; mixed per-line preserve (whole-file mode only); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
