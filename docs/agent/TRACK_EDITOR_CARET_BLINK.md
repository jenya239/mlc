# Track: Editor caret blink (UX L8 + live)

Parent: [../PLAN.md](../PLAN.md) §41; residual from
[../GUI_UX_TESTING.md](../GUI_UX_TESTING.md) L8 (`caret_blink_phases` /
`UxClock`) and [archive/tracks/TRACK_UX_HEADLESS.md](../archive/tracks/TRACK_UX_HEADLESS.md)
(«L8 blink deferred»). Live: crude `(frame_index / 30) % 2` in
`misc/editor/demo_live.mlc`.

## Status: **active** (2026-07-16) — queue head

## Next step

**STEP=2** — Wire `demo_live` + UX scenario + ux gate.

### STEP=1 done (2026-07-16)

- `misc/editor/ux/caret_blink.mlc` + unit; gate `caret_blink_unit ok`.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §41 → STEP=1.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in `demo_live.mlc` `(frame_index / 30) % 2 == 0` (~30 frames half-cycle
at ~60 fps ≈ 500 ms on / 500 ms off) and GUI_UX_TESTING L8
(`caret_blink_phases` + `UxClock`; no `sleep`).

| Item | Choice |
|------|--------|
| Module | `misc/editor/ux/caret_blink.mlc` |
| API | `caret_blink_period_ms() -> i32`; `caret_blink_visible(clock_ms: i32, period_ms: i32) -> bool` |
| Period default | **1000** ms full cycle (half on, half off) — matches ~60 frames @ 16 ms |
| Visibility | on when `((clock_ms % period_ms) * 2) < period_ms` after clamp; `period_ms <= 0` → always **true** |
| Clock | `UxDriver` `SetClock` / `WaitFrames` only in scenario; no `sleep` |
| Live clock | `demo_live`: `caret_blink_visible(frame_index * 16, caret_blink_period_ms())` |
| Scenario | `misc/editor/ux_scenarios/caret_blink_phases.mlc` → print `ux_ok caret_blink_phases` |
| Unit (STEP=1) | `misc/editor/tests/caret_blink_unit.mlc` — pure phases, no GLFW |
| Wire (STEP=2) | replace frame_index blink in `demo_live`; register scenario in `run_ux_gate.sh` |
| Probe | assert bool phases via unit/scenario; no FBO |
| REG | Prefer **no** `lib/mlc/`; no `compiler/` |

### Exact exports

```text
caret_blink_period_ms() -> i32
caret_blink_visible(clock_ms: i32, period_ms: i32) -> bool
```

### Gate names

| Step | Script / token |
|------|----------------|
| 1 | `scripts/run_editor_caret_blink_unit.sh` → `caret_blink_unit ok` |
| 2 | `scripts/run_ux_caret_blink_phases.sh` + `run_ux_gate.sh` → `ux_ok caret_blink_phases` / `[ux gate] all ok`; `run_editor_demo_live_fs_compile.sh` |

### Non-goals (Decision)

IME; grapheme; tree-sitter; SCRIPT_VM; LANG_AUTO_CYCLE; MIR Epic 5;
Fontconfig; live FBO caret MAE; `compiler/` / `lib/mlc/` changes.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Helper + unit | `caret_blink_unit ok` |
| 2 | Wire `demo_live` + UX scenario + ux gate | compile + `[ux gate] all ok` |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze period + exports + placement — done.
2. Scenario name = GUI_UX_TESTING L8 — done.

**STEP=1**
1. `misc/editor/ux/caret_blink.mlc` + `misc/editor/tests/caret_blink_unit.mlc`.
2. Gate: `bash scripts/run_editor_caret_blink_unit.sh`.

**STEP=2**
1. `demo_live.mlc` uses helper; add scenario + `run_ux_gate.sh` entry.
2. `run_ux_gate.sh` + demo_live compile green.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- IME / grapheme clusters
- Live FBO caret MAE
- tree-sitter / SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
