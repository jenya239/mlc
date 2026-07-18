# Track: Editor auto-indent on Enter

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#10**.
Enter inserts bare `\n`; caret lands at column 0 — leading indent of the
current line is not preserved. Review gate: `enter_keeps_indent` (L1). Size **S**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1 + newline regression + compile.
Anti-false-done: `b711fff7`…`20cb92d5` (STEP=0–2). Wire present:
`leading_indent_of_line` + `edit_insert_newline` (`"\n"+indent`);
`editor_ux_insert_newline` → edit path (not bare `"\n"` insert);
app/demo_live inherit. **reopen: none**.

Honest residual: no smart/language indent; no Tab/Shift-Tab (#26); spaces/tabs
preserved as bytes only; L1 exercises ux API not GLFW Enter inject.

| Gate | Result |
|------|--------|
| `run_ux_enter_keeps_indent.sh` | `ux_ok enter_keeps_indent` EXIT=0 |
| `run_ux_newline_caret.sh` | `ux_ok newline_caret_and_spaces` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#11 EDITOR_CURRENT_LINE_HL`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `b711fff7` | Decision freeze + open |
| 1 | `5c9f95ff` | L1 red harness |
| 2 | `20cb92d5` | edit_insert_newline + edit_apply wire |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | `edit_insert_newline` / Enter inserts only `"\n"`; new line has no leading indent |
| Indent source | **Leading whitespace of the line that holds the caret** (before insert): run of ASCII space (`' '`) and tab (`'\t'`) from line start. Stop at first non-ws or EOL. Empty line → empty indent |
| Apply | Insert `"\n" + indent`; caret after indent on the new line. Non-empty selection: same as insert path (replace selection, then newline+indent from the line of the **start** of the range after collapse — prefer: delete selection first via existing insert semantics, indent from line of caret after delete / of selection start) |
| Tabs vs spaces | **Preserve bytes as-is** (spaces stay spaces, tabs stay tabs). No expand/convert. Width config → `#26 EDITOR_INDENT_MODEL` |
| Smart indent | **Out of scope** (no language-aware indent after `{`, no dedent on `}`) |
| API | Extend `edit_insert_newline` (document layer) so `editor_ux_insert_newline` / `editor_app_insert_newline` / `demo_live` Enter path inherit automatically |
| Scenarios | L1: `enter_keeps_indent` — doc with indented line (e.g. `"  foo"`); caret at EOL; Enter; assert text `"  foo\n  "` and caret column = indent width |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` `document/edit` (+ ux/app inherit; scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok enter_keeps_indent` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1 scenario first (`enter_keeps_indent`) | **done** (red: `ux_fail enter indent text`) |
| 2 | newline+indent in `edit_insert_newline` (+ inherit wire) | **done** (`ux_ok`; `demo_live_fs_compile_ok`) |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Smart/language indent; Tab/Shift-Tab / indent width (#26); CRLF (#12); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
