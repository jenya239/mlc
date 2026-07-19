# Track: Editor Indent Model

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#26**.
No indent policy: Tab/Shift-Tab do not indent/outdent; no tabs-vs-spaces or
width config. Auto-indent (#10) only copies leading bytes on Enter. Review
gates: `indent_width_config` (L0) + L1 Tab/Shift-Tab. Size **M**.

## Status: **active** (2026-07-19) — STEP=3 done; next Critic

## Next step

**STEP=4** — Critic: gates; archive

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | No indent model: cannot configure tabs vs spaces / width; Tab does not insert indent; Shift+Tab does not outdent |
| Config | `EditorIndentConfig { use_tabs: i32, indent_width: i32 }` — `use_tabs==0` → spaces; `!=0` → hard tabs. Default: **spaces**, `indent_width=2` |
| Unit | Spaces mode: string of `indent_width` ASCII spaces. Tabs mode: single `"\t"` (width still used for outdent space-budget and future soft-tab display) |
| Clamp | `indent_width` clamped to **1…8** on set; invalid → clamp |
| Tab (collapsed) | Insert one indent **unit** at caret (replace empty selection) |
| Tab (range) | Indent every line intersecting selection: prepend unit at line start |
| Shift+Tab | Outdent every intersecting line: if line starts with `"\t"` → remove one tab; else remove up to `indent_width` leading spaces (fewer if short). Empty indent → no-op for that line |
| Ctrl+Tab | **Unchanged** — still `CmdNextTab` / `CmdPrevTab` (command bus). Indent only when **ctrl is up** |
| Soft-tab display | **Out of scope** this track — caret/nav/draw may still count `\t` as 1 column (honest residual vs #24). Config width is for insert/outdent policy |
| Detect/convert | No per-file detect; no convert-on-save; no UI settings panel |
| API | `misc/editor/document/indent.mlc` (or `ux/indent.mlc`): `editor_indent_config_default`, `editor_indent_config_set_width`, `editor_indent_unit`, `edit_indent_lines` / `edit_outdent_lines` (or ux wrappers). Wire `demo_live`: binding `"tab"` + shift, ctrl==0 → indent/outdent |
| Scenarios | L0: `indent_width_config` — default width=2 spaces unit `"  "`; set width=4 → `"    "`; set use_tabs → unit `"\t"`; token `[mlc-editor] indent_width_config ok` **or** `ux_ok indent_width_config` (pick one style; prefer unit script like CRLF if L0-under-tests, else ux_scenarios). L1: `tab_shift_tab_indents` — spaces mode: Tab on `"foo"` caret0 → `"  foo"`; Shift+Tab undoes; multi-line selection prepends/outdents; token `ux_ok tab_shift_tab_indents` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` document/ux + scenario/script + thin demo_live. No `compiler/` / no `lib/mlc/` unless Tab ABI gap (already have `GLFW_KEY_TAB` → `"tab"`) |
| REG | only if `lib/mlc/` touched |

### Gates

| Step | Token |
|------|-------|
| 1 | L0 `indent_width_config` red → later green |
| 2–3 | `ux_ok tab_shift_tab_indents` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_enter_keeps_indent.sh` → `ux_ok enter_keeps_indent` |
| Critic | REG only if `lib/mlc/` changed |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L0 scenario first (`indent_width_config`) + stub | **done** (2026-07-19) — stub red: `ux_fail indent_width_config default_width` |
| <!-- sub-steps: 1) L0 harness + run script; 2) stub config API → red; 3) enter_keeps_indent + demo_live_fs_compile green --> |
| 2 | indent config + indent/outdent edit API | **done** (2026-07-19) — real config + `edit_indent_tab`/`edit_outdent_lines`; L0+L1 green |
| <!-- sub-steps: 1) EditorIndentConfig + unit/indent/outdent; 2) L0 green + L1 green; 3) enter_keeps_indent regression --> |
| 3 | demo_live Tab/Shift+Tab wire (ctrl up) | pending |
| <!-- sub-steps: 1) binding tab without ctrl; 2) demo_live_fs_compile; 3) gates --> |
| 4 | Critic: gates; archive | pending |

## Out of scope

Soft-tab column display in caret/nav/draw; per-file indent detect; convert tabs↔spaces on save; settings UI; smart language indent; `compiler/`; changing Ctrl+Tab tab-cycle.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
