# Agent roles (Orchestrator maintains)

**INSTRUCTIONS_REV:** `2026-05-28-cleaner`

| Role | STEP | Duty |
|------|------|------|
| **Cleaner** | `cleanup-sweep` | Junk files, stale docs; keep active agent docs |
| **Critic** | `critique-audit` | Re-check done steps vs commits/tests |
| **Orchestrator** | `roles-review` | Roles, cadence, ROLES.md |
| **Driver** | `<n>` | One TRACK sub-step, verify, commit |
| **Planner** | `plan-refresh` | TRACK from PLAN/RESEARCH |
| **Backlog** | `backlog-review` | git vs TRACK hygiene |
| **Meta** | `meta-review` | overnight log, supervisor |
| **Scribe** | every turn | SESSION, TRACK status |

## Rotation

| Every N | ROLE |
|---------|------|
| 16 | Orchestrator |
| 20 | Backlog |
| 12 | Meta |
| 10 | Cleaner |
| 8 | Planner (mlc only) |
| 6 | Critic |
| else | Driver |

Checklist: `cr/docs/agent/ROLES.md` (Cleaner section).
