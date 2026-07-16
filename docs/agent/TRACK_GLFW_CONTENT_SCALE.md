# Track: GLFW window content scale (HiDPI)

Parent: [../PLAN.md](../PLAN.md) §37; residual from
[archive/tracks/TRACK_STDLIB_IO_FS.md](../archive/tracks/TRACK_STDLIB_IO_FS.md)
STEP=7 deferred.

## Status: **active** (2026-07-16) — queue head

§36 closed without content_scale; open as small focused track.

## Next step

**STEP=4** — Critic: gates + REG if `lib/mlc/`; archive.

### STEP=3 done (2026-07-16)

- `demo_live` `font_size()` × `glfw_gl_window_content_scale_y`; compile ok.

### STEP=2 done (2026-07-16)

- Headless smoke `content_scale_ok`.

### STEP=1 done (2026-07-16)

- `glfw_gl_window_content_scale_x/y` in glfw_window_gl + `gl_window.mlc`.
- Compile gate `content_scale_compile_ok`.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §37 → STEP=1.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in existing `mlc::gl::glfw_gl_window_width/height` +
`glfwGetWindowContentScale` (GLFW 3.3+).

| Item | Choice |
|------|--------|
| Placement | `runtime` `glfw_window_gl.hpp`/`.cpp` + MLC `lib/mlc/common/stdlib/gl/gl_window.mlc` |
| API | Two bare exports: `glfw_gl_window_content_scale_x() -> f64`, `glfw_gl_window_content_scale_y() -> f64` (match width/height split; no record) |
| C++ | `mlc::gl::glfw_gl_window_content_scale_x/y` → `glfwGetWindowContentScale` on current window |
| Missing / no window | return `1.0` / `1.0` (same for both axes) |
| Headless smoke | After `glfw_gl_context_begin`: both finite, `> 0.0`; token `content_scale_ok` |
| Typical values | Often `1.0` on standard DPI; HiDPI may be `2.0` — do not assert exact value |
| Editor wire (STEP=3) | **optional** — one chrome metric × scale in `demo_live` only if one-liner; else mark N/A at STEP=3 |
| REG | `lib/mlc/` touch → `scripts/regression_gate.sh` before Critic; **no** `compiler/` |
| Namespace | C++ `mlc::gl::*`; MLC bare names on `gl_window.mlc` |

### Exact exports

```text
glfw_gl_window_content_scale_x() -> f64
glfw_gl_window_content_scale_y() -> f64
```

### Non-goals (Decision)

Fontconfig; full HiDPI layout rewrite; framebuffer-size vs window-size unification;
SCRIPT_VM; LANG_AUTO_CYCLE; MIR Epic 5; `compiler/` changes.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Runtime + ABI + MLC extern | **done** (2026-07-16): `content_scale_compile_ok` |
| 2 | Headless smoke token (`content_scale_ok`) | **done** (2026-07-16): `content_scale_ok` |
| 3 | Optional: demo_live / editor use scale | **done** (2026-07-16): `demo_live_fs_compile_ok` |
| 4 | Critic: gates + REG if `lib/mlc/`; archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze table; name exports exactly — done.
2. Non-goals + headless behavior — done.

**STEP=1** — **done**
1. C++ wrapper around `glfwGetWindowContentScale` — done.
2. MLC extern on `gl_window.mlc` — done.

**STEP=2** — **done**
1. Smoke: after `glfw_gl_context_begin`, scales `> 0` — done.

**STEP=3** (optional / N/A if heavy)
1. Multiply one chrome metric by scale in demo_live — or mark N/A.

**STEP=4** — Critic; `next` = Planner.

## Out of scope

- Fontconfig / system font enumeration
- Full HiDPI layout redesign
- SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Foreign SCRIPT_VM docs: do not reopen implementation.
