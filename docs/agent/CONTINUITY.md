# Continuity (include in every queued agent prompt)

**Path:** `docs/agent/CONTINUITY.md` — paste in each enqueued message (after `AGENT_TOKEN=…`).

**INSTRUCTIONS_REV:** `2026-05-26-cr-session-watchdog` — bump when workflow/rules change.

## Turn start (before any work)

1. Read this file + [DEVELOPMENT.md](DEVELOPMENT.md) + active [TRACK_*.md](.) — **every turn** (no cache; instructions apply on the fly).
2. Compare `INSTRUCTIONS_REV` to `instructions_rev` in [SESSION.md](SESSION.md).
3. **If different:** first line of assistant reply (visible in chat):

   `Инструкции обновлены (rev <old> → <new>): <one-line summary of what changed>.`

   Then update `instructions_rev` in SESSION before enqueue.
4. Read `ROLE=` and `STEP=` from queued prompt, or pick role + step (see **Role rotation**).

## MCP `user-cr-cursor` — только через token

Self-методов (`cursor_enqueue_self`, `cursor_send_self`, `cursor_active_composer`) **нет**.  
Routing — token из `cursor_agent_register` bubble **или** `CR_AGENT_COMPOSER_ID` / `composerId` в register|enqueue. **Без CDP bind на active composer** (orch из другого чата иначе уходит не туда).

### Tools

| Tool | Args | Назначение |
|------|------|------------|
| `cursor_agent_register` | — | Выдать token (result → bubble чата в vscdb) |
| `cursor_agent_resolve` | `token` | token → composerId, workspace |
| `cursor_enqueue_send` | `token`, `text` | Следующий шаг в очередь composer |
| `cursor_send` | `token`, `text`, `queue?`, `queueOnBusy?` | CDP send / queue |
| `cursor_session` | `token` или `composerId` | **Health:** busy, queue, modal, окно |
| `cursor_snapshot` | `token` | Live CDP (окна, switch); если session мало |
| `cursor_get_chat` | `token` | История чата из БД |
| `cursor_list_chats` | — | Глобальный список (не для routing) |
| `cursor_db_info`, `cursor_refresh_db` | — | Служебные |

### Жизненный цикл token

1. **`cursor_agent_register`** → `cr-agent-…` (+ bind к active composer / `composerId` / `CR_AGENT_COMPOSER_ID`)
2. **`cursor_enqueue_send`** с тем же token — **сразу**, bind работает до записи в vscdb
3. **`cursor_agent_resolve({ token })`** — vscdb или bind
4. Следующие turn: token из tool result в vscdb (устойчиво между сессиями)

`token not found` без bind → `register` с `composerId: "f8e0a645-…"` или открыть mlc-чат и register без args (CDP bind).

### Начало turn

- Строка **`AGENT_TOKEN=cr-agent-…`** в сообщении → использовать её.
- Иначе **`cursor_agent_register`**.

### Конец turn (обязательно — не останавливаться)

1. **Scribe** — update [SESSION.md](SESSION.md) (template in file; mandatory).
2. `cursor_agent_register` (из **этого** mlc-чата — bind по CDP)
3. `cursor_enqueue_send({ token, text })` — **в том же turn**, не ждать vscdb
4. Опционально `cursor_agent_resolve` — проверка workspace

```
AGENT_TOKEN=cr-agent-…
INSTRUCTIONS_REV=2026-05-26-cr-session-watchdog
ROLE=Driver|Planner|Backlog
STEP=<track-sub-step|plan-refresh|backlog-review>
@docs/agent/CONTINUITY.md
@docs/agent/DEVELOPMENT.md
@docs/agent/TRACK_<name>.md

<one verifiable sub-step for this role>
```

Token дублируется: tool result в vscdb + первая строка промпта.

При agent busy: только `cursor_enqueue_send` (не `cursor_send`).

### cr-watchdog (in-process)

Poll в том же процессе, что `npm run dev` / `npm start` (modals + drain + observe windows + slow recover).

```bash
npm run dev   # watchdog автоматически
CR_WATCHDOG=0 npm run dev   # без watchdog
```

Tab **watchdog** в UI или `GET /api/watchdog/stats`. Standalone: `npm run watchdog:start` (legacy socket).

