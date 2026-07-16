# Track: GLFW window content scale (HiDPI)

Parent: [../PLAN.md](../PLAN.md) §37; residual from
[archive/tracks/TRACK_STDLIB_IO_FS.md](../archive/tracks/TRACK_STDLIB_IO_FS.md)
STEP=7 deferred.

## Status: **active** (2026-07-16) — queue head

§36 closed without content_scale; open as small focused track.

## Next step

**STEP=0** — Decision freeze: ABI shape + smoke token.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| Placement | `runtime` `glfw_window_gl` + MLC `gl_window.mlc` exports |
| API | `glfw_gl_window_content_scale_x/y() -> f64` (or pair record) via `glfwGetWindowContentScale` |
| Missing window | return `1.0` / `1.0` |
| Editor wire | optional STEP: scale UI metrics in `demo_live` / app chrome — only if trivial |
| REG | touch `lib/mlc/` → REG before Critic |
| Non-goals | Fontconfig; full DPI layout rewrite; SCRIPT_VM; MIR Epic 5 |

### Exact exports (draft)

```text
glfw_gl_window_content_scale_x() -> f64
glfw_gl_window_content_scale_y() -> f64
```

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Runtime + ABI + MLC extern | compile smoke |
| 2 | Headless smoke token (`content_scale_ok`) | `run_glfw_content_scale_smoke.sh` |
| 3 | Optional: demo_live / editor use scale | compile smoke |
| 4 | Critic: gates + REG if `lib/mlc/`; archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze table; name exports exactly.
2. Non-goals + headless behavior (scale ≥ 1.0 typical).

**STEP=1**
1. C++ wrapper around `glfwGetWindowContentScale`.
2. MLC extern on `gl_window.mlc`.

**STEP=2**
1. Smoke: after `glfw_gl_context_begin`, scales are finite and > 0.

**STEP=3** (optional / skip if heavy)
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
