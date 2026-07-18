# Track: Editor undo coalesce (typing groups)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#6**.
`history_push_before_edit` runs on every `editor_app_insert_text` / demo
`text_buffer` path → each keystroke is its own undo entry. Review:
`typing_coalesces_into_one_undo` (L1.5).

## Status: **active** (2026-07-18) — STEP=1 done; STEP=2 next

## Next step

**STEP=2** — Implement kind+idle coalesce; wire app/demo; green scenario.

### STEP=1 done (2026-07-18)

- Stub `history_push_before_edit_kind` (always push); L1.5 red harness
  `typing_coalesces_into_one_undo` (+ run script)

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #6 → active.

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
| 2 | Coalesce in history + app/demo wire | scenario + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2**
1. Implement kind+idle coalesce; wire insert/backspace; break on other edits.
2. Gates: scenario ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #7).

## Out of scope

Find (#7); drag-autoscroll (#9); multi-cursor (#23); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
