# FFI-слой MLC — инфраструктурные требования (2026-07)

Parent: [PLAN.md](PLAN.md) §10 (Nim — C++ interop), Фаза 9 (новая, см. ниже).
Trigger: обсуждение биндингов к libpq/OpenGL/GTK/ffmpeg — все четыре сегодня
блокированы одним и тем же набором отсутствующих примитивов языка/stdlib, не
специфичным для конкретной библиотеки. Этот документ описывает только
инфраструктуру; сами биндинги — отдельные будущие треки (не создавать раньше
времени, см. §7).

## 1. Факт: текущее состояние (проверено в коде, не по памяти)

`extern fn` в MLC сегодня — разметка без ABI. Пайплайн:

| Слой | Файл | Факт |
|------|------|------|
| Лексер/парсер (mlcc) | `compiler/frontend/lexer.mlc`, `compiler/frontend/parser/decls.mlc:490-504` | `extern fn name(...)` → `DeclFn` с телом `ExprExtern` (пустой маркер) |
| Ruby парсер | `lib/mlc/source/parser/declaration_parser.rb:10-54,428-432,764-779` | `external: true`, тело опционально |
| Checker | `compiler/checker/check/check.mlc:138,167-171` | `is_extern_body`; E038 — default-параметры запрещены на extern |
| Codegen (mlcc) | `compiler/codegen/decl_cpp.mlc:517,538,1145,1154` | extern → `empty_cpp_declaration()` — **никакого C++ не генерируется** |
| Ruby codegen | `lib/mlc/backends/cpp/codegen.rb:254` | extern (`body.nil?`) → нет forward declaration |

`extern "C"` существует только в парсере C++-заголовков (`compiler/cpp_parse/parser/cpp_decls.mlc`) для *чтения* чужих `.h` — в синтаксисе самого MLC такой конструкции нет.

**Opaque type:** есть только в Ruby-бутстрапе (`export type Window` без `=` →
`OpaqueType`, `lib/mlc/registries/type_registry.rb:150-153`). В self-hosted
`mlcc` — нет: `parse_type_decl` без `=` уходит в `parse_variants`
(`compiler/frontend/parser/decls.mlc:699-733`), не в opaque.

**Импорт `.h`:** `compiler/cpp_parse/header_import.mlc` парсит C++ заголовок и
регистрирует типы/функции в registry для checker'а — но (а) не эмитит
`#include` в сгенерированный `.cpp` (`compiler/codegen/cpp_naming.mlc:113`
берёт `.hpp` только от MLC-модулей), (б) C struct конвертируется в MLC record
**по значению** (`header_import.mlc:153-160`) — копия полей, не
layout-совместимый доступ к C-памяти.

**Линковка:** `compiler/build_bin.sh` собирает только
`runtime/src/{io,core,profile}` (`:18-22`), никаких `-l*`
(`lib/mlc/common/modular_compilation/modular_compiler.rb:83-88` — та же
цепочка). Внешнюю библиотеку слинковать штатно нельзя.

**Callback:** `fn(...)` всегда → `std::function<Ret(Args...)>`
(`compiler/codegen/decl/type_gen.mlc:60-63`, Ruby `type_mapper.rb:126-132`).
C ABI требует `Ret(*)(Args...)` — моста нет.

**Что уже есть и можно переиспользовать:** `char*`↔`mlc::String`
(`runtime/include/mlc/core/string.hpp:80-86,127-128`), nullable через
`mlc::option::from_nullable` для `shared_ptr` (`runtime/include/mlc/core/option.hpp:10-31`),
синтаксис-заготовка в `PLAN.md:564-567` (`extern fn sqrt(x: f64) -> f64 =
"sqrt" from "<cmath>"`, `extern type FILE = "FILE" from "<stdio.h>"`) — не
реализован, но синтаксическое направление верное, этот документ его
формализует и расширяет.

## 2. Семь инфраструктурных пробелов (общие для любого C/C++ биндинга)

Ни один из четырёх примеров (libpq, OpenGL, GTK, ffmpeg) не заработает без
всех семи — это не выбор "сделать один, пропустить другой", это один слой.

