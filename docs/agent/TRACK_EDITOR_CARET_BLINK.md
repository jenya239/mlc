# Track: Editor caret blink (UX L8 + live)

Parent: [../PLAN.md](../PLAN.md) §41; residual from
[../GUI_UX_TESTING.md](../GUI_UX_TESTING.md) L8 (`caret_blink_phases` /
`UxClock`) and [archive/tracks/TRACK_UX_HEADLESS.md](../archive/tracks/TRACK_UX_HEADLESS.md)
(«L8 blink deferred»). Live: crude `(frame_index / 30) % 2` in
`misc/editor/demo_live.mlc`.

## Status: **active** (2026-07-16) — queue head

After §40 UTF-8 codepoint columns closed. Next editor/UX gap: ship L8 caret
blink via fake clock (no `sleep`); share helper with `demo_live`.

## Next step

**STEP=0** — Decision freeze: period, helper API, scenario vs unit gates.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| Scope v1 | Pure `caret_blink_visible(clock_ms, period_ms) -> bool` (+ optional period const) |
| Clock | `UxDriver` `SetClock` / `WaitFrames` only — no `sleep` |
| Scenario | `caret_blink_phases` → `ux_ok caret_blink_phases`; register in `run_ux_gate.sh` |
| Live | Replace `demo_live` frame_index blink with helper (same period) |
| Probe | Prefer snapshot/expect on visibility bool; no FBO required |
| REG | Prefer no `lib/mlc/`; no `compiler/` |
| Non-goals | IME; grapheme; tree-sitter; SCRIPT_VM; MIR Epic 5; Fontconfig |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Helper + unit and/or UX scenario | unit / `ux_ok` token |
| 2 | Wire `demo_live` + `run_ux_gate.sh` green | compile + ux gate |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze period_ms default + export names + file placement (`ux/` vs `ui/`).
2. Confirm scenario name matches GUI_UX_TESTING L8.

**STEP=1**
1. Land helper + unit (or scenario that asserts on/off phases via SetClock).
2. Gate script token.

**STEP=2**
1. `demo_live` uses helper; register scenario in `run_ux_gate.sh`.
2. `run_ux_gate.sh` → `[ux gate] all ok`; demo_live compile ok.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- IME / grapheme clusters
- Live FBO caret MAE (already have CPU stand-in L9-adjacent)
- tree-sitter / SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
