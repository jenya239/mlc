# Track: Editor active-tab title without brackets

Parent: [../../PLAN.md](../../PLAN.md) §70.
Residual of [TRACK_EDITOR_ACTIVE_TAB_TINT](TRACK_EDITOR_ACTIVE_TAB_TINT.md)
(§68). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `active_tab_title_plain_stable` ×2 +
`active_tab_tint_stable` + `demo_live` compile + HEAD `run_ux_gate` (85 scenarios).
Anti-false-done: `95c15893`…`1f45a8f7` (STEP=0–2); three `[title]` wraps removed;
selection_* fill remains; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); scrollbar thumb hover still OOS (#32).

| Gate | Result |
|------|--------|
| `run_ux_active_tab_title_plain_stable.sh` | `ux_ok active_tab_title_plain_stable` EXIT=0 (×2) |
| `run_ux_active_tab_tint_stable.sh` | `ux_ok active_tab_tint_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (85 scenarios) | `[ux gate] all ok` EXIT=0 (retry; first run raced idle_cpu with compile) |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `95c15893` | Decision: drop active-tab `[title]` brackets |
| 1 | `68fa8020` | Red/stable stub |
| 2 | `1f45a8f7` | Drop three wraps + L0 green |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Active tab is already marked by `theme.selection_*` idle fill (§68), but titles still wrap in `[…]` at three `demo_live` sites. Brackets are redundant and were OOS of §68 |
| Strategy (v1) | Remove the three `"[" + title + "]"` active-title wraps (layout / hover-layout / draw). Keep dirty `*` suffix and selection fill. No Theme changes |
| Primary gate | Red: no green `run_ux_active_tab_title_plain_stable.sh`. Green: `ux_ok active_tab_title_plain_stable` — no `"[" + … + "]"` active wraps; `theme.selection_*` active fill remains; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` title loops, scripts/scenario |
| REG | no |
| Out of scope | Scrollbar thumb hover; change selection tint; ICU; SCRIPT_VM; MIR Epic 5; reopen §68 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: brackets still present | **done** |
| 2 | Drop brackets; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
