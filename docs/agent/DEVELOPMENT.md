# Development (after CONTINUITY block in queued prompt)

Re-read [CONTINUITY.md](CONTINUITY.md) every turn — rules apply without restart.

## Principles

- **One sub-step per prompt** — one concern; bisect-friendly.
- **One layer per sub-step** — `compiler/` XOR `lib/mlc/`, not both (except tiny import fix).
- **Verify before next queue:** Tier A green every turn; Tier B before commit / TRACK close (see ladder).
- **Plans:** [PLAN.md](../PLAN.md) north star; [TRACK_*.md](.) active work; **Planner** extends TRACK, **Driver** executes.

## Roles (see CONTINUITY)

- **Driver** — code + verify (most turns).
- **Planner** — TRACK rows from PLAN; **if tests red → enqueue Driver `test-fix` first, no new features**; no `compiler/`.
- **Backlog** — TRACK vs git hygiene; no `compiler/`.

## Verification ladder (compiler changes)

**Tier A — every Driver turn (target &lt;5 min warm):**
```bash
bash scripts/dev_gate_fast.sh
```
`run_tests` (reuse `compiler/out/tests/run_tests` if tests unchanged) + `mlcc --check-only main.mlc` if `out/mlcc` exists + arch lint. No fuzz / LSP / differential.

**Tier B — before commit, TRACK close, or after `compiler/**` / fuzz / LSP edits:**
1. `compiler/tests/build_tests.sh` → full gate (~30 min cold). Abort if &gt; 10 min silent.
2. If **`lib/mlc/**` touched:** `bundle exec rake test_mlc`.
3. If **`compiler/**` touched:** `compiler/build.sh` → self-host diff:
   ```bash
   compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
   compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
   .tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
   diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
   ```
   Use `build_bin.sh`, not bare `g++` — parallel + ccache.
4. Step **14** in TRACK: mandatory self-host before closing visitor batch.

**Tier C — CI / release:** `.github/workflows/build-mlcc-once.yml` or manual full Tier B on clean tree.
Also: `bash scripts/regression_gate.sh` — frozen stdout suite, then `scripts/run_examples_compile_sweep.sh` (compile+link every `fn main` under `misc/examples`/`misc/gui`; soft-skips missing sysdeps).

## Current priority

