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

## Current priority

**Critic** `critique-audit` on closed [TRACK_LANG_INT_OVERFLOW](../archive/tracks/TRACK_LANG_INT_OVERFLOW.md).
Then STDLIB_BACKEND (TCP/HTTP track — create before start).

## Step sizing (good vs bad)

| Good | Bad |
|------|-----|
| Step 9: one arm + tests | «Migrate all expr_eval» |
| Planner adds steps 10–12 | Driver invents scope without TRACK |
| Backlog flags uncommitted > 15 | Silent TRACK drift |

## Code rules

- No abbreviations (`.cursor/rules/no-abbreviations.mdc`).
- Minimal diff; match surrounding style.

## Orchestration

Queued identical driver prompts; state in [SESSION.md](SESSION.md) `next`.
No MCP enqueue. Commits without asking the user.
