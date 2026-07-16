# Track: Editor UTF-8 codepoint columns (line_index)

Parent: [../PLAN.md](../PLAN.md) §40; gap from [../EDITOR.md](../EDITOR.md)
(«better UTF-8 grapheme/column rules»); residual note in
`misc/editor/document/line_index.mlc` («codepoint columns deferred»).

## Status: **active** (2026-07-16) — queue head

## Next step

**STEP=3** — Critic: gates (+ REG if `lib/mlc/`); archive.

### STEP=2 done (2026-07-16)

- `LineIndex.text` stored; call sites pass `line_index.text`.
- Gates: line_index / selection / navigation / status_bar units ok; demo_live compile.

### STEP=1 done (2026-07-16)

- Helpers + converters with `text:`; unit covers ASCII + `café`.
- Gate: `bash scripts/run_editor_line_index_unit.sh` → `line_index_unit ok`.
- Call sites still on old arity → STEP=2.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §40 → STEP=1.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in `line_index.mlc` comment «codepoint columns deferred» and existing
byte-column math in `line_index_offset_to_position` /
`line_index_position_to_offset`.

| Item | Choice |
|------|--------|
| Scope v1 | UTF-8 **codepoint** columns (1 scalar value = 1 column) |
| Grapheme | **out** — residual; no ICU / combining-cluster merge |
| Invalid UTF-8 | Treat each bad byte as one column (advance 1); do not panic |
| API keep | `TextPosition.column` meaning becomes **codepoint** within line |
| Helpers (private or export) | `utf8_codepoint_byte_length(lead: i32) -> i32`; `utf8_count_codepoints(text, start, end_exclusive) -> i32`; `utf8_byte_offset_for_codepoint_column(text, start, end_exclusive, column) -> i32` |
| Must change | `line_index_offset_to_position`, `line_index_position_to_offset` (need line text slice — pass `text: string` **or** store nothing extra: rebuild callers pass text) |
| Text source | Both converters take additional `text: string` matching `line_index` **OR** keep signature and require `LineIndex` + external text via new overloads — **freeze: add `text: string` parameter** to the two converters |
| Call-site audit (STEP=2) | All `line_index_offset_to_position` / `position_to_offset` call sites |
| Gate | Extend `scripts/run_editor_line_index_unit.sh` / `line_index_unit.mlc` |
| REG | Prefer **no** `lib/mlc/`; no `compiler/` |

### Exact signatures (frozen)

```text
# helpers (in line_index.mlc or utf8_columns.mlc)
utf8_codepoint_byte_length(lead_byte: i32) -> i32
utf8_count_codepoints(text: string, start: i32, end_exclusive: i32) -> i32
utf8_byte_offset_for_codepoint_column(
  text: string, start: i32, end_exclusive: i32, column: i32
) -> i32

# converters — ADD text parameter
line_index_offset_to_position(
  line_index: LineIndex, text: string, byte_offset: i32
) -> TextPosition

line_index_position_to_offset(
  line_index: LineIndex, text: string, position: TextPosition
) -> i32
```

`lead_byte` / `byte_at` values: compare as integer code units (0–255), same as
existing `byte_at` usage elsewhere.

### Non-goals (Decision)

Grapheme clusters; emoji ZWJ; tree-sitter; SCRIPT_VM; LANG_AUTO_CYCLE; MIR Epic 5;
`compiler/` changes; promoting helpers to stdlib in this track.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Helpers + converter impl + unit (ASCII + multi-byte) | **done** (2026-07-16) `line_index_unit ok` |
| 2 | Update all call sites; other editor units green | **done** (2026-07-16) |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze codepoint-only + signatures — done.
2. List functions that must change — done.

**STEP=1** — **done**
1. Helpers + converters — done (`string_byte_u8` lead).
2. Extended `line_index_unit.mlc` — done.

**STEP=2** — **done**
1. Call sites + `LineIndex.text` — done.
2. Units + demo_live compile — done.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- Full grapheme clusters / emoji ZWJ
- tree-sitter
- SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
