# Track: Editor Find (next/prev + match highlights)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#7**.
No find session, next/prev, or match highlights — only selection highlight and
syntax spans. Review gate: `find_highlights_matches` (L1).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1 + compile. Anti-false-done:
`febabd0f`…`9e102bdb` (STEP=0–2). Wire present: literal `find_session_set_query`
+ next/prev; `find_session_seed_from_selection`; `find_visible_highlights`;
CmdFind/Next/Prev; demo_live seed+draw; GLFW `f`/`f3`. **reopen: none**.

Honest residual: no find-panel chrome (Decision); query seed from selection only;
matches not auto-rebuilt on every edit (rebuild on Find commands); L1 exercises
find API not full Ctrl+F inject path.

| Gate | Result |
|------|--------|
| `run_ux_find_highlights_matches.sh` | `ux_ok find_highlights_matches` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#8 EDITOR_GOTO_LINE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `febabd0f` | Decision freeze + open |
| 1 | `7c5c8f7b` | L1 red harness |
| 2 | `9e102bdb` | find + commands + demo_live + GLFW f/f3 |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | No Find: cannot jump between occurrences or see all matches in the viewport |
| Match | Literal **case-sensitive** byte substring over document text. Empty query → zero matches. No regex; no whole-word (defer) |
| Query entry | **Seed from non-empty selection** on Find (Ctrl+F / `CmdFind`). Persist `FindSession.query`. Full find-panel chrome / status typing UI **out of scope** this track (keeps M); Replace is `#14` |
| Session | `FindSession { query, matches: [{start,finish}], active_index }` (+ rebuild matches when query/doc changes). Active match drives selection |
| Navigation | `find_next` / `find_prev` wrap around; set selection to match range; `editor_ux_ensure_caret_visible` (existing) |
| Highlight | Matches intersecting **visible** byte/line range → draw report rects (reuse selection-highlight shape); active match may use distinct color token. Scenario asserts highlight list, not pixels |
| Commands | `CmdFind` (seed query from selection if non-empty, rebuild, jump to first/next), `CmdFindNext` (F3), `CmdFindPrev` (Shift+F3). Wire in command bus + `demo_live` |
| Scenarios | L1: `find_highlights_matches` — fixed doc with ≥2 matches; set query; assert match count + visible-range highlight spans; next moves selection to following match |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` document/ux find + commands + `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok find_highlights_matches` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1 scenario first (`find_highlights_matches`) | **done** (red harness) |
| 2 | Find session + next/prev + visible highlights + demo/commands wire | **done** |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Replace (#14); find panel chrome / incremental query typing UI; regex; goto-line (#8); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
