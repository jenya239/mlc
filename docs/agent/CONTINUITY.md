# Continuity (include in every queued agent prompt)

**Path:** `docs/agent/CONTINUITY.md` — paste in each enqueued message (after `AGENT_TOKEN=…`).

## MCP `user-cr-cursor` — только через token

Self-методов (`cursor_enqueue_self`, `cursor_send_self`, `cursor_active_composer`) **нет**.  
Routing — **только** по `token` из `cursor_agent_register`.

### Tools

| Tool | Args | Назначение |
|------|------|------------|
| `cursor_agent_register` | — | Выдать token (result → bubble чата в vscdb) |
| `cursor_agent_resolve` | `token` | token → composerId, workspace |
| `cursor_enqueue_send` | `token`, `text` | Следующий шаг в очередь composer |
| `cursor_send` | `token`, `text`, `queue?`, `queueOnBusy?` | CDP send / queue |
| `cursor_snapshot` | `token` | Live: busy, окно, composer |
| `cursor_get_chat` | `token` | История чата из БД |
| `cursor_list_chats` | — | Глобальный список (не для routing) |
| `cursor_db_info`, `cursor_refresh_db` | — | Служебные |

### Жизненный цикл token

1. **`cursor_agent_register`** → `cr-agent-…`
2. **`cursor_agent_resolve({ token })`** → этот mlc-чат (`composerId`, `workspacePath`)
3. Любой send/snapshot/get_chat — **с тем же `token`**

`token not found` → новый `register`. Не подставлять чужой `composerId` вручную.

### Начало turn

- Строка **`AGENT_TOKEN=cr-agent-…`** в сообщении → использовать её.
- Иначе **`cursor_agent_register`**.

### Конец turn (обязательно — не останавливаться)

1. `cursor_agent_register`
2. `cursor_agent_resolve({ token })` — check mlc workspace
3. `cursor_enqueue_send({ token, text })`:

```
AGENT_TOKEN=cr-agent-…
@docs/agent/CONTINUITY.md
@docs/agent/DEVELOPMENT.md
@docs/agent/TRACK_<name>.md

<one verifiable step>
```

Token дублируется: tool result в vscdb + первая строка промпта.

При agent busy: `cursor_send({ token, text, queue: true })`.

### Heartbeat (~каждые 5 шагов)

`register` → `resolve` → enqueue короткий промпт с новым `AGENT_TOKEN=…` + update `SESSION.md`.

### Health при сбое

1. Новый `register`
2. `cursor_snapshot({ token })`
3. Retry `cursor_enqueue_send` once
4. Fail → log `SESSION.md`, heartbeat; stop только по правилам ниже

## When to stop (only these)

- Queue broken after retry + unrecoverable snapshot error
- Verify fail, root cause unclear after one debug pass
- User said stop / destructive action without approval

Иначе: fix → verify → **register + enqueue** (без вопросов пользователю).

## Commits

Verified unit → TRACK update → enqueue commit-prep (commit только по команде user).

## Session stats (`SESSION.md`)

`agent_token_last`, resolve ok/fail, queue ok/fail, tests, self-host diff.

## Roles

| Role | Duty |
|------|------|
| Driver | TRACK + register + `cursor_enqueue_send` |
| Verifier | build, run_tests, mlcc2 diff |
| Scribe | SESSION, TRACK, mlc-memory |
