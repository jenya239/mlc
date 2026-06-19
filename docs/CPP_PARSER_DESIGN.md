# C++ Parser Design для mlcc

## Цель

Парсить заголовочные файлы C++ (`.h`, `.hpp`) для импорта внешних библиотек в mlcc.
**Сохраняем trivia** (пробелы, переводы строк, комментарии) на токенах.
Это обеспечивает roundtrip: `cpp_to_source(cpp_parse(source)) == source`.

## Базис

Используем существующие типы из `compiler/cpp/cpp_ast.mlc` (CppType, CppExpr, CppStmt, CppDecl, CppProgram).
Расширяем их, не заменяем.

Ориентир — Ruby парсер в `lib/cpp_ast/` (~11k строк): trivia-модель, Pratt expressions, roundtrip.

---

## Trivia-модель

Каждый токен несёт два поля — `leading_trivia` и `trailing_trivia` (строки).
Trivia включает: пробелы, табы, переводы строк, `// комментарии`, `/* комментарии */`, `#pragma`-строки, `[[...]]`.

### CppToken (расширить в lexer.mlc)

```mlc
type CppToken = CppToken {
  kind: CppTKind,
  lexeme: string,          // исходный текст токена
  leading_trivia: string,  // всё до токена (пробелы, комментарии, newlines)
  trailing_trivia: string, // пробелы/комментарий до следующего newline
  line: i32,
  col: i32
}
```

### Алгоритм лексера

Перед каждым значимым токеном: собрать whitespace, `//`-комментарии, `/* */`-комментарии, `#`-строки как `leading_trivia`.
После токена: trailing whitespace до `\n` (не включая) как `trailing_trivia`.
`\n` уходит в `leading_trivia` следующего токена.

### to_source

Каждый узел AST реализует `cpp_to_source` (рекурсивный обход):

```mlc
fn cpp_token_to_source(tok: CppToken) -> string =
  tok.leading_trivia + tok.lexeme + tok.trailing_trivia

fn cpp_program_to_source(program: CppProgram) -> string =
  program.decls.map(cpp_decl_to_source).join('')
```

Roundtrip: `cpp_program_to_source(cpp_parse(cpp_tokenize(src)).program) == src`

---

## Что нужно парсить из заголовков

```cpp
#pragma once
#include <vector>
#include "mylib.h"
namespace std { }
namespace foo::bar { }
using Alias = std::vector<int>;
using namespace std;
typedef unsigned long size_t;
class Foo;
struct Bar;
struct Point { float x; float y; };
class Foo : public Bar {
public:
    Foo(int x);
    virtual ~Foo();
    int method(const std::string& s) const;
    static constexpr int MAX = 100;
private:
    int value_;
};
enum Color { Red, Green, Blue };
enum class Status : uint8_t { Ok = 0, Err = 1 };
int add(int a, int b);
template<typename T> T max(T a, T b);
[[nodiscard]] std::optional<int> parse(std::string_view s);
extern "C" { void c_func(int x); }
bool operator==(const Foo& a, const Foo& b);
inline constexpr size_t kMaxSize = 1024;
```

---

## Расширения CppTKind (lexer.mlc)

Добавить недостающие ключевые слова:

```
CKClass | CKEnum | CKTypedef | CKExtern | CKSigned | CKUnsigned
CKLong | CKShort | CKVirtual | CKOverride | CKFinal | CKPublic
CKPrivate | CKProtected | CKExplicit | CKMutable | CKVolatile
CKFriend | CKDo | CKSwitch | CKCase | CKDefault | CKContinue
CKGoto | CKTry | CKCatch | CKThrow | CKSizeof | CKAlignof
CKDecltype | CKStaticAssert
```

Добавить типы токенов:

```
CLChar(string)    // char literal 'a', '\n'
CLFloat(string)   // float literal как строка
CLHex(string)     // hex literal 0xFF как строка
CHash             // # (отдельный, не COp)
CDot              // .
CEllipsis         // ...
CAssignOp(string) // +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
```

Изменения в лексере:
- Хранить `lexeme` — исходный текст токена (нужен для roundtrip)
- `//`, `/* */` → `leading_trivia` следующего токена
- `#`-строки препроцессора → `leading_trivia`
- `[[attributes]]` → `leading_trivia`
- Char literals `'a'`, `'\n'`, `'\x41'`
- Float: `3.14f`, `1.0e-5`
- Hex: `0xFF`, binary `0b1010` → CLHex(string)
- Строки с prefix: `u8"..."`, `L"..."` → CLStr (префикс в lexeme)
- Raw strings `R"(raw)"` → CLStr

---

## Расширения CppType (cpp_ast.mlc)

