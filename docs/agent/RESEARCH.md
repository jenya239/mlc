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
| Meta recovery: finish WIP + commit, enqueue next STEP (not same Driver STEP) | 2026-06-05 TYPE_ALIASES `Driver:1` loop |
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
| 2026-06-15 | Guard stuck `Driver:3:LOOP_CONTRACTS` — `AGENTS.md` + README worktree-only; verify interrupted | Meta: commit STEP=3 docs; TRACK step 3 → done; no re-enqueue STEP=3; enqueue **Driver STEP=4** once |
| 2026-06-18 | Guard stuck `Driver:5:MLCC2_CHECKER_PARITY` — ctor `CppCall` fix in `expr_visitor_cpp.mlc`; p1 `RecordLitFields(...)` parens; mlcc2 E030 persists (point_repro, main); corpus mlcc2=0 | Meta: TRACK step 5 → blocked; STEP=6 pending; no re-enqueue STEP=5; enqueue **Driver STEP=6** once |
| 2026-06-18 | Guard stuck `Driver:4:MLCC2_CHECKER_PARITY` — STEP=4 E030 on mlcc2 (`record_lit_merge`); mlcc=0; build_tests+build.sh ok; fold/flat_map/while fixes tried | Meta: supervisor ok (MCP enqueue unavailable); TRACK step 4 → blocked; no re-enqueue STEP=4; enqueue **Driver STEP=5** once (bisect E030 in p1 checker, then self-host diff) |
| 2026-05-19 | Guard stuck `Driver:3:CPP_HEADER_IMPORT` — STEP=3 not started (`parser/types.mlc` absent); STEP=1–2 in worktree uncommitted (`cpp_tokens.mlc`, `lexer.mlc`, `cpp_ast` types); last gate **1033/0** **diff_exit=0** | Meta: no re-enqueue STEP=3; TRACK STEP=3 pending; SESSION enqueue payload for Driver STEP=3 (new token); commit batch deferred (user) |
| 2026-05-19 | Guard stuck `Driver:5:FORMATTER` — STEP=5 WIP (`mlcc fmt`, `format_cli`/`ast_printer`); mlcc parse `()`/`if`/`end`; codegen `parse_result`→`predicates::` collision | Meta: gate **1030/0** **diff_exit=0** after `parse_parsed` rename; TRACK FORMATTER **closed**; no re-enqueue STEP=5; enqueue **Driver PHASE26 STEP=1** once |
| 2026-05-19 | Guard stuck `Driver:2:FORMATTER` — STEP=2 WIP (`ast_printer` indent); nested `do/end` double-indent; top-level `const` broke Ruby parse | Meta: supervisor MCP unavailable; TRACK step 2 → done; gate **1016/0** **diff_exit=0**; no re-enqueue STEP=2; enqueue **Driver STEP=3** once |

Orchestration items marked done: duplicate-enqueue guard, Driver STEP loop recovery path in Meta role, stale-step skip + single pending enqueue, Critic→Driver handoff without re-enqueue same STEP, **Meta drain stuck STEP=1 → enqueue STEP=2 (DESTRUCTURING_APPLY)**, **post-commit stuck STEP → enqueue STEP+1 (RENAME_ABBREV s12→s13)**, **batch uncommitted recovery → single commit + Planner plan-refresh (2026-06-15, `bf6c46e8`)**, **FORMATTER stuck STEP=2 → finish gate + enqueue STEP=3 (2026-05-19)**, **FORMATTER stuck STEP=5 → close track + enqueue PHASE26 STEP=1 (2026-05-19)**, **CPP_HEADER_IMPORT stuck Driver:3 → no re-enqueue; STEP=3 pending unstarted (2026-05-19)**.

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


---

## Generics на record-типах (ParseResult<T> и аналоги)

**Связано:** TRACK_TYPE_ALIASES step 6 (skip), PLAN.md §Phase 2.5 п.5

**Проблема:** В `frontend/` есть ~15 одинаковых структур `{ value: T, parser: Parser }` типа ParseResult. Без generics на record-типах их нельзя объединить в одну. Step 6 TYPE_ALIASES (`ParseResult<T>` через type alias) заблокирован этим.

**Что нужно исследовать:**
1. Как далеко реализованы generics сейчас: `[string]` работает, `Shared<T>` — через C++ template. Что именно не работает для record-generics?
2. Минимальный синтаксис: `type Result<T> = Result { value: T, error: string }` — что нужно добавить в parser/checker/codegen?
3. Оценка объёма: отдельный трек или часть Phase 2.5?

**Действие для Planner:** создать TRACK_GENERICS_RECORD.md или добавить шаги в TRACK_PHASE1.md.
