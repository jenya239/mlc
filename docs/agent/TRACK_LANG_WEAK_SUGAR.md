# Track: `.weak()`/`.upgrade()` sugar on `Shared`/`Weak`

Parent: [../PLAN.md](../PLAN.md), [../MEMORY_MODEL.md](../MEMORY_MODEL.md).
Source: [../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #4
(Уровень 0 из части «память и связность»).

## Status: **open**

**Проблема:** `grep -rn "Weak<" compiler/ runtime/` = 0 — `Weak<T>` документирован
(`MEMORY_MODEL.md`: *"Циклические структуры требуют `Weak<T>`. Забытый `Weak`
→ утечка. Компилятор не детектирует."*), но ни разу не использован даже в
5000+ строках self-hosted компилятора. Гипотеза аудита: нет удобного
паттерна создания/разыменования — только сахар может это проверить.

## Синтаксис

```mlc
let parent_ref = parent.weak()        // Shared<T> -> Weak<T>

match child.parent.upgrade() {        // Weak<T> -> Option<Shared<T>>
  Some(p) => use(p),
  None    => handle_dead_parent()
}
```

## Реализация

- `.weak()` → `std::weak_ptr(shared_ptr)`, ноль нового рантайма.
- `.upgrade()` → `wp.lock()` + проверка на `nullptr`, обёрнуто в `Option`
  (`Some`/`None`) — единственно корректная сигнатура, совместима с `match`
  без изменений в pattern-matching коде.
- Checker: правило в `builtin_method_return_type`
  (`compiler/checker/semantic_type_structure.mlc:369`) — `.weak()` на
  `Shared<T>` возвращает `Weak<T>`, `.upgrade()` на `Weak<T>` возвращает
  `Option<Shared<T>>`.
- Codegen: найти, где сейчас генерируются встроенные методы на `Shared<T>`
  (искать в `compiler/codegen/` обработку `TGeneric("Shared", ...)`) — по
  аналогии добавить `TGeneric("Weak", ...)`.
- Первым делом — Ruby-эталон (`lib/mlc/`), затем mlcc (конвенция проекта):
  проверить, есть ли `Weak<T>` вообще в Ruby-пайплайне (`grep -rn "Weak" lib/mlc/`)
  — если нет, реализация начинается там.

## Repro

```mlc
type Node = { value: i32, parent: Weak<Node> }

fn main() -> i32 = do
  let root = Shared.new(Node { value: 1, parent: Weak.none() })
  let child_parent_ref = root.weak()
  match child_parent_ref.upgrade() {
    Some(p) => p.value,
    None => -1
  }
end
```

До фикса: `.weak()`/`.upgrade()` — `E001` (метод не найден). После: exits 0,
результат `1`.

## Verify gate

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

После реализации — переиспользовать `Weak<T>` хотя бы в одном реальном месте
self-hosted компилятора, где сейчас потенциальный цикл (например
parent-указатели в AST, если такие есть) — иначе эргономика снова останется
непроверенной на практике.
