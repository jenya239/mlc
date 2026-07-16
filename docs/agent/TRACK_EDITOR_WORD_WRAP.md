# Track: Editor soft word-wrap (layout)

Parent: [../PLAN.md](../PLAN.md) §42; residual «no wrap v1» in
[../EDITOR.md](../EDITOR.md) / `misc/editor/ARCHITECTURE.md` /
`layout/visible_range.mlc`. Uncommitted WIP:
`misc/editor/layout/word_wrap.mlc` + `demo_live` imports (absorb, do not
discard).

## Status: **active** (2026-07-16) — queue head

After §41 caret blink closed. Next editor gap: soft wrap by **codepoint**
columns (prefer break at space); wire scroll/hit-test/live.

## Next step

**STEP=0** — Decision freeze: break rules, API surface, absorb WIP path.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| Scope v1 | Soft wrap → `[VisualRow]` per doc line; mono `char_width` columns |
| Columns | UTF-8 **codepoints** (reuse §40 helpers); grapheme out |
| Break | Prefer last space before `max_columns`; else hard cut at max |
| Module | Absorb `misc/editor/layout/word_wrap.mlc` (WIP) |
| API draft | `wrap_max_columns`, `visual_rows_for_line`, `visual_rows_for_document` (or equiv), `wrap_offset_at_point` |
| Wire | `demo_live` + scroll/hit already partially in WIP — finish + unit |
| REG | Prefer no `lib/mlc/`; no `compiler/` |
| Non-goals | Hyphenation; proportional fonts; IME; tree-sitter; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Absorb/stabilize `word_wrap.mlc` + unit | unit token |
| 2 | Wire scroll/hit/`demo_live`; compile green | compile (+ spot unit) |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze break rules + exact exports from WIP audit.
2. Note which WIP files are in-scope (do not commit unrelated foreign runtime/IO).

**STEP=1**
1. Commit `word_wrap.mlc` (+ unit); leave unrelated dirty alone.
2. Gate: `scripts/run_editor_word_wrap_unit.sh` (name TBD at STEP=0).

**STEP=2**
1. Finish `demo_live` / scroll integration from WIP as needed.
2. `run_editor_demo_live_fs_compile.sh` (stash foreign deps if needed).

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- Grapheme / IME / proportional metrics
- tree-sitter / SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes
- Unrelated foreign WIP (`runtime/io`, folder_panel extras) unless required for compile

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
- Do not `git add` foreign files outside TRACK scope.
