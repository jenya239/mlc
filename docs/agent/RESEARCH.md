# Research backlog (Planner / orchestrator)

????????? ??? `plan-refresh`. ?????????: rustc-dev-guide, Rustlantis/OOPSLA'24, production agent patterns.

## Compiler � ???????????? / ???????????? (????? TRACK_PHASE1)

| ???? | ????? | ???????? |
|------|-------|----------|
| Fuzz smoke ? differential | mlcc vs Ruby backend ?? ????? AST; ?????? miscompile | Rustlantis, rustc fuzzing guide |
| `--check-only` ? CI gate | codegen ?? ????? ??? checker-only PR | Phase 1 step 6 ? |
| panic ?????? silent unreachable | crash-safety ? ?????????? | Phase 1 �4 |
| Minimize ICE repro | treereduce-???????? shrink ??? .mlc | rustc-dev-guide/fuzzing |
| Negative corpus | invalid .mlc ? ?????????? E-code, ?? segfault | Glacier-style |

## Compiler � ?????????????????? (?????? ????? Phase 1 closed)

| ???? | ????? |
|------|-------|
| `build_registry` COW audit | PLAN �1 hotspots |
| Benchmark harness ? `benchmarks/` | regression ?? record_baseline |
| `--emit=mir`-?????? | ?????? work ??? fuzz (rustc `--emit=mir`) |

## Orchestration (cr + mlc agent)

| ???? | ?????? |
|------|--------|
| `pickNextAgentStep` � track priority | ? |
| Guard: CDP send, ?? ????????? enqueue | ? |
| Usage gate ?? composer token ring | ? `probe:usage`, `CR_USAGE_PAUSE_PCT` |
| Idempotency / cooldown ?? nudge | ? 15m |
| Planner ?????? 8 driver turns | CONTINUITY |
| ????????? research turn (cheap model) | ??? ? ~2 ??????, ??? code |

## Cursor

- Usage: `.token-ring-progress` ? composer bar ? `npm run probe:usage` (cr).
- Settings ? Usage: ??? ???? (Composer vs premium) � ???????? ? UI, guard ?????? ??? ring ?88%.
- mlc Driver: **Composer 2.5 Fast**; Sonnet/Opus � ?????, Planner ??? ????? ?????? user.

## Meta recovery log (orchestration)

| Date | Issue | Action |
|------|-------|--------|
| 2026-05-29 | Guard blocked `Driver:1` loop on TRACK_STMT_BRIDGE | Planner enqueued STEP=1; no Driver commit (`stmt_cpp.mlc` unchanged); Meta: supervisor ok, single recovery enqueue |
| 2026-05-29 | Guard `Driver:1:STMT_BRIDGE` already ran | Meta finished STEP=1 commit (763 pass, self-host); enqueue STEP=2 once |
| 2026-05-31 | Stale FN_PROTO/TRAIT Driver prompts; duplicate `Planner:plan-refresh` / `Driver:4` enqueue blocks | Critic confirmed EXTEND_HELPER steps 1–3 (856 pass, DIFF_EXIT=0); queue drained; Meta: supervisor ok, single enqueue Driver:4 |
| 2026-06-03 | Guard stuck `Driver:1:DESTRUCTURING_APPLY` — STEP=1 lexer done; verify incomplete (`let_pat.mlc` scope fix uncommitted; build_bin failed on scoped bindings) | Meta: supervisor `no_window` warn only; TRACK step 1 → done; no re-enqueue STEP=1; enqueue Driver STEP=2 once |
| 2026-06-04 | MODULE_TU closed without commit; REGISTRY s1 worktree-only | Critic reopen MODULE_TU step 6; gate 937/0; enqueue Driver STEP=6 commit batch |
| 2026-06-04 | DECL_FRAGMENT steps 1–3 worktree-only | Critic: OK worktree, not HEAD; 939/0; STEP=4 pending; no reopen |
| 2026-06-04 | Meta: stuck Driver:4 DECL_FRAGMENT after assistant | STEP=4 code OK worktree (`CppHostEntryMain`); hung verify (`build_tests\|grep`); supervisor `no_window` warn only; enqueue **STEP=5** not STEP=4 |
| 2026-06-04 | TRACK_DECL_FRAGMENT_CLEANUP closed | STEP=5: 0 production `CppDeclFragment`; 940/0; Planner plan-refresh enqueued |
| 2026-06-04 | Guard loop `Driver:1:RENAME_ABBREV` while **STRING_MATCH** active (steps 1–5 done, 952/0) | Meta: supervisor ok; RENAME STEP=1 deferred; fix TRACK table; enqueue **Driver STRING_MATCH STEP=6** once (not RENAME STEP=1) |
| 2026-06-05 | Guard stuck `Driver:12:RENAME_ABBREV` after assistant — STEP=12 committed (`c3eb310d`, 961/0, self-host diff empty) | Meta: supervisor ok; TRACK step 12 → done; no re-enqueue STEP=12; enqueue **Driver STEP=13** once |
| 2026-06-05 | Guard stuck `Driver:13:RENAME_ABBREV` after assistant — STEP=13 committed (`c487d3fc`, 961/0, self-host diff empty) | Meta: supervisor ok; TRACK step 13 → done; no re-enqueue STEP=13; enqueue **Driver STEP=14** once |

Orchestration items marked done: duplicate-enqueue guard, Driver STEP loop recovery path in Meta role, stale-step skip + single pending enqueue, Critic→Driver handoff without re-enqueue same STEP, **Meta drain stuck STEP=1 → enqueue STEP=2 (DESTRUCTURING_APPLY)**, **post-commit stuck STEP → enqueue STEP+1 (RENAME_ABBREV s12→s13)**.

---

## Многопоточность (будущее, после Phase 4)

**Текущее состояние:** single-threaded by design. Runtime имеет `task.hpp` (C++20 coroutines, заготовка) и `net/http.hpp` (внутренние потоки), но языковых примитивов нет.

**Хорошая база уже есть:**
- Value semantics — передача по значению безопасна между потоками без синхронизации
- COW (`Array`, `HashMap`, `String`) — каждый поток работает со своей копией после detach
- `Shared<T>` — `shared_ptr` с атомарным refcount, безопасно передавать

**Что добавить (по приоритету):**

| # | Примитив | Описание |
|---|---------|-----------|
| 1 | `Channel<T>` | Message passing; потоки обмениваются копиями значений — самая безопасная модель |
| 2 | `spawn` + `Task<T>` | Запуск задач; `task.hpp` уже есть, нужен языковый синтаксис |
| 3 | `Arc<T>` | Thread-safe `Shared<T>`; явно обозначает объекты разделённые между потоками |
| 4 | `Mutex<T>` | Мютекс вокруг значения, не глобальный; `mutex.lock(fn mut val => ...)` |

**COW фикс:** detach (проверка `use_count` + глубокое копирование) не атомарен. Перед вводом потоков нужно либо запретить мутацию шаред массивов, либо сделать detach атомарным.

**Модель:** Go/Rust-стиль — «обменивайся копиями, не разделяй память». Ложится на value semantics которые уже есть.

**Не добавлять:** `AtomicInt`, глобальные `Mutex`, `async/await` в полном объёме.

