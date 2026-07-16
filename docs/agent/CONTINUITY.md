# Continuity (paste into every turn placed in the Cursor queue)

**Path:** `docs/agent/CONTINUITY.md`.

**INSTRUCTIONS_REV:** `2026-07-16-glfw-content-scale` — bump when workflow/rules change.

Orchestration: **обычная очередь сообщений Cursor** (оператор вручную ставит в очередь N одинаковых копий driver-промпта). Никакого MCP-роутинга, токенов, CDP, watchdog — этот подход (`agent-loop`/`cr`) отменён, архив: `docs/archive/CONTINUITY_AGENT_LOOP_MCP.md`, `docs/archive/TRACK_ORCH_DEV.md`.

## Turn start (before any work)

1. Read this file + [DEVELOPMENT.md](DEVELOPMENT.md) + active [TRACK_*.md](.) — **every turn** (no cache; instructions apply on the fly).
2. Consult [../specs/index.md](../specs/index.md) — load tagged product docs **on demand** only (match active track; skip bulk `docs/` reads).
3. Compare `INSTRUCTIONS_REV` below to `instructions_rev` in [SESSION.md](SESSION.md).
4. **If different:** one line `Инструкции обновлены (rev <old> → <new>): <summary>.` → patch SESSION → **continue STEP** (rev sync is not a turn; do not stop).
5. Read `ROLE=`/`STEP=`/`TRACK=` from **`next`** field of the last turn in [SESSION.md](SESSION.md) — the queued prompt text is identical every time, state lives in SESSION, not in the prompt. If `next` is missing/stale, pick role + step (see **Role rotation**).
6. If TRACK marks that STEP `done` already → move to next pending STEP (idempotent; a stale/duplicate queued message must not redo work).
7. **Не доверять `done` в TRACK/SESSION слепо.** Если STEP помечен `done`, но `git log --oneline -1 -- <заявленные файлы>` не находит коммит с этими изменениями (например работа осталась uncommitted на границе сессий) — считать STEP **не сделанным**, откатить статус на `open`, переделать и закоммитить заново. Инцидент 2026-07-09: `TRACK_LANG_CLOSURE_ESCAPE` STEP=1 был помечен done без коммита, следующая сессия унаследовала пустое дерево — поймал только Cleaner на STEP=cleanup-sweep, а не Driver на старте следующего turn.
8. **`compiler/out/mlcc` в рабочем дереве уже modified не тобой** (другой процесс/предыдущий turn ещё не закоммитил свою пересборку) → не трогать, не коммитить, не откатывать; работать поверх, коммитить только свои файлы явным списком в `git add` (не `git add -A`/`git commit -am`).

## Конец turn (обязательно — не останавливаться)

1. **Scribe** — update [SESSION.md](SESSION.md): заполнить `| done |` (конкретные файлы/функции), `| verify |` или `| result |`, и `| next |` (ROLE=/STEP=/TRACK для следующего turn). Пустой или placeholder `done`/`next` — нарушение.
2. Commit + push (см. **Commits**).
3. Stop. Не звать никакие enqueue/register-инструменты — их нет. Следующая копия промпта уже стоит в очереди Cursor и по приходу прочитает `next` из SESSION.md.

Queued prompt (тот же текст в каждом сообщении очереди):

