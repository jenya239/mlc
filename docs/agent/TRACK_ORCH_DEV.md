# Track: Orchestrator / Multi-agent System Development

Parent: [../PLAN.md](../PLAN.md) �9; parallel to all MLC phases

## Status: **closed** (2026-06-05)


**STEP=4 note (2026-06-05 Driver):** `AgentTarget.transport` + `paneId`; `tmux-observe`; unified `batchTick` CDP+tmux. Gate: step tests 14/14; build ok; npm test 331/344 (13 pre-existing). Commit `e271c82` (cr).

**STEP=6 note (2026-06-05 Driver):** `meetings/parse-meeting`, `db/meetings`, `meetings/sync` + fs.watch; `listMeetings` DB+dir; progress tab section. Gate: meetings tests 5/5; build ok; npm test 331/345 (14 pre-existing). Commit `daa31ec` (cr).

**STEP=5 note (2026-06-05 Driver):** `session/parse-turns`, `db/turns`, `session/sync` (fs.watch); `loadCachedSessionTurns` in progress report. Gate: session tests 3/3; build ok; npm test 326/340 (14 pre-existing). Commit `fbc130c`.

**STEP=3 note (2026-06-05 Driver):** `src/tmux/` � `listPanes`, `sendKeys`, `capturePaneOutput` (injectable `TmuxRunner`); `registerAgent` ? `agent_states` upsert. Gate: tmux tests 6/6; build ok; full `npm test` 324/337 (13 pre-existing). Commit `ed84883`.

**STEP=2 note (2026-06-05 OrchestratorDev):** `recordEnqueueCost` on enqueueSend; `GET /api/billing`; billing UI tab. Gate: billing tests 8/8; commit `9ced441`.

**STEP=1 note (2026-06-05 OrchestratorDev):** `db/schema.sql`, `db/migrations/001_initial.sql`, `src/db/migrate.ts` + `cr-paths.ts`; gate: migrate tests 3/3, `npm run build` ok; full `npm test` 311/325 (14 pre-existing fails).

**Goal:** Develop `cr` server and multi-agent orchestration in parallel with MLC compiler work. ~20% of total Driver turns (every 5th mlc Driver turn ? OrchestratorDev turn on cr workspace).

## Architecture

**Transport:** tmux (for terminal agents � Claude, custom scripts). Cursor agents via CDP (existing).  
**Storage:** hybrid � SQLite (cr/db/) for mutable/queryable data; git markdown ??? ?????????? ? ??????.

| Layer | Storage | Examples |
|-------|---------|---------|
| Instructions, plans, tracks | markdown + git | PLAN.md, TRACK_*.md, ROLES.md, SESSION.md |
| Mutable state, metrics, cache | SQLite (`cr/db/cr.db`) | turns, costs, meetings, agent states |

## Verify gate (every step)

```bash
cd /home/jenya/workspaces/current/cr
npm test
npm run build
```

---

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | DB schema: `cr/db/schema.sql` � tables `turns`, `cost_entries`, `agent_states`, `meetings`; migration runner ? `cr/src/db/` | done |
| 2 | Billing: ?????? `cost_entries` ??? ?????? enqueue (cursor_usage context%, ??????, timestamp); web UI tab `/billing` | done |
| 3 | tmux transport: `cr/src/tmux/` � `listPanes()`, `sendKeys(pane, text)`, `capturePaneOutput(pane, lines)`, `registerAgent(pane, role, token)` | done |
| 4 | Multi-agent targets: transport cdp|tmux + unified watchdog | done |
| 5 | Session DB cache: sync SESSION.md turns ? SQLite `turns` table on fs.watch; progress reads from DB | done |
| 6 | Meeting rooms: docs/agent/meetings + listMeetings + progress UI | done |

## Notes

- **DB:** `better-sqlite3` (sync API, zero deps friction). Schema migrations � ??????? numbered SQL ??????.
- **Billing approx:** Cursor ?? ?????? ?????? ?????? � ???????????? `cursor_usage` context% ??? ??????. Claude/OpenAI API responses ???????? exact counts ? ??????????.
- **tmux:** `tmux new-session -d -s <name>` � detached, ????? ????? SSH disconnect. Watchdog: `send-keys -t <pane> "prompt" Enter`; `capture-pane -pt <pane>` (??? `-e`) � ?????? ????? ??? escape. TUI-???????? (?????, clipboard, Shift+Enter) ???????????? ??? ???????????? ??????????. ????????????? "????? ????????": polling capture-pane ?? ???????????? ?????? (N ?????? ??? ?????????) ??? ????? known prompt-?????? ??????. ???? ????? ???????????? `--no-tui`/`--print` � ???????????.
- **Git + DB:** markdown = source of truth ??? Agent-readable ??????????; DB = ??????????? ??????, ????? ???????? ? ??????????? ?? SESSION.md / TRACK_*.md.
- All steps: cr workspace only. No changes to `compiler/` or `lib/mlc/`.
