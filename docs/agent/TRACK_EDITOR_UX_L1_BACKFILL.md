# Track: Editor UX L1 Backfill (§38/§40/§42/§43)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#30**.
Source: Opus review §2.1 (`editor_tdd_ux_20260717_*`) — those tracks closed on
L0 unit + compile-gate only; Standing discipline now requires L1 for visible
UX. Size **M**.

## Status: **active** (2026-07-19) — STEP=0–2 done; next STEP=3

## Next step

**STEP=3** — Green §40 + §42 (`utf8_caret_display_column`, `caret_after_word_wrap`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `d1e0c679` | Decision freeze + open |
| 1 | `d46eae16` | Four red L1 stubs + run scripts + `run_ux_l1_backfill_red.sh` |
| 2 | `bda37d1f` | Green breadcrumb + nav back/forward via `folder_browser_resolve_click` |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | §38 FOLDER_BROWSER, §40 UTF8_COLUMNS, §42 WORD_WRAP, §43 FOLDER_NAV closed without `ux_scenarios/*` (L0+compile only). Standing discipline: not verified UX |
| Scope | **Add** four L1 scenarios (+ scripts). Prefer calling existing `ux/*` / panel / layout APIs — extract thin helpers only if click path still lives only in `demo_live` |
| §38 | `folder_breadcrumb_navigates` — breadcrumb hit → current folder path / listing updates (not only L0 `folder_panel_unit`) |
| §40 | `utf8_caret_display_column` — document with multi-byte / fullwidth char; caret column / Right-arrow respects **display** columns (post-#28 wcwidth; not byte index) |
| §42 | `caret_after_word_wrap` — soft-wrapped long line; caret or hit-test at wrap boundary matches wrap model (not only `word_wrap_unit`) |
| §43 | `folder_nav_back_forward` — push ≥2 paths; back then forward; assert current path / `can_back`/`can_forward` via UX path |
| Tokens | `ux_ok folder_breadcrumb_navigates`, `ux_ok utf8_caret_display_column`, `ux_ok caret_after_word_wrap`, `ux_ok folder_nav_back_forward` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/ux_scenarios/*` + `scripts/run_ux_*.sh`; thin `ux/*` / `folder_panel` only if needed. No `compiler/`. No `lib/mlc/` |
| REG | no |

### Gates

| Step | Token |
|------|--------|
| 1 | `ux_ok l1_backfill_red` (four stubs fail) |
| 2–3 | all four `ux_ok …` |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | existing `run_editor_folder_nav_unit.sh` / `run_editor_word_wrap_unit.sh` / `run_editor_line_index_unit.sh` / `run_editor_folder_panel_unit.sh` stay green when touched |
| Critic | REG skip (no `lib/mlc/`) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | Four L1 red harnesses + `run_ux_*.sh` stubs | **done** (2026-07-19) — `ux_ok l1_backfill_red` |
| 2 | Green §38 + §43 (folder breadcrumb + nav back/forward) | **done** (2026-07-19) — `ux_ok folder_breadcrumb_navigates`; `ux_ok folder_nav_back_forward` |
| 3 | Green §40 + §42 (display column caret + wrap caret/hit) | pending — two `ux_ok` + demo |
| 4 | Critic: all four + units + demo; archive; backlog #30 done | pending |

## Out of scope

New folder/UTF8/wrap features; grapheme/ZWJ; mass-migrate other L0-only tracks; `compiler/`; `lib/mlc/`; changing #28 display-column rules except asserting them in L1.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**`, `CLAUDE.md`, `capture_analyzer.rb`, reddit drafts alone.
- Anti-false-done: git log must show claimed files before marking done.
