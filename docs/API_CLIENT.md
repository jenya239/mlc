# API-клиенты (JSON derive + OpenAPI codegen) — дизайн

Parent: [PLAN.md](PLAN.md), [STDLIB_BACKEND.md](STDLIB_BACKEND.md) §2 (HTTP
клиент уже в факт-таблице).

## 1. Факт: текущее состояние

| Компонент | Файл | Состояние |
|-----------|------|-----------|
| HTTP-клиент | `lib/mlc/common/stdlib/net/https_client.mlc`; thin `runtime/include/mlc/net/curl_abi.hpp` | libcurl, `https_send`/`https_get`/`https_post`. TLS: `scripts/run_https_client_gate.sh`. Блокирующий вызов, без стриминга. Заглушка `fetch` в `http.mlc` не является этим клиентом |
| JSON runtime (C++) | `runtime/include/mlc/json/json.hpp:19-28` | `std::variant<monostate, bool, double, mlc::String, vector<JsonValue>, nlohmann::json>` — числа `double`, объекты — настоящий `nlohmann::json` map |
| JSON язык (MLC) | `lib/mlc/common/stdlib/data/json.mlc` | `JsonNumber(f64)`, `JsonObject(Map<str, JsonValue>)`. Объекты C++ runtime — `nlohmann::json`, не этот `Map`. `parse_json` текста, который не является JSON, возвращает `JsonNull` |
| `derive` | `compiler/checker/check/derive_validation.mlc` | `Display`, `Eq`, `Ord`, `Hash`, `Json`. Generic `Json` — E072. Поле `Map`, `Shared` или функция — E069 |
| Типизированная (де)сериализация | `derive { Json }` | Record и нерекурсивная сумма, включая вложенный record, массив и `Option`. Целые сужаются из `f64`; `i64`, `u64`, `usize` точны до ±2^53 |
| OpenAPI codegen | `scripts/openapi_codegen.rb` | Mini Petstore: `[T]`, вариант `NameCase(Name)`, `ApiResult<T>`. Тела функций — `Err`, `fetch` не вызывается |

## 2. Предпосылка: исправить рассинхронизацию JSON-типа — **done** 2026-07-09

1. `JsonNumber(f32)` → `JsonNumber(f64)` — **done**.
2. `JsonObject(str, JsonValue)` → `JsonObject(Map<str, JsonValue>)` — **done**
   (`as_object` / `json_object` helpers added).

## 3. `derive { Json }` — типизированная (де)сериализация

```mlc
type User = { id: i64, name: string, email: string } derive { Json }
```

Генерирует (аналогично существующему `Display`/`Hash` в
`compiler/codegen/decl.mlc`):

```mlc
fn to_json(self: User) -> JsonValue
fn from_json(value: JsonValue) -> Result<User, JsonError>
```

Конвенции реализации:

- Имя JSON-ключа = имя поля. Ключевое слово C++ в идентификаторе проходит
  sanitize (`class` → `class_`). Параметр `from_json` — `__json_value`.
  Атрибутов переименования полей нет.
- `Option<T>`: отсутствующий ключ и JSON `null` оба дают пустой
  `std::optional`.
- Sum-типы (`type Status = Active | Inactive(string) | Pair(i64, string)`) →
  tagged representation (**зафиксировано** STEP=3, 2026-07-09):
  - unit-вариант → JSON-строка `"Active"`;
  - один payload → `{"tag":"Inactive","value":...}`;
  - несколько payload → `{"tag":"Pair","fields":[...]}`.
- `JsonError` = `MissingField(string) | TypeMismatch(string, string)`.
  Вторая строка — ожидаемый тип (`integer`, `number`, `string`,
  `known unit variant`, `array length N`).
- Поле `Map`, `Shared` или функция: диагностика чекера, в mlcc код E069.
  Generic `derive { Json }`: E072.
- Необобщённая сумма с двумя и более вариантами, которая содержит саму
  себя или другую такую сумму, понижается в `struct { std::variant<...> _; }`
  с конструктором на каждый вариант. Поле по значению, чей тип — такая сумма,
  хранится как `std::shared_ptr`. Тип в MLC остаётся суммой. `derive { Json }`
  проходит этот круг. Поле, написанное как `Shared<T>`, не делает сумму
  циклической и по-прежнему E069.
  Обобщённая сумма и сумма из одного варианта не поддержаны.
  `Option` такой суммы — `std::optional<std::shared_ptr<Sum>>`:
  `Some(value)` это `std::make_shared`, `None` — пустой optional.
  Массив такой суммы — `mlc::Array<std::shared_ptr<Sum>>`.
  `derive` этих полей `Option` и массива не поддержан.
  Привязка `match` к упакованному полю — сама сумма (`*field`).
  Привязка `Some(child)` для `Option` циклической суммы — сама сумма
  (`*(*optional)`). Элемент массива такой суммы — сама сумма.
  `derive { Display }` печатает её через `Type_to_string(*field)`.
  `derive { Eq }` и `derive { Ord }` сравнивают суммы, а не указатели.
  `derive { Hash }` хеширует её через `Type_hash(*field)`. Другие поля
  по-прежнему только `i32`, `bool`, `string`.

