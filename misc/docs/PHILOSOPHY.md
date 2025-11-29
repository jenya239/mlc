# Философия языка

Основная идея: **статический основной поток, динамика — как временный буфер.** Мы стремимся к статике, но не навязываем её: если данные “протекают” из внешнего мира (JSON, конфиги, плагины), их можно временно держать в открытой форме, но в подходящий момент мы возвращаем их в статический тип.

Ключевые тезисы
- **Статика по умолчанию.** Литералы записей → статические формы (struct), точечный доступ гарантирован. Хорошо для производительности и читаемости.
- **Динамика как оболочка.** Открытая форма (map/variant) для внешних данных и быстрых прототипов. Доступ через точку/индекс — но без статических гарантий, с подсказками линта.
- **Мост обратно в статику.** Утилиты `shape_as/shape_try/get` (и аналоги) для валидации и приведения к статическому типу в момент, когда это уместно.
- **Безопасность владения.** `Unique/Shared/Weak` везде, сырые указатели — только в `unsafe`; обратные ссылки — слабые. Динамические объекты не ломают эти правила.
- **Минимум магии.** Никаких открытых классов/монкипатча/глубокой runtime-рефлексии; расширения — через трейты/extend/модули, статически.
- **Предсказуемая производительность.** Статические формы → struct, быстрый доступ; открытые формы → честный map/variant с понятной ценой. Нет скрытых деоптимизаций.
- **Прозрачный переход.** Появление динамики видно (добавили поле/индексный доступ → форма открылась, линт подсказал), возвращение в статику — через явную проверку формы.
- **Гибридность там, где нужно.** Смешанные паттерны: статичная оболочка + динамический payload для событий/JSON, с последующей валидацией.

Практический смысл
- Основной код (домены, ядро) — статичен, прогнозируем и оптимизируем.
- Внешний мир/прототипирование — может быть динамичным, но контролируемым: держим открыто столько, сколько надо, затем закрепляем тип.
- Инструменты (линт/IDE) поддерживают переходы: предупреждают о потере статичности, предлагают привести к форме/типу.

---

# Типовые сценарии: статика с контролируемой динамикой

## 1) Чисто статический путь
```mlc
let p = { x: 1, y: 2 }   // закрытая форма
let sum = p.x + p.y      // точка гарантирует поля
// p.z  // ошибка типов
```

## 2) Временная динамика (открытие формы)
```mlc
let user = { name: "Ann" }   // закрытая
user.city = "Paris"          // форма открылась (warning от линта)
let city = user.city         // Variant/Option, статической гарантии нет
```

## 3) JSON → динамика → статик
```mlc
struct Config { api: str, retries: i32 }

let raw = json_parse(read_file("cfg.json"))   // открытая форма
let cfg = shape_as<Config>(raw)?              // валидация, Result
use(cfg.api, cfg.retries)                     // дальше статический тип
```

## 4) Мягкая валидация
```mlc
let raw = json_parse(...)
let cfg = shape_try<Config>(raw).unwrap_or(default_cfg())
```

## 5) Частичный доступ к открытому объекту
```mlc
let raw = json_parse(...)
let retries = get<i32>(raw, "retries").unwrap_or(3)
```

## 6) Смешанный объект: статичная оболочка + динамический payload
```mlc
struct Msg {
  id: i64
  payload: Map<string, Variant>  // всегда открытая форма внутри
}

let m = Msg{ id: 1, payload: { kind: "evt" } }
m.payload.extra = 123
let kind = get<str>(m.payload, "kind").unwrap_or("none")
```

## 7) Дерево с безопасным владением и динамическим payload
```mlc
struct Node {
  data: Map<string, Variant>
  children: Shared<Node>[]
  parent: Weak<Node>

  fn add_child(self, child: Shared<Node>) -> void = do
    child.parent = downgrade(self)
    @children.push(child)
  end
}

fn new_node(payload) -> Shared<Node> =
  make_shared(Node{ data: payload, children: [], parent: Weak::null() })

let root = new_node({ kind: "root" })
let leaf = new_node({ kind: "leaf", value: 3 })
root.add_child(leaf)
```

## 8) События: динамический payload, слабые слушатели
```mlc
struct EventBus {
  listeners: Weak<Fn(Map<string, Variant>) -> void>[]

  fn emit(self, payload: Map<string, Variant>) -> void = do
    for l in @listeners do
      match l.upgrade() with
        | Some(f) => f(payload)
        | None    => ()
      end
    end
  end
}

fn on_click(payload) -> void = do
  let x = get<i32>(payload, "x").unwrap_or(0)
  let y = get<i32>(payload, "y").unwrap_or(0)
  log("click", x, y)
end

let bus = make_shared(EventBus{ listeners: [] })
let h = make_shared(on_click)
bus.listeners.push(downgrade(h))
bus.emit({ x: 10, y: 20 })
```

## 9) Открытие через индексный доступ
```mlc
let cfg = { retries: 3 }    // закрытая
cfg["timeout"] = 1000       // открытие формы (warning)
let t = cfg.timeout         // Variant/Option
```

## 10) Контролируемый мост обратно в статик
```mlc
struct User { name: str, age: i32 }

fn to_user(obj) -> Result<User, str> = do
  let name = get<str>(obj, "name")
  let age  = get<i32>(obj, "age")
  if name.is_none() || age.is_none() then
    Err("bad user")
  else
    Ok(User{ name: name.unwrap(), age: age.unwrap() })
  end
end

let u = to_user(load_json("user.json"))?
```

## 11) Статика для горячих путей, динамика — точечно
- Основные структуры (VDOM/компоненты/лейаут) — закрытые записи → struct в C++, быстрый доступ.
- Динамический payload событий/конфигов — открытая форма, обработка через `get`/валидацию, затем приведение к статике.

## 12) Безопасность сохраняется
- Обратные ссылки — `Weak`, владение — `Shared/Unique`.
- Динамические поля не ломают политику владения: вложенные объекты/указатели всё равно должны соблюдать `Shared/Weak`.
- Сырые указатели — только в `unsafe`.