```mlc
export type CppType =
  | CppTypeName(string)                          // int, float, MyClass, std::string
  | CppTypeTemplate(string, [Shared<CppType>])   // vector<int>
  | CppTypeRef(Shared<CppType>)                  // T&
  | CppTypeRRef(Shared<CppType>)                 // T&&
  | CppTypePtr(Shared<CppType>)                  // T*
  | CppTypeConst(Shared<CppType>)                // const T
  | CppTypeVolatile(Shared<CppType>)             // volatile T
  | CppTypeDecltype(string)                      // decltype(expr)
  | CppTypeAuto                                  // auto
  | CppTypeVoid                                  // void
  | CppTypeArray(Shared<CppType>, string)        // T[N]
  | CppTypePack(Shared<CppType>)                 // T...
```

## Расширения CppDecl (cpp_ast.mlc)

```mlc
export type CppParam = CppParam {
  name: string,
  parameter_type: Shared<CppType>,
  default_value: string   // как строка — для roundtrip
}

export type CppFnModifiers = CppFnModifiers {
  is_const: bool, is_virtual: bool, is_override: bool,
  is_static: bool, is_explicit: bool, is_noexcept: bool,
  is_deleted: bool, is_defaulted: bool, is_pure_virtual: bool
}

export type CppAccessLevel = CppPublic | CppProtected | CppPrivate

export type CppMember =
  | CppMemberField(Shared<CppType>, string, string)  // type, name, default
  | CppMemberFn(Shared<CppDecl>)
  | CppMemberAccess(CppAccessLevel)                  // public: / private:
  | CppMemberTypeAlias(string, Shared<CppType>)      // using Inner = T

export type CppClassDef = CppClassDef {
  is_struct: bool, name: string,
  base_classes: [(CppAccessLevel, string)],
  members: [CppMember],
  template_params: string
}

// Новые варианты CppDecl:
// | CppClassDecl(CppClassDef)
// | CppEnumDecl(string, bool, string, [(string, string)])
//   name, is_scoped, underlying_type, [(name, value_expr)]
// | CppTypedefDecl(string, Shared<CppType>)
// | CppForwardDecl(string, string)            // kind, name
// | CppTemplateDecl(string, Shared<CppDecl>)  // params_raw, wrapped
// | CppExternBlock(string, [Shared<CppDecl>])
// | CppPragma(string)
// | CppDefineConst(string, string)            // name, value
```

---

## Структура парсера

```
compiler/cpp/
  lexer.mlc              расширить (trivia, все ключевые слова, char/float/hex)
  predicates.mlc         расширить
  cpp_ast.mlc            расширить типы
  parser/
    types.mlc            parse_type(tokens, pos) -> CppType
    exprs.mlc            Pratt — для default args, array sizes, constexpr
    decls.mlc            главный: class, fn, enum, namespace, template, using, typedef
    stmts.mlc            пропуск тел функций
    skip.mlc             skip_braces, error recovery
    to_source.mlc        cpp_to_source: AST -> исходный текст через trivia
  parser.mlc             entry point
```

---

## Алгоритм parse_decl (decls.mlc)

```
parse_decl(tokens, pos):
  1. storage/linkage: extern, static, inline, constexpr
  2. если #:          parse_preprocessor_line
  3. если template:   parse_template_wrapper
  4. если namespace:  parse_namespace
  5. если class:      parse_class
  6. если enum:       parse_enum
  7. если using:      parse_using
  8. если typedef:    parse_typedef
  9. иначе:           parse_function_or_var
     - parse_type -> тип
     - parse_name -> имя (через ::)
     - если (:  функция
     - если = или ;: переменная
     - если {:  skip_braces
  10. при неудаче: skip_to_semicolon_or_brace
```

## parse_type (types.mlc)

```
parse_type(tokens, pos):
  // Квалификаторы слева: const, volatile, unsigned, signed, long, short
  modifiers = собрать
  // Базовый тип: ident с :: и <>
  base = parse_qualified_name(tokens, pos)
  if <: parse template args
  // Правые: *, &, &&
  loop: if *: CppTypePtr; if &: CppTypeRef; if &&: CppTypeRRef
  apply_left_qualifiers(base, modifiers)
```

---

## Тесты (compiler/tests/test_cpp_parser.mlc)

### Структурные

```mlc
assert_parse("#include <vector>", [CppInclude(true, "vector")])
assert_parse("using Size = std::size_t;", [CppUsing("Size", "std::size_t")])
assert_parse("struct Point { float x; float y; };", [CppClassDecl(...)])
assert_parse("int add(int a, int b);", [CppFnProto2("add", CppTypeName("int"), [...])])
assert_parse("enum class Color : uint8_t { Red = 0 };", [CppEnumDecl("Color", true, "uint8_t", [...])])
assert_parse("namespace std { class string; }", [CppNamespace("std", [CppForwardDecl("class","string")])])
assert_parse("template<typename T> T max(T a, T b);", [CppTemplateDecl("typename T", CppFnProto2(...))])
```

