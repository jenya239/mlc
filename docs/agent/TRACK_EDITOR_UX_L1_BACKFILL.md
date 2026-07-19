# Track: Editor UX L1 Backfill (§38/§40/§42/§43)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#30**.
Source: Opus review §2.1 (`editor_tdd_ux_20260717_*`) — those tracks closed on
L0 unit + compile-gate only; Standing discipline now requires L1 for visible
UX. Size **M**.

## Status: **active** (2026-07-19) — STEP=0–3 done; next Critic

## Next step

**STEP=4** — Critic: all four L1 + units + demo; archive; backlog #30 done.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `d1e0c679` | Decision freeze + open |
| 1 | `d46eae16` | Four red L1 stubs + run scripts + `run_ux_l1_backfill_red.sh` |
| 2 | `bda37d1f` | Green breadcrumb + nav back/forward via `folder_browser_resolve_click` |
| 3 | *(this turn)* | Green utf8 display-column caret + wrap caret/hit |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | §38 FOLDER_BROWSER, §40 UTF8_COLUMNS, §42 WORD_WRAP, §43 FOLDER_NAV closed without `ux_scenarios/*` (L0+compile only). Standing discipline: not verified UX |
| Scope | **Add** four L1 scenarios (+ scripts). Prefer calling existing `ux/*` / panel / layout APIs — extract thin helpers only if click path still lives only in `demo_live` |
| §38 | `folder_breadcrumb_navigates` — breadcrumb hit → current folder path / listing updates |
| §40 | `utf8_caret_display_column` — display columns + up/down preferred column (post-#28) |
| §42 | `caret_after_word_wrap` — wrap visual row + hit-test at wrap boundary |
| §43 | `folder_nav_back_forward` — back/forward via `folder_browser_resolve_click` |
| Tokens | `ux_ok folder_breadcrumb_navigates`, `ux_ok utf8_caret_display_column`, `ux_ok caret_after_word_wrap`, `ux_ok folder_nav_back_forward` |
| Module touch | `misc/editor/ux_scenarios/*` + `scripts/run_ux_*.sh`. No `compiler/`. No `lib/mlc/` |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Four L1 red harnesses | **done** — `ux_ok l1_backfill_red` |
| 2 | Green §38 + §43 | **done** — two `ux_ok` |
| 3 | Green §40 + §42 | **done** (2026-07-19) — `ux_ok utf8_caret_display_column`; `ux_ok caret_after_word_wrap` |
| 4 | Critic: gates; archive | pending |

## Out of scope

Horizontal arrow byte-step (residual; L1 covers display-column model + vertical preferred column); grapheme/ZWJ; `compiler/`; `lib/mlc/`.