### 2.1 `RawPointer[T]` — непрозрачный указатель

Нет типа для `PGconn*`, `GtkWidget*`, `AVFrame*`. Без него нет opaque handle.

```mlc
extern type PGconn = "PGconn" from "<libpq-fe.h>"
// использование: RawPointer[PGconn]
```

`RawPointer[T]`: не refcounted, не `Shared<T>`, сравнение на `null`, явный
`is_null`. Codegen → `T*` напрямую, без обёртки.

### 2.2 `extern fn ... = "c_name" from "<header>"` — реальный codegen

Расширение уже предложенного в `PLAN.md:564-567` синтаксиса: помимо
`#include`, генерировать настоящий C++ вызов (сегодня — пустая строка).

```mlc
extern fn PQconnectdb(conninfo: RawPointer[Byte]) -> RawPointer[PGconn]
  = "PQconnectdb" from "<libpq-fe.h>"
```

→ `#include <libpq-fe.h>` в сгенерированный `.cpp` того модуля, где объявлена
функция (не глобально — иначе конфликт имён между модулями с разными C-хедерами).

### 2.3 Линковка внешних библиотек

`extern fn`/`extern type` с `from "<header>"` не знает, из какой библиотеки
линковать символ — заголовок и библиотека не всегда совпадают по имени
(`libpq-fe.h` → `-lpq`). Нужна явная декларация на уровне модуля:

```mlc
extern lib "pq"
```

→ `build_bin.sh` собирает список `extern lib` по всем импортированным модулям
программы, добавляет `-l<name>` в финальную линковку (не в каждый `.o`).

### 2.4 `extern fn(Args) -> Return` как тип — C function pointer

Отличный от closure-типа `fn(Args) -> Return` (который всегда `std::function`
после `TRACK_LANG_CLOSURE_ESCAPE`/template — оба варианта несовместимы с C
ABI). `extern fn(...)` в позиции типа = `Ret(*)(Args...)`, запрет на capture
(проверяется как non-escaping template функция, но без generic — конкретный
raw pointer).

```mlc
extern fn PQsetNoticeProcessor(
  conn: RawPointer[PGconn],
  processor: extern fn(RawPointer[Byte], RawPointer[Byte]) -> Unit,
  argument: RawPointer[Byte]
) -> extern fn(RawPointer[Byte], RawPointer[Byte]) -> Unit
  = "PQsetNoticeProcessor" from "<libpq-fe.h>"
```

Передаваемая как `processor` MLC-функция обязана быть top-level `fn` без
захвата (проверяется при вызове, не при объявлении — тот же checker-проход,
что для non-escaping closures, переиспользовать `compiler/checker/escape_analysis.mlc`).

### 2.5 Владение opaque-хендлом (RAII без общего `defer`)

Общий `defer` (см. `CONCURRENCY_V2.md` п.25) в языке не реализован — не
делать его прерогативой этого трека. Вместо этого — узкий механизм именно
для `extern type`: явная функция освобождения в декларации типа.

```mlc
extern type PGconn = "PGconn" from "<libpq-fe.h>"
  drop "PQfinish"
```

`RawPointer[PGconn]`, полученный как **владеющий** результат функции,
помеченной `-> owned RawPointer[T]` (маркер на уровне сигнатуры, не типа —
не каждый `RawPointer[PGconn]` владеет, `PQerrorMessage` возвращает
non-owning `char*`), генерирует RAII-обёртку (`std::unique_ptr` с custom
deleter `PQfinish`) вместо голого указателя. Без маркера — голый `T*`,
ответственность на вызывающем (как в C).

### 2.6 Layout-совместимость C struct

Header import сегодня копирует C struct в MLC record по значению
(`header_import.mlc:153-160`) — непригодно для структур, которые C-код
мутирует по адресу (`AVFrame`, `PGresult` — впрочем последние два в основном
opaque, не голый struct, так что для libpq/GTK/OpenGL это не блокер).
**Для ffmpeg-класса библиотек с "открытыми" struct** нужен отдельный режим
без refcounting-обёртки поверх record — не проектировать сейчас (нет
конкретного case), зафиксировать как известный будущий пробел (см. §7).

