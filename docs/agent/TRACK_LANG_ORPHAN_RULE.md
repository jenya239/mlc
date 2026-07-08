# Track: orphan rule for `extend Type : Trait`

Parent: [../PLAN.md](../PLAN.md). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #7.

## Status: **open**

**Проблема:** сейчас `extend T : Trait` разрешён в любом модуле, без
проверки когерентности. При росте кодовой базы (self-host уже 5000+ строк,
30 `extend` по `compiler/`) конфликтующие impl одного и того же `(T, Trait)`
в разных модулях — трудноуловимый класс ошибок (какая реализация победит на
линковке/в codegen — недетерминировано без явного правила).

## Правило

`extend T : Trait` разрешён только в модуле, где объявлен **`T`** ИЛИ
**`Trait`** (классический orphan rule, как в Rust/Haskell — не давать
реализовывать чужой трейт для чужого типа).

## Файлы

- `compiler/checker/registry.mlc:521` (`register_decl_extend_into_registry`),
  `:605` (`register_extend_method_into_registry`), `:630`
  (`register_decl_extend_if`) — здесь `extend` регистрируется в
  `TypeRegistry`/`ImplRegistry`. Нужно: при регистрации знать модуль
  объявления `T` (искать, откуда типы вообще регистрируются с привязкой к
  модулю — вероятно `TypeRegistry` уже хранит модуль объявления типа для
  других целей, например импортов) и модуль объявления `Trait` аналогично,
  сравнить с модулем текущего `extend`.
- Если `TypeRegistry`/`TraitRegistry` пока не хранят модуль объявления —
  это блокер побольше, чем «просто добавить проверку»: сначала завести это
  поле, обновить конструкторы регистрации. Зафиксировать в этом треке, не
  делать тихо в рамках другой задачи.

## Repro

Файл `a.mlc`:
```mlc
export type Point = { x: i32, y: i32 }
```

Файл `b.mlc` (другой модуль, не объявляет ни `Point`, ни `Display`):
```mlc
import { Point } from './a'

extend Point : Display {   // orphan — ни Point, ни Display не объявлены здесь
  fn to_string(self: Point) -> string = "point"
}
```

До фикса: компилируется без ошибок. После: compile error "orphan impl:
`extend Point : Display` must be declared in the module defining `Point` or
`Display`".

Контрольный случай (не должен ломаться) — `extend` в модуле, объявляющем
сам тип: любой существующий `extend` в `compiler/` (например
`compiler/checker/names.mlc` — `extend NamesPass : ExprVisitor<...>`, если
`NamesPass` объявлен в том же файле) должен продолжать компилироваться без
изменений — это регресс-проверка того, что правило не ловит легитимные
случаи.

## Verify gate

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

**Важно:** перед merge — прогнать правило на всём `compiler/` и `lib/mlc/`
(если применимо к Ruby-эталону) как self-check: 0 существующих `extend`
должны нарушать новое правило. Если что-то нарушает — это уже существующий
технический долг, зафиксировать отдельно, не менять правило под него молча.
