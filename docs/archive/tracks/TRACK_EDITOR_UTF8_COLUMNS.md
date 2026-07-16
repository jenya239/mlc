# Track: Editor UTF-8 codepoint columns (line_index)

Parent: [../../PLAN.md](../../PLAN.md) §40; gap from [../../EDITOR.md](../../EDITOR.md)
(«better UTF-8 grapheme/column rules»).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=3):** Re-ran line_index / selection / navigation /
status_bar units + demo_live compile. No `lib/mlc/` → REG skipped (Decision).
Anti-false-done: `83fd96f1`…`bd92d925` (STEP=0–2). **reopen: none**.

| Gate | Result |
|------|--------|
| `run_editor_line_index_unit.sh` | `line_index_unit ok` |
| `run_editor_selection_unit.sh` | `selection_unit ok` |
| `run_editor_navigation_unit.sh` | `navigation_unit ok` |
| `run_editor_status_bar_unit.sh` | `status_bar_unit ok` |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=3 done (2026-07-16)

- Critic gates; archive.

### STEPs 0–2 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `83fd96f1` | Decision frozen |
| 1 | `a412134c` | `line_index_unit ok` |
| 2 | `bd92d925` | call sites + units + demo_live |

## Decision (frozen) — summary

UTF-8 **codepoint** columns in `line_index`; converters take `text:`; helpers
`utf8_codepoint_byte_length` / `utf8_count_codepoints` /
`utf8_byte_offset_for_codepoint_column`; grapheme/ICU out; no `compiler/` /
no `lib/mlc/`.

## Out of scope (unchanged)

Grapheme clusters; emoji ZWJ; tree-sitter; SCRIPT_VM; MIR Epic 5; `compiler/`.
