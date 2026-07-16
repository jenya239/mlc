# Track: Editor soft word-wrap (layout)

Parent: [../../PLAN.md](../../PLAN.md) §42; residual «no wrap v1» in
[../../EDITOR.md](../../EDITOR.md) / `misc/editor/ARCHITECTURE.md`.

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=3):** Re-ran unit + demo_live compile on clean deps.
No `lib/mlc/` → REG skipped (Decision). Anti-false-done:
`a67cf4d1`…`65d77eec` (STEP=0–2). **reopen: none**.

| Gate | Result |
|------|--------|
| `run_editor_word_wrap_unit.sh` | `word_wrap_unit ok` |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=3 done (2026-07-16)

- Critic gates; archive.

### STEPs 0–2 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `a67cf4d1` | Decision frozen |
| 1 | `8d7a2761` | `word_wrap_unit ok` |
| 2 | `65d77eec` | `demo_live_fs_compile_ok` |

## Decision (frozen) — summary

Soft wrap by UTF-8 codepoint columns; prefer last space before
`max_columns`; absorb `word_wrap.mlc`; `demo_live` scroll/hit/draw/caret;
no `compiler/` / no `lib/mlc/`.

## Out of scope (unchanged)

Hyphenation; proportional fonts; grapheme/IME; tree-sitter; SCRIPT_VM;
MIR Epic 5.
