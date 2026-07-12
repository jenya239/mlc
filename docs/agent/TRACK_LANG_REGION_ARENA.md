# Track: scoped region/arena (`region r do ... end`)

Parent: [../PLAN.md](../PLAN.md) §7 / queue after STDLIB_DOCS,
[../MLC.md](../MLC.md) §C1 (phantom types — прямая зависимость). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md), §1.4
(единственная рекомендация аудита, реально *расширяющая* выразительность
связных графов, не просто сахар).
Predecessor closed (Critic OK 2026-07-12):
[../archive/tracks/TRACK_STDLIB_DOCS.md](../archive/tracks/TRACK_STDLIB_DOCS.md).
TEXT_GL override closed (Critic OK 2026-07-12):
[../archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md](../archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md).

## Status: **active** (STEP=3 done 2026-07-12) — STEP=4 codegen next

## Next step

**STEP=4** — Codegen: `region` → pmr arena; `r.alloc` → placement-new.

### STEP=3 done (2026-07-12)

- `region_escape.mlc` + E091: return / closure capture / outer assign / Region field type.
- Wired in `check.mlc` (`region_escape_diagnostics`, `region_field_type_diagnostics`).
- Smoke: `mlcc --check-only` ok/alloc green; ret/closure/assign/field → E091.
- Codegen (STEP=4) and e2e fixtures (STEP=5) not in this turn.

### STEP=2 done (2026-07-12)

- `infer_region_method.mlc`: `RegionTag_L_C` phantom, `RegionHandle<Tag>`, `r.alloc(v) -> Region<Tag,T>`, reject non-`alloc`.
- `infer.mlc` / `transform.mlc`: binder via `__region_handle_new` let (names-visible).
- `registry.mlc` + `check.mlc` globals for `__region_handle_new`.
- Smoke: `mlcc --check-only` empty/`alloc`/bind exit 0; `r.nope()` → E082.

## Зачем

Единственный способ получить Ruby/JS-подобную лёгкость для связного
изменяемого графа (**включая циклы**) без нарушения принципа «без GC/
рантайма» — если весь граф целиком живёт в рамках одного scope. Прямое
попадание в реальный код: `compiler/frontend/ast.mlc` — массовое
`Shared<Expr>`/`Shared<Decl>`, каждый узел несёт атомарный refcount,
который при построении дерева в одной функции (например одном проходе
парсера) не нужен.

## Математическое ограничение (checker обязан ловить, иначе UB)

Ни одна ссылка на объект региона не должна пережить `end` региона. Это
**узкий escape-анализ**, НЕ полный borrow checker: региональные значения
несут фантомный lifetime-параметр, и правило одно — `Region<'r, _>` не
может появиться в типе возврата или поля, живущего дольше `'r`.

## Зависимость: phantom types (C1)

Синтаксис требует фантомных type-параметров (`Region<'r, Node>`, где `'r`
не встречается в полях `Node`). **C1 уже реализован** —
`compiler/checker/registry.mlc:34,90-94,587,708`
(`algebraic_decl_phantom_type_params`, `compute_phantom_type_params`) —
зависимость снята, можно опираться на существующий механизм вместо
реализации с нуля.

## Синтаксис (гипотеза)

```mlc
region r do
  let root = r.alloc(Node { children: [], value: 1 })
  let leaf = r.alloc(Node { children: [], value: 2 })
  build_tree(root, leaf)   // циклы ВНУТРИ региона допустимы
  serialize(root)          // результат НЕ должен содержать region-ссылок наружу
end   // весь буфер r освобождается разом
```

## C++ трансляция

`region` → RAII-обёртка над `std::pmr::monotonic_buffer_resource` (C++17,
доступно в C++20). `r.alloc(...)` → placement new в этот resource. Единое
освобождение = деструктор буфера. Ноль per-node refcount внутри региона.

## Решения (закрыты 2026-07-11, заменяют старые открытые вопросы)

1. **Без явного лайфтайм-синтаксиса.** Никакого `Region<'r, T>` с
   пользовательским `'r`. Вместо этого — обычный generic-параметр,
   инстанцируемый компилятором: каждый `region r do ... end` блок
   синтезирует уникальный phantom-tag тип (`RegionTag_N`, невидимый
   пользователю, та же механика что уже даёт C1
   `algebraic_decl_phantom_type_params`). `r: RegionHandle<Tag>`,
   `r.alloc(...) -> Region<Tag, T>`. Функции, работающие с региональными
   значениями **внутри** блока, просто дженерики по `Tag` как по любому
   обычному type param — ничего нового в синтаксисе вызова не требуется,
   инференс подхватывает `Tag` так же, как любой другой generic argument.
2. **Полный запрет выхода за границу, без generic-исключения.** Второй
   вариант (функция generic по lifetime, не сохраняющая ссылку) отклонён —
   сам трек уже называет его «значительно сложнее в checker», а первичная
   цель (парсер строит дерево в одной функции) не требует этой гибкости.
   Правило: `Region<Tag, T>` (и любой тип, содержащий его как поле) не
   может: (a) быть значением `return` из функции, объявленной **вне**
   текущего `region`-блока по лексической вложенности; (b) быть захвачен
   замыканием, которое переживает блок (тот же escape-анализ что уже есть
   для `TRACK_LANG_CLOSURE_ESCAPE`, applied to region-tagged values); (c)
   быть записан в поле значения, само не являющегося `Region<Tag, _>`.
   Функции, объявленные вне блока но дженерик по `Tag`, вызываемые **из
   внутри** блока — разрешены (пункт 1), это не нарушает правило, потому
   что вызов происходит внутри лексической границы региона.
