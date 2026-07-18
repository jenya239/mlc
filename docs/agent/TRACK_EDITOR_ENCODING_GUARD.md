# Track: Editor encoding guard (BOM + UTF-8 + binary refuse)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#13**.
`open_buffer_from_path` loads raw bytes with no BOM strip, no UTF-8 check,
no binary refuse — NUL/garbage and UTF-8 BOM pollute the buffer. Review gates:
`binary_file_refused`, `utf8_bom_detected` (L0). Size **M**.

## Status: **active** (2026-07-18) — STEP=2 done; next Critic

## Next step

**STEP=3** — Critic: re-run gates; archive.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Open accepts any bytes; UTF-8 BOM stays in text; binary/invalid UTF-8 not refused |
| Hook | Guard inside `open_buffer_from_path` after `file_read_to_string`, **before** CRLF detect/normalize |
| BOM | If raw starts with UTF-8 BOM bytes `EF BB BF` → strip them from text; set `had_bom: 1` on `OpenBuffer`. Else `had_bom: 0` |
| Save BOM | If `had_bom != 0`, prefix `EF BB BF` on the encoded save payload (after CRLF encode). Preserve round-trip |
| Binary refuse | If raw (pre-BOM-strip) contains a NUL (`\0`) byte → do **not** load document; `last_error` set (e.g. `"Binary file refused: " + path`); empty document; `had_bom: 0` |
| UTF-8 validity | After BOM strip: validate UTF-8 structural sequences (reuse `utf8_codepoint_byte_length` / walk). Invalid → same refuse path as binary (`last_error` mentions invalid UTF-8) |
| `open_buffer_from_text` | Unchanged (trusted in-memory); `had_bom: 0` |
| Propagate | `had_bom` through `open_buffer_*` helpers like `line_ending` |
| Scenarios | L0 `binary_file_refused`: write fixture with embedded NUL; open → `open_buffer_has_error`; document empty. L0 `utf8_bom_detected`: write `"\u{FEFF}hello\n"` / raw BOM+`hello\n`; open → no error; document `"hello\n"`; `had_bom==1`; save → disk still starts with BOM |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/document/save.mlc` (+ tiny encoding helper if needed; L0 tests + scripts). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `[mlc-editor] binary_file_refused ok` + `[mlc-editor] utf8_bom_detected ok` |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `run_editor_save_unit.sh` + `run_editor_crlf_preserved_on_save.sh` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L0 red harnesses (`binary_file_refused`, `utf8_bom_detected`) | **done** (red: `binary not refused`; `bom not stripped`) |
| 2 | BOM strip/preserve + NUL/UTF-8 refuse in `open_buffer_from_path` | **done** (both L0 ok; save+crlf+demo) |
| 3 | Critic: gates; archive | pending |

## Out of scope

Non-UTF-8 encodings (Latin-1/UTF-16 open); status-bar encoding label wire; charset conversion UI; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
