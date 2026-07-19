# Track: Editor UX L1 Backfill (§38/§40/§42/§43)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#30**.
Source: Opus review §2.1 (`editor_tdd_ux_20260717_*`) — those tracks closed on
L0 unit + compile-gate only; Standing discipline now requires L1 for visible
UX. Size **M**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran four L1 + folder/line_index/wrap units +
demo compile. Anti-false-done: `d1e0c679`…`505ec20a` (STEP=0–3); scenarios +
`run_ux_*.sh` present; no `compiler/` / `lib/mlc/` in track commits → REG skip.
Wire: `folder_browser_resolve_click` (breadcrumb + nav); display-col round-trip
+ up/down preferred column; wrap visual row + `wrap_offset_at_point`.
**reopen: none**.

Honest residual: horizontal arrows still byte-step (`move_horizontal`); L1
covers display-column model + vertical preferred column only.

| Gate | Result |
|------|--------|
| `run_ux_folder_breadcrumb_navigates.sh` | `ux_ok folder_breadcrumb_navigates` EXIT=0 |
| `run_ux_folder_nav_back_forward.sh` | `ux_ok folder_nav_back_forward` EXIT=0 |
| `run_ux_utf8_caret_display_column.sh` | `ux_ok utf8_caret_display_column` EXIT=0 |
| `run_ux_caret_after_word_wrap.sh` | `ux_ok caret_after_word_wrap` EXIT=0 |
| `run_editor_folder_panel_unit.sh` | `folder_panel_unit ok` EXIT=0 |
| `run_editor_folder_nav_unit.sh` | `folder_nav_unit ok` EXIT=0 |
| `run_editor_line_index_unit.sh` | `line_index_unit ok` EXIT=0 |
| `run_editor_word_wrap_unit.sh` | `word_wrap_unit ok` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#31a EDITOR_COLUMN_PIXEL_MODEL_FIX`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `d1e0c679` | Decision freeze + open |
| 1 | `d46eae16` | Four red L1 stubs + run scripts + `run_ux_l1_backfill_red.sh` |
| 2 | `bda37d1f` | Green breadcrumb + nav back/forward via `folder_browser_resolve_click` |
| 3 | `505ec20a` | Green utf8 display-column caret + wrap caret/hit |
| 4 | this Critic | close + archive |

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
| 3 | Green §40 + §42 | **done** — `ux_ok utf8_caret_display_column`; `ux_ok caret_after_word_wrap` |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

Horizontal arrow byte-step (residual; L1 covers display-column model + vertical preferred column); grapheme/ZWJ; `compiler/`; `lib/mlc/`.
