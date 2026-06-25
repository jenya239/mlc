# Track: Turn audit log (who / when / why)

Parent: [../PLAN.md](../PLAN.md); [TRACK_ORCH_DEV.md](TRACK_ORCH_DEV.md)

## Status: **closed** (STEP=4 done)

| Step | Item | Status |
|------|------|--------|
| 1 | Guard writes `TURNLOG.jsonl` | done |
| 2 | Sync TURNLOG → cr.db `turn_audit_events` | done |
| 3 | Progress dashboard: TURNLOG section | done |
| 4 | CONTINUITY: SESSION `done` after turn | done |

---

## Sources

| Source | Role |
|--------|------|
| `docs/agent/TURNLOG.jsonl` (git) | append-only machine audit |
| `SESSION.md` (git) | human done/verify |
| `cr.db turn_audit_events` | queryable mirror |
| mlc-memory | decisions only, not per-turn |

---

## Gate

```bash
bash scripts/turn_audit_gate.sh
```

Checks: valid `TURNLOG.jsonl`; latest SESSION turn has concrete `done` + `verify`/`result`; optional `cr.db` row count.

Lint only:

```bash
python3 scripts/session_turn_lint.py
```

---

## cr.db schema

Table `turn_audit_events`: `ts`, `event`, `role`, `step`, `track`, `why`, `prompt_key`, `db_status`, …

Sync: guard tick + `startTurnLogWatcher` + progress report load.

---

## Query

```bash
tail -20 docs/agent/TURNLOG.jsonl | jq .
sqlite3 ~/.cursor/cr.db "SELECT ts,event,role,step,why FROM turn_audit_events ORDER BY ts DESC LIMIT 10;"
```