## 4. OpenAPI codegen

В отличие от C-заголовков (`FFI_LAYER.md` §3, генератор туда explicitly
отложен из-за неоднозначности парсинга произвольного C) — OpenAPI-спека
(JSON Schema-основа) формализована достаточно, чтобы генератор был
реалистичен без прецедента:

- Отдельный инструмент — **Ruby-скрипт** (правило проекта: скрипты только
  Ruby/JS), не часть компилятора `mlcc`. Вход: `openapi.yaml`/`.json`.
  Выход: `.mlc`-файлы — `type`-декларации с `derive { Json }` под каждую
  `components.schemas` запись + `fn`-клиенты под каждый `paths.*.operationId`.
- Маппинг `oneOf`/`allOf`/`anyOf` на MLC sum-types/record-композицию — не
  покрывать сразу всю OpenAPI 3.1 спеку, ограничиться `object`/`array`/
  примитивы + простой `oneOf` (discriminated union) в первой версии.
- Генерируемая функция-клиент:

```mlc
fn get_user(client: ApiClient, id: i64) -> Task<Result<User, ApiError>>
```

использует существующий `fetch`/`fetch_with_options` + `derive { Json }`
для тела ответа — никакой новой инфраструктуры HTTP не требуется.

## 5. Что не требует новых примитивов

- **Auth** (API key, Bearer token) — заголовок через существующий
  `Headers.set()` (`runtime/include/mlc/net/http.hpp:25-38`).
- **Retry/timeout/backoff** — библиотека поверх `fetch` + `StopToken`
  (готов, `CONCURRENCY_V2` closed).
- **Произвольные (не-OpenAPI) JSON API** — `derive { Json }` + вручную
  написанные типы, без генератора.
- **GraphQL** — свой клиент (query — строка), переиспользует `derive { Json }`
  для маппинга ответа; не в этом треке (отдельный, не создавать заранее).

## 6. Не в этом треке (out of scope)

- Настоящая async-интеграция `fetch()` (сейчас блокирующий поток внутри
  `Task`) — отдельная задача, не блокирует §3-4.
- gRPC/protobuf — отдельный крупный трек, только под конкретную задачу.
- Полное покрытие OpenAPI 3.1 (webhooks, links, серверные шаблоны) — брать
  минимальный практичный подмножество.

## 7. Порядок реализации

1. §2 — исправить `JsonNumber(f32)`→`f64`, `JsonObject`→`Map<str, JsonValue>`
   на MLC-уровне — **done** 2026-07-09 (`json.mlc` + `json_value_type_sync_test`).
2. `JsonError` тип + `derive { Json }` в Ruby-бутстрапе: checker
   (`derive_validation.mlc` — добавить `"Json"` в список известных трейтов)
   + codegen (`to_json`/`from_json` генерация по полям record-типа).
3. `derive { Json }` для sum-типов (tagged representation, решение по §3).
4. `derive { Json }` в self-hosted (`compiler/`), self-host verify gate.
5. OpenAPI codegen Ruby-скрипт — MVP на `object`/`array`/примитивы + простой
   `oneOf`, тест на реальной небольшой публичной спеке (например,
   Petstore — стандартный пример OpenAPI).

## 8. Критерий приёмки — **met** 2026-07-09 (TRACK closed)

