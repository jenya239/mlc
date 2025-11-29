# Стандартный набор ссылочных политик (база на C++/GSL/Boost)

Цель: зафиксировать общепринятую семантику “умных указателей” для языка и stdlib. Опираться на практики C++ (`unique_ptr/shared_ptr/weak_ptr`, GSL `observer_ptr`, handle+generation из игровых/ресурсных систем).

## Набор типов (алиасы/обёртки)
- `Unique<T>` — единоличное владение (аналог `unique_ptr`). Move-only. Нет разделения блоков, нет downgrade.
- `Shared<T>` — разделённое владение (аналог `shared_ptr`). Копируемый, refcount в control block.
- `Weak<T>` — наблюдатель к `Shared`. Не владеет. `upgrade()` → `Option<Shared<T>>`.
- `Observer<T>` — невладеющая ссылка (аналог `gsl::observer_ptr`). Не проверяет жизнь, только договорённость (использовать кратко/в пределах блока).
- `Handle<T>` — дескриптор `id+generation`, резолвится через таблицу/арену. Предотвращает use-after-free без refcount.
- `Span<T>` — невладеющий view на последовательность.
- `Optional<T>` — nullable без указателей (чтобы не плодить `Shared?`).

Дополнительно (по мере появления):
- `Intrusive<T>` — refcount в объекте (аналог `intrusive_ptr`).
- `Pinned<T>` — запрет перемещения в памяти.
- Маркеры потоков: `Send`, `Sync` (если вводим модель потоков).

## Фабрики (единственная точка создания)
- `make_unique(T) -> Unique<T>`
- `make_shared(T) -> Shared<T>`
- `downgrade(Shared<T>) -> Weak<T>`
- `upgrade(Weak<T>) -> Option<Shared<T>>`
- `handle_of(T) -> Handle<T>` (регистрирует в таблице/арене)
- `span(ptr, len) -> Span<T>`

Правило: не создавать новый control block из сырого указателя; все `Shared/Weak` проходят через фабрики.

## Трейты/контракты
- `Borrow<T>` / `BorrowMut<T>` — выдача временной ссылки.
- `Downgrade<T>` / `Upgrade<T>` — пара `Shared/Weak`.
- `Dispose` — явное освобождение ресурса (если нужно).
- `Resolve<T>` — для `Handle<T>` → lookup в таблице/арене.
- Маркеры: `Pin`, `Send`, `Sync`, `NoCycle`.

## Правила использования (чеклист)
- По умолчанию `Unique`; `Shared` — только если реально нужно разделённое владение.
- Обратные ссылки (parent↔child, observer) — `Weak` или `Handle`, не `Shared`.
- Не-владеющие ссылки в пределах блока — `Observer`/`Span`; сырые указатели только в `unsafe`/FFI.
- Графы/ресурсы/кеши — `Handle` (id+generation), а не `Shared` между всем миром.
- Нельзя делать `Shared` из сырого, если уже есть другой `Shared` на тот же объект.
- Для циклов: одна сторона `Weak`/`Handle`; линт запрещает двусторонний `Shared`.
- Пулы/кеши не держат мир: хранить `Weak`/`Handle`.

## Инкапсуляция и безопасность
- Поля control block/указателей — приватные. Конструкторы скрыты; внешнему миру — только фабрики/функции из std/mem.
- Аннотации на полях/параметрах: `@owner`, `@weak_ref`, `@borrowed`, `@pinned`, `@nocycle` — для линта.
- Сырые указатели только в `unsafe` и только на границах FFI/оптимизаций.

## Примеры (псевдо-MLC)
```mlc
struct Node {
  priv children: Shared<Node>[]
  priv parent: Weak<Node>

  fn add_child(self, child: Shared<Node>) -> void = do
    child.parent = downgrade(self)
    @children.push(child)
  end
}

fn make_leaf() -> Shared<Node> = make_shared(Node{ children: [], parent: Weak::null() })
```

```mlc
struct TextureHandle { id: u32, gen: u32 }  // alias для Handle<Texture>

fn load_texture(path: str) -> TextureHandle = handle_of(Texture::load(path))

fn draw(tex: TextureHandle) -> void = do
  match resolve(tex) with
    | Some(t) => t.bind()
    | None    => log("dead handle")
  end
end
```

```mlc
struct BufferView {
  data: Span<u8>
  fn checksum(self) -> u32 = crc32(@data)
}
```

