# Track: Editor active-tab title without brackets

Parent: [../PLAN.md](../PLAN.md) §70.
Residual of [TRACK_EDITOR_ACTIVE_TAB_TINT](../archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TINT.md)
(§68). Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert `[title]` brackets still wrap active tab titles; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: drop active-tab `[title]` brackets |

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
| 1 | Red: brackets still present | pending |
| 2 | Drop brackets; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
