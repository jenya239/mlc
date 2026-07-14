# Editor UX harness

Headless semantic scenarios for the editor shell.
Design: [docs/GUI_UX_TESTING.md](../../../docs/GUI_UX_TESTING.md).
Track: [docs/agent/TRACK_UX_HEADLESS.md](../../../docs/agent/TRACK_UX_HEADLESS.md).

## Run all

```bash
scripts/run_ux_gate.sh
```

Expect `[ux gate] all ok`. No display required (`MLC_GLFW_VISIBLE=0` on GL-related entries).

## Layout

```text
misc/gui/ux/          # snapshot, expect, driver, draw_report, mae
misc/editor/ux/       # probe + product apply helpers
misc/editor/ux_scenarios/  # one .mlc scenario per gate entry
scripts/run_ux_*.sh   # compile+run one scenario
scripts/run_ux_gate.sh
```

## Add a scenario

1. Write `misc/editor/ux_scenarios/<name>.mlc` with `fn main() -> i32` that prints
   `ux_ok <name>` on success and returns 0.
2. Add `scripts/run_ux_<name>.sh` (copy an existing `run_ux_*.sh`; point `ENTRY` at
   the scenario; grep for `ux_ok <name>`).
3. Register in `scripts/run_ux_gate.sh` via `run_one …`.
4. Document one line under Scenarios in `docs/GUI_UX_TESTING.md`.
5. Prefer L1 probes (`UxSnapshot` + `ux_check_expect`) over pixels; MAE only when
   needed (see `caret_glyph_mae`).

Do not use `sleep` for timing — use `WaitFrames` / `SetClock` on `UxDriver`.
