# Track: Editor Syntax Highlight MLC Richer

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#34**.
Found 2026-07-19 live-demo audit. Size **M**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran richer L2 + glyph L2 + demo. Anti-false-done:
`8ada9c87`…`24cf8f39` (STEP=0–3); no `compiler/` / `lib/mlc/` → REG skip.
Wire: `highlighter` number/type/operator (+ extra keywords); `Theme` RGB +
`theme_rgb_for_tag`; draw path already tag-driven (no demo re-slice).
**reopen: none**.

Honest residual: non-`.mlc` buffers still flat (Decision); no hex/float exponents;
no semantic/scope coloring.

| Gate | Result |
|------|--------|
| `run_ux_syntax_mlc_richer_tags.sh` | `ux_ok syntax_mlc_richer_tags` EXIT=0 |
| `run_ux_syntax_glyph_color.sh` | `ux_ok syntax_glyph_color_matches_theme` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#35 EDITOR_MINIMAP`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8ada9c87` | Decision freeze + open |
| 1 | `722ae431` | Red L2 stub + run scripts |
| 2 | `e2a9733f` | number/type/operator + Theme RGB; green L2 |
| 3 | `24cf8f39` | glyph L2 + `demo_live_fs_compile_ok` |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | `syntax/highlighter.mlc` tags only `keyword` / `string` / `comment`. `.mlc` buffers lack number / builtin-type / operator coloring → mostly monochrome between keywords. Theme `theme_rgb_for_tag` only knows those three tags |
| Scope tags (MLC) | Add three tags: (1) `number` — decimal digit run (optional leading `-` only if preceded by non-ident; no float exponent this track; no hex); (2) `type` — fixed builtin-name set: `i32` `i64` `u32` `u64` `f32` `f64` `bool` `string` `unit` `Shared` `Owned` `Weak` `Array` `Map` `Result` `Option` `Channel` `Mutex`; (3) `operator` — multi-char first: `->` `=>` `==` `!=` `<=` `>=` `&&` `\|\|`, then single `+ - * / % = < > : ? \| . !` when not start of `//`. Scan after string/comment/ident branches |
| Keywords | Keep existing set; optionally add `extend` `trait` `impl` `for` `in` `break` `continue` if cheap in same STEP — prefer yes (still `keyword` tag) |
| Theme | Add `Theme` RGB fields `number_*` / `type_*` / `operator_*` + Solarized Light (+ dark stub) values; extend `theme_rgb_for_tag`. Map roughly: number≈magenta, type≈yellow/orange, operator≈base00 — concrete floats frozen in STEP=2 from Solarized table, not invented mid-flight |
| Non-`.mlc` | **Limitation stays** this track: no generic fallback lexer for `.cpp`/`.md`/… — still flat default text. Document in TRACK Out of scope / SESSION residual |
| Token | `ux_ok syntax_mlc_richer_tags` — L2: `highlight_mlc` on a fixed snippet yields ≥1 span each of `number`/`type`/`operator`; `theme_rgb_for_tag` / glyph helper RGB for those tags match theme fields (ε 0.002). Keep `ux_ok syntax_glyph_color_matches_theme` green |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `misc/editor/syntax/highlighter.mlc`, `ui/theme.mlc`, scenario + scripts; `demo_live` only if compile needs import (draw path already uses tags). No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | tree-sitter; non-MLC languages; semantic (scope-aware) coloring; hex/float exponents; `#35` minimap |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L2 red harness + run scripts | **done** — `ux_ok syntax_mlc_richer_red` |
| 2 | Tags + theme RGB + green token | **done** — `ux_ok syntax_mlc_richer_tags` |
| 3 | Keep glyph L2; `demo_live_fs_compile_ok` | **done** |
| 4 | Critic: gates; archive | **done** (closed) |

<!-- STEP=1: stub fail token; red script -->
<!-- STEP=2: highlighter + Theme fields + theme_rgb_for_tag; green L2 -->
<!-- STEP=3: regression glyph L2 + compile -->

## Out of scope

Non-MLC lexers; tree-sitter; `#35`; `compiler/`; `lib/mlc/`.
