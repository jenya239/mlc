# Track: Editor wrap per-frame on large file

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#36**
(pulled forward 2026-07-20 — measured frame-latency regression). Size **L**.

## Status: **active** (2026-07-20) — STEP=1 done → Driver STEP=2

## Next step

**STEP=2** — wrap-count cache API + green `ux_ok wrap_count_cache_stable` (idle/scroll 0 recomputes; edit +1).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | `document_visual_row_count(draw_text, line_index, wrap_columns)` walks **every** document line. Measured 2026-07-20: ~230–260ms (1MB/8k lines), ~2.3–2.8s (10MB/79k lines) per call. Live path calls it **unconditionally twice per frame** in `demo_live.mlc` (~848 and ~1623) inside `while glfw_gl_context_should_close()`, including idle and pure scroll — same shape as pre-`#1d` full stringify, separate from `document_frame_snapshot` |
| Strategy (v1) | **Cache full-document visual row count** keyed by `(document identity / snapshot text fingerprint, wrap_columns)`. Recompute only when the key changes (edit that resnapshots, tab switch, wrap_columns change from resize/font zoom). Pure scroll / idle frames **must not** call full `document_visual_row_count`. Same discipline as `#1d` `document_frame_snapshot` — not a new viewport-only wrap engine this track |
| Viewport-only wrap | **Out of scope for v1** (honest residual). Full Sublime-class O(viewport) wrap layout may follow later if cache+edit cost still hurts typing on 10MB; gate for this track is call-count / “0 full recomputes on idle/scroll”, not wall-clock FPS in CI |
| Cache home | Prefer thin helper next to wrap/snapshot (`layout/word_wrap.mlc` or small `layout/wrap_cache.mlc` / field on app state) + `demo_live` uses cached count at both call sites. Do not leave a third uncached call site |
| Invalidate | On any path that already does `document_frame_snapshot` after mutation; when `wrap_max_columns(...)` result differs from cached `wrap_columns`; on buffer/tab identity change |
| Token / gate | `ux_ok wrap_count_cache_stable` — L2: after warm, N idle ticks → recompute_count unchanged; one text/wrap_columns change → +1. Plus `demo_live_fs_compile_ok` at STEP=3 |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `misc/editor/**` only (layout + demo_live + ux_scenarios/tests). No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Viewport-only wrap engine; fixing `open_buffer_from_path` 10MB open latency (secondary finding); minimap; concurrency §47; `compiler/`; `lib/mlc/` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red harness: per-frame full wrap on idle (expect fail today) | **done** — `ux_ok wrap_count_cache_red` |
| 2 | Wrap-count cache + invalidate; green token | open |
| 3 | `demo_live` both sites use cache; `demo_live_fs_compile_ok` | open |
| 4 | Critic: gates; archive | open |

## Out of scope

Viewport-only wrap; open-path I/O profile; `compiler/`; `lib/mlc/`; §47 concurrency.
