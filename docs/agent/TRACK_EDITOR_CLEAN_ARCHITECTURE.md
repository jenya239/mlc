# Track: Editor development discipline — TDD, Clean Architecture, UX backlog

Parent: [../PLAN.md](../PLAN.md) §45. Origin: user request 2026-07-17 — put
`misc/editor` development "on proper rails": more TDD, Clean Architecture,
Clean Code, and full UX depth ("million small details"), formalized as mlc
tracks rather than ad-hoc polish turns (§36-43 pattern).

## Status: **active** (2026-07-17) — STEP=0, STEP=3 done; STEP=1 refined (see below), not started

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

## Decision (frozen 2026-07-17, refined 2026-07-17 post-Opus-review)

External review: [`mlc-support/responses/editor_tdd_ux_20260717_114221.md`](../../../mlc-support/responses/editor_tdd_ux_20260717_114221.md)
(Opus, $2.24). Confirms the process gap and **refines** the STEP=1 fix below —
read it before touching `demo_live.mlc`, do not implement from this table alone.

| Item | Choice |
|------|--------|
| Standing discipline | Frozen, docs-only, in [../GUI_UX_TESTING.md](../GUI_UX_TESTING.md) §"Standing discipline" — applies to every `misc/editor/**` TRACK from this point. Review found §38/§40/§42/§43 closed on `*_unit`/compile-gate only, no L1 scenario, despite being user-visible behavior — retroactive backfill is backlog item `EDITOR_UX_L1_BACKFILL` (§46 queue), not redone here. |
| `demo_live.mlc` fix — **refined, supersedes original STEP=1 wording** | Original plan ("extract `main()` into named phase functions, same file") is **insufficient** per review §2.2: `demo_live.mlc:545-1521` does not just need splitting, it **duplicates** logic that already exists and is tested in `ux/edit_apply.mlc`, `ux/scroll.mlc`, `ux/tab_strip.mlc`, `ux/tree_hit.mlc` (inline command dispatch ~1069-1200, inline text/enter/backspace handling ~1257-1291, inline tree/breadcrumb hit-test ~1150-1420 — evidence from review). Real STEP=1 target: `demo_live.main()` becomes `poll → editor_app_*(app, input) → draw(app)`; every inline mutation gets replaced by a call to the existing tested `ux/*`/`app/*` function, not reimplemented inline. This is `EDITOR_DEMO_ORCHESTRATOR` (§46 backlog #1) — same track, just renamed/refined, not a new one. |
| Record-copy boilerplate | Review flags `EditorAppState`/`EditorUxState` full-record-copy-per-mutation (~350+ lines, 5 duplicated `text_viewport_rect` copies) as a real bug risk (silently dropped field on new state additions), not just style. Candidate compiler track `LANG_RECORD_UPDATE` (functional record-update syntax) — §46 backlog #29, **not required** before §46 #1-2, but should land before `EDITOR_MULTI_CURSOR` (#23) adds another state field multiplying the copies. |
| UX completeness backlog | **Done** — see [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) (PLAN §46), 30 atomic items sourced from the review, ordered by user value. |
| Scope boundary | This track owns process/discipline + framing the `demo_live.mlc` fix. Actual `EDITOR_DEMO_ORCHESTRATOR` execution (renaming inline logic to `ux/*` calls) happens as backlog item §46 #1, gated normally (TDD-scenario-first, Critic). |

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Freeze standing discipline in `GUI_UX_TESTING.md`; open this track + PLAN §45 | **done** (2026-07-17) |
| 1 | ~~Extract `main()` into phase functions~~ **superseded** — real fix is `EDITOR_DEMO_ORCHESTRATOR`, tracked as §46 backlog #1, not executed from this track directly | **reframed, not started** |
| 2 | (was: re-measure file size) — folded into §46 #1's own Critic gate | **superseded** |
| 3 | Planner: ran external review (`mlc-support/responses/editor_tdd_ux_20260717_114221.md`) → opened `TRACK_EDITOR_UX_BACKLOG.md`, PLAN §46 | **done** (2026-07-17) |
| 4 | Critic close: confirm discipline section present in `GUI_UX_TESTING.md`, backlog track opened with correct ordering, STEP=1/2 reframing recorded (not silently dropped) | pending, closes track |

## Non-goals (this track)

- No new editor features/UX fixes here — that is `TRACK_EDITOR_UX_BACKLOG` (opened at STEP=3).
- No rewrite of `ux_scenarios`/harness itself (`TRACK_UX_HEADLESS`, closed, is the frozen design).
- No change to `compiler/` — pure `misc/editor` + docs.
