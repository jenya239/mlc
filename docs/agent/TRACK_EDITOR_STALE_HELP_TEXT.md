# Track: Editor stale help text (folder UX copy)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#1c**.
Found 2026-07-17: in-app README/guide/changelog still describe tree
expand/collapse; live UI is breadcrumb + back/forward (§38).

## Status: **active** (2026-07-18) — STEP=1 done; STEP=2 Critic next

## Next step

**STEP=2** — Critic: gates; archive; `next` = Planner (§46 #1d).

### STEP=1 done (2026-07-18)

- Rewrote `sample_readme_text` / `sample_guide_text` / `sample_changelog_text`
  + file header: breadcrumbs / back/forward / open file
- Gate: `demo_live_fs_compile_ok`

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #1c → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | `sample_readme_text` / `sample_guide_text` / `sample_changelog_text` (and related demo copy) still say expand/collapse tree; `demo_live` uses folder browser breadcrumbs + nav history, never `file_tree_expand`/`file_tree_collapse` |
| Fix | Copy-only: describe breadcrumbs, back/forward, click file to open, parent/`..` where accurate. No behavior change |
| Scope files | `misc/editor/demo_live.mlc` sample helpers (+ file header comment if it still claims `[+]/[-]` expand UX). No `compiler/` / `lib/mlc/` |
| Scenario | **none** (backlog: copy-only; Standing discipline does not require L2 for docs-in-app without behavior change) |
| Gate | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog row | **done** (2026-07-18) |
| 1 | Fix stale sample_* / header copy in `demo_live.mlc` | **done** |
| 2 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2** — Critic; `next` = Planner (§46 #1d).

## Out of scope

Behavior/UX changes; keyboard nav; #1d large-file stringify; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
