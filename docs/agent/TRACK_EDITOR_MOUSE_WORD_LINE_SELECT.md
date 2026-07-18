# Track: Editor mouse word/line select (double / triple click)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#4**.
Single-click caret via `nav_click` / `editor_ux_click_text` exists; no
click-count / timing; double/triple do not select word/line. Word edges
already in `document/word_boundary.mlc` (#3).

## Status: **active** (2026-07-18) — STEP=0 done; STEP=1 next

## Next step

**STEP=1** — L2 scenarios first: `double_click_selects_word` +
`triple_click_selects_line` (Standing discipline); red harness if needed.

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #4 → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Mouse down in text always collapses to caret (`nav_click`); no multi-click detector; cannot select word (double) or line (triple) |
| Word select | At click byte offset: selection `[word_boundary_prev(text, offset+ε), word_boundary_next(text, offset)]` using existing ASCII word class (#3). If click on separator, select nearest word or empty collapsed at offset (prefer: expand to word containing offset if inside word; else treat as single click — freeze: if `is_word_char` at offset or offset-1, select that word; else caret) |
| Line select | Selection from `line_index_line_start(line)` through line content end (exclude trailing `\n` if present — match existing `line_content_end` in `selection_apply`) |
| Click count | Stateful detector: `last_click_ms`, `last_click_offset` (or pixel), `click_count`. Same-point + within threshold → increment; else reset to 1. Threshold default **500 ms**. Count wraps: 1=caret, 2=word, 3=line, 4+=restart at 1 (Sublime-like) |
| Timing / L2 | Scenarios drive clock via `UxDriver` `SetClock` / `WaitFrames` (no `sleep`). Helper API must accept explicit `clock_ms` for headless tests |
| Apply path | Thin `ux/` helper (e.g. `editor_ux_click_text_counted` / multi-click state in app) composing word/line select + update detector; `demo_live` uses it on text-area mouse-down edge instead of bare `nav_click` |
| Scenarios | L2: `double_click_selects_word`, `triple_click_selects_line` (backlog names) |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux/app + `demo_live` (+ reuse `word_boundary`). No `compiler/` / `lib/mlc/`; no new GLFW ABI |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok double_click_selects_word` + `ux_ok triple_click_selects_line` (+ run scripts) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L2 scenarios first (`double_click_selects_word`, `triple_click_selects_line`) | scenario fails or harness ready |
| 2 | multi-click detector + word/line select + `demo_live` wire | scenarios + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1**
1. Add `ux_scenarios/double_click_selects_word.mlc` + `triple_click_selects_line.mlc` (+ run scripts).
2. Prefer red before STEP=2 (stub counted-click returns single-click caret).

**STEP=2**
1. Multi-click state + word/line selection helpers; unit if cheap.
2. Wire `demo_live` text mouse-down through counted path (`clock_ms`).
3. Gates: both scenarios ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #5).

## Out of scope

Shift-click extend (#5); drag-autoscroll (#9); wide-char (#28); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