### 2.7 Concurrency-контракт для extern

Уже описан в `docs/CONCURRENCY_V2.md:369-380` (design only): метаданные
`thread_safe | blocking | thread_affine` на `extern fn`/`extern type`.

```mlc
extern fn PQexec(conn: RawPointer[PGconn], query: RawPointer[Byte])
  -> RawPointer[PGresult]
  = "PQexec" from "<libpq-fe.h>"
  blocking
```

Без чекера, который знает про `blocking`, вызов `PQexec` внутри
`Isolate`/`async`-контекста (когда они появятся) незаметно завешивает
воркер — тот самый класс багов, ради которого весь CONCURRENCY_V2 затевался.
**Зависимость:** реализуется после `TRACK_CONCURRENCY_V2` STEP=1 (`Send`/`Sync`
предикаты уже существуют как база для checker-инфраструктуры).

## 3. Что явно НЕ входит в этот слой

- Генератор биндингов (аналог `bindgen`/`gir`) — писать после первого
  ручного биндинга (см. `LANGUAGE_AUDIT`-обсуждение сессии), не раньше: нет
  данных, какие C-конструкции реально нужно покрывать.
- Variadic C-функции (`printf`-стиль) — редко нужны для целевых
  библиотек (libpq/OpenGL/GTK не требуют variadic в горячем пути), отложить
  до конкретного случая.
- Layout-совместимый "raw struct" режим (§2.6) — нет прецедента без ffmpeg,
  не проектировать заранее.
- Сами биндинги к libpq/OpenGL/GTK/ffmpeg — отдельные треки, создавать только
  после закрытия этого слоя и только под конкретную задачу (см. §7).

## 4. Порядок реализации (конвенция проекта: Ruby-эталон первым)

1. `RawPointer[T]` — тип в стандартной библиотеке (Ruby, потом self-hosted).
2. `extern fn ... = "name" from "<header>"` — реальный codegen (`#include` +
   вызов), сначала для функций без callback-параметров.
3. `extern lib "name"` — пробрасывание `-l<name>` в `build_bin.sh`.
4. `extern type Name = "CName" from "<header>"` в self-hosted парсере
   (сегодня есть только в Ruby) + `drop "c_function"` для владения.
5. `extern fn(...)` как тип — C function pointer, отдельно от closure типа.
6. Concurrency-метаданные (`blocking`/`thread_safe`/`thread_affine`) на
   `extern fn`/`extern type` — после `TRACK_CONCURRENCY_V2` STEP=1.

## 5. Proof-of-concept (не в этом треке, следующий трек после закрытия слоя)

Минимальный сквозной тест слоя — 3-4 функции libpq (`PQconnectdb`, `PQexec`,
`PQgetvalue` или аналог, `PQfinish`) без callback: покрывает §2.1-2.3, §2.5,
§2.7 (`blocking`), не требует §2.4 (callback) и §2.6 (raw struct). Именно
поэтому libpq — правильный первый case, не OpenGL/GTK/ffmpeg (см. предыдущее
обсуждение в чате: OpenGL/GTK/ffmpeg требуют либо генератор (тысячи
символов), либо §2.4/§2.6 в полном объёме).

## 6. Критерий приёмки слоя — **met** 2026-07-09 (`TRACK_FFI_LAYER` closed)

| # | Критерий | Результат |
|---|----------|-----------|
| 1 | `RawPointer[T]` → `T*`, `is_null`/сравнение | done (STEP=1–2; Ruby + self-hosted) |
| 2 | `extern fn … = "c" from "<h>"` → `#include` + C call | done (STEP=3; probe `cabs`/`sqrt`) |
| 3 | `extern lib "x"` → `-lx` at link | done (STEP=4; `mlc_link_libs.txt` + `-lm`) |
| 4 | `extern type` + `drop` → `using` + `Owned*` unique_ptr deleter | done (STEP=5; FILE/fclose probe). `owned` return-marker + ASan smoke **deferred** |
| 5 | `extern fn(...)` type → C fptr; reject non-top-level | done (STEP=6; `type_identity_t<Ret(*)(…)>`; lambda → E003) |
| 6 | Self-host `mlcc`→`mlcc2`→`diff` identical | done (STEP=8; empty `diff -rq`; `regression_gate` 20/0) |

