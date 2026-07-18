# Track: Editor undo coalesce (typing groups)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#6**.
`history_push_before_edit` runs on every `editor_app_insert_text` / demo
`text_buffer` path → each keystroke is its own undo entry. Review:
`typing_coalesces_into_one_undo` (L1.5).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1.5 + compile. Anti-false-done:
`27a18f9f`…`b0d5dabf` (STEP=0–2). Wire present: `history_push_before_edit_kind`
same-kind + 1000ms idle; `editor_app_insert_text`/`backspace`/`newline` via
`frame_index*16`; word-delete clears coalesce. **reopen: none**.

Honest residual: scenario exercises history API directly (not full
`editor_app_insert_text`); optional idle/newline break asserts not in L1.5;
demo cut/paste keep bare `history_push_before_edit` (Decision break path).

| Gate | Result |
|------|--------|
| `run_ux_typing_coalesces_into_one_undo.sh` | `ux_ok typing_coalesces_into_one_undo` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#7 EDITOR_FIND`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `27a18f9f` | Decision freeze + open |
| 1 | `3f11d0f4` | L1.5 red harness |
| 2 | `b0d5dabf` | coalesce + app wire |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Every insert pushes a history snapshot; typing a word requires N undos |
| Coalesce rule | Same **edit kind** within **idle window** → skip push (continue current undo group). First edit of a group still pushes before-state |
| Kinds | `insert_text` coalesces with `insert_text`; `backspace` with `backspace`. Cross-kind always breaks. Newline, delete-word, cut, paste, undo/redo always break (push or clear coalesce) |
| Idle window | **1000 ms** (`coalesce_idle_ms`). Explicit `clock_ms` in API for headless; demo uses `frame_index * 16` (or equivalent monotonic ms) |
| API | Extend history: `history_push_before_edit_kind(history, document, selection, kind, clock_ms)` (+ kind constants). Non-coalescing call sites keep `history_push_before_edit` (= always push + clear coalesce) |
| Apply path | `editor_app_insert_text` / `editor_app_backspace` use kind+clock; `demo_live` char path already via `editor_app_insert_text` — pass clock. Cut/paste/newline/word-delete break |
| Scenarios | L1.5: `typing_coalesces_into_one_undo` — insert several chars with close clocks; assert undo stack depth 1; one undo restores pre-typing doc+selection. Optional assert: newline or idle>1000 breaks into second group |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` `document/history` + `app/state` (+ scenario/script; demo clock wire if needed). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok typing_coalesces_into_one_undo` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1.5 scenario first (`typing_coalesces_into_one_undo`) | **done** (red harness) |
| 2 | Coalesce in history + app/demo wire | **done** |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Find (#7); drag-autoscroll (#9); multi-cursor (#23); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
