# Track: Editor large-file — no repeated full stringify

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#1d**
(pulled forward from #22 — frame-latency / Sublime-parity).

## Status: **active** (2026-07-18) — STEP=2 done; STEP=3 Critic next

## Next step

**STEP=3** — Critic: gates; grep hot path ≤1 flatten pattern; archive.

### STEP=2 done (2026-07-18)

- `document_frame_snapshot` → one `document_to_string` + `line_index_from_string`
- `demo_live`: frame snapshot; refresh on edit; end resnapshot only if
  `!line_index_matches_document`; draw reuses `frame.text`
- Save-path stringify kept; gates green

### STEP=1 done (2026-07-18)

- Stub multi-flatten + red L2 harness

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #1d → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | `demo_live` calls `document_to_string` **3× per frame** (`draw_text_early` / `draw_text_pre` / `draw_text`), and `line_index_rebuild` stringifies again via `line_index_from_document` — O(n) flatten × ~4 on every frame; large-file latency regression |
| Goal (this track) | **≤1 full flatten per frame** on the live draw path: one snapshot `{ text, line_index }`, reuse for wrap / hit / highlight / glyph draw. Save/dump may still stringify |
| Non-goal | Piece-table range APIs that avoid any full flatten (defer); rewrite all unit tests |
| API | Add `document_frame_snapshot(doc) -> DocumentFrameSnapshot` (name flexible) that stringifies once and builds `LineIndex` from that string (`line_index_from_string`), not a second flatten |
| Wire | `demo_live` frame loop uses one snapshot; delete redundant `document_to_string` / redundant `line_index_rebuild` where the snapshot already provides both |
| Standing discipline | L2 `large_file_no_full_stringify` — assert snapshot / frame-prep path does not re-flatten (e.g. large fixture + single snapshot drives wrap/draw helpers; Critic greps demo_live hot path ≤1 call) |
| Module touch | Prefer `misc/editor/document/*` + `demo_live.mlc` (+ thin `ux/` helper if cleaner). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_scenarios/large_file_no_full_stringify.mlc` + run script → `ux_ok large_file_no_full_stringify` |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog row | **done** (2026-07-18) |
| 1 | L2 scenario first (`large_file_no_full_stringify`) | **done** (red harness) |
| 2 | `document_frame_snapshot` + wire `demo_live` ≤1 flatten/frame | **done** |
| 3 | Critic: gates; grep hot path; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2** — **done**

**STEP=3** — Critic; `next` = Planner (§46 #2).

## Out of scope

Zero-flatten piece ranges; #2 keyboard nav; tree-sitter; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario token in SESSION `done`.