Concurrency attrs (STEP=7): `blocking`/`thread_safe`/`thread_affine` parse +
registry; Send/Sync honor `thread_safe` / reject `thread_affine` (Arc.new E082).
Isolate-context `blocking` lint — future (no Isolate call-site gate yet).

## 7. После закрытия слоя — что дальше (не создавать треки заранее)

- **libpq proof-of-concept** (см. §5) — первый реальный биндинг, валидирует
  слой end-to-end.
- **OpenGL** — только после появления генератора биндингов (тысячи функций
  руками не размечать) — генератор — отдельная задача после первого ручного
  случая.
- **GTK/ffmpeg** — не брать без конкретного продукта/задачи, которая их
  требует (см. обсуждение в чате 2026-07-09): эффорт сопоставим с отдельным
  крупным подпроектом, отдельно от §2.4 (GObject refcounting ≠ `Shared<T>`,
  GTK signals) и §2.6 (ffmpeg struct/union/макросы).

## 8. Стратегия «без hand-written C++» (2026-07-11)

Пользователь: весь функционал (GUI/text/TCP/HTTP/DB/crypto) должен собираться
через `mlcc` без Ruby, и без ручного C++ — либо автогенерация в сборке, либо
прямой импорт заголовков сторонней библиотеки.

**Граница (не смешивать два разных типа C++):**

| Тип | Пример | Судьба |
|-----|--------|--------|
| Истинно неустранимый примитив ОС/libc | аллокация (`malloc`/`realloc`), атомарный inc/dec, `pthread_create`/`pthread_mutex_*` | **остаётся** как `extern fn ... from "<cstdlib>"/"<stdatomic.h>"/"<pthread.h>"` — прямой импорт реального заголовка, не наша логика |
| Ядро рантайма (`core/`/`concurrency/`) — **исправлено 2026-07-11, было ошибочным исключением** | `core/string.hpp`, `core/array.hpp` (COW/growth policy), `core/hashmap.hpp`, `concurrency/{mutex,channel,task,thread_pool}.hpp` | **портировать на self-hosted MLC** поверх примитивов выше — «рантайм остаётся C++ навсегда» было неверной уступкой (пользователь: «ручного C++ не должно быть нигде вообще, иначе в MLC нет смысла») → [TRACK_LANG_SELF_HOSTED_RUNTIME](agent/TRACK_LANG_SELF_HOSTED_RUNTIME.md) |
| FFI-адаптер к сторонней C-библиотеке (в основном bookkeeping/error-handling, не алгоритм) | `db/postgres.hpp`, `crypto/sodium.hpp`, часть `net/tcp.hpp` (i32-handle-table) | **убрать**: прямой `extern fn ... = "c_name" from "<header>"` + `extern type`+`drop` (слой уже закрыт, инфраструктура не нужна) — bookkeeping-логика (hex-encode, handle-table) переписывается на MLC |
| Бизнес-логика/алгоритм, написанный на C++ вместо MLC (не биндинг ни к чему) | WebSocket protocol **ported** 2026-07-11; MSDF EDT/SDF **ported** 2026-07-11 ([TRACK_TEXT_MSDF_TO_MLC](agent/TRACK_TEXT_MSDF_TO_MLC.md) — `msdf_shim` deleted, thin `msdf_bridge` FreeType mask only) | **портировать на MLC целиком**, без FFI вообще для этой части |
| Runtime function-pointer loader (нужен из-за самого GL ABI, не биндинг конкретной функции) | `gl/glfw_gl_dispatch.cpp` (`glfwGetProcAddress`+`reinterpret_cast`), `gl/loader_shim.cpp` (EGL/GLES2) | **убрать своими руками вообще** — `GLAD2` (сгенерированный один раз через `glad.sh`, вендоренный `.c`) резолвит function pointers сама через `#define glDrawArrays glad_glDrawArrays`; уже закрытый `extern fn ... from "<header>"` вызывает её прозрачно, без нового примитива каста |

