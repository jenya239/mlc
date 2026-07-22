# Track: Editor idle CPU budget (live sample)

Parent: [../PLAN.md](../PLAN.md) §51.
Residual of [TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU](../archive/tracks/TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md)
(§46 #38). Size **S**.

## Status: **active** (2026-07-22) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: no live idle CPU budget gate (gap).

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | #38 fixed idle busy-loop via `content_dirty` / snapshot cache / `wait_events_timeout`, but the original gate ("sample process CPU while idle, assert &lt;10%") was never measured — Critic marked it optional. Without a live `/proc` sample, a future regression could restore high idle CPU while L2 snapshot gates stay green |
| Strategy (v1) | (1) Headless launch of existing `demo_live` binary (`MLC_GLFW_VISIBLE=0`, fixed fixture e.g. `README.md`). (2) Warm-up, then sample `/proc/<pid>/stat` `utime+stime` over N seconds with no input. (3) Assert CPU% = `100 * delta_jiffies / (HZ * N)` under budget (**&lt;10%**). (4) Reuse compile artifact from `run_editor_demo_live_fs_compile` when present; otherwise compile once. SKIP if glfw/font/mlcc missing (same as compile gate). Do **not** change idle path unless gate is red after implement |
| Primary gate | Red: no `run_ux_idle_cpu_budget_stable.sh` (or script asserts gap). Green: `ux_ok idle_cpu_budget_stable` EXIT=0 with measured CPU% &lt;10; auto-discovered by `run_ux_gate` |
| Module touch | `scripts/run_ux_idle_cpu_budget_*.sh` only (no `compiler/` / `lib/mlc/`; demo_live only if gate red requires fix) |
| REG | no |
| Out of scope | wrap-budget HarfBuzz; Profile full-frame phases; further idle optimize unless red; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no live idle CPU budget script | open |
| 2 | Implement sample script; green under 10% | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves stable script missing or documents gap -->
<!-- STEP=2: /proc sample + budget assert; SKIP path for missing deps -->
<!-- STEP=3: Critic archive -->

## Out of scope

Reopen #38/#50 numbered STEPs; wrap-budget HarfBuzz rewrite.
