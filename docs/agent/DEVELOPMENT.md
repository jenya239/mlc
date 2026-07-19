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

**Queue head:** Driver `STEP=3` `TRACK_EDITOR_INDENT_MODEL` (§46 `#26`).
**`TRACK_EDITOR_INDENT_MODEL` (§46 #26)** active; STEP=2 done (L0+L1 green); Decision frozen.
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
