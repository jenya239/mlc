# Методы во frontend

Снято чтением `compiler/frontend`. Файлы frontend не менялись.

Ключевого слова `impl` нет. Лексер в `compiler/frontend/lexer.mlc` знает слово `extend` и даёт токен `KExtend`. Объявление разбирает `parse_declaration` в `compiler/frontend/parser/decls.mlc`: ветка `kind.is_extend()` вызывает `parse_extend_decl`.

Форма блока — фигурные скобки, не `do ... end`. `parser_after_optional_lbrace` пропускает `{`, цикл методов идёт до `}`.

## Блок метода

Узел `DeclExtend(string, string, [Shared<Decl>], Span)` — `compiler/frontend/ast.mlc`, вариант `Decl`.

Поля: имя типа, имя трейта, список членов, span имени типа. Поля параметров типа у узла нет.

Пример без трейта. Текст из `specs/regression/programs/method_call.mlc`:

```
extend Counter {
  fn read_value(self: Counter) -> i32 = self.value
}
```

даёт `DeclExtend("Counter", "", [метод], span)`. Пустая вторая строка — нет двоеточия и имени трейта.

Форма с трейтом `extend Box : Iter { ... }` кладёт имя трейта во вторую строку. Это тот же узел.

## Метод с параметром `self`

Член `fn` разбирает `parse_extend_method`. Узел — `DeclFn` из той же суммы `Decl`:

`DeclFn(string, [string], [[string]], [Shared<Param>], Shared<TypeExpr>, Shared<Expr>, [WhereClauseBound])`.

Имя — склейка `type_name + "_" + fn_name`. Для примера выше это `Counter_read_value`.

Первый параметр всегда синтетический `Param`. Имя `self`, `is_mut` ложь, тип `TyNamed(type_name)`. Аннотация `self: Counter` в источнике только сдвигает парсер (`advance_past_optional_self_type`); в узел попадает `TyNamed` имени из `extend`, не разобранный тип после двоеточия. Дальнейшие параметры, чьё имя не `self`, дописываются следом.

Тело — выражение после `=`, узел `Expr` (`self.value` в примере — поле записи).

`extern fn` внутри блока разбирает `parse_extend_extern_method` и тоже даёт `DeclFn`, но `self` вставляется только если первый параметр источника — `self`. Тело — `ExprExtern`.

## Срез без generics-параметров

Рассматриваемый срез — определение метода без параметров типа. `parse_extend_method` и `parse_extend_extern_method` строят `DeclFn` со вторым списком `[]`, третьим списком `[]` и пустым where. У метода внутри `extend` generics-параметров нет.

У `DeclExtend` списка параметров типа нет. Текст `extend Shared<T>` парсер принимает (`parse_type_args` после `<`), но значение аргументов отбрасывается: в узле остаётся голое имя типа. Аргументы типа у трейта (`extend AppError : From<ParseError>`) склеиваются в строку имени трейта, отдельным списком параметров типа не становятся. Оба случая вне этого среза.
