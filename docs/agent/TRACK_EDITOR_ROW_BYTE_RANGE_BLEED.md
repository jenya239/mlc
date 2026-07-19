# Track: Editor Row Byte-Range Bleed

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#33b**
(pulled forward — correctness bug after live-demo audit 2026-07-19).
**Not** PLAN §33b `EDITOR_LIVE_POLISH` (closed). Size **M**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — Red L2 harness + run script for `row_byte_range_matches_line`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `cc4cb1e9` | Decision freeze + open |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Live content rows show a constant ~5-byte chop from the line start and append ~2 bytes from the **next** document line (e.g. `"export type…"` → `"t type…" + "lo"`). Offset is length-independent — byte-range bug, not wrap/theme |
| Symptom freeze | For a known multi-line ASCII buffer, each drawn `VisualRow` for doc line N must satisfy: `text.byte_substring(row.byte_start, row.byte_end - row.byte_start)` equals the intended slice of line N only — **no** bytes with index ≥ `line_index_line_start(N+1)` (or `text_length` on last line). Unwrapped single-row line: equals `line_text_at` (no trailing `\n`) |
| Root cause | **Not frozen** — Driver must locate before patching. Suspects (ordered): (1) `visual_rows_for_line` / `line_end_byte` in `layout/word_wrap.mlc`; (2) `utf8_byte_offset_for_display_column` / display-column walk in `document/line_index.mlc` (esp. wrap continue + `#31a`/`#28` interaction); (3) `editor_ux_append_syntax_colored_row` span clipping; (4) `demo_live` misuse of `row.byte_start`/`byte_end`. Do **not** guess-fix |
| Fix rule | One root cause; minimal patch in the owning module. Prefer pure layout/index helpers so all callers benefit. `demo_live` only if it re-slices incorrectly |
| Token | `ux_ok row_byte_range_matches_line` — L2: build fixed buffer + `LineIndex`; `collect_visible_visual_rows` (or `visual_rows_for_line` per line); assert each row's substring is wholly within that line's `[line_start, line_end_byte)` and matches expected content for that visual segment |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `misc/editor/layout/word_wrap.mlc` and/or `document/line_index.mlc` and/or `ux/syntax_glyph_color.mlc` and/or `demo_live`; scenario + `scripts/run_ux_row_byte_range_matches_line.sh`. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | `#33c` chrome theme; `#33d` parent double-click; richer syntax tags (`#34`); minimap; proportional layout |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L2 red harness + `run_ux_row_byte_range_matches_line.sh` | pending — `ux_ok row_byte_range_red` |
| 2 | Trace root + fix; green L2 token | pending — `ux_ok row_byte_range_matches_line` |
| 3 | `demo_live` wire if needed; `demo_live_fs_compile_ok` | pending |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1: stub scenario that fails on current bleed; run script -->
<!-- STEP=2: locate byte_start/end bug; green assert -->
<!-- STEP=3: only if demo path still wrong after helper fix; always compile gate -->

## Out of scope

Theme drift; tree parent click; `compiler/`; `lib/mlc/`; syntax highlight expansion.
