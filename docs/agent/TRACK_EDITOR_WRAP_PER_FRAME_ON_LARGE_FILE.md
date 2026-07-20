# Track: Editor wrap per-frame on large file

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#36**
(pulled forward 2026-07-20 — measured frame-latency regression). Size **L**.

## Status: **active** (2026-07-20) — STEP=2 done → Driver STEP=3

## Next step

**STEP=3** — wire `wrap_count_cache_*` into both `demo_live` per-frame sites; `demo_live_fs_compile_ok`.

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
| 3 | `demo_live` both sites use cache; compile | open |
| 4 | Critic: gates; archive | open |

## Out of scope

Viewport-only wrap; open-path I/O profile; `compiler/`; `lib/mlc/`; §47 concurrency.