**`Taking longer than expected…`** под баблом (`.ui-collapsible-shimmer`, пара `composer-human-ai-pair-container`):
1. Подождать `CR_WATCHDOG_SLOW_MS` (default 90s)
2. Stop → dismiss modal (Don't revert) → resubmit если в draft есть `AGENT_TOKEN=`
3. Статистика: `npm run watchdog:stats` → `windows`, `slow_recoveries_total`, `/log`

Отключить auto-recover: `CR_WATCHDOG_SLOW_RECOVER=0`. Таймауты: `CR_WATCHDOG_SLOW_MS=90000`, `CR_WATCHDOG_BUSY_MS=600000`.  
Панель: `curl --unix-socket ~/.cursor/cr-watchdog.sock http://localhost/`

**Backlog turn:** если `watchdog:status` fail → enqueue `STEP=cr-watchdog-start` (Driver: start + status в SESSION).

### Heartbeat (~каждые 3 шага)

`register` → `cursor_session({ token })` → enqueue короткий промпт + SESSION (uncommitted, verify, `queueLength`, `modal`).

### Health при сбое

1. Новый `register`
2. `cursor_session({ token })` — смотреть `agent.busy`, `queueLength`, `modal`
3. `modal !== "none"` или queue застряла → убедиться что watchdog жив; иначе `watchdog:start`
4. Retry `cursor_enqueue_send` once
5. Fail → log SESSION, mlc-memory blocker; stop только по правилам ниже

## Hard limits (stop and fix — do not enqueue next sub-step)

| Limit | Action |
|-------|--------|
| Uncommitted diff **> 15 files** (или > 5 junk: `.cursor/`, `test_t*.mlc`, `*.tmp`) | Commit-prep or revert junk; split sub-step |
| **`compiler/` + `lib/mlc/`** in one sub-step | Split; one layer per sub-step |
| Debug artifacts in `compiler/` root (`lexer_*.mlc`, `test_*.mlc`) | Delete or move to `.tmp_*`; never commit |
| Verify running **> 10 min** without output | Kill process; log in SESSION; enqueue `STEP=debug-verify` с описанием последней команды |
| **3 verify fails** on same sub-step | Pause; log blocker in mlc-memory + SESSION; enqueue analysis sub-step |
| SESSION not updated this turn | Update before enqueue |

## When to stop (only these)

- Queue broken after retry + unrecoverable session error (CDP down и watchdog не помог)
- Verify fail, root cause unclear after one debug pass **and** blocker logged
- User said stop / destructive action without approval

Иначе: fix → verify → **register + enqueue** следующий sub-step из TRACK/PLAN.

## Commits

Verified sub-step → TRACK update → **commit самостоятельно**, не ждать команды user.

Commit message: `step N: <one line>` (английский, без Co-authored-by).  
После коммита — сразу enqueue следующий sub-step.

Cross-cutting work: prefer WIP branch (`feat/…`), not large dirty tree on `main`.

## Session stats (`SESSION.md`)

`instructions_rev`, `agent_token_last`, `step_last`, uncommitted_files, verify result, queue ok/fail, tests, self-host diff.

SESSION хранит последние **5 turn**; старые архивируются в блок `### Archive` внизу файла (без удаления).

## Roles

| Role | Duty | Touches `compiler/`? |
|------|------|----------------------|
| **Driver** | One TRACK sub-step → verify → commit-prep enqueue | yes |
| **Verifier** | (with Driver) build_tests; + test_mlc if lib/mlc; self-host if compiler/** | — |
| **Planner** | Extend TRACK from PLAN; mark done; split oversized steps | **no** |
| **Backlog** | PLAN vs TRACK vs git; blockers; hygiene flags | **no** |
| **Scribe** | SESSION every turn; TRACK status; mlc-memory | docs only |

### Role rotation (same chat, alternate turns)

Read `driver_turns_since_plan` from [SESSION.md](SESSION.md) (Scribe maintains).

| Condition | Next `ROLE=` |
|-----------|--------------|
| No open `pending` step in TRACK | **Planner** (`STEP=plan-refresh`) |
| Every **8** Driver turns | **Planner** |
| Every **20** Driver turns | **Backlog** (`STEP=backlog-review`) |
| Otherwise | **Driver** (next pending sub-step) |

After Planner or Backlog turn → enqueue **Driver** with next `STEP=`.

**Planner не повторяется:** если TRACK уже содержит numbered pending шаг → сразу `ROLE=Driver STEP=<тот шаг>`, не plan-refresh.

Planner/Backlog **must not** edit `compiler/` or `lib/mlc/` — docs + mlc-memory only.

### Planner turn (`STEP=plan-refresh`)

1. Read [PLAN.md](../PLAN.md) (north star) + active TRACK.
2. Append missing sub-steps (numbered, one concern each, verify gate).
3. Mark completed steps `done` with commit hash if missing.
4. Update **Next step** section; do not delete deferred items.
5. SESSION: `driver_turns_since_plan=0`; enqueue Driver.

### Backlog turn (`STEP=backlog-review`)

1. `git status -sb`, uncommitted file count, `origin/main..HEAD`.
2. TRACK steps marked done but no matching commit → fix status.
3. Blockers → mlc-memory `known_limitations` or `notes`.
4. Flag: uncommitted > 15, self-host stale, debug junk in `compiler/`.
5. No code changes; enqueue Driver or Planner if TRACK empty.

### Driver turn

Pick lowest **pending** step in TRACK; combine with Verifier + Scribe as today.