1. **done** — `derive_json_test.rb` record round-trip (`i64`/`string`/`Option`/`Array`).
2. **done** — sum tagged Json round-trip (unit / 1-field / N-field).
3. **done for `JsonError` labels** — `MissingField` / `TypeMismatch` with the
   field name and an expected-type word. `Map`, `Shared`, a function field,
   and a generic derive are checker diagnostics (E069 / E072), not `JsonError`.
   A non-generic multi-variant recursive sum lowers to a wrapper struct;
   by-value cyclic fields are `std::shared_ptr`. A `Shared<T>` field does not
   make the sum cyclic. Generic and single-variant
   recursion stay unsupported. `Option` of a cyclic sum is
   `std::optional<std::shared_ptr<Sum>>`; an array of a cyclic sum is
   `mlc::Array<std::shared_ptr<Sum>>`. `derive { Display, Eq, Ord, Hash, Json }`
   of those fields reads the child sum (`*(*field)` for `Option`, `*element`
   for an array). `derive { Hash }` allows those fields when the enclosing
   type is that cyclic sum. `Some(child)` on that `Option` binds the sum
   (`*(*optional)`). A nested `Node(Some(child))` binds `child` as the sum
   (`*(*field)`), and `Node(None)` is the empty optional. `Node(Some(Leaf))`
   requires that child to be `Leaf` (`holds_alternative<Leaf>` on
   `(*(*field))._`). A constructor with fields inside that `Some` is checked
   the same way: `Node(Some(Pair(left, _)))` binds `left` as the sum
   (`*field`), and `Node(Some(Node(Some(child))))` binds `child` as the sum.
   `Kids([first])` binds `first` as the sum (`*element`) when the array length
   is 1, and `Kids([])` is the empty array. `Kids([first, ...rest])` binds
   `first` as the sum and `rest` as the remaining `[Tree]`
   (`mlc::Array<std::shared_ptr<Tree>>` from `cbegin() + 1`). Indexing that
   tail reads the sum. `Kids([Leaf])` requires that element to be `Leaf`
   (`holds_alternative<Leaf>` on `(*element)._`). `Kids([Node(left, _)])`
   binds `left` as the sum (`*field`). `Kids([Node(Node(left, _), _)])` binds
   that inner `left` as the sum. Another constructor around it,
   `Kids([Node(Node(Node(left, _), _), _)])`, binds `left` the same way.
   `Kids([Node(Node(left, _), Node(right, _))])` binds both `left` and
   `right` as the sum (`*field`). `Kids([Node(left, _), Node(right, _)])`
   binds both `left` and `right` as the sum (`*field`).
   `Kids([Node(Node(left, _), _), Node(right, _)])` binds both `left` and
   `right` as the sum (`*field`). `Kids([Node(left, _), ...rest])` binds
   `left` as the sum and `rest` as the remaining `[Tree]`. Indexing that
   tail reads the sum. A nested constructor in that prefix,
   `Kids([Node(Node(left, _), _), ...rest])`, binds `left` the same way.
   `Kids([Node(Node(left, _), _), Node(Node(right, _), _)])` binds both
   `left` and `right` as the sum (`*field`).
   `Kids([Node(Node(left, _), Node(mid, _)), Node(right, _)])` binds `left`,
   `mid`, and `right` as the sum (`*field`). `Node(Kids([child]), _)` binds
   `child` as the sum (`*element`). `Node(Kids([child, ...rest]), _)` binds
   `child` as the sum and `rest` as the remaining `[Tree]`.
   `Node(Kids([child]), Node(right, _))` binds both as the sum.
   `Kids([Node(Kids([child]), _)])` binds `child` as the sum (`*element`).
   `Kids([Node(Kids([child, ...rest]), _)])` binds `child` as the sum and
   `rest` as the remaining `[Tree]`.    `Kids([Node(Kids([Node(left, _)]), _)])`
   binds `left` as the sum (`*field`).    `Kids([Node(Kids([Node(Node(left, _), _)]), _)])`
   binds that inner `left` as the sum.
   `Kids([Node(Kids([Node(Node(left, _), Node(right, _))]), _)])` binds both
   `left` and `right` as the sum (`*field`).
   `Kids([Node(Kids([Node(Node(left, _), _), Node(Node(right, _), _)]), _)])`
   binds both `left` and `right` as the sum (`*field`). An array element of
   a cyclic sum is the sum.
   `derive { Display, Eq, Ord }` reads a boxed
   child as the sum (`*field`). `derive { Hash }` hashes that child with
   `Type_hash(*field)` and still limits every other field to `i32`, `bool`,
   and `string`.
4. **partial** — `scripts/openapi_codegen.rb` emits mini Petstore that `mlcc`
   and `clang++ -fsyntax-only` accept (`[T]`, `PetCase(Pet)`, `ApiResult<T>`).
   Client bodies return `Err` and do not call `fetch`. Live mock-server stays
   deferred.
5. **done for the compiler binary** — self-host `mlcc`→`mlcc2`→`diff` identical
   (`regression_gate.sh` 20/0 on 2026-07-09; the same empty diff after the
   2026-10-01 Json checker and codegen edits). That diff does not exercise
   `derive { Json }`, because `compiler/` does not derive it. Behavior is
   `test/mlc/derive_json_test.rb` on Ruby and mlcc.