3. **Тип целиком региональный или целиком не региональный — без смешения.**
   Record не может одновременно иметь региональные и `Shared`/`Owned`
   поля. Если региональному узлу нужна ссылка на что-то за пределами
   региона — поле хранит примитивный идентификатор (индекс/`i32` id), не
   `Shared<T>`-хендл, и разрешение id→объект происходит explicit через
   внешнюю таблицу вне региона (тот же паттерн camera-relative addressing,
   уже отмеченный в [TRACK_GUI_CANVAS_GRAPH](TRACK_GUI_CANVAS_GRAPH.md) для
   больших координатных полей — не переизобретать отдельно).

## Steps (реализация)

| Step | Item | Status |
|------|------|--------|
| 1 | Parser: `region <name> do ... end` block syntax → AST node (`ExprRegion(name, [Stmt], Span)` parallel to `ExprScope`; reuse `parse_statements_until_end` / keyword dispatch like `scope`) | **done** (2026-07-12) |
| 2 | Checker: synthesize per-block phantom `RegionTag`; typecheck `r.alloc(value) -> Region<Tag, T>`; wire into existing phantom-type machinery (`compiler/checker/registry.mlc` `compute_phantom_type_params`) rather than a parallel mechanism | **done** (2026-07-12) |
| 3 | Checker: escape diagnostic — reject `Region<Tag, T>` (or any type containing it) in `return`, in a closure capture that outlives the block, or as a field of a non-regional type. New diagnostic code (`E0XX`, pick next free in `compiler/checker/diagnostic_codes.mlc`) | **done** (2026-07-12) E091 |
| 4 | Codegen: `region` → RAII wrapper over `std::pmr::monotonic_buffer_resource`; `r.alloc(...)` → placement-new into that resource; zero refcount overhead for regional values inside the block | pending |
| 5 | UB-repro negative test: regional reference smuggled past `end` via each of the three escape vectors (return/closure/field) — each must be a **compile error**, not a runtime crash; one e2e fixture per vector under `compiler/tests/e2e/` | pending |
| 6 | Positive test: cyclic mutable graph built entirely inside one `region` block (the motivating case — parser-style tree/graph construction), verify zero `Shared<T>`-style atomic refcount ops in generated C++ for the regional allocations (grep generated `.cpp`, not just "it compiles") | pending |
| 7 | Self-host verify: `compiler/out/mlcc` → `mlcc2`, diff identical (touches checker+codegen) | pending |
| 8 | `scripts/regression_gate.sh` green | pending |
| 9 | Docs: `MLC.md` §C1 area — document `region`/`RegionHandle`/`Region<Tag,T>`; note the three escape-prohibition vectors explicitly | pending |
| 10 | Optional/stretch: apply `region` to a real internal hot path with a mutable cyclic graph (`compiler/frontend/ast.mlc` node construction during one parse pass, as named in "Зачем" above) as a proof-of-value, **only if** Steps 1-9 are stable and this doesn't risk destabilizing the self-hosted parser — separate sub-step, easy to defer/skip if risky | pending |

### STEP=1 sub-steps (Driver)

1. Lexer/keyword: ensure `region` is recognized (keyword or ident dispatch) so
   `region name do` is not parsed as a call/ident.
2. AST: add `ExprRegion(string, [Shared<Stmt>], Span)` next to `ExprScope` in
   `compiler/frontend/ast.mlc`; update `expr_span` / printers / visitors as
   required for compile (mirror every `ExprScope` match arm).
3. Parser: `parse_region_expr` mirroring `parse_scope_expr` but binder is a bare
   ident after `region` (no `|binder|`); body via `parse_statements_until_end`.
4. Minimal smoke: one `compiler/tests/` or e2e parse-only / check fixture that
   `region r do 0 end` parses (checker may still reject `alloc` until STEP=2 —
   empty body / unit result OK if that typechecks today).
5. Do **not** implement checker escape rules or codegen in STEP=1.
6. Verify: self-host not required yet if only parser/AST; run
   `bundle exec rake test_compiler_mlc` (or targeted parser test) green;
   TRACK Next=STEP=2.

## Progress

- **Planner** (2026-07-12): activated after STDLIB_DOCS Critic OK; STEP=1 =
  `ExprRegion` + parse; Decisions 1–3 unchanged.
- **Driver STEP=1** (2026-07-12): `ExprRegion` in `ast.mlc`; `parse_region_expr`
  in `exprs.mlc`; mirror arms in infer/transform/escape/move/spawn/verify;
  `test_region.mlc` parse smoke; `mlcc --check-only` region smoke OK.
- **Planner** (2026-07-12): resumed after TEXT_GL Critic OK (CONTINUITY one-shot
  override complete); TEXT_SHIM_TO_MLC stays **open** queued after this track;
  Next=STEP=2 checker phantom tag + `r.alloc`.

## Verify gate (когда дойдёт до реализации)

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

## Out of scope

- Full borrow checker / overlapping mutable borrows across regions.
- Multi-region graphs with cross-region pointers (use ids, Decision 3).
- GC / tracing collector as alternative to arenas.
