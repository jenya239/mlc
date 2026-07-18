# Track: Editor Replace (one + all on Find)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#14**.
Find (#7) has query/next/prev/highlights; no replace-one / replace-all.
Review gate: `replace_one_and_all` (L1). Size **M**.

## Status: **active** (2026-07-18) — STEP=0–1 done; next STEP=2

## Next step

**STEP=2** — replace-one/all API + `CmdReplaceOne`/`CmdReplaceAll` + demo_live wire.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Cannot replace find matches; only navigate/highlight |
| Base | Reuse `FindSession` + literal case-sensitive matches from `#7`. No regex |
| Replacement | Add `replacement: string` on `FindSession` (default `""`). Set via `find_session_set_replacement(session, replacement)` |
| Replace one | If `active_index` valid: delete match range, insert `replacement` (edit path / piece-table); rebuild matches on new text; select next match after edit (or none). Empty query / no matches → no-op |
| Replace all | Apply replacements **from last match to first** (stable offsets); one rebuild after; selection on last inserted range or collapsed at end of last replace. Empty → no-op |
| History | Each replace-one = one undo step; replace-all = **one** undo step (single history push before bulk edit) |
| Query/replacement entry | No replace-panel chrome (same as Find). L1 sets query+replacement via API. Live: `CmdReplaceOne` / `CmdReplaceAll`; seed `replacement` from **clipboard** when non-empty on those commands (query still from Find / selection seed) |
| Commands | `CmdReplaceOne` (Ctrl+H), `CmdReplaceAll` (Ctrl+Shift+H). Wire bus + `demo_live` (+ GLFW if needed) |
| Scenarios | L1: `replace_one_and_all` — doc with ≥3 literal matches; set query+replacement; replace_one → text/count; replace_all → all replaced; assert via snapshot text |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux/find (+ edit/history), commands, `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok replace_one_and_all` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_find_highlights_matches.sh` → `ux_ok find_highlights_matches` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1 scenario first (`replace_one_and_all`) | **done** (2026-07-18) — stubs red: `ux_fail replace_one text` |
| 2 | replace-one/all API + commands + demo_live wire | pending |
| 3 | Critic: gates; archive | pending |

## Out of scope

Replace panel chrome; regex/whole-word; case-insensitive; preview count UI; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
