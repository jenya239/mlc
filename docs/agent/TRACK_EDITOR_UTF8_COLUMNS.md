# Track: Editor UTF-8 codepoint columns (line_index)

Parent: [../PLAN.md](../PLAN.md) §40; gap from [../EDITOR.md](../EDITOR.md)
(«better UTF-8 grapheme/column rules»); residual note in
`misc/editor/document/line_index.mlc` («codepoint columns deferred»).

## Status: **active** (2026-07-16) — queue head

After §39 font config. Next editor gap: columns as UTF-8 **codepoints**
(not bytes). Full grapheme clusters (ZWJ/emoji) deferred — no ICU.

## Next step

**STEP=0** — Decision freeze: codepoint vs grapheme; API touch points.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| Scope v1 | UTF-8 **codepoint** columns in `line_index` (+ helpers) |
| Grapheme | **out** — document as residual; no ICU/Fontconfig |
| API | `line_index` byte↔position uses codepoint column; keep type `TextPosition.column` |
| Wire | navigation/hit-test that assume byte==column — audit + fix if broken |
| REG | Prefer no `lib/mlc/`; if touched → REG before Critic |
| Non-goals | tree-sitter; SCRIPT_VM; MIR Epic 5; `compiler/`; full grapheme |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Codepoint helpers + `line_index` unit | unit token |
| 2 | Wire callers / fix regressions | unit or compile gate |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze: codepoint-only v1; name helpers.
2. List `line_index` functions that must change.

**STEP=1**
1. Implement UTF-8 codepoint advance in `document/line_index.mlc` (or sibling).
2. Unit: multi-byte chars → column ≠ byte offset.

**STEP=2**
1. Fix navigation/hit if they assume byte columns.
2. Existing editor units still green.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- Full grapheme clusters / emoji ZWJ
- tree-sitter
- SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
