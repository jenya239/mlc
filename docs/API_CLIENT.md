# API-клиенты (JSON derive + OpenAPI codegen) — дизайн

Parent: [PLAN.md](PLAN.md), [STDLIB_BACKEND.md](STDLIB_BACKEND.md) §2 (HTTP
клиент уже в факт-таблице).

## 1. Факт: текущее состояние

| Компонент | Файл | Состояние |
|-----------|------|-----------|
| HTTP-клиент | `runtime/include/mlc/net/http.hpp` | libcurl, `fetch`/`fetch_sync`, TLS работает. `fetch()` — псевдо-async (`std::async` + блокирующий `.get()` внутри `Task`) |
| JSON runtime (C++) | `runtime/include/mlc/json/json.hpp:19-28` | `std::variant<monostate, bool, double, mlc::String, vector<JsonValue>, nlohmann::json>` — числа `double`, объекты — настоящий `nlohmann::json` map |
| JSON язык (MLC) | `lib/mlc/common/stdlib/data/json.mlc:10-16` | `JsonNumber(f64)`, `JsonObject(Map<str, JsonValue>)` — **aligned** with C++ `double` + object map (STEP=1, 2026-07-09) |
| `derive` | `compiler/checker/check/derive_validation.mlc:12-15` | Поддерживает `Display, Eq, Ord, Hash`. Механизм готов, список расширяем — codegen-правило в `compiler/codegen/decl.mlc` |
| Типизированная (де)сериализация | — | **Нет.** Работа с ответом API — вручную через `json_get`/`as_string`/`as_number` по generic-дереву |
| OpenAPI codegen | — | Нет |

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

Конвенции (зафиксировать при реализации, не оставлять неявными):

- Имя JSON-ключа = имя поля as-is (snake_case уже совпадает со стилем MLC);
  переименование полей (`#[json(rename = "...")]`-класс атрибутов) — не в
  первой версии, отдельный follow-up при реальной необходимости.
- `Option<T>` поле → отсутствующий или `null` JSON-ключ маппится в `None`;
  присутствующий — в `Some(value)`. Не путать "ключ отсутствует" и "ключ
  null" в первой версии (оба → `None`).
- Sum-типы (`type Status = Active | Inactive(string)`) → tagged representation,
  конкретный формат (`{"tag": "Active"}` vs `"Active"`) решить перед
  реализацией — большинство реальных API используют строковые enum без
  vararg-полей и объект с discriminator-полем для вариантов с данными.
- `JsonError` — новый тип (`MissingField(string) | TypeMismatch(string, string)
  | ...`), не существует сегодня ни в runtime, ни в языке.

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
   на MLC-уровне (Ruby-бутстрап, потом self-hosted). Регрессия: существующие
   тесты на `json.mlc`, если есть.
2. `JsonError` тип + `derive { Json }` в Ruby-бутстрапе: checker
   (`derive_validation.mlc` — добавить `"Json"` в список известных трейтов)
   + codegen (`to_json`/`from_json` генерация по полям record-типа).
3. `derive { Json }` для sum-типов (tagged representation, решение по §3).
4. `derive { Json }` в self-hosted (`compiler/`), self-host verify gate.
5. OpenAPI codegen Ruby-скрипт — MVP на `object`/`array`/примитивы + простой
   `oneOf`, тест на реальной небольшой публичной спеке (например,
   Petstore — стандартный пример OpenAPI).

## 8. Критерий приёмки

1. `derive { Json }` на record-типе с `i64`/`string`/`Option<T>`/`Array<T>`
   полями — round-trip `to_json` → `from_json` даёт исходное значение.
2. `derive { Json }` на sum-типе — round-trip для каждого варианта.
3. `JsonError` содержит достаточно информации для диагностики (какое поле,
   какой ожидался тип) — не просто `"parse error"`.
4. OpenAPI codegen генерирует компилируемый `.mlc` из тестовой спеки
   (Petstore или аналог), сгенерированный клиент реально вызывает мок-сервер
   и парсит ответ.
5. Self-host: `mlcc` → `mlcc2` → `diff` идентичен после шагов 2-4.
