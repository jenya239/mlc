# Track: Editor encoding guard (BOM + UTF-8 + binary refuse)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#13**.
`open_buffer_from_path` loads raw bytes with no BOM strip, no UTF-8 check,
no binary refuse — NUL/garbage and UTF-8 BOM pollute the buffer. Review gates:
`binary_file_refused`, `utf8_bom_detected` (L0). Size **M**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran both L0 + save + crlf + compile.
Anti-false-done: `ba28446d`…`e141ac97` (STEP=0–2). Wire present:
NUL refuse; BOM strip + `had_bom` + re-prefix on save; `utf8_text_is_valid`
before CRLF normalize. **reopen: none**.

Honest residual: no status-bar encoding label; no Latin-1/UTF-16 open;
`open_buffer_from_text` trusted (no guard); invalid-UTF-8 covered by validator
but no dedicated L0 scenario token.

| Gate | Result |
|------|--------|
| `run_editor_binary_file_refused.sh` | `[mlc-editor] binary_file_refused ok` EXIT=0 |
| `run_editor_utf8_bom_detected.sh` | `[mlc-editor] utf8_bom_detected ok` EXIT=0 |
| `run_editor_save_unit.sh` | `[mlc-editor] save_unit ok` EXIT=0 |
| `run_editor_crlf_preserved_on_save.sh` | `[mlc-editor] crlf_preserved_on_save ok` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#14 EDITOR_REPLACE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `ba28446d` | Decision freeze + open |
| 1 | `3359c05e` | L0 red harnesses + `had_bom` stub |
| 2 | `e141ac97` | BOM/NUL/UTF-8 guard + save BOM |
| 3 | this Critic | close + archive |

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
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Non-UTF-8 encodings (Latin-1/UTF-16 open); status-bar encoding label wire; charset conversion UI; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