## Минимальная реализация для std/mem
- Алиасы: `Unique`, `Shared`, `Weak`, `Observer`, `Handle`, `Span`, `Optional`.
- Фабрики: `make_unique`, `make_shared`, `downgrade`, `upgrade`, `handle_of`.
- Трейты: `Borrow`, `Downgrade`, `Upgrade`, `Resolve` (для handle).
- Линт/стиль: обратные ссылки — `Weak`, запрет двойного `Shared`, сырые — только `unsafe`.
# Обобщённый контейнер ссылочной политики

Идея: есть каркас `Ref<P, T>`, где `P` описывает политику владения/доступа (own/share/weak/handle/pin/borrow), а `T` — полезный тип. Контракт задаётся трейтами и аннотациями; конструкторы приватны, создание — через фабрики, чтобы не плодить отдельные контрольные блоки.

## Политики (примерный набор)
- `Own`: единоличное владение, move-only (аналог Unique/Box).
- `Share`: разделённое владение с refcount, clone, downgrade → Weak.
- `WeakRef`: наблюдение без удержания, upgrade → Share.
- `Borrow`: временное заимствование (lifetime ограничен областью).
- `Handle`: дескриптор (id + поколение), резолвится через таблицу/арену.
- `Pin`: запрет перемещения в памяти после пина.
- Маркеры потоков: `Send`, `Sync` (опционально).

## Трейты-контракты
- `Own<T>`: кто владеет, move/clone правила.
- `Borrow<T>` / `BorrowMut<T>`: выдача временной ссылки/доступа.
- `Downgrade<T>` / `Upgrade<T>`: пара Share/Weak.
- `Pin<T>`: фиксация адреса, запрет move.
- `Dispose`: явное освобождение/закрытие ресурса.
- (Опционально) `Resolve<T>`: для handle → lookup в таблице/арене.
- Маркеры: `Send`, `Sync`, `NoCycle`, `Pinned`.

## Шаблон объявления
Условный синтаксис, не привязанный к конкретному языку:
```mlc
// Политика как маркер
policy Share
  // доп. параметры политики при необходимости
end

// Контейнер со скрытым блоком
struct Ref<P, T> {
  priv block: ControlBlock<T>  // или дескриптор для handle
}

// Трейтные реализации для конкретных политик
impl<T> Borrow<T> for Ref<Share, T> { fn borrow(@) -> &T = ... }
impl<T> Downgrade<T> for Ref<Share, T> { fn downgrade(@) -> Ref<WeakRef, T> = ... }
impl<T> Upgrade<T> for Ref<WeakRef, T> { fn upgrade(@) -> Option<Ref<Share, T>> = ... }
impl<T> Borrow<T> for Ref<Own, T> { fn borrow(@) -> &T = ... }
// ...

// Фабрики — единственная точка создания
fn make_shared<T>(value: T) -> Ref<Share, T> = ...
fn make_unique<T>(value: T) -> Ref<Own, T> = ...
fn downgrade<T>(r: Ref<Share, T>) -> Ref<WeakRef, T> = ...
```

## Правила безопасности
- Конструкторы/поля приватны; только фабрики создают `Ref<Policy, T>`, чтобы не было дублирования контрольных блоков.
- Аннотации на полях/параметрах: `@owner`, `@weak_ref`, `@borrowed`, `@pinned`, `@nocycle`.
- Линт: обратные ссылки → `WeakRef`; сырые указатели только в `unsafe`; `Upgrade` должен проверять валидность.
- Handle: хранит (id, поколение), lookup в общей таблице; предотвращает висение на освобождённых ресурсах.

## Минимальный стартовый набор
- `Ref<Share, T>` = Shared
- `Ref<WeakRef, T>` = Weak
- `Ref<Own, T>` = Unique/Box
- `Handle<T>` = `Ref<HandlePolicy, T>` с таблицей/поколениями
- Фабрики: `make_shared`, `make_unique`, `downgrade`, `upgrade`, `handle_of`
- Трейты: `Borrow`, `Downgrade`, `Upgrade`, `Dispose` (по мере надобности)

## Что это даёт
- Единый каркас для всех “указатель‑паттернов”.
- Контракты выражены трейтами, политика — параметром `P`.
- Приватность и фабрики защищают от неправильного создания.
- Линты/атрибуты могут автоматически проверять, что обратные ссылки слабые, сырые указатели — только в `unsafe`, и т. д.
