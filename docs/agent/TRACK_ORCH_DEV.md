# Track: Orchestrator / Multi-agent System Development

Parent: [../PLAN.md](../PLAN.md) §9; parallel to all MLC phases

## Status: **open** (step 2 done â€” 2026-06-05)— 2026-06-05)

**STEP=2 note (2026-06-05 OrchestratorDev):** `recordEnqueueCost` on enqueueSend; `GET /api/billing`; billing UI tab. Gate: billing tests 8/8; commit `9ced441`.

**STEP=1 note (2026-06-05 OrchestratorDev):** `db/schema.sql`, `db/migrations/001_initial.sql`, `src/db/migrate.ts` + `cr-paths.ts`; gate: migrate tests 3/3, `npm run build` ok; full `npm test` 311/325 (14 pre-existing fails).

**Goal:** Develop `cr` server and multi-agent orchestration in parallel with MLC compiler work. ~20% of total Driver turns (every 5th mlc Driver turn ? OrchestratorDev turn on cr workspace).

## Architecture

**Transport:** tmux (for terminal agents — Claude, custom scripts). Cursor agents via CDP (existing).  
**Storage:** hybrid — SQLite (cr/db/) for mutable/queryable data; git markdown ??? ?????????? ? ??????.

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
| 1 | DB schema: `cr/db/schema.sql` — tables `turns`, `cost_entries`, `agent_states`, `meetings`; migration runner ? `cr/src/db/` | done |
| 2 | Billing: ?????? `cost_entries` ??? ?????? enqueue (cursor_usage context%, ??????, timestamp); web UI tab `/billing` | done |
| 3 | tmux transport: `cr/src/tmux/` — `listPanes()`, `sendKeys(pane, text)`, `capturePaneOutput(pane, lines)`, `registerAgent(pane, role, token)` | pending |
| 4 | Multi-agent targets: extend `AGENT_TARGETS` ? `transport: 'cdp' \| 'tmux'` ? `paneId?`; unified watchdog loop | pending |
| 5 | Session DB cache: sync SESSION.md turns ? SQLite `turns` table ??? ????????? ????? (fs.watch); ????????-?????? ?????? ?? DB | pending |
| 6 | Meeting rooms: `docs/agent/meetings/YYYY-MM-DD-<topic>.md`; `listMeetings()` ?????? ?? DB+dir; render ? web UI progress tab | pending |

## Notes

- **DB:** `better-sqlite3` (sync API, zero deps friction). Schema migrations — ??????? numbered SQL ??????.
- **Billing approx:** Cursor ?? ?????? ?????? ?????? — ???????????? `cursor_usage` context% ??? ??????. Claude/OpenAI API responses ???????? exact counts ? ??????????.
- **tmux:** ????? ?????????????? ????? `tmux new-session -d -s <name>`, ???????? pane ID. cr watchdog: `tmux capture-pane -pt <pane>` ??? ???????????, `tmux send-keys -t <pane> "<prompt>" Enter` ??? ????????.
- **Git + DB:** markdown = source of truth ??? Agent-readable ??????????; DB = ??????????? ??????, ????? ???????? ? ??????????? ?? SESSION.md / TRACK_*.md.
- All steps: cr workspace only. No changes to `compiler/` or `lib/mlc/`.
