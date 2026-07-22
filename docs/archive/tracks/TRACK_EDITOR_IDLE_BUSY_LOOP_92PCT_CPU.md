# Track: Editor idle busy-loop 92% CPU

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#38**
(pulled forward 2026-07-22 — measured idle CPU / user "тормозит"). Size **L**.

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=4):** Re-ran stable + `demo_live` compile + HEAD `run_ux_gate` (14 scenarios).
Anti-false-done: `4f630e01`…`669297b5` (STEP=0–3); no bare `document_frame_snapshot(` in
`demo_live`; all sites use `frame_snapshot_cache_tick`; idle → `glfw_gl_context_wait_events_timeout`;
no `sleep_ms`; `misc/editor/**` + thin `runtime/src/gl/` wait wrapper → REG skip; no `compiler/`/`lib/mlc/`.
L2: idle `dirty=0` keeps `snapshot_count`. **reopen: none**.

Honest residual: live process CPU sample (&lt;10%) optional, not measured here — **open** §51
`TRACK_EDITOR_IDLE_CPU_BUDGET`. Caret blink may freeze
while idle (activity flag has no blink deadline) — **fixed** §50
`TRACK_EDITOR_IDLE_CARET_BLINK` (Critic OK 2026-07-22). Profile scopes only on dirty snapshot tick, not
full frame phases. Historical `run_ux_idle_frame_snapshot_red.sh` expects pre-wire demo_live and
fails post-green — use `run_ux_idle_frame_snapshot_stable.sh` only.

| Gate | Result |
|------|--------|
| `run_ux_idle_frame_snapshot_stable.sh` | `ux_ok idle_frame_snapshot_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (14 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + thin GL wait) |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#37 EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `4f630e01` | Decision: dirty/wait/snapshot-cache + Profile |
| 1 | `ae535a18` | Red L2 stub + red/stable scripts |
| 2 | `2dd8d528` | `frame_snapshot_cache` green; wait API; Profile on dirty |
| 3 | `669297b5` | `demo_live` activity early-out + cache tick + wait |
| 4 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | Idle `demo_live` holds **~92% of one core** (measured 2026-07-22 on `README.md`, no input). Loop always does full frame work: `document_frame_snapshot` at top, full layout/draw, `glfwPollEvents`, `sleep_ms(16)`, `glfwSwapInterval(0)`. No dirty-flag / wait-for-event idle path |
| Strategy (v1) | (1) Coarse Profile scopes on dirty snapshot. (2) Dirty flag on input/edit/resize. (3) Idle `glfwWaitEventsTimeout`. (4) Cache `document_frame_snapshot` on idle |
| Primary gate | Idle ticks with dirty=false must **not** re-call `document_frame_snapshot` |
| Module touch | `misc/editor/**`; minimal `runtime/src/gl/` for WaitEventsTimeout |
| REG | no (`lib/mlc/` out) |
| Out of scope | `#37` minimap bleed; viewport-only layout; `compiler/`; full GL batching |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: idle-frame / snapshot-every-tick probe | **done** |
| 2 | Profile scopes + dirty/wait/snapshot-cache implement | **done** |
| 3 | `demo_live` idle path green; compile | **done** |
| 4 | Critic: gates; archive | **done** |

## Out of scope

`#37`; full renderer batching unless profile names it; `lib/mlc/`; concurrency tracks.
