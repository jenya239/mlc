# Track: Editor Wide Char Columns

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#28**.
After §40, `TextPosition.column` is a UTF-8 **codepoint** index (1 per
codepoint). Fullwidth CJK and emoji need **wcwidth-style** display width
(typically 2). Review gate: `wide_char_column_width` (L0). Size **M**.

## Status: **active** (2026-07-19) — STEP=1 done; next STEP=2

## Next step

**STEP=2** — display-width helpers + line_index converters

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Codepoint columns treat CJK/emoji as width 1 → caret/hit-test/draw misaligned vs monospace cell grid |
| Column meaning | `TextPosition.column` becomes **display columns** (wcwidth-lite sum), not codepoint index. Byte↔position converters in `line_index` switch to display columns; selection stays byte offsets |
| Width rules | Per decoded codepoint: (1) ASCII printable / most BMP → **1**; (2) East-Asian Wide / Fullwidth ranges + common emoji blocks → **2**; (3) combining marks / zero-width → **0**; (4) C0 controls / DEL / NUL → **0** (or skip). No ICU; no Unicode EastAsianWidth database dump — compact range table in MLC |
| Soft tab `\t` | Still **1** display column this track (honest residual vs #26 indent width); do not soft-expand tabs here |
| Grapheme / ZWJ | **Out of scope** — ZWJ emoji sequences may sum multiple width-2 codepoints (overcount); no grapheme cluster join |
| API | `misc/editor/document/line_index.mlc` (or thin `utf8_width.mlc`): `utf8_codepoint_display_width`, `utf8_count_display_columns`, `utf8_byte_offset_for_display_column`; `line_index_offset_to_position` / `line_index_position_to_offset` use display columns |
| Call sites | Prefer automatic via line_index; audit navigation hit-test / wrap / caret draw if they bypass converters with raw codepoint counts |
| Scenarios | L0: `wide_char_column_width` — `"ab"` → 2; `"中"` → 2; `"😀"` (U+1F600) → 2; `"a中b"` → 4; token `ux_ok wide_char_column_width` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` document (+ scenario/script); thin demo only if draw/hit bypasses line_index. No `compiler/`. No `lib/mlc/` unless forced |
| REG | only if `lib/mlc/` touched |

### Gates

| Step | Token |
|------|-------|
| 1 | L0 `wide_char_column_width` red → later green |
| 2–3 | `ux_ok wide_char_column_width` (+ run script); existing `line_index` / nav units if touched |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_drop_file_opens_tab.sh` → `ux_ok drop_file_opens_tab` |
| Critic | REG only if `lib/mlc/` changed |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L0 scenario first (`wide_char_column_width`) + stub | **done** (2026-07-19) — stub red: `ux_fail wide_char_column_width cjk` |
| <!-- sub-steps: 1) scenario + run script; 2) stub width helpers → red; 3) drop_file + demo_live_fs_compile green --> |
| 2 | display-width helpers + line_index converters | pending |
| <!-- sub-steps: 1) width table + count/offset helpers; 2) switch offset↔position; 3) L0 green --> |
| 3 | audit call sites / units; demo_live if needed | pending |
| <!-- sub-steps: 1) fix bypasses of codepoint-count; 2) line_index/nav unit green; 3) demo_live_fs_compile --> |
| 4 | Critic: gates; archive | pending |

## Out of scope

Grapheme clusters; emoji ZWJ/skin-tone join; ICU/HarfBuzz shaping columns; soft-tab display expansion; RTL/bidi; IME; `compiler/`; changing selection byte model.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
