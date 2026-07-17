# Track: Editor demo orchestrator (`demo_live` → `ux/*`)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#1**;
origin [archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md](../archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md).
Evidence: [`mlc-support/responses/editor_tdd_ux_20260717_114221.md`](../../../mlc-support/responses/editor_tdd_ux_20260717_114221.md) §2.2.

## Status: **active** (2026-07-18) — STEP=0–2 done; STEP=3 next

## Next step

**STEP=3** — Tree/breadcrumb hits → `tree_hit` / folder helpers.

### STEP=2 done (2026-07-18)

- `editor_ux_click_tab_strip_at` (explicit strip rect); `editor_app_click_tab_strip`
  (close + activate, live chrome geometry).
- `demo_live` tab close/activate → wrapper; hit helpers only used for draw.
- Gates: `ux_ok tab_strip_click_activates`, `demo_live_fs_compile_ok`.

### STEP=1 done (2026-07-18)

- `editor_ux_backspace` in `ux/edit_apply.mlc`; `editor_app_insert_text` /
  `insert_newline` / `backspace` in `app/state.mlc` (history + ux).
- `demo_live` text/enter/backspace → those wrappers (no inline `edit_*`).
- Gates: `newline_caret_and_spaces`, `edit_unit`, `copy_paste_roundtrip`,
  `demo_live_fs_compile_ok`.

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #1 → active; CONTINUITY queue head.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Goal | `demo_live.main()` becomes thin: `poll → editor_app_*(app, input) → draw(app)`; stop duplicating tested `ux/*` logic inline |
| Scope | **Behavior-preserving refactor only** — no new UX features (§46 #1b+) |
| Call, don't reimplement | Prefer existing exports: `ux/edit_apply`, `ux/scroll`, `ux/tab_strip`, `ux/tree_hit`, `ux/clipboard_apply`, `ux/selection_apply`, `app/frame_command` / `app/state` |
| New helpers | Thin `editor_app_*` wrappers in `misc/editor/app/` **only** if needed to keep `demo_live` free of mutation blobs; no second copy of edit/scroll/tab/tree logic |
| Forbidden | Big-bang rewrite; new scene/widget framework; renaming blobs into `*_phase` without deleting the inline duplicate; `compiler/` / `lib/mlc/` |
| Standing discipline | Behavior unchanged → existing L1 scenarios must stay green (same commit as each Driver STEP). Name gate tokens in SESSION `done` |
| REG | no `lib/mlc/`; no `compiler/` |

### Inline clusters to retire (from review; approximate, re-verify line numbers)

| Cluster | Prefer |
|---------|--------|
| Text / Enter / Backspace mutation | `editor_ux_insert_text`, `editor_ux_insert_newline`, related edit/selection helpers |
| Tab title/close hit + activate | `tab_strip_hit_*`, `editor_ux_click_tab_strip` |
| Tree / breadcrumb hit | `tree_hit_*`, `editor_ux_click_tree`, folder_panel helpers already wired for nav |
| Wheel / caret-visible scroll | `editor_ux_wheel_scroll`, `editor_ux_ensure_caret_visible`, clamp helpers |

### Gates (every Driver STEP + Critic)

| Token | Script / suite |
|-------|----------------|
| compile | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| UX L1 | existing `misc/editor/ux_scenarios/*.mlc` via their run scripts (at least: `tab_strip_click_activates`, `tree_click_opens_tab`, `wheel_scroll_keeps_caret_visible`, `newline_caret_and_spaces`, `copy_paste_roundtrip` — whichever scripts already exist for these) |
| units | any `misc/editor/tests/*_unit.mlc` already covering touched `ux/*` — do not drop |

Do **not** invent a new scenario for this track unless a cluster has **no** existing gate (then add minimal L1, same commit).

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog row | **done** (2026-07-18) |
| 1 | Edit path → `edit_apply` (+ related) | **done** (2026-07-18) compile + newline/edit/copy_paste |
| 2 | Tab strip clicks → `tab_strip` / `editor_ux_click_tab_strip` | **done** (2026-07-18) compile + `tab_strip_click_activates` |
| 3 | Tree/breadcrumb hits → `tree_hit` / folder helpers | compile + `tree_click_opens_tab` |
| 4 | Scroll/wheel → `scroll` helpers; thin `main` shape check | compile + `wheel_scroll_keeps_caret_visible` |
| 5 | Critic: gates green; no duplicate clusters left for scope; archive | close |

### Sub-steps (Driver)

**STEP=1**
1. Find inline insert/newline/backspace in `demo_live.mlc`; replace with `ux/edit_apply` (or thin `app/` wrapper).
2. Re-run compile + edit-related UX gates; SESSION names tokens.

**STEP=2**
1. Replace inline tab hit/activate/close with `ux/tab_strip` exports.
2. Gate: `tab_strip_click_activates` + compile.

**STEP=3**
1. Replace inline tree/breadcrumb hit with `ux/tree_hit` / folder helpers (keep §43 `folder_nav_*` wire).
2. Gate: `tree_click_opens_tab` + compile.

**STEP=4**
1. Replace inline editor wheel/clamp/caret-visible with `ux/scroll`; confirm `main` is orchestrator-shaped (no large mutation blobs for clusters above).
2. Gate: `wheel_scroll_keeps_caret_visible` + compile.

**STEP=5** — Critic; `next` = Planner (§46 #1b).

## Out of scope

Solarized glyphs (#1b), help-text copy (#1c), large-file stringify (#1d),
keyboard nav (#2), multi-cursor, find/replace, `compiler/`, SCRIPT_VM.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario/unit tokens in SESSION `done`.
