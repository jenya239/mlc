# Track: Editor idle CPU budget (live sample)

Parent: [../../PLAN.md](../../PLAN.md) §51.
Residual of [TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU](TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md)
(§46 #38). Size **S**.

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=3):** Re-ran stable ×3 + idle caret/snapshot stables + `demo_live`
compile + HEAD `run_ux_gate` (66 scenarios). Anti-false-done: `9ed7c2c3`…`d9150fb0`
(STEP=0–2); paint-only caret overlay (`solid_renderer_flush_over`, no full-frame clear);
live `/proc` min-of-rounds CPU% ≤10 (boundary: fail only if `>` budget — Critic fix for
integer `==10` flake). `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: wrap budget still uniform `char_width` (out of scope); Profile scopes only
on dirty snapshot. Historical `run_ux_idle_cpu_budget_red.sh` expects pre-wire stable stub
and fails post-green — use `run_ux_idle_cpu_budget_stable.sh` only.

| Gate | Result |
|------|--------|
| `run_ux_idle_cpu_budget_stable.sh` | `ux_ok idle_cpu_budget_stable` EXIT=0 (×3) |
| `run_ux_idle_caret_blink_stable.sh` | `ux_ok idle_caret_blink_stable` EXIT=0 |
| `run_ux_idle_frame_snapshot_stable.sh` | `ux_ok idle_frame_snapshot_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (66 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; select next or document idle).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9ed7c2c3` | Decision: live `/proc` CPU% budget |
| 1 | `620e9483` / `3204369b` | Red/stable stub |
| 2 | `d9150fb0` | Paint-only overlay + `/proc` sample |
| 3 | (this) | Critic: ≤10% boundary fix; archive |

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | #38 L2 gates miss live idle CPU regression |
| Strategy (v1) | Live `/proc` sample (min of rounds); paint-only caret overlay; budget **≤10%** |
| Primary gate | `ux_ok idle_cpu_budget_stable`; full `run_ux_gate` |
| Module touch | `scripts/run_ux_idle_cpu_budget_*.sh` + `misc/editor/demo_live.mlc` |
| REG | no |
| Out of scope | wrap-budget HarfBuzz; Profile full-frame; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no live idle CPU budget script | **done** |
| 2 | Implement sample script; green under 10% | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Reopen #38/#50 numbered STEPs; wrap-budget HarfBuzz rewrite.