**Уточнение по итогам чтения кода (2026-07-11), меняет более раннюю оценку
в чате**: `header_import.mlc` (авто-парсер `.h` в decl'ы) **не является
блокером** ни для одного из перечисленных — слой `extern fn`/`RawPointer`/
`extern type`+`drop`/`extern lib` закрыт 2026-07-09 и работает вручную для
небольшого числа функций (`postgres`/`sodium`/GL — десятки, не тысячи
символов). Автогенератор биндингов остаётся design-only, не строить без
конкретной необходимости (см. §3, §7 — не изменилось).

**GL-loader — отдельное уточнение (2026-07-11)**: ни `RawPointer→fn-ptr`
каст, ни ручная MLC-таблица не нужны. `GLAD2`/`libepoxy` резолвят GL
function pointers **внутри себя** (`#define glDrawArrays glad_glDrawArrays`
→ глобальная переменная-указатель, ленивый резолв в сгенерированном/системном
коде) — уже закрытый `extern fn ... from "<header>"` вызывает их прозрачно.
Выбор — `GLAD2` (вендоренный сгенерированный `.c`, без runtime-зависимости
от системной `.so`, лучше для будущего embed-режима), не `libepoxy`.

**Треки:**

- [TRACK_FFI_SHIM_MIGRATION](archive/tracks/TRACK_FFI_SHIM_MIGRATION.md) —
  **closed** 2026-07-11: Postgres/Crypto/Tcp → `*_abi.hpp`/`*_bridge.hpp`;
  bookkeeping partial MLC; TcpStream residual for websocket.
- [TRACK_TEXT_MSDF_TO_MLC](agent/TRACK_TEXT_MSDF_TO_MLC.md) — EDT/SDF
  алгоритм на MLC, без C++.
- [TRACK_STDLIB_HTTP_MLC](archive/tracks/TRACK_STDLIB_HTTP_MLC.md) — HTTP-парсер/
  роутер на MLC (пересмотрено: не обёртка, порт логики).
- [TRACK_STDLIB_WEBSOCKET_TO_MLC](agent/TRACK_STDLIB_WEBSOCKET_TO_MLC.md) —
  framing/handshake на MLC.
- [TRACK_STDLIB_LOGIC_TO_MLC](agent/TRACK_STDLIB_LOGIC_TO_MLC.md) — Env/Log/
  Validation: перевод пайплайна Ruby→mlcc + чистая MLC-логика где возможно.
- [TRACK_GL_GLAD_MIGRATION](agent/TRACK_GL_GLAD_MIGRATION.md) — GL-вызовы
  через вендоренный GLAD2, без ручного C++ dispatch (заменяет отменённые
  `TRACK_FFI_POINTER_CAST`/`TRACK_GL_LOADER_TO_MLC`, см.
  `archive/tracks/`).
- [TRACK_LANG_SELF_HOSTED_RUNTIME](agent/TRACK_LANG_SELF_HOSTED_RUNTIME.md) —
  `core`/`concurrency` (~4500 строк) на self-hosted MLC поверх
  `malloc`/atomics/`pthread` через `extern fn`; максимальный приоритет и
  максимальный риск (весь компилятор и все MLC-программы зависят от
  корректности).

## 9. Safety contract — не реализовано (см. [TRACK_FFI_SAFETY](agent/TRACK_FFI_SAFETY.md))

Слой §2 даёт ABI-механизм, не даёт safety-гарантий. `extern fn`/
`RawPointer[T]`/`extern type` — unsafe escape hatch без синтаксического
маркера (в отличие от Rust `unsafe fn`/`unsafe {}`): lifetime, null-checks
и реальная ABI-сверка сигнатуры остаются на совести автора биндинга,
компилятор их не проверяет. Concurrency-атрибуты (§2.7) реализованы, но
ничто не требует их указывать — дефолт при отсутствии атрибута не
зафиксирован явно. Детали, открытые вопросы и план — в
`agent/TRACK_FFI_SAFETY.md`, отдельный трек, не блокирует использование
слоя как есть.
