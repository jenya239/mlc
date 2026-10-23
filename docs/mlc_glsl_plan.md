# План: GLSL-бэкенд для mlc (`mlcc --target=glsl`)

**Статус:** черновик, не трек. Снят с клона `/tmp/mlc` на `afb5d8a`, не с текущего HEAD. M0–M* не запускать. Порядок приоритетов и запрет начинать GLSL-бэкенд — в [MLC_CPP_FIRST_PORTABLE_COMPUTE_RFC.md](MLC_CPP_FIRST_PORTABLE_COMPUTE_RFC.md). На 2026-10-23 по текущему дереву подтверждено только это: суффикс `f32` есть в `compiler/frontend/lexer.mlc`, варианта `TF32` в `compiler/checker/registry_type.mlc` нет. Остальные пути и числа строк перед использованием перепроверять.

Дата: 2026-10-21. Репозиторий: https://github.com/jenya239/mlc (изучен клон `/tmp/mlc`, HEAD `afb5d8a`; в репозиторий ничего не вносилось).
Потребитель: curved-space path tracer «Horus» (`/workspace/viz/v16_dive/pt_anim4/pt4.frag`, ~55 КБ, хост `anim4.py` на moderngl).

---

## 0. Что есть в mlc сейчас (на что опираемся)

| Слой | Где | Что важно для GLSL |
|---|---|---|
| Лексер | `compiler/frontend/lexer.mlc` | Суффиксы `f32`, `u32` уже лексятся (`numeric_type_suffixes`), но **типа `TF32` нет**. Токена `@` нет. |
| AST | `compiler/frontend/ast.mlc` | `Decl = DeclFn \| DeclType \| DeclTypeAlias \| DeclTrait \| DeclExtend \| DeclExternType \| …`. Общего механизма атрибутов нет; есть только хвостовые атрибуты у `extern` (`blocking`, `thread_safe`, `thread_affine` — `parse_extern_concurrency_attributes` в `parser/decls.mlc`). |
| Типы | `compiler/checker/registry_type.mlc` | `Type = TI32 \| TString \| TBool \| TUnit \| TI64 \| TF64 \| TU8 \| TUsize \| TChar \| TArray \| TShared \| TNamed \| TGeneric \| TPair \| TTuple \| TFn \| TAssoc \| TUnknown`. Нет `f32/u32`, нет фиксированных массивов, нет векторов. |
| Реестр | `TypeRegistry { function_index, adt_index, record_index, … }` | `FunctionIndex.function_concurrency_attributes: Map<string,[string]>` — **готовый прецедент** для хранения атрибутов функций (сюда же ляжет `gpu`). |
| Инференс | `compiler/checker/infer/*.mlc` (~4K строк) | Бинарные операции: `binary_operation_result_type(op, left)` в `semantic_type_structure.mlc` — результат = тип левого операнда; для `vec3 * f32` придётся расширять. Встроенные методы — по образцу `infer_array_method.mlc`, `infer_atomic_method.mlc`. |
| SemanticIR | `compiler/ir/semantic_ir.mlc` | Типизированное дерево: каждое выражение несёт `Shared<Type>` и `Span`. **Это вход GLSL-бэкенда.** |
| MIR | `compiler/mir/*` | CFG-IR, но операнды только `Int/Bool/Str/Unit` (нет float), используется для VM/дампов. Для GLSL пока **не годится** — опускаемся из SemanticIR, как основной путь CppAST. |
| CppAST | `compiler/cpp_ir/cpp_ast.mlc` (172 строки) | Образец: `CppType`, `CppStatement`, `CppExpression`, `CppDeclaration`, `CppFile`. Печать — `compiler/cpp_emit/print.mlc` (742 строки, чистые функции `print_*`). |
| Дженерики/трейты | `codegen/cpp_naming.mlc`, `decl_cpp_trait.mlc` | Дженерики → C++ `template<typename …>`, трейты → `concept`/`template<typename Self>`. **Мономорфизации в mlcc нет** — для GLSL её надо написать. |
| Sum types | README | → `std::variant` + `std::visit`. Для GLSL — tagged struct + `switch`. |
| Пайплайн | `compiler/pipeline.mlc`, `compile_options.mlc` | `ModularCompileInput { …, emit_layout, cpp_mode }`, `PassManager` с дескрипторами `verify_ast → checker → transform → codegen`. Флаги `--emit-layout=`, `--cpp-mode=` — шаблон для `--target=`/`--variant=`. |
| Extern | `extern fn f(..) -> T = "cname" from "<hdr>"` | Удобный механизм объявления GLSL-builtin'ов (`texture`, `imageLoad`, `atomicAdd`) в прелюдии. |
| Тесты | `compiler/tests/*.mlc`, `tests/e2e/run_e2e.sh`, `tests/fixtures/`, `codegen_harness.mlc` | Golden-тесты строим по образцу `codegen_harness` + shell-гейтов `run_*_smoke.sh`. |
| Процесс | `docs/MLC.md` §«Процесс реализации» | «Ruby-эталон не опережается». Исключение уже есть — concurrency только в mlcc (§«Два пайплайна»). GLSL-таргет оформляем так же: **mlcc-only**, строка в таблице split, Ruby-бутстрап не трогаем (исходники нового бэкенда пишем на подмножестве MLC, которое понимает Ruby-компилятор, чтобы `build.sh` продолжал работать). |

Среда на боксе: Mesa llvmpipe даёт **GL 4.5 Core**, `create_standalone_context(backend="egl", require=430)` + `compute_shader` проверены — compute-шейдеры и пиксель-тесты можно гонять на CPU бокса. `glslangValidator` не установлен (`apt install glslang-tools`). `compiler/out/mlcc` в клоне не собран (нужен `MLCC_FORCE_RUBY=1 compiler/build.sh`).

Текущий шейдер: `#version 330`, ~70 uniform'ов (в т.ч. массивы `uShT[12]`, `uKH[73]`, `uLens[4]`, `uFig[6]`), 2 `sampler2DArray` (GS-последовательности), 4 MRT-выхода (`oCol, oG1..oG3`), режим `uSchem` — **runtime-ветка**. Gray-Scott считается на CPU (`gs3.gs_steps`, numba, 9-точечный лапласиан, периодические границы) и грузится как `.npy` → texture array.

---

## 1. GPU-подмножество MLC

Правило: функция/тип помечается `@gpu` (или весь модуль — `@gpu module`, см. §3). Внутри действует подмножество:

