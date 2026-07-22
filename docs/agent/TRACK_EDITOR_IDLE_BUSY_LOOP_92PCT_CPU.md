# Track: Editor idle busy-loop 92% CPU

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#38**
(pulled forward 2026-07-22 — measured idle CPU / user "тормозит"). Size **L**.

## Status: **active** (2026-07-22) — STEP=0 frozen → Driver STEP=1

## Next step

**STEP=1** — Red: idle-frame probe / harness showing unconditional per-frame work
(`document_frame_snapshot` every tick, no dirty skip).

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | Idle `demo_live` holds **~92% of one core** (measured 2026-07-22 on `README.md`, no input). Loop always does full frame work: `document_frame_snapshot` at top (`demo_live.mlc:794`), full layout/draw, `glfw_gl_context_swap_poll` → `glfwPollEvents` (`glfw_window_gl.cpp`), `sleep_ms(16)` (`demo_live.mlc:2283`), `glfwSwapInterval(0)` (vsync off). No dirty-flag / wait-for-event idle path. Snapshot alone ~88 μs — not enough alone for 92%; dominant phase not named (sandbox: `perf`/`ptrace` blocked) |
| Strategy (v1) | (1) Coarse `Profile.scope_begin`/`scope_end` around frame phases (same API as `compiler/profile.mlc` / `mlc::profile`) to name the hot phase. (2) Dirty flag: set on input/edit/resize/focus/animation need; when clean, skip CPU-bound layout/snapshot/draw (or draw last frame only). (3) Idle wait: prefer `glfwWaitEventsTimeout` (thin runtime wrapper next to `glfw_gl_context_swap_poll`) over busy `PollEvents`+fixed `sleep_ms(16)`. (4) Cache `document_frame_snapshot` on idle (same anti-pattern class as `#36` wrap / `#35` minimap caches) |
| Primary gate | L2/unit or script probe: idle ticks with dirty=false must **not** re-call `document_frame_snapshot` (call-count / tick counter). Optional secondary: process CPU sample &lt;10% while idle for N seconds when `/proc` available |
| Module touch | `misc/editor/**`; minimal `runtime/src/gl/` (+ header) only if WaitEventsTimeout binding is missing |
| REG | no (`lib/mlc/` out) |
| Out of scope | `#37` minimap bleed; viewport-only layout rewrite; `compiler/` feature work; full GL batching redesign unless profile proves it is the named hot path |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: idle-frame / snapshot-every-tick probe | open |
| 2 | Profile scopes + dirty/wait/snapshot-cache implement | open |
| 3 | `demo_live` idle path green; compile | open |
| 4 | Critic: gates; archive | open |

<!-- STEP=1: red harness — assert current loop still hits snapshot every frame or idle-CPU red script EXIT≠0 -->
<!-- STEP=2: Profile labels; dirty flag + wait timeout + snapshot cache module -->
<!-- STEP=3: wire demo_live; green idle probe / optional CPU sample; demo_live_fs_compile_ok -->

## Out of scope

`#37`; full renderer batching unless profile names it; `lib/mlc/`; concurrency tracks.