### Roundtrip

```mlc
fn assert_roundtrip(source: string) =
  let result = cpp_parse(cpp_tokenize(source))
  assert cpp_program_to_source(result.program) == source

assert_roundtrip("#include <vector>\n")
assert_roundtrip("// compute sum\nint add(int a, int b);\n")
assert_roundtrip("#pragma once\n")
assert_roundtrip("#ifndef FOO_H\n#define FOO_H\n#endif\n")
assert_roundtrip("enum class Status : uint8_t { Ok = 0, Err = 1 };\n")
assert_roundtrip("template<typename T>\nT identity(T x);\n")
assert_roundtrip("class Foo {\npublic:\n    int get() const;\nprivate:\n    int value_;\n};\n")
assert_roundtrip("namespace foo {\n    struct Bar { int x; };\n}\n")
```

### Error recovery

```mlc
let result = cpp_parse(cpp_tokenize("void ??? garbage; int ok();"))
assert result.errors.length() > 0
assert result.program.decls.length() > 0  // ok всё равно распознана
```

---

## Фазы реализации

| Фаза | Файл(ы) | Дней |
|-------|---------|------|
| A: Лексер с trivia | lexer.mlc, predicates.mlc | 1-2 |
| B: Типы AST | cpp_ast.mlc | 0.5 |
| C: to_source + roundtrip тесты лексера | parser/to_source.mlc | 0.5 |
| D: Парсер типов | parser/types.mlc | 1 |
| E: Парсер деклараций | parser/decls.mlc, skip.mlc | 2-3 |
| F: Парсер выражений | parser/exprs.mlc | 1 |
| G: Тесты (≥20 структурных + ≥10 roundtrip) | tests/test_cpp_parser.mlc | параллельно |
| H: Интеграция с import системой mlcc | отдельный трек | — |

---

## Ограничения

- Тела функций пропускаются через skip_braces (но с trivia для roundtrip)
- Макросы `#define MACRO(x)` — не вычисляем
- `#ifdef` — структурные маркеры, не вычисляем
- Coroutines, C++20 modules — в будущем
- Не полный стандарт C++ (достаточно для STL + распространённых библиотек)


---

## Предусловия: фичи языка MLC до начала разработки

Фаза 3.5 начинается только после завершения **Phase 2.5 + 2.6 + 2.7**.

### Уже есть в языке

| Фича | Состояние |
|------|-----------|
| `{ ...base, field: val }` обновление записей | **работает** |
| `?` оператор (`Result` и `Option`) | **работает** |
| `Option<T>` с `.map()`, `.filter()`, `.find()` | **работает** |
| `extend Type { fn method(self) }` | **работает** |
| HOF: `.map()`, `.filter()`, `.fold()` на массивах | **работает** |
| Record destructuring `let { a, b } = r` | **работает** |

### Требует добавления (Phase 2.7)

| Фича | Зачем |
|------|-------|
| Обобщённые алиасы `type Foo<T>` | избежать 10+ одинаковых result-типов |
| `guard let Pat = expr else return` | ранный выход без пирамиды match |
| String match (`match word { 'fn' -> ... }`) | уже в Phase 2.5 |

### Рекомендуемый паттерн C++ парсера

C++ парсер должен следовать тому же паттерну что и встроенный MLC парсер:

```mlc
// CppParser как cursor, аналогично frontend/parser/predicates.mlc:
type CppParser = CppParser { tokens: [CppToken], pos: i32, errors: [string] }

extend CppParser {
  fn kind(self) -> CppTKind = ...
  fn advance(self) -> CppParser = CppParser { ...self, pos: self.pos + 1 }
  fn peek(self, offset: i32) -> CppTKind = ...
  fn at_end(self) -> bool = ...
  fn error(self, msg: string) -> CppParser = CppParser { ...self, errors: self.errors + [msg] }
}

// Функции парсера:
fn parse_type(parser: CppParser) -> CppParseResult<Shared<CppType>> = ...
fn parse_decl(parser: CppParser) -> CppParseResult<Shared<CppDecl>> = ...

// vs текущий подход (cpp/parser.mlc) — неидиоматичный:
fn parse_type_tokens(tokens: [CppToken], position: i32) -> CppTypeResult = ...
```

Текущий `compiler/cpp/parser.mlc` (передаёт `tokens` + `position` отдельно) будет переписан целиком после Phase 2.7.