```
INSTRUCTIONS_REV=2026-07-16-glfw-content-scale
@docs/agent/CONTINUITY.md
@docs/agent/DEVELOPMENT.md
@docs/agent/SESSION.md

Прочитай `next` из последней записи SESSION.md — это ROLE/STEP/TRACK для этого turn.

Перед работой: `git status` + `git log --oneline -15`. Чужой uncommitted diff (`compiler/out/mlcc`, SCRIPT_VM already in-tree design-only, `.tmp/**`) — не удалять чужое; `git add` явным списком. Анти-false-done / анти-stale-docs — как в CONTINUITY.md. После правок `lib/mlc/` — `scripts/regression_gate.sh` перед Critic close.

Выполни один проверяемый sub-step. Смена status/STEP TRACK → тем же коммитом `docs/PLAN.md`. Закрытие трека → `next` = Critic на этот трек. `SESSION.md` > ~600 строк → архив. Не `git add -f` бинарники.

Обнови SESSION.md (`done`/`verify`/`next`). Commit + push. Stop.
```

## Hard limits (stop and fix — do not move to next sub-step)

| Limit | Action |
|-------|--------|
| Uncommitted diff **> 15 files** (или > 5 junk: `.cursor/`, `test_t*.mlc`, `*.tmp`) | Commit-prep or revert junk; split sub-step |
| **`compiler/` + `lib/mlc/`** in one sub-step | Split; one layer per sub-step |
| Debug artifacts in `compiler/` root (`lexer_*.mlc`, `test_*.mlc`) | Delete or move to `.tmp_*`; never commit |
| Verify running **> 10 min** without output | Kill process; log in SESSION; `next` = `STEP=debug-verify` с описанием последней команды |
| **3 verify fails** on same sub-step | Pause; log blocker in mlc-memory + SESSION; `next` = analysis sub-step |
| SESSION not updated this turn | Update before commit |
| SESSION `done` empty / placeholder | Fill concrete `done` + verify; run `scripts/session_turn_lint.py` |
| SESSION.md > **~600 строк** | Архивировать старые turn's в `SESSION_HISTORY.md` **в этом же turn** до записи новой строки — не ждать Cleaner-ротацию (инцидент 2026-07-09: доросло до 928 без вмешательства между Cleaner-турнами) |
| TRACK status/STEP изменился (особенно close) | В том же коммите обновить строку трека + приоритетную цепочку в `docs/PLAN.md` (инцидент 2026-07-09: `PLAN.md` говорил "STEP=1 next" на треке, где уже было done STEP=4) |
| **`TRACK_MIR_VM_FULL` STEP=12 done (Epic 4 закрыт)** | **STOP GATE, не Driver STEP=13.** Epic 5 не авторизован без явной команды пользователя в чате (см. HARD STOP GATE в самом треке). `next` = `ROLE=Planner` — выбрать следующий трек из очереди `PLAN.md`, не открывать Epic 5 |
| **Этот turn закрыл трек (Driver STEP=N был последним pending, TRACK помечен closed)** | `next` = **`ROLE=Critic STEP=critique-audit TRACK=<только что закрытый>`** — обязательно, даже если следующий трек в очереди `PLAN.md` уже известен и есть pending шаг. Инцидент 2026-07-10: 5 закрытий подряд (TRAMPOLINE, BLOCK_ID_COLLISION, LOWERING_GAPS, CLI_STDIN, TEXT_RENDERING) — ни разу не запустился Critic, правило «Planner не повторяется» (см. ниже) съедало ротацию, потому что pending-шаг в следующем треке был всегда. Проверка на старте turn: если последняя запись `SESSION.md` содержит `close`/`closed` в `result` — этот HARD LIMIT применяется **раньше** любого другого выбора `next`, включая numbered pending step |
| **`CONCURRENCY_SPAWN_DETACH`** (PLAN.md §11b) | **closed** 2026-07-11 (Critic). Was: bare `spawn` serialized via `~future`. Delivered: E089 + MLC `scope`→TaskScope + accept-loop curl gate. Do not reopen; Planner takes «без C++» (§20b/d/e) |
| **«без hand-written C++» инициатива** (PLAN.md §20, треки `FFI_SHIM_MIGRATION`/`TEXT_MSDF_TO_MLC`/`STDLIB_HTTP_MLC`/`STDLIB_WEBSOCKET_TO_MLC`/`STDLIB_LOGIC_TO_MLC`/`GL_GLAD_MIGRATION`) | **§20 initiative closed** 2026-07-11, все 20a-e done. `FFI_POINTER_CAST`/`GL_LOADER_TO_MLC` superseded тем же днём (GLAD2/epoxy сами резолвят function pointers) → `docs/archive/tracks/`, не открывать снова. `LANG_SELF_HOSTED_RUNTIME` рассмотрен и **отклонён** тем же днём → `docs/archive/tracks/`, не открывать снова |
| **Одноразовый priority override 2026-07-12 (`TEXT_GL_PERF_BASELINE`)** | Пользователь нашёл 2 реальных бага в `text_dashboard_demo.mlc` (CPU-нагрузка от per-call `FT_Init_FreeType`; кривой baseline — bearing не перенесён из `text_renderer_shim.cpp`), root cause уже в треке. Вставлен ПЕРЕД `LANG_REGION_ARENA` STEP=2+ в очереди `PLAN.md`. `LANG_REGION_ARENA` STEP=1 был uncommitted-WIP в момент вставки — **не откатывать**, дать закоммититься естественно; но следующий `ROLE=Planner`-выбор после того коммита — `TEXT_GL_PERF_BASELINE`, не `STEP=2` того же трека. Это одноразовое исключение из правила «Driver продолжает текущий трек» ниже — не общий паттерн, не повторять для будущих находок без такой же явной пользовательской команды |
| **~300-step backlog (PLAN.md §21-29, построен 2026-07-11)** | Порядок: `EXAMPLES_CI` → `FFI_EXTERN_DEDUP` → `GUI_INPUT_ROBUSTNESS` → `STDLIB_HTTP_HARDENING` → `CONCURRENCY_SUPERVISOR` → `CONCURRENCY_TEST_HARNESS` T6/T7 → `LANG_DOCS` → `STDLIB_DOCS` → `LANG_REGION_ARENA` → `PACKAGE_MANAGER` → `DEBUG_SOURCE_MAP` → `GUI_CANVAS_GRAPH` (Phase A→B→C→D, под-треки `TRACK_GUI_SCENE_PHASE_{A,B,C,D}.md` создаются Planner'ом при старте каждой фазы). Все ungated — пользователь 2026-07-11 явно авторизовал реализацию `LANG_REGION_ARENA`/`PACKAGE_MANAGER`/`DEBUG_SOURCE_MAP`/`GUI_CANVAS_GRAPH` (ранее design-only/deferred/low-priority), Driver открывает Step 1 любого из них без доп. разрешения, по порядку очереди |
| **`TRACK_EDITOR_MVP` (PLAN §33)** | **closed** 2026-07-14. Не открывать numbered STEPs |
| **`TRACK_UX_HEADLESS` (PLAN §33a)** | **closed** 2026-07-14. L8 blink residual ок |
| **`TRACK_EDITOR_LIVE_POLISH` (PLAN §33b)** | **closed** 2026-07-15 (Critic OK). Archive. Не открывать numbered STEPs |
| **`TRACK_GUI_SCENE_PHASE_D` (PLAN §10c/§29)** | **closed** 2026-07-16 (Critic OK). Archive. Epic A–D done |
| **`TRACK_GUI_SCENE_PATH_MATCH` (PLAN §34)** | **closed** 2026-07-16 (Critic OK). Archive |
| **`TRACK_SCENE_FORM_LIVE` (PLAN §35)** | **closed** 2026-07-16 (Critic OK). Archive |
| **`TRACK_STDLIB_IO_FS` (PLAN §36)** | **closed** 2026-07-16 (Critic OK). Archive. STEP=7 → §37 |
| **`TRACK_GLFW_CONTENT_SCALE` (PLAN §37) — queue head** | GLFW HiDPI content_scale ABI + smoke. Prefer `runtime`/`gl_window.mlc`; no `compiler/` |
| **`TRACK_MLC_SCRIPT_VM`** | **design-only, NOT authorized** — do not open STEP=1 without explicit user command |
| **`TRACK_LANG_AUTO_CYCLE` (PLAN §19)** | Gated — не открывать без явной команды пользователя |
| **`TRACK_GUI_SCENE_PHASE_C` drift** | Historical; Phase C archived. Ignore if SESSION stale |
| **`TRACK_EDITOR_MVP` placement** | Design: `docs/EDITOR.md`, archive. Код → `misc/editor/`. Live polish → archived `TRACK_EDITOR_LIVE_POLISH` |

## When to stop (only these)

- Verify fail, root cause unclear after one debug pass **and** blocker logged
- User said stop / destructive action without approval

Иначе: fix → verify → записать `next` в SESSION (следующий sub-step из TRACK/PLAN) → commit → push.

## Commits

Verified sub-step → TRACK update → **commit самостоятельно**, не ждать команды user.

**Никогда не помечать TRACK-шаг `done` без коммита в том же turn.** Если turn прерывается (лимит контекста, ошибка, конец сессии) до commit — STEP остаётся `open`/не упоминается как done ни в TRACK, ни в SESSION `done`; в `issues` пишется "uncommitted, не завершено", `next` = повторить тот же STEP.

Commit message: `step N: <one line>` (английский, без Co-authored-by).  
После коммита — push.

Cross-cutting work: prefer WIP branch (`feat/…`), not large dirty tree on `main`.

## Session stats (`SESSION.md`)

`instructions_rev`, `step_last`, `test_gate` (`ok`|`fail`), uncommitted_files, verify result, tests, self-host diff.

**SESSION.md читается целиком каждый turn (в очереди) — держать маленьким.** Хранить последние ~15-20 turn (это же весь сегодняшний рабочий день); при превышении ~500 строк — Cleaner переносит более старые turn's **целиком в отдельный файл** `docs/archive/SESSION_HISTORY.md` (append, без удаления истории), не в секцию внутри самого SESSION.md — иначе файл продолжит расти и раздувать каждый queued-промпт (было 11000 строк до чистки 2026-07-09).

**MCP memory:** [.agent/memory-policy.md](../../.agent/memory-policy.md), [TRACK_MEMORY_MCP.md](../archive/tracks/TRACK_MEMORY_MCP.md) (closed, archived).

### SESSION turn template (обязательные поля)

```markdown
### Turn YYYY-MM-DD HH:MM (Role TRACK step N — <one-line title>)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | N |
| track   | TRACK_FOO_BRIDGE |
| started | YYYY-MM-DD HH:MM |
| elapsed | ~N min |
| done    | <что конкретно изменено: файлы, функции, подход> |
| result  | <тесты: N pass; self-host: ok/fail; diff: N lines> |
| issues  | <проблемы и как решили, или "none"> |
| next    | ROLE=Driver STEP=N+1 TRACK_FOO_BRIDGE |
```

**Правила:**
- `done` **обязателен до commit** — конкретно: файлы, функции, подход; gate `scripts/session_turn_lint.py`.
- `started` — время начала этого turn.
- `elapsed` — приблизительно в минутах.
- `done` — конкретно: "добавил `gen_foo_cpp`, убрал `CppStmtFragment` в `stmt_cpp.mlc:42`".
- `result` — числа тестов, exit-код self-host, количество изменённых строк.
- `issues` — всё нестандартное: retry, compile error, workaround.

## Roles

| Role | Duty | Touches `compiler/`? |
|------|------|----------------------|
| **Driver** | One TRACK sub-step → verify → commit | yes |
| **Verifier** | (with Driver) `scripts/dev_gate_fast.sh` each turn; full `build_tests.sh` Tier B before commit; + test_mlc if lib/mlc; self-host if compiler/** | — |
| **Planner** | Extend TRACK from PLAN; mark done; split oversized steps | **no** |
| **Backlog** | PLAN vs TRACK vs git; blockers; hygiene flags | **no** |
| **Scribe** | SESSION every turn; TRACK status; mlc-memory | docs only |
| **Critic** | Re-audit recent `done` steps vs git/tests; max skepticism | fix-only |
| **Cleaner** | Junk files, stale docs, orphan dot-dirs; keep active agent docs | delete/docs |
| **Orchestrator** | Roles, rotation cadence, ROLES.md + CONTINUITY | docs only |

### Multiple tracks open simultaneously (since 2026-07-11 backlog, §21-29)

Раньше почти всегда был открыт 1-2 трека. Теперь одновременно `pending`
у 12+ треков (§21-29). Правило выбора, чтобы Driver не прыгал между
треками каждый turn:

1. Если в `SESSION.md next` уже указан `TRACK=`, и у этого трека остались
   `pending` шаги — продолжать **этот же** трек, не переключаться на
   "более интересный", даже если у другого трека шаг короче/проще.
2. Трек считается доведённым до блокера/паузы (можно переключаться) только
   если: (a) он закрылся (`closed` → следующий turn обязательно Critic, см.
   HARD LIMIT ниже), либо (b) Driver уперся в реальный blocker (3 verify
   fails, см. HARD LIMIT), либо (c) явный `deferred`/gate внутри самого
   TRACK-файла.
3. Выбор **нового** трека (нет текущего WIP) — строго по порядку очереди
   из hard-limit-строки «~300-step backlog» ниже, не по алфавиту файлов и
   не "что показалось проще с первого взгляда".
4. `GUI_CANVAS_GRAPH` фазы (Phase A→B→C→D) — то же правило внутри трека:
   Phase B не начинается, пока Phase A не закрыта; Planner создаёт
   `TRACK_GUI_SCENE_PHASE_B.md` только в turn, где Phase A закрывается, не
   заранее.

### Role rotation (same chat, alternate turns)

Read `driver_turns_since_plan` from [SESSION.md](SESSION.md). Full table: [ROLES.md](ROLES.md).

**Приоритет над счётчиком (надёжнее для непрерывного дешёвого агента — счётчик легко забыть увеличить):**
- Только что закрыл TRACK (`STEP=verify-gate`/close) → следующий turn **Critic** (`critique-audit`) на этот закрытый трек, до открытия новой работы.
- Нет открытого pending шага ни в одном TRACK → **Planner**.
- Иначе — счётчик по таблице ниже.

| Condition | Next `ROLE=` |
|-----------|--------------|
| **`test_gate=fail`** or verify shows **`run_tests` / `build_tests` / `dev_gate` fail** | **`Driver` `STEP=test-fix`** (fix reds before TRACK steps; Planner must not open new work until green) |
| Every **16** Driver turns | **Orchestrator** (`STEP=roles-review`) |
| Every **20** Driver turns | **Backlog** (`STEP=backlog-review`) |
| Every **12** Driver turns | **Meta** — process/recovery notes |
| Every **10** Driver turns | **Cleaner** (`STEP=cleanup-sweep`) |
| Every **8** Driver turns | **Planner** (`STEP=plan-refresh`) |
| Every **6** Driver turns | **Critic** (`STEP=critique-audit`) |
| Every **35** Driver turns | **Reviewer** (`STEP=history-review`) |
| Every **18** Driver turns | **Monitor** (`STEP=process-monitor`) |
| No open `pending` step in TRACK | **Planner** |
| Otherwise | **Driver** (next pending sub-step) |

After Planner or Backlog turn → SESSION `next` = **Driver** with next `STEP=`.

**Planner не повторяется:** если TRACK уже содержит numbered pending шаг → сразу `ROLE=Driver STEP=<тот шаг>`, не plan-refresh.

Planner/Backlog **must not** edit `compiler/` or `lib/mlc/` — docs + mlc-memory only.

### Planner turn (`STEP=plan-refresh`)

1. Read [PLAN.md](../PLAN.md) (north star) + active TRACK.
2. Append missing sub-steps (numbered, one concern each, verify gate).
3. Mark completed steps `done` with commit hash if missing.
4. Update **Next step** section; do not delete deferred items.
5. Для каждого pending шага добавь/уточни **sub-steps** — 2-4 пункта что конкретно делать (файл, функция, изменение). Пример:
   ```
   | 3 | Eliminate `gen_foo` string path | pending |
   <!-- sub-steps: 1) добавить `gen_foo_cpp` в `codegen/foo.mlc`; 2) заменить вызов в `decl_cpp.mlc:42`; 3) run verify gate -->
   ```
6. SESSION: `driver_turns_since_plan=0`; `next` = Driver.

### Backlog turn (`STEP=backlog-review`)

1. `git status -sb`, uncommitted file count, `origin/main..HEAD`.
2. TRACK steps marked done but no matching commit → fix status.
3. Blockers → mlc-memory `known_limitations` or `notes`.
4. Flag: uncommitted > 15, self-host stale, debug junk in `compiler/`.
5. No code changes; `next` = Driver, or Planner if TRACK empty.

### Cleaner turn (`STEP=cleanup-sweep`)

1. `git status` — do not delete uncommitted Driver WIP.
2. Remove: debug junk in `compiler/` root, `.tmp_*`, stale duplicate docs (not CONTINUITY/ROLES/active TRACK/PLAN).
3. Trim outdated sections in DEVELOPMENT/README if they contradict active TRACK.
4. Dot-dirs: junk only; keep `.cursor/rules`, MCP, hooks.
5. TRACK closed and sitting in `docs/agent/` (not the 2-3 currently open ones) → `git mv` to `docs/archive/tracks/`; fix links that pointed at old path.
6. SESSION.md > ~500 lines → move older turns to `docs/archive/SESSION_HISTORY.md` (append), keep only recent turns live.
7. `runtime/test/*`/`compiler/out/tests/*` compiled binaries (no extension, `file` says ELF) tracked in git → `git rm --cached` + confirm `.gitignore` covers the pattern (`run_concurrency_smoke.sh` recompiles them unconditionally, tracking is always noise).
8. `docs/PLAN.md` строка/приоритетная цепочка не совпадает с реальным статусом TRACK (closed/STEP number) → синхронизировать.
9. Commit `cleanup: …` or docs-only; list paths in SESSION; `next` = Driver.

### Driver turn

Pick lowest **pending** step in TRACK; combine with Verifier + Scribe as today.
