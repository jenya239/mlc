# Track: Editor Indent Model

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#26**.
No indent policy: Tab/Shift-Tab do not indent/outdent; no tabs-vs-spaces or
width config. Auto-indent (#10) only copies leading bytes on Enter. Review
gates: `indent_width_config` (L0) + L1 Tab/Shift-Tab. Size **M**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran L0 + L1 + enter_keeps_indent + demo compile.
Anti-false-done: `be225699`…`bd513f48` (STEP=0–3). Wire present:
`EditorIndentConfig` + `edit_indent_tab`/`edit_outdent_lines`;
`editor_ux_*` / `editor_app_*`; demo_live Tab/Shift+Tab when ctrl up
(Ctrl+Tab cycle unchanged). No `lib/mlc/` → REG skipped.
**reopen: none**.

Honest residual: soft-tab display still 1 col; no per-file detect/convert/UI;
L1 covers spaces insert/outdent + multiline (not tabs-mode live path);
settings panel OOS.

| Gate | Result |
|------|--------|
| `run_ux_indent_width_config.sh` | `ux_ok indent_width_config` EXIT=0 |
| `run_ux_tab_shift_tab_indents.sh` | `ux_ok tab_shift_tab_indents` EXIT=0 |
| `run_ux_enter_keeps_indent.sh` | `ux_ok enter_keeps_indent` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#27 EDITOR_DROP_FILE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `be225699` | Decision freeze + open |
| 1 | `777f1d89` | L0 red harness + stub |
| 2 | `f016ae90` | config + indent/outdent API |
| 3 | `bd513f48` | demo_live Tab/Shift+Tab wire |
| 4 | this Critic | close + archive |

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
| Scenarios | L0: `indent_width_config` — default width=2 spaces unit `"  "`; set width=4 → `"    "`; set use_tabs → unit `"\t"`; token `ux_ok indent_width_config`. L1: `tab_shift_tab_indents` — spaces mode: Tab on `"foo"` caret0 → `"  foo"`; Shift+Tab undoes; multi-line selection prepends/outdents; token `ux_ok tab_shift_tab_indents` |
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
| 2 | indent config + indent/outdent edit API | **done** (2026-07-19) — real config + `edit_indent_tab`/`edit_outdent_lines`; L0+L1 green |
| 3 | demo_live Tab/Shift+Tab wire (ctrl up) | **done** (2026-07-19) — ux/app wrappers + demo_live Tab/Shift+Tab (ctrl up); Ctrl+Tab unchanged |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

Soft-tab column display in caret/nav/draw; per-file indent detect; convert tabs↔spaces on save; settings UI; smart language indent; `compiler/`; changing Ctrl+Tab tab-cycle.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