| MLC | В `@gpu` | GLSL |
|---|---|---|
| `f32`, `i32`, `u32`, `bool` | да (новый `TF32`, `TU32`) | `float`, `int`, `uint`, `bool` |
| `f64` | запрещено по умолчанию (E-GPU-F64), `--allow-f64` → `double` | |
| `i64`, `u8`, `usize`, `char`, `string` | нет | — |
| `Vec2/3/4`, `IVec*`, `UVec*`, `BVec*`, `Mat2/3/4` | встроенные (§4) | `vec*`, `ivec*`, `uvec*`, `bvec*`, `mat*` |
| Фикс. массив `[T; N]` (новый тип `TFixedArray(T, N)`) | да, N — литерал или `@static`-константа | `T name[N]` |
| `[T]` (COW Array), `Map`, `Shared<T>`, `Owned<T>`, `Region` | нет (E-GPU-HEAP) | — |
| `[T]` как поле буфера | только в `@buffer` как последнее поле (runtime-sized) | `T data[];` в SSBO |
| Record | да, если все поля GPU-типы | `struct` |
| Sum type, нерекурсивный | да | tagged struct (§1.1) или специализация |
| Sum type, рекурсивный (`Shared<Self>` в варианте) | только как `@static`-значение (вычисляется при компиляции) | разворачивается |
| `match` | да | `switch(tag)` / цепочка `if` (по guard'ам и литералам) |
| `if/while/for i in a..b`, `break/continue`, `return` | да | то же |
| `let mut`, `const` | да | локальные переменные, `const` |
| Лямбды, `\|>`, частичное применение | только если инлайнятся статически (аргумент `fn`-параметра известен при компиляции → мономорфизация по функции) | нет |
| Рекурсия (в т.ч. взаимная) | нет (E-GPU-RECURSION); self-tail-рекурсия автоматически → `while` | |
| `Result`, `Option`, `?` | `Option<T>` — да как tagged struct `{bool has; T v;}`; `?` нет | |
| Строковая интерполяция, `println` | нет (позже: `debug_printf` через `GL_EXT_debug_printf`) | |
| Дженерики | да, **мономорфизируются** (§2.3) | имена `sdf_union__f32` |
| Трейты, `extend` | да, статическая диспетчеризация (уже так в C++) → свободные функции `Type_method` | |
| `spawn`, `Mutex`, `Channel`, … | нет | |
| `inout`/`out` | `mut`-параметр (`fn f(mut g: Vec3)` уже есть: `function_parameter_mutability_flags`) → `inout` | `inout` |

### 1.1. Sum types → GLSL

Две стратегии, выбираются автоматически, переопределяются атрибутом:

1. **Tagged struct** (по умолчанию для значений, приходящих из рантайма — uniform/буфер/результат hit-теста):
   ```mlc
   @gpu type Material =
     | Stone { alb: Vec3, sigma: f32, glow: f32, tint: Vec3 }
     | Metal { f0: Vec3, rough: f32 }
     | Emissive { le: Vec3 }
   ```
   →
   ```glsl
   struct Material { int tag; vec3 f0; float f1; float f2; vec3 f3; };  // поля упакованы по слотам типов
   const int Material_Stone = 0, Material_Metal = 1, Material_Emissive = 2;
   ```
   Упаковка: слоты одного GLSL-типа переиспользуются между вариантами (Stone.alb и Metal.f0 оба `vec3` → общий слот). Таблица «вариант.поле → слот» хранится в `GlslLayoutIndex` и в манифесте (§6). Альтернатива `@layout(wide)` — без переиспользования, удобнее для отладки.
2. **Специализация** (`@static`-значение, либо `match` по параметру, который на всех вызовах константен): ветки сворачиваются, tagged struct не создаётся. Это основной путь для `Variant = Full | Schematic` (§5) и для «сцены как данных».

`match` → `switch (m.tag) { case Material_Stone: { vec3 alb = m.f0; … } }`; выражение-`match` в позиции значения → временная переменная + `switch` (GLSL не умеет statement-expressions; та же проблема, что решена в C++ через `CppInvokedBlock` — здесь решаем линеаризацией, §2.2).

### 1.2. Рекурсия
- Граф вызовов строится по SemanticIR (`SemanticExpressionCall` с `SemanticExpressionIdent`-callee). SCC > 1 или self-loop → ошибка `E-GPU-RECURSION` с цепочкой вызовов.
- Self-tail-call → цикл (`while(true){ …; params = args; continue; }`) — трансформ на SemanticIR до lowering.
- Нехвостовая ограниченная рекурсия (door recursion, отражения) — явный `@unroll(N)` (клонирование функции N раз с заглушкой на дне) или ручной перевод на явный стек `[T; N]`. В pt4 door recursion уже итеративна (`gRec`), так что для сцены это не блокер.

---

## 2. Новый IR: GlslAST + печать

### 2.1. Файлы (параллельно C++-бэкенду)

```
compiler/glsl_ir/glsl_ast.mlc          — GlslType/GlslExpr/GlslStmt/GlslDecl/GlslModule
compiler/glsl_ir/glsl_builtins.mlc     — таблица builtin-функций и swizzle-правил
compiler/codegen_glsl/lower_module.mlc — SemanticLoadItem[] → GlslModule
compiler/codegen_glsl/lower_expr.mlc   — выражения + линеаризация (let-hoisting)
compiler/codegen_glsl/lower_match.mlc  — match → switch/if, tagged structs
compiler/codegen_glsl/monomorphize.mlc — инстанцирование дженериков и @static-параметров
compiler/codegen_glsl/layout.mlc       — std140/std430 layout, binding allocation
compiler/codegen_glsl/manifest.mlc     — JSON-манифест (§6)
compiler/glsl_emit/print.mlc           — GlslAST → текст
compiler/checker/gpu_subset.mlc        — проверка подмножества (§3)
compiler/checker/infer/infer_vector.mlc — типы векторов, swizzle, операторы
compiler/std/gpu/prelude.mlc           — extern-объявления GLSL builtins
compiler/tests/glsl/                   — golden + гейты
```

### 2.2. GlslAST (эскиз, стиль как в `cpp_ast.mlc`)

```mlc
export type GlslScalar = GlslFloat | GlslInt | GlslUint | GlslBool | GlslDouble

export type GlslType =
  | GlslTypeScalar(GlslScalar)
  | GlslTypeVec(GlslScalar, i32)             // vec3, ivec2, …
  | GlslTypeMat(i32, i32)                    // mat3, mat3x4
  | GlslTypeStruct(string)
  | GlslTypeArray(Shared<GlslType>, i32)     // -1 = runtime-sized (последнее поле SSBO)
  | GlslTypeSampler(string)                  // "sampler2D", "sampler2DArray"
  | GlslTypeImage(string, string)            // ("image2D", "rg32f")
  | GlslTypeVoid

export type GlslExpr =
  | GlslFloatLit(string) | GlslIntLit(i32) | GlslUintLit(string) | GlslBoolLit(bool)
  | GlslIdent(string)
  | GlslCall(string, [Shared<GlslExpr>])          // функции, конструкторы vec3(...), builtins
  | GlslField(Shared<GlslExpr>, string)           // поле структуры
  | GlslSwizzle(Shared<GlslExpr>, string)         // .xyz/.rgba — отдельно от Field для валидации
  | GlslIndex(Shared<GlslExpr>, Shared<GlslExpr>)
  | GlslBinary(string, Shared<GlslExpr>, Shared<GlslExpr>)
  | GlslUnary(string, Shared<GlslExpr>)
  | GlslTernary(Shared<GlslExpr>, Shared<GlslExpr>, Shared<GlslExpr>)

export type GlslStmt =
  | GlslDeclVar(Shared<GlslType>, string, Shared<GlslExpr>, bool)   // bool = const
  | GlslDeclVarNoInit(Shared<GlslType>, string)
  | GlslAssign(Shared<GlslExpr>, string, Shared<GlslExpr>)          // "=", "+=", …
  | GlslExprStmt(Shared<GlslExpr>)
  | GlslIf(Shared<GlslExpr>, [Shared<GlslStmt>], [Shared<GlslStmt>])
  | GlslFor(Shared<GlslStmt>, Shared<GlslExpr>, Shared<GlslStmt>, [Shared<GlslStmt>])
  | GlslWhile(Shared<GlslExpr>, [Shared<GlslStmt>])
  | GlslSwitch(Shared<GlslExpr>, [GlslCase], [Shared<GlslStmt>])  // default
  | GlslReturn(Shared<GlslExpr>) | GlslReturnVoid | GlslBreak | GlslContinue | GlslDiscard
  | GlslBlock([Shared<GlslStmt>])
  | GlslBarrier(string)                       // "barrier", "memoryBarrierShared", …
  | GlslLine(i32, i32)                        // #line N srcid — аналог CppLineDirective
  | GlslComment(string)

export type GlslCase = GlslCase { labels: [i32], body: [Shared<GlslStmt>] }

export type GlslParamQual = GlslIn | GlslOut | GlslInOut
export type GlslParam = GlslParam { qual: GlslParamQual, type_value: Shared<GlslType>, name: string }

export type GlslDecl =
  | GlslVersion(i32, string)                                    // 430, "core"
  | GlslExtension(string, string)
  | GlslDefine(string, string)
  | GlslStruct(string, [GlslField])
  | GlslConstDecl(Shared<GlslType>, string, Shared<GlslExpr>)
  | GlslUniform(GlslLayout, Shared<GlslType>, string)           // свободный uniform / sampler / image
  | GlslUniformBlock(GlslLayout, string, [GlslField], string)   // std140
  | GlslBufferBlock(GlslLayout, string, [GlslField], string, string)  // std430, readonly/writeonly
  | GlslSharedVar(Shared<GlslType>, string)                     // shared (compute)
  | GlslInVar(GlslLayout, Shared<GlslType>, string)
  | GlslOutVar(GlslLayout, Shared<GlslType>, string)
  | GlslLocalSize(i32, i32, i32)
  | GlslGlobalVar(Shared<GlslType>, string, Shared<GlslExpr>)   // приватные глобалы (gSh, seed…)
  | GlslFnProto(Shared<GlslType>, string, [GlslParam])
  | GlslFnDef(Shared<GlslType>, string, [GlslParam], [Shared<GlslStmt>])
  | GlslBlankLine | GlslCommentDecl(string)

export type GlslLayout = GlslLayout { binding: i32, location: i32, set: i32, format: string, packing: string }
export type GlslField = GlslField { type_value: Shared<GlslType>, name: string }
export type GlslStage = GlslStageFragment | GlslStageCompute | GlslStageVertex
export type GlslModule = GlslModule { stage: GlslStage, variant: string, decls: [Shared<GlslDecl>] }
```

Принципиальные отличия от CppAST:
- **Нет выражений-блоков.** MLC — expression-oriented (`if … then … else … end`, `do … end`, `match` как значение). Lowering делает *линеаризацию*: каждое выражение со statement'ами внутри выносится во временную переменную (`_t12`), простые `if`-выражения без побочных эффектов — в `?:`. Это единый проход `lower_expr` → `(stmts, expr)`, как ANF.
- **Порядок объявлений.** GLSL требует define-before-use: печатаем структуры в топологическом порядке, затем прототипы всех функций (`GlslFnProto`), затем определения — проще, чем сортировать.
- **Имена.** Модульные префиксы как в `cpp_naming.mlc` (`sdf_box`), плюс экранирование зарезервированных GLSL-слов (`input`, `output`, `filter`, `sample`, `smooth`, `flat`, `texture`, …) и префиксов `gl_`.

### 2.3. Мономорфизация
`monomorphize.mlc` — worklist от точек входа (`@fragment`/`@compute` функции): для каждого вызова с конкретными аргументами типов и/или `@static`-значениями создаёт клон `SemanticDeclarationFn` с подставленными типами (`checker/substitution.mlc` уже умеет подстановку) и свёрнутыми константами. Ключ инстанса: `name + mangled(type_args) + mangled(static_args)`. Трейт-методы резолвятся статически (в C++ это делает компилятор C++, здесь — мы). Недостижимые функции не печатаются (tree shaking бесплатно).

### 2.4. Печать (`glsl_emit/print.mlc`)
Копия структуры `cpp_emit/print.mlc`: `print_glsl_type`, `print_glsl_expr` (с минимальными скобками по таблице приоритетов, чтобы golden-файлы были читаемы), `print_glsl_stmt(depth)`, `print_glsl_decl`, `print_glsl_module`. Float-литералы всегда с точкой (`1.0`, `1e5` → `1e5` допустимо, `3` → `3.0`). Опция `--glsl-line-directives` эмитит `#line` на `.mlc`-строки (как `CppLineDirective`) — ошибки драйвера будут указывать в исходник MLC.

---

## 3. Checker: принуждение к GPU-подмножеству

### 3.1. Синтаксис
- Лексер: новый токен `@` + идентификатор → `TokenAttribute(name)`; аргументы `@unroll(8)`, `@binding(2)`, `@workgroup(16,16,1)`.
- AST: обёртка `DeclAttributed([Attribute], Shared<Decl>)` по образцу `DeclExported` (`decl_inner` уже разворачивает обёртки — добавить ветку), `Attribute = Attribute { name: string, args: [Shared<Expr>], span: Span }`. Атрибуты на полях записей/параметрах — `ParamAttributed`/поле `attrs` в `Param`.
- `@gpu module` первой строкой файла → все декларации модуля неявно `@gpu`.

Набор атрибутов:

| Атрибут | На чём | Смысл |
|---|---|---|
| `@gpu` | fn, type, module | Проверять подмножество; эмитить в GLSL |
| `@fragment` | fn | Точка входа фрагментного шейдера; параметры — `in`, результат — record → `layout(location=i) out` |
| `@compute(16,16,1)` | fn | Точка входа compute; параметр `id: InvocationId` даёт `gl_GlobalInvocationID` и т.п. |
| `@uniforms` | type (record) | Поля → uniform'ы (по умолчанию свободные, `@uniforms(block)` → std140 UBO) |
| `@buffer(std430, readonly)` | type | SSBO |
| `@binding(n)` | поле/параметр | Явная привязка; иначе — автоназначение |
| `@static` | параметр fn, `const` | Значение известно при компиляции → специализация |
| `@unroll(N)` | fn, цикл | Развернуть ограниченную рекурсию/цикл |
| `@host` | fn | Явно CPU-only (нельзя звать из `@gpu`) |

### 3.2. Регистрация и проверка
- В `FunctionIndex` добавить `function_target_attributes: Map<string,[string]>` (рядом с `function_concurrency_attributes`), в `AdtIndex`/`RecordIndex` — `type_target_attributes`.
- Новый проход `checker/gpu_subset.mlc` после inference (ему нужны типы): обходит тела `@gpu`-функций в SemanticIR и выдаёт диагностики с кодами в `checker/diagnostic_codes.mlc`:
  - `E-GPU-HEAP` — `TArray`/`TShared`/`Map`/`string`/`Region` в типе локала, параметра, поля;
  - `E-GPU-CALL` — вызов не-`@gpu` функции (кроме builtins прелюдии);
  - `E-GPU-RECURSION` — цикл в графе вызовов (с путём);
  - `E-GPU-CLOSURE` — лямбда, которая не инлайнится статически;
  - `E-GPU-F64`, `E-GPU-I64` — неподдерживаемые скаляры;
  - `E-GPU-RECURSIVE-TYPE` — рекурсивный sum type не-`@static`;
  - `E-GPU-DYN-INDEX-WRITE` (warning) — запись по динамическому индексу в локальный массив (медленно на GPU);
  - `E-GPU-STATIC` — `@static`-аргумент не вычислим при компиляции.
- Транзитивность: `@gpu` пропагирует на вызываемые функции из того же модуля автоматически (inference атрибута по графу вызовов), а для импортированных — требуется явная пометка (это граница API модуля). Так библиотека `sdf.mlc` остаётся вызываемой и с CPU (C++ таргет) — **один исходник для GPU и для CPU-тестов** (§7.4).
- Тот же проход работает и при `--target=cpp`: `@gpu`-функции компилируются в C++ (векторы → `mlc::gpu::vec3` из нового `runtime/include/mlc/gpu/vec.hpp`), что даёт CPU-эталон.

---

## 4. Встроенные векторные типы, swizzle, uniforms, буферы, workgroups

### 4.1. Типы
В `Type` добавить: `TF32`, `TU32`, `TVec(Shared<Type>, i32)`, `TMat(i32, i32)`, `TFixedArray(Shared<Type>, i32)`, `TSampler(string)`, `TImage(string, string)`. Имена в исходнике: `Vec2 Vec3 Vec4 IVec2.. UVec2.. BVec2.. Mat2 Mat3 Mat4 Mat2x3…`, `Sampler2D`, `Sampler2DArray`, `Image2D<Rg32f>`.

Почему встроенные, а не `type Vec3 = { x: f32, … }` + трейт `Add` (D2 в `docs/MLC.md` — **ещё не реализован в mlcc**): нужны swizzle, смешанные операции `vec*scalar`, `mat*vec`, покомпонентные builtins (`max(q, 0.0)`), и прямое соответствие GLSL. D2 можно реализовать позже, и тогда пользовательские типы (комплексные числа для Riemann sheets) получат операторы тем же путём.

### 4.2. Инференс
- `infer_vector.mlc`: правила `binary_operation_result_type` для пар (`vecN op vecN`, `vecN op f32`, `f32 op vecN`, `matN * vecN`, `vecN * matN`, `matN * matN`, сравнения → `bool` только для скаляров; покомпонентные `lessThan` и т.д. — функции).
- Литералы: `1.0` в контексте `f32` — `f32` (контекстная типизация литералов уже упомянута в C4: «без суффикса — вывод из контекста»). В `@gpu`-модуле дефолт вещественного литерала = `f32`, а не `f64`.
- Конструкторы: `vec3(x)`, `vec3(v2, z)`, `vec4(v3, 1.0)` — builtin-функции с перегрузкой по суммарной размерности (проверка: сумма компонент = N или ровно 1 скаляр).
- Swizzle: в `infer_expr_field` — если тип объекта `TVec(T, n)` и имя поля ∈ `[xyzw]{1,4}` | `[rgba]{1,4}` | `[stpq]{1,4}` без смешивания наборов и с индексами < n → `T` или `TVec(T, len)`. Присваивание в swizzle (`p.xz = …`) допустимо только без повторов (`check_mutations.mlc`).
- Builtins: `compiler/std/gpu/prelude.mlc` — `extern fn` с перегрузкой по типам (`length`, `dot`, `cross`, `normalize`, `mix`, `clamp`, `smoothstep`, `step`, `fract`, `floor`, `abs`, `min`, `max`, `sin`, `cos`, `atan`, `exp`, `pow`, `sqrt`, `mod`, `texture`, `texelFetch`, `imageLoad`, `imageStore`, `atomicAdd`, `barrier`, `float_bits_to_uint`…). Генерик-форма: `extern fn mix<T: GenFloat>(a: T, b: T, t: f32) -> T = "mix"` — трейт-маркер `GenFloat` реализован для `f32, Vec2..4`.

### 4.3. Uniforms, буферы, bindings
```mlc
@gpu @uniforms type Frame = {
  res: Vec2, frame: i32, time: f32,
  cam_pos: Vec3, cam_rot: Mat3, tan_f: f32,
  kick: f32, bass: f32, hi: f32, build: f32,
  kick_hist: [f32; 73],
  lens: [Vec4; 4]
}
@gpu @buffer(std430, readonly) type GsParams = { feed: [f32], kill: [f32] }  // runtime-sized
```
- Точка входа принимает их параметрами: `@fragment fn main(frag: FragIn, u: Frame, gs: Sampler2DArray) -> GBuffer`. Lowering превращает параметры-ресурсы в глобальные объявления, обращения `u.time` → `uTime` (свободные uniform'ы, совместимо с текущим `anim4.py`, где имена `uXxx`) или `Frame.time` (UBO-режим).
- Схема имён по умолчанию: `u` + PascalCase поля (`cam_pos` → `uCamPos`), переопределяется `@name("uCamPos")` — это позволяет **сохранить API хоста** без переписывания `anim4.py`.
- Binding allocator (`layout.mlc`): детерминированный порядок (порядок параметров точки входа, затем полей), отдельные пространства для `sampler`, `image`, `ubo`, `ssbo`; явные `@binding(n)` резервируются первыми; конфликт → ошибка.
- std140/std430 offsets считаются в компиляторе (vec3 = 16-байтное выравнивание и т.п.) и пишутся в манифест — хосту не нужно гадать.

### 4.4. Compute и workgroups
```mlc
@gpu @compute(16, 16, 1)
fn gs_step(id: Invocation, p: GsUniforms,
           src: Image2D<Rg32f> @readonly @binding(0),
           dst: Image2D<Rg32f> @writeonly @binding(1)) -> unit = …
```
- `Invocation { global: UVec3, local: UVec3, group: UVec3, local_index: u32 }` → `gl_GlobalInvocationID` и т.д.
- `@shared let tile: [Vec2; 18 * 18]` → `shared vec2 tile[324];` + `barrier()` из прелюдии. Проверка: `@shared` только в `@compute`.
- Манифест содержит `local_size` и рекомендуемую формулу `groups = ceil(size / local_size)`.

---

## 5. Compile-time специализация: full / schematic

Сейчас `pt4.frag` — один шейдер, `uSchem` — runtime-флаг, а `schematic.py` — отдельный рендерер на том же шейдере. Цель: **один исходник, два (и более) скомпилированных варианта**, schematic не платит за path tracing и наоборот.

Механизм (три уровня, от простого к мощному):

1. **`@static`-константы модуля + `--variant`.**
   ```mlc
   @gpu type Variant = Full | Schematic
   @static const VARIANT: Variant = variant_from_cli()      // mlcc --variant=schematic
   @static const SPP_MAX: i32 = match VARIANT { Full => 64, Schematic => 1 }
   ```
   Константы сворачиваются в SemanticIR (простой evaluator по константным выражениям: арифметика, `match`/`if` по `@static`, вызовы `@static fn`). `if VARIANT == Schematic then … else … end` → ветка удаляется (DCE), недостижимые функции не эмитятся мономорфизатором.
2. **`@static`-параметры функций** → мономорфизация (`shade__Full`, `shade__Schematic`). Это позволяет иметь обе версии в одном модуле (например, «схематичный» вторичный луч внутри полной сцены).
3. **Сцена как `@static`-данные** (stretch, M5): рекурсивный sum type
   ```mlc
   type Sdf = Box { b: Vec3 } | Torus { t: Vec2 } | Union(Shared<Sdf>, Shared<Sdf>) | Translate(Vec3, Shared<Sdf>) | Bump(f32, Shared<Sdf>)
   ```
   вычисляется при компиляции (VM `compiler/vm/` уже интерпретирует MIR — можно расширить на float или написать маленький evaluator по SemanticIR), и `@static fn eval_sdf(s: Sdf, p: Vec3) -> f32` разворачивается в плоский GLSL. Для варианта schematic те же узлы подменяются (`Bump` → identity, `fbm`-детали отключены, `max_steps` меньше).

Вывод компилятора: `out/pt.full.frag`, `out/pt.schematic.frag`, `out/gs_step.comp`, `out/manifest.json` (варианты перечислены внутри). Флаг `--variant=all` компилирует все значения enum `Variant`.

Политика различий (что «schematic» отрезает) задаётся в MLC, а не в хосте: `n_bounces`, NEE, SSS-толщина (`stoneThick` — 10 доп. вызовов `map`), шум/fbm в `stoneBump`, количество шагов трассировки (1100 → 300), MRT-выходы (у schematic только `oCol`+`oG1`).

---

## 6. Интероп с хостом: JSON-манифест

`mlcc --target=glsl --emit-manifest` пишет `manifest.json`:

```json
{
  "mlc_version": "afb5d8a",
  "source_hash": "sha256:…",
  "programs": [
    {
      "name": "pt", "variant": "full", "stage": "fragment", "file": "pt.full.frag", "glsl_version": "430 core",
      "inputs":  [{"name": "uv", "type": "vec2", "location": 0}],
      "outputs": [{"name": "oCol", "type": "vec4", "location": 0}, {"name": "oG1", "type": "vec4", "location": 1}],
      "uniforms": [
        {"name": "uTime", "type": "float", "mlc": "Frame.time"},
        {"name": "uKH", "type": "float", "array": 73, "mlc": "Frame.kick_hist"},
        {"name": "uLens", "type": "vec4", "array": 4, "mlc": "Frame.lens"}
      ],
      "samplers": [{"name": "uGSA", "type": "sampler2DArray", "binding": 0, "mlc": "gs"}],
      "images": [], "ubos": [], "ssbos": [],
      "enums": {"Material": {"Stone": 0, "Metal": 1, "Emissive": 2}},
      "tagged_layouts": {"Material": {"Stone.alb": "f0", "Metal.f0": "f0"}}
    },
    {
      "name": "gs_step", "variant": "default", "stage": "compute", "file": "gs_step.comp",
      "local_size": [16, 16, 1],
      "images": [{"name": "src", "format": "rg32f", "binding": 0, "access": "readonly"},
                 {"name": "dst", "format": "rg32f", "binding": 1, "access": "writeonly"}],
      "uniforms": [{"name": "uFeed", "type": "float"}, {"name": "uKill", "type": "float"}],
      "ssbos": [{"name": "GsParams", "binding": 2, "layout": "std430",
                 "fields": [{"name": "feed", "type": "float", "offset": 0, "runtime_array": true}]}]
    }
  ]
}
```

Python-сторона — маленький модуль `viz/mlc_host.py` (~150 строк):
- `load(manifest, ctx, variant)` → `ctx.program(...)`/`ctx.compute_shader(...)`;
- `set_uniforms(prog, dict)` — валидирует имена/типы/длины массивов по манифесту (сейчас в `anim4.Renderer.set` опечатка в имени uniform'а молча игнорируется: `if k in P`);
- `bind(prog, samplers=…, images=…, ssbos=…)` по биндингам из манифеста;
- `dispatch(cs, size)` — считает число групп по `local_size`;
- опционально генерирует `frame_types.py` (dataclass `Frame` по `@uniforms`-записи) — типобезопасный хост.

Переход `anim4.py`: `require=330` → `require=430`; вершинный шейдер остаётся рукописным (или генерируется тривиальный fullscreen-триангл); `set()` заменяется на `mlc_host.set_uniforms`.

---

## 7. Тестирование

1. **Golden GLSL** (`compiler/tests/glsl/golden/*.mlc` + `*.expected.glsl` + `*.expected.manifest.json`): гейт `run_glsl_golden.sh` компилирует, `diff -u` с эталоном; `--update` перезаписывает. Покрытие: каждый узел GlslAST, swizzle, линеаризация `if/match`-выражений, tagged struct, мономорфизация, специализация `--variant`. Плюс негативные тесты на каждый `E-GPU-*` (ожидаемый код диагностики и span) — по образцу `test_checker.mlc` (`assert_eq_int(... 'W-EXTERN-ATTR' ...)`).
2. **Валидация драйвером-независимым фронтендом:** `glslangValidator -S frag|comp --target-env opengl` на всех golden-выходах и на сгенерированной сцене (ловит ошибки, которые llvmpipe прощает). Дополнительно — компиляция через moderngl на llvmpipe (`ctx.program`, `ctx.compute_shader`).
3. **Численная эквивалентность compute:** `gs_step.comp` против `gs3.gs_steps` (numba, float32) на 128×128, 200 шагов, одинаковые начальные условия → `max|ΔU|, max|ΔV| < 1e-4` (порядок сложений различается → не бит-в-бит).
4. **CPU-эталон из того же исходника:** `@gpu`-функции компилируются `--target=cpp` (§3.2) → unit-тест на C++ сравнивает `sd_box/sd_torus/sd_capsule` с результатами GLSL-версии, считанными из SSBO (compute-шейдер-«пробник», вызывающий функцию на сетке точек). Это ловит ошибки lowering без рендеринга.
5. **Pixel-diff против `pt4.frag`:**
   - Детерминированный режим: `uSchem=1` (детерминированная ветка, без RNG), 160×90, кадры 0, 120, 360; плюс G-buffer (нормаль, глубина, sheet) в `oG1/oG2` полной версии при `uSpp=1, uBounces=0` — они детерминированы.
   - Сценарий «сплайс»: скрипт `splice.py` вырезает из `pt4.frag` рукописные `sdBox/sdTorus/sdCap/U` (и далее `map`, материалы…) и подставляет сгенерированные функции — рендерим оригинал и сплайс одной программой хоста, сравниваем: `max abs diff ≤ 2/255` после тонмапа, доля пикселей с diff > 1/255 < 0.1 %. Расхождения визуализируем heat-map'ой.
   - Для полного path tracing (стохастика): фиксируем `uFrame`, `uSBase`, PCG-seed — PCG целочисленный, поэтому последовательность совпадает; сравниваем 16 spp по PSNR > 45 дБ (float-reassociation даёт единичные расхождения траекторий).
6. **Регресс производительности:** время кадра llvmpipe 160×90 и на поде (Runpod) — сгенерированный шейдер не медленнее рукописного более чем на 5 %; линеаризация не должна плодить сотни временных (`glslangValidator` + счёт инструкций SPIR-V через `spirv-opt --print-all` как грубая метрика).
7. **Самосборка:** после изменений в checker прогон `diff -rq` mlcc/mlcc2 (README) — новый бэкенд не должен менять C++-выход компилятора самого себя.

---

## 8. Вехи

Оценки — для одного разработчика с агентами, «рабочие дни» чистого времени; разброс ±50 %.

| Веха | Содержание | Критерий готовности | Оценка |
|---|---|---|---|
| **M0. Каркас** | Собрать `mlcc` (`MLCC_FORCE_RUBY=1 compiler/build.sh`); `--target=glsl`, `--variant=`, `--emit-manifest` в `compile_options.mlc` / `ModularCompileInput`; пустые `glsl_ir/glsl_ast.mlc`, `glsl_emit/print.mlc` с unit-тестами печати; токен `@` + `DeclAttributed`; гейт `run_glsl_golden.sh`; `glslang-tools` на боксе. | `mlcc --target=glsl empty.mlc` → валидный `#version 430` шейдер с пустым `main`, проходит glslangValidator. | 3–4 дня |
| **M1. Скаляры + векторы + функции** | `TF32/TU32/TVec/TMat/TFixedArray`; литералы f32 по контексту; swizzle; операторы vec/mat; прелюдия builtins; record → struct; lowering fn/let/if/while/for/return; линеаризация выражений-блоков; `@gpu`-проверка (HEAP, CALL, F64, RECURSION); `@fragment` + `@uniforms` (свободные uniform'ы с `@name`). | SDF-примитивы из `pt4.frag` (`sdBox`, `sdTorus`, `sdCap`, `U`, `r2`, `rotX/rotY`) компилируются и совпадают с рукописными на сетке точек (тест §7.4). | 8–12 дней |
| **M2. Compute + ресурсы + манифест (первый срез)** | `@compute`, `Invocation`, `Image2D`, `Sampler2DArray`, SSBO std430, `@shared`, `barrier`; binding allocator; `manifest.json`; `viz/mlc_host.py`; Gray-Scott compute. | **Первый срез (§9)** зелёный: GS на GPU ≈ numba; мини-сцена из 3 SDF рендерится через сгенерированный шейдер и совпадает со сплайсом в `pt4.frag`. | 6–9 дней |
| **M3. Sum types, match, дженерики** | Tagged struct с упаковкой слотов; `match` → `switch`; `Option`; мономорфизация дженериков и трейт-методов; tail-call → loop; `@unroll(N)`; материалы (`Mat`, translucent stone `stoneThick`/SSS) и BSDF на MLC. | Материалы + `evalBSDF/sampleBSDF` из `pt4.frag` на MLC; pixel-diff G-buffer + 16 spp PSNR > 45 дБ. | 10–14 дней |
| **M4. Специализация + полный порт сцены** | `@static`-константы и параметры, `--variant=all`, DCE; порт `map()`, `trace()` (геодезические по `n = 1 + 2Φ`, `phiGrad`, `pointMass`), door recursion, Riemann sheets (`gSh`, `smod3`), mirrors, вертебры, мандала, GS-обёртка стен, аудио-драйверы (`kickAt`, `uKH`) как `@uniforms`-модуль. `anim4.py` переключается на манифест. | `pt.full.frag` и `pt.schematic.frag` из одного исходника; pixel-diff со `pt4.frag` на 3 эталонных кадрах в обоих режимах; рендер 20-секундного клипа без регресса скорости > 5 %. | 15–20 дней |
| **M5. Эргономика и «сцена как данные»** | Рекурсивный `Sdf` как `@static`-данные + compile-time evaluator; `#line`-маппинг ошибок драйвера в `.mlc`; `debug_printf`; генерация `frame_types.py`; D2 (операторные трейты) для пользовательских типов (комплексные числа для Riemann sheets); документация в `docs/` + строка в таблице «Два пайплайна». | Новая сцена/эффект добавляется только в MLC, без правки Python, кроме хореографии значений. | 10–15 дней |

Итого до полного порта (M0–M4): ~6–8 недель; M5 — ещё 2–3.

---

## 9. Первый срез (минимальный вертикальный путь)

Цель — за M0–M2 (≈ 3–4 недели) получить **работающий GLSL из MLC** в реальном хосте, без sum types и дженериков.

### 9.1. Исходник `viz/mlc_scene/slice1.mlc`
```mlc
@gpu module

// --- 3 SDF-примитива (1:1 с pt4.frag:140–146) ---
fn sd_box(p: Vec3, b: Vec3) -> f32 = do
  const q = abs(p) - b
  length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0)
end

fn sd_torus(p: Vec3, t: Vec2) -> f32 = do
  const q = vec2(length(p.xz) - t.x, p.y)
  length(q) - t.y
end

fn sd_capsule(p: Vec3, a: Vec3, b: Vec3, r: f32) -> f32 = do
  const pa = p - a
  const ba = b - a
  const h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0)
  length(pa - ba * h) - r
end

type Hit = { d: f32, mat: f32 }
fn hit_union(a: Hit, d: f32, m: f32) -> Hit = if d < a.d then Hit { d: d, mat: m } else a end

@uniforms type SliceU = { res: Vec2, time: f32, cam_pos: Vec3, cam_rot: Mat3, tan_f: f32, kick: f32 }

fn scene(p: Vec3, u: SliceU) -> Hit = do
  let mut r = Hit { d: 1e5, mat: 0.0 }
  r = hit_union(r, sd_box(p - vec3(0.0, 0.8, 4.0), vec3(1.2, 0.6, 0.4)), 1.0)
  r = hit_union(r, sd_torus(p - vec3(0.0, 2.55, 0.0), vec2(0.47 + 0.05 * u.kick, 0.055)), 3.0)
  r = hit_union(r, sd_capsule(p, vec3(0.0, -2.0, 0.0), vec3(0.0, -0.5, 0.0), 0.05), 3.0)
  r
end

type FragIn = { uv: Vec2 }
type Out = { col: Vec4 @location(0), g1: Vec4 @location(1) }

@fragment
fn main(f: FragIn, u: SliceU) -> Out = do
  const ndc = f.uv * 2.0 - 1.0
  let mut x = u.cam_pos
  const dir = normalize(u.cam_rot * vec3(ndc.x * u.tan_f * u.res.x / u.res.y, ndc.y * u.tan_f, 1.0))
  let mut t = 0.0
  let mut h = Hit { d: 1e5, mat: 0.0 }
  for i in 0..256 do
    h = scene(x, u)
    if h.d < 0.001 then break end
    x = x + dir * h.d
    t = t + h.d
  end
  Out { col: vec4(vec3(h.mat / 15.0), 1.0), g1: vec4(0.0, 0.0, 0.0, t) }
end

// --- Gray-Scott, 9-точечный лапласиан (1:1 с gs3.gs_steps), периодические границы ---
@gpu @buffer(std430, readonly) type GsMaps = { fk_d: [Vec4] }   // feed, kill, Du, Dv на пиксель
@uniforms type GsU = { n: i32 }

fn at(img: Image2D<Rg32f>, i: i32, j: i32, n: i32) -> Vec2 =
  image_load(img, ivec2((i + n) % n, (j + n) % n)).xy

@compute(16, 16, 1)
fn gs_step(id: Invocation, u: GsU, maps: GsMaps @binding(2),
           src: Image2D<Rg32f> @readonly @binding(0), dst: Image2D<Rg32f> @writeonly @binding(1)) -> unit = do
  const i = i32(id.global.x)
  const j = i32(id.global.y)
  if i >= u.n || j >= u.n then return () end
  const c = at(src, i, j, u.n)
  const lap = 0.2 * (at(src, i - 1, j, u.n) + at(src, i + 1, j, u.n) + at(src, i, j - 1, u.n) + at(src, i, j + 1, u.n))
            + 0.05 * (at(src, i - 1, j - 1, u.n) + at(src, i - 1, j + 1, u.n) + at(src, i + 1, j - 1, u.n) + at(src, i + 1, j + 1, u.n))
            - c
  const m = maps.fk_d[j * u.n + i]
  const uvv = c.x * c.y * c.y
  const nu = c.x + m.z * lap.x - uvv + m.x * (1.0 - c.x)
  const nv = c.y + m.w * lap.y + uvv - (m.x + m.y) * c.y
  image_store(dst, ivec2(i, j), vec4(nu, nv, 0.0, 0.0))
end
```

Ожидаемый фрагмент вывода (`out/slice1.default.frag`):
```glsl
#version 430 core
struct Hit { float d; float mat; };
uniform vec2 uRes; uniform float uTime; uniform vec3 uCamPos; uniform mat3 uCamRot; uniform float uTanF; uniform float uKick;
float sd_box(vec3 p, vec3 b) { vec3 q = abs(p) - b; return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0); }
...
```

### 9.2. Что нужно реализовать для среза (подмножество M0–M2)
- Лексер/парсер: `@attr`, `[T; N]` (не обязательно в срезе), `for i in a..b` (есть), атрибуты на полях.
- Типы: `TF32`, `TVec`, `TMat3`, `Image2D<Rg32f>`, `Invocation`; swizzle; `vec*f32`, `mat3*vec3`.
- Без sum types, без дженериков, без `@static` — только records, функции, циклы.
- Lowering: линеаризация `if`-выражения в `hit_union` (→ `?:`, т.к. без побочных эффектов), `do…end`, record-литерал → конструктор `Hit(d, m)`, `return ()` в `unit`-функции → `return;`.
- Печать + манифест + `mlc_host.py`.

### 9.3. Проверки среза
1. `glslangValidator` на `slice1.default.frag` и `gs_step.comp` — 0 ошибок.
2. GS: 128×128, 200 шагов на llvmpipe vs `gs3.gs_steps` (numba) → `max|Δ| < 1e-4`; затем 1024² × 70 шагов (как один слой `gs4.py`) — сравнение скорости CPU-numba vs GPU (на поде).
3. SDF: compute-пробник на сетке 64³ — `sd_box/sd_torus/sd_capsule` vs копии из `pt4.frag` → бит-в-бит (одинаковые выражения), допускается 1 ULP.
4. Сплайс: в `pt4.frag` рукописные `sdBox/sdTorus/sdCap` заменяются сгенерированными; `uSchem=1`, 160×90, 3 кадра → `max diff == 0` (ожидаем полное совпадение, т.к. выражения идентичны).
5. Хост: `mlc_host.load(manifest)` в мини-скрипте рендерит `slice1` и пишет PNG; опечатка в имени uniform'а падает с понятной ошибкой.

---

## 10. Риски и смягчение

| Риск | Вероятность | Смягчение |
|---|---|---|
| **Бутстрап через Ruby**: новые модули mlcc собираются Ruby-компилятором; если код бэкенда использует фичи, которых нет в Ruby (`lib/mlc/`), `build.sh` упадёт. | высокая | Писать бэкенд на «скучном» MLC (records, match, while, `[T]`, `Map`) — как остальной `compiler/`; GLSL-фичи языка (векторы, `@gpu`) нужны только в *пользовательских* программах, и реализуются только в mlcc (по прецеденту concurrency, фиксируется в таблице «Два пайплайна»). |
| Расширение `Type` новыми вариантами ломает exhaustive `match` в десятках мест (`TF64 =>` встречается в `stmt_cpp.mlc`, `let_pat.mlc`, `stmt_eval.mlc` …). | высокая | Отдельный коммит M1a: добавить варианты + механически дополнить все `match` (компилятор сам покажет места), прогнать `rake test_compiler_mlc` и mlcc/mlcc2 diff до любой GLSL-логики. |
| Линеаризация expression-oriented кода раздувает шейдер временными переменными → регистровое давление, медленнее рукописного. | средняя | `?:` для чистых веток, copy-propagation на GlslAST, сравнение SPIR-V размера и времени кадра в гейте; драйверы хорошо чистят SSA-временные. |
| Отсутствие мономорфизации в mlcc (дженерики сейчас → C++ templates). | средняя | В M1–M2 дженерики запрещены в `@gpu` (явная ошибка), мономорфизатор — M3, с переиспользованием `checker/substitution.mlc`. |
| Семантика float: MLC сейчас знает только `f64`; литералы, `%`/`mod`, сравнения NaN, деление на 0 (в MLC — panic для целых) отличаются от GLSL. | средняя | В `@gpu` чёткое определение: целочисленное деление на 0 — UB как в GLSL (документировать), `%` для float запрещён (`mod()`), литералы `f32` по контексту. |
| Нестабильность GLSL на разных драйверах (llvmpipe vs NVIDIA на поде). | средняя | glslangValidator как строгий арбитр; гейт прогоняется на обоих; не полагаться на неявные преобразования `int→float` (всегда явные конструкторы). |
| Pixel-diff path tracer'а шумит из-за стохастики. | средняя | Детерминированные режимы (`uSchem`, G-buffer, `uBounces=0`), фиксированный PCG-seed, PSNR-порог для полного PT. |
| Размер работы по порту `pt4.frag` (~550 строк плотного GLSL, глобальное состояние `gSh`, `gRec`, `gShadow`, `seed`). | высокая | Глобалы → явная запись `PathState` с `mut`-передачей (`inout`) — честнее и тестируемее; порт поэтапно со сплайсом: каждый перенесённый блок сразу заменяется в `pt4.frag` и проверяется pixel-diff, оригинал — живой эталон до конца M4. |
| Ветки в main-репозитории mlc идут быстро (активная разработка script_vm и т.п.) → конфликты. | средняя | Бэкенд изолирован в новых каталогах; точки касания с общим кодом — `Type`, лексер, `compile_options`, `pipeline` — маленькие коммиты, быстро вливать. |
| `@static`-evaluator и «сцена как данные» разрастаются в отдельный интерпретатор. | средняя | Ограничить M4 константным свёртыванием + мономорфизацией по `@static`-параметрам; полноценный evaluator (на базе `compiler/vm/`) — только в M5 и только если M4 показал нужду. |

---

## 11. Открытые решения (зафиксировать в M0)

1. Синтаксис атрибутов: `@gpu` (новый токен) vs ключевое слово `gpu fn` (как `extern fn`). Рекомендация — `@attr`: расширяемо, не засоряет ключевые слова, и пригодится для `@inline`, `@deprecated` в C++-таргете.
2. Свободные uniform'ы (совместимо с `anim4.py`) vs UBO std140 по умолчанию. Рекомендация — свободные в M1–M4 (минимум изменений хоста), UBO — опция `@uniforms(block)`.
3. Исходники сцены — в репозитории mlc (`misc/examples/gpu/`) или в `viz/`? Рекомендация — библиотека `sdf/materials/effects` как пакет (`docs/PACKAGE_MANAGER.md`, git URL + SHA pin), сцена Horus — в `viz/`.
4. SPIR-V (через `glslangValidator -V`) как будущий таргет для Vulkan/WebGPU — не в этом плане, но GlslAST проектируем так, чтобы `#version 450` + `layout(set=…)` было опцией печати (`GlslLayout.set` уже заложен).