**Queue head:** Planner `PLAN` (authorized queue empty after §50 close).
**`TRACK_EDITOR_IDLE_CARET_BLINK` (§50)** **closed** 2026-07-22 (Critic OK). Archived.
**`TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP` (§49)** **closed** 2026-07-22 (Critic OK). Archived.
**`TRACK_UX_GATE_AUTO_DISCOVER` (§48)** closed 2026-07-22 (Critic OK); archived.
**`TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP` (§46 #37)** closed 2026-07-22 (Critic OK); archived.
**`TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU` (§46 #38)** closed 2026-07-22 (Critic OK); archived.
**`TRACK_EDITOR_UX_BACKLOG` (§46)** **done** 2026-07-22 — #1…#38 closed (#22 superseded).
**`TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE` (§47 #11)** closed 2026-07-22; archived.
**`TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE` (§47 #10)** closed 2026-07-21; archived.
**`TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE` (§47 #9)** closed 2026-07-21; archived.
**`TRACK_CONCURRENCY_FFI_METADATA` (§47 #8)** closed 2026-07-21; archived.
**`TRACK_CONCURRENCY_ATOMICS` (§47 #7)** closed 2026-07-21; archived.
**`TRACK_MLC_CONCURRENCY_REFINEMENT` (§47)** **done** 2026-07-22 — `#1`–`#11` closed.
**`TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED` (§47 #6)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING` (§47 #5)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_SYNC_TRAIT` (§47 #4)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_MOVE_TRACKING` (§47 #3)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_SEND_BOUND` (§47 #2)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK` (§47 #1)** closed 2026-07-20; archived.
**`TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE` (§46 #36)** closed 2026-07-20; archived.
**`TRACK_EDITOR_MINIMAP` (§46 #35)** closed 2026-07-20; archived.
**`TRACK_EDITOR_CONTENT_SCROLLBAR` (§46 #33e)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TREE_PARENT_DOUBLE_CLICK` (§46 #33d)** closed 2026-07-19; archived.
**`TRACK_EDITOR_CHROME_THEME_DRIFT` (§46 #33c)** closed 2026-07-19; archived.
**`TRACK_EDITOR_ROW_BYTE_RANGE_BLEED` (§46 #33b)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TREE_EXPAND_COLLAPSE` (§46 #33)** closed 2026-07-19; archived.
**`TRACK_EDITOR_CHROME_HOVER_STATE` (§46 #32)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TAB_STRIP_CLASSIC_LAYOUT` (§46 #31)** closed 2026-07-19; archived.
**`TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX` (§46 #31a)** closed 2026-07-19; archived.
**`TRACK_EDITOR_UX_L1_BACKFILL` (§46 #30)** closed 2026-07-19; archived.
**`TRACK_LANG_RECORD_UPDATE` (§46 #29)** closed 2026-07-19; archived.
**`TRACK_EDITOR_WIDE_CHAR_COLUMNS` (§46 #28)** closed 2026-07-19; archived.
**`TRACK_EDITOR_DROP_FILE` (§46 #27)** closed 2026-07-19; archived.
**`TRACK_EDITOR_INDENT_MODEL` (§46 #26)** closed 2026-07-19; archived.
**`TRACK_EDITOR_CONTEXT_MENU` (§46 #25)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TRAILING_WS_VIZ` (§46 #24)** closed 2026-07-19; archived.
**`TRACK_EDITOR_MULTI_CURSOR` (§46 #23)** closed 2026-07-19; archived.
**`TRACK_EDITOR_DIRTY_CLOSE_L1` (§46 #21)** closed 2026-07-19; archived.
**`TRACK_EDITOR_SESSION_CARET_RESTORE` (§46 #20)** closed 2026-07-19; archived.
**`TRACK_EDITOR_COMMENT_TOGGLE` (§46 #19)** closed 2026-07-19; archived.
**`TRACK_EDITOR_BRACKET_AUTOCLOSE` (§46 #18)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TAB_REORDER` (§46 #15)** closed 2026-07-18; archived.
**`TRACK_EDITOR_REPLACE` (§46 #14)** closed 2026-07-18; archived.
**`TRACK_EDITOR_ENCODING_GUARD` (§46 #13)** closed 2026-07-18; archived.
**`TRACK_EDITOR_CRLF_PRESERVE` (§46 #12)** closed 2026-07-18; archived.
**`TRACK_EDITOR_CURRENT_LINE_HL` (§46 #11)** closed 2026-07-18; archived.
**`TRACK_EDITOR_AUTO_INDENT` (§46 #10)** closed 2026-07-18; archived.
**`TRACK_EDITOR_DRAG_AUTOSCROLL` (§46 #9)** closed 2026-07-18; archived.
**`TRACK_EDITOR_GOTO_LINE` (§46 #8)** closed 2026-07-18; archived.
**`TRACK_EDITOR_FIND` (§46 #7)** closed 2026-07-18; archived.
**`TRACK_EDITOR_UNDO_COALESCE` (§46 #6)** closed 2026-07-18; archived.
**`TRACK_EDITOR_SHIFT_CLICK_EXTEND` (§46 #5)** closed 2026-07-18; archived.
**`TRACK_EDITOR_MOUSE_WORD_LINE_SELECT` (§46 #4)** closed 2026-07-18; archived.
**`TRACK_EDITOR_WORD_BOUNDARIES` (§46 #3)** closed 2026-07-18; archived.
**`TRACK_EDITOR_KEYBOARD_NAV_WIRE` (§46 #2)** closed 2026-07-18; archived.
**`TRACK_EDITOR_DEMO_ORCHESTRATOR` (§46 #1)** closed 2026-07-18; archived.
**`TRACK_EDITOR_CLEAN_ARCHITECTURE` (§45)** / §43 / §44 archived.
SCRIPT_VM / AUTO_CYCLE / MIR Epic 5 — gated; do not open.
## Step sizing (good vs bad)

| Good | Bad |
|------|------|
| Step 9: one arm + tests | «Migrate all expr_eval» |
| Planner adds steps 10–12 | Driver invents scope without TRACK |
| Backlog flags uncommitted > 15 | Silent TRACK drift |

## Code rules

- No abbreviations (`.cursor/rules/no-abbreviations.mdc`).
- Minimal diff; match surrounding style.

## Orchestration

Queued identical driver prompts; state in [SESSION.md](SESSION.md) `next`.
No MCP enqueue. Commits without asking the user.
