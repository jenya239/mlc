# Agent roles (Orchestrator maintains)

**INSTRUCTIONS_REV:** `2026-05-28-cleaner`

| Role | STEP | Duty |
|------|------|------|
| **Cleaner** | `cleanup-sweep` | Junk files, stale docs; keep active agent docs |
| **Critic** | `critique-audit` | Re-check done steps vs commits/tests |
| **Auditor** | `doc-audit` | Найти противоречия между доками и реализацией; см. [AUDITOR.md](AUDITOR.md) |
| **Orchestrator** | `roles-review` | Roles, cadence, ROLES.md |
| **Driver** | `<n>` | One TRACK sub-step, verify, commit |
| **Planner** | `plan-refresh` | TRACK from PLAN/RESEARCH; **tests red → Driver `test-fix` first** |
| **Backlog** | `backlog-review` | git vs TRACK hygiene |
| **Meta** | `meta-review` | overnight log, supervisor |
| **Researcher** | `research-turn` | Взять тему из RESEARCH.md, исследовать, возможно создать новый трек; см. [RESEARCHER.md](RESEARCHER.md) |
| **Reviewer** | `history-review` | Просмотреть последние закрытые треки: что упустили, что нужно создать как новый трек; см. [REVIEWER.md](REVIEWER.md) |
| **Monitor** | `process-monitor` | Проверить системные индикаторы: SESSION обновляется, треки прогрессируют, логи пишутся, ротация работает, планы актуальны; см. [MONITOR.md](MONITOR.md) |
| **Blogger** | `blog-post` | пост в блог о закрытых треках; см. BLOG.md |
| **OrchestratorDev** | `orch-dev-N` | Развитие cr/оркестратора: billing, meetings, tmux, DB; берёт шаги из TRACK_ORCH_DEV.md; работает в cr workspace | 
| **Scribe** | every turn | SESSION, TRACK status |

## Rotation

| Every N | ROLE |
|---------|------|
| 16 | Orchestrator |
| 20 | Backlog |
| 12 | Meta |
| 10 | Cleaner |
| 25 | Blogger (mlc only) |
| 8 | Planner (mlc only) |
| 6 | Critic |
| 30 | Auditor |
| 40 | Researcher |
| 35 | Reviewer |
| 5 | OrchestratorDev |
| 18 | Monitor |
| else | Driver |

Checklist: `cr/docs/agent/ROLES.md` (Cleaner section).
