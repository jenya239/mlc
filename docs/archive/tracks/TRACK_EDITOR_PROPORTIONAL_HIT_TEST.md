# Track: Editor proportional hit-test / caret / selection

Parent: [../../PLAN.md](../../PLAN.md) §53.
Residual of [TRACK_EDITOR_WRAP_BUDGET_ADVANCE](TRACK_EDITOR_WRAP_BUDGET_ADVANCE.md)
(§52 Decision OOS). Size **M**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `proportional_hit_test_stable` ×2 + `wrap_budget_advance_stable` +
`wrap_count_cache_stable` + `demo_live` compile + HEAD `run_ux_gate` (68 scenarios).
Anti-false-done: `a2424a12`…`6a1b1ce9` (STEP=0–2); `wrap_offset_at_point_pixel` /
`pixel_x_for_byte_offset` / `byte_offset_at_pixel_x`; demo_live click/caret/selection wire;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: selection highlight still doc-line based (not per visual wrap row);
`*_red` post-green fails (use stable only). ICU grapheme OOS.

| Gate | Result |
|------|--------|
| `run_ux_proportional_hit_test_stable.sh` | `ux_ok proportional_hit_test_stable` EXIT=0 (×2) |
| `run_ux_wrap_budget_advance_stable.sh` | `ux_ok wrap_budget_advance_stable` EXIT=0 |
| `run_ux_wrap_count_cache_stable.sh` | `ux_ok wrap_count_cache_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (68 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; select next or document idle).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `a2424a12` | Decision: proportional hit/caret/selection |
| 1 | `fe1f77e5` | Red/stable stub |
| 2 | `6a1b1ce9` | Advance-sum hit + demo wire |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Hit-test/caret/selection used mono `char_width` while wrap uses shaped advances |
| Strategy (v1) | Pixel↔byte via cumulative advances within wrap row; column fallback if shape fails |
| Primary gate | `ux_ok proportional_hit_test_stable`; full `run_ux_gate` |
| Module touch | `word_wrap.mlc` + `demo_live` + scripts/scenario |
| REG | no |
| Out of scope | ICU grapheme; rewrite all column APIs; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no proportional hit-test script | **done** |
| 2 | Implement advance-sum hit/caret/selection; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

ICU; full column-model delete; SCRIPT_VM; MIR Epic 5; visual-row selection geometry.
