# Track: Editor Find (next/prev + match highlights)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#7**.
No find session, next/prev, or match highlights — only selection highlight and
syntax spans. Review gate: `find_highlights_matches` (L1).

## Status: **active** (2026-07-18) — STEP=1 done; STEP=2 next

## Next step

**STEP=2** — Implement find scan + next/prev + highlights; wire commands/demo.

### STEP=1 done (2026-07-18)

- Stub `ux/find.mlc` (empty matches); L1 red harness `find_highlights_matches`
  (+ run script). Note: match field `finish` (not `end` — codegen)

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #7 → active.

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
| 2 | Find session + next/prev + visible highlights + demo/commands wire | scenario + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2**
1. Implement find match scan + session + highlight report; wire CmdFind/Next/Prev + `demo_live`.
2. Gates: scenario ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #8).

## Out of scope

Replace (#14); find panel chrome / incremental query typing UI; regex; goto-line (#8); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
