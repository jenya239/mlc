# Track: Editor wrap per-frame on large file

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#36**
(pulled forward 2026-07-20 — measured frame-latency regression). Size **L**.

## Status: **closed** (2026-07-20) — Critic OK

**Critic 2026-07-20 (STEP=4):** Re-ran stable + `demo_live` compile. Anti-false-done:
`ebde8886`…`9cdaed0a` (STEP=0–3); `misc/editor/**` only → REG skip; no `compiler/`/`lib/mlc/`.
Wire: `layout/wrap_cache.mlc` + both `demo_live` per-frame sites use `wrap_count_cache_tick`
(no bare `document_visual_row_count` in the frame loop). L2 idle ticks keep
`recompute_count`; text/`wrap_columns` invalidate. **reopen: none**.

Honest residual: v1 still full-document recompute on edit/resize (not viewport-only —
Decision out of scope). Cache keys by full `text` string equality. Historical
`run_ux_wrap_count_cache_red.sh` expects stub-red (EXIT≠0) and fails post-green —
use `run_ux_wrap_count_cache_stable.sh` only.

| Gate | Result |
|------|--------|
| `run_ux_wrap_count_cache_stable.sh` | `ux_ok wrap_count_cache_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| REG / self-host | N/A (editor-only) |

## Next step

**closed** — Critic OK. Queue → Driver §47 `#3 CONCURRENCY_MOVE_TRACKING` STEP=1.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `ebde8886` | Decision: cache by (text, wrap_columns); not viewport-only v1 |
| 1 | `e4aef328` | Red L2 stub + red/stable scripts |
| 2 | `542f1341` | `wrap_cache.mlc` + green `wrap_count_cache_stable` |
| 3 | `9cdaed0a` | `demo_live` both sites → tick cache |
| 4 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | `document_visual_row_count` full-document walk every frame in `demo_live` (~848 / ~1623) |
| Strategy (v1) | Cache keyed by `(text, wrap_columns)` via `layout/wrap_cache.mlc`; idle ticks reuse |
| Viewport-only wrap | Out of scope v1 |
| Token / gate | `ux_ok wrap_count_cache_stable`; STEP=3 `demo_live_fs_compile_ok` |
| Module touch | `misc/editor/**` only |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red harness stub | **done** — `ux_ok wrap_count_cache_red` |
| 2 | Wrap-count cache + green token | **done** — `ux_ok wrap_count_cache_stable` |
| 3 | `demo_live` both sites use cache; compile | **done** — `demo_live_fs_compile_ok` |
| 4 | Critic: gates; archive | **done** |

## Out of scope

Viewport-only wrap; open-path I/O profile; `compiler/`; `lib/mlc/`; §47 concurrency.
