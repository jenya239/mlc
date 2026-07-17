# Track: Editor large-file — no repeated full stringify

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#1d**
(pulled forward from #22 — frame-latency / Sublime-parity).

## Status: **active** (2026-07-18) — STEP=1 done; STEP=2 next

## Next step

**STEP=2** — `document_frame_snapshot` one flatten + wire `demo_live`;
green `large_file_no_full_stringify` + `demo_live_fs_compile_ok`.

### STEP=1 done (2026-07-18)

- Stub `document/frame_snapshot.mlc` (`flatten_count: 3`, 3× stringify)
- L2 `ux_scenarios/large_file_no_full_stringify.mlc` + run script → **red**
  (`ux_fail expected flatten_count=1 got 3`)

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
| 2 | `document_frame_snapshot` + wire `demo_live` ≤1 flatten/frame | scenario + compile |
| 3 | Critic: gates; grep hot path; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2**
1. Implement snapshot API; rebuild line_index from snapshot text.
2. Wire `demo_live` early/pre/draw to one snapshot.
3. Gates: scenario ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #2).

## Out of scope

Zero-flatten piece ranges; #2 keyboard nav; tree-sitter; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario token in SESSION `done`.
