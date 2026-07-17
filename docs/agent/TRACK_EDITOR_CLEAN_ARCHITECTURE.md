# Track: Editor development discipline — TDD, Clean Architecture, UX backlog

Parent: [../PLAN.md](../PLAN.md) §45. Origin: user request 2026-07-17 — put
`misc/editor` development "on proper rails": more TDD, Clean Architecture,
Clean Code, and full UX depth ("million small details"), formalized as mlc
tracks rather than ad-hoc polish turns (§36-43 pattern).

## Status: **active** (2026-07-17) — STEP=0 done (Decision), STEP=1 not authorized yet

## Why this track exists

§36-43 shipped real features (folder browser/nav, font config, UTF-8
columns, caret blink, word wrap) with a Decision→Driver→Driver→Critic cycle
each, and most already used `ux_scenarios/`/`tests/*_unit.mlc`. What was
missing: (1) no *standing* rule making scenario-first mandatory for every
future editor STEP — it happened by convention, not by gate; (2)
`demo_live.mlc` grew to 1521 lines with a single ~977-line `main()` (lines
545-1521) — the one clear Clean Architecture violation in an otherwise
well-layered tree (`document/`, `layout/`, `ui/`, `workspace/`, `ux/` are
all small, single-concern, under ~10 KB each); (3) no explicit backlog for
the "million small UX details" (multi-cursor, drag & drop, IME, find/
replace, session restore, crash-safety, encoding, …) — only what happened
to get a `ux_scenarios/*.mlc` so far (11 scenarios, L1-L9 partial).

## Decision (frozen 2026-07-17)

| Item | Choice |
|------|--------|
| Standing discipline | Frozen **now**, docs-only, in [../GUI_UX_TESTING.md](../GUI_UX_TESTING.md) §"Standing discipline" — applies to every `misc/editor/**` TRACK from this point, not just this one. No code change needed to freeze a rule. |
| `demo_live.mlc` decomposition | **Scoped, not started.** STEP=1 (next Driver turn on this track) extracts `main()`'s ~977 lines into named phase functions (`demo_live_input_phase`, `demo_live_layout_phase`, `demo_live_draw_phase`, or similar split — exact boundaries decided by the Driver reading the current loop body, not prescribed here) inside the same file first (behavior-preserving reshuffle, verified by running `scripts/run_editor_live_demo.sh` smoke + existing `ux_scenarios`/`tests` gate unchanged). Moving phases to separate files is a later sub-step once the split is proven behavior-identical. |
| UX completeness backlog | **Not hand-drafted here.** Source of truth will be a fresh external review (`mlc-support/responses/editor_tdd_ux_*.md`, Opus, 2026-07-17+) read by the next Planner turn on this track, which then opens `TRACK_EDITOR_UX_BACKLOG.md` with atomic numbered items (same granularity as §36-43, one Decision→Driver→Critic cycle each, not one giant "UX polish" track). Do not invent the backlog from memory without that review — text-editor UX gap lists drift fast and duplicate what `ux_scenarios/` already covers (see L1-L9 table in `GUI_UX_TESTING.md`). |
| Scope boundary | This track owns process/discipline + the `demo_live.mlc` decomposition. It does **not** implement new UX features — those are `TRACK_EDITOR_UX_BACKLOG` items opened later, gated the normal way. |

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Freeze standing discipline in `GUI_UX_TESTING.md`; open this track + PLAN §45 | **done** (2026-07-17) |
| 1 | Extract `demo_live.mlc` `main()` into named phase functions, same file, behavior-preserving; verify `run_editor_live_demo.sh` smoke + full `ux_scenarios`/`tests` gate unchanged | pending |
| 2 | Re-measure: any phase function still >150 lines or file >500 lines → split further or move to a new file under `misc/editor/app/` | pending |
| 3 | Planner: read `mlc-support/responses/editor_tdd_ux_*.md` (once generated) → open `TRACK_EDITOR_UX_BACKLOG.md` with numbered atomic items in `PLAN.md` queue | pending |
| 4 | Critic close: confirm STEP=1-2 behavior-identical (no scenario regressions), discipline section present in `GUI_UX_TESTING.md`, backlog track opened | pending, closes track |

## Non-goals (this track)

- No new editor features/UX fixes here — that is `TRACK_EDITOR_UX_BACKLOG` (opened at STEP=3).
- No rewrite of `ux_scenarios`/harness itself (`TRACK_UX_HEADLESS`, closed, is the frozen design).
- No change to `compiler/` — pure `misc/editor` + docs.
