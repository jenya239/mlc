#include "test_checker.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "diagnostic_codes.hpp"

namespace test_checker {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace check;
using namespace diagnostic_codes;

int check_error_count(mlc::String source) noexcept;

int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept;

mlc::String first_checker_error_line(mlc::String source) noexcept;

mlc::String first_checker_error_line_with_path(mlc::String source, mlc::String source_path) noexcept;

bool checker_first_error_contains_nonfunction_i32(mlc::String program_source) noexcept;

test_runner::TestResult assert_checker_diagnostic(mlc::String test_name, mlc::String source, mlc::String expected) noexcept;

mlc::Array<test_runner::TestResult> checker_tests() noexcept;

int check_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errs] = err; return errs.size(); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));}

int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [error_list] = err; return error_list.size(); }
}, check::check_with_context(decls::parse_program(lexer::tokenize(entry_source).tokens), decls::parse_program(lexer::tokenize(full_source).tokens)));}

mlc::String first_checker_error_line(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [error_lines] = err; return error_lines.size() > 0 ? error_lines[0] : mlc::String(""); },
  [&](const ast::Ok<check::CheckOut>& ok) -> mlc::String { auto [_w0] = ok; return mlc::String(""); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));}

mlc::String first_checker_error_line_with_path(mlc::String source, mlc::String source_path) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [error_lines] = err; return error_lines.size() > 0 ? error_lines[0] : mlc::String(""); },
  [&](const ast::Ok<check::CheckOut>& ok) -> mlc::String { auto [_w0] = ok; return mlc::String(""); }
}, check::check(decls::parse_program_with_source_path(lexer::tokenize(source).tokens, source_path)));}

bool checker_first_error_contains_nonfunction_i32(mlc::String program_source) noexcept{
mlc::String line = first_checker_error_line(program_source);
return line.contains(mlc::String("called value is not a function")) && line.contains(mlc::String("(got i32)"));
}

test_runner::TestResult assert_checker_diagnostic(mlc::String test_name, mlc::String source, mlc::String expected) noexcept{return test_runner::assert_eq_str(test_name, first_checker_error_line(source), expected);}

mlc::Array<test_runner::TestResult> checker_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty program - 0 errors"), check_error_count(mlc::String("")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("diagnostic code catalog count"), diagnostic_codes::diagnostic_code_catalog_count(), 81));
results.push_back(test_runner::assert_eq_str(mlc::String("diagnostic code E001 catalog"), diagnostic_codes::diagnostic_code_e001(), mlc::String("E001")));
results.push_back(test_runner::assert_true(mlc::String("diagnostic code E020 non-empty"), diagnostic_codes::diagnostic_code_e020().length() == 4));
results.push_back(test_runner::assert_eq_int(mlc::String("type alias resolves in return type - 0 errors"), check_error_count(mlc::String("type Words = [string]\nfn main() -> Words = []")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("cyclic type alias - 1 error"), check_error_count(mlc::String("type A = A\nfn main() -> i32 = 0")), 1));
results.push_back(test_runner::assert_true(mlc::String("cyclic type alias E081"), first_checker_error_line(mlc::String("type Loop = Loop\nfn main() -> i32 = 0")).contains(mlc::String("error[E081]"))));
results.push_back(test_runner::assert_eq_int(mlc::String("type alias resolves in parameter type - 0 errors"), check_error_count(mlc::String("type Count = i32\nfn f(value: Count) -> Count = value")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("chained type alias resolves - 0 errors"), check_error_count(mlc::String("type Offset = i32\ntype Count = Offset\nfn f() -> Count = 1")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("generic record field substitutes type param - 0 errors"), check_error_count(mlc::String("type Box<T> = { value: T }\nfn f(b: Box<i32>) -> i32 = b.value")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("generic type alias expands - 0 errors"), check_error_count(mlc::String("type Cell<T> = T\nfn f(x: Cell<i32>) -> i32 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("mutual cyclic type alias - 2 errors"), check_error_count(mlc::String("type A = B\ntype B = A\nfn main() -> i32 = 0")), 2));
results.push_back(test_runner::assert_true(mlc::String("mutual cyclic type alias E081"), first_checker_error_line(mlc::String("type A = B\ntype B = A\nfn main() -> i32 = 0")).contains(mlc::String("error[E081]"))));
results.push_back(test_runner::assert_eq_int(mlc::String("type alias return mismatch - 1 error"), check_error_count(mlc::String("type Words = [string]\nfn f() -> Words = 1")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning literal - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("fn return type mismatch — span on body expression"), first_checker_error_line(mlc::String("fn f() -> i32 = \"hello\"")), 1, 17, mlc::String("return type: expected i32, got string"), mlc::String("E004")));
results.push_back(test_runner::assert_eq_int(mlc::String("fn return type matches — 0 errors"), check_error_count(mlc::String("fn f() -> string = \"hello\"")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn generic return T — 0 errors"), check_error_count(mlc::String("fn id<T>(x: T) -> T = x")), 0));
results.push_back(assert_checker_diagnostic(mlc::String("negative E001: undefined ident exact"), mlc::String("fn f() -> i32 = badident"), mlc::String("error[E001]: undefined: badident\n  --> 1:17")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E002: not callable exact"), mlc::String("fn f() -> i32 = 42()"), mlc::String("error[E002]: called value is not a function (got i32)\n  --> 1:19")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E003: call argument type exact"), mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(\"str\")"), mlc::String("error[E003]: argument expects i32, got string\n  --> 2:19")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E004: return type exact"), mlc::String("fn f() -> i32 = \"hello\""), mlc::String("error[E004]: return type: expected i32, got string\n  --> 1:17")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E031: immut assign exact"), mlc::String("fn f() -> i32 = do let x = 1; x = 2; x end"), mlc::String("error[E031]: cannot assign to immutable binding: x\n  --> 1:31")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E068: question on non-Result exact"), mlc::String("fn f() -> i32 = do let x: i32 = 42; x? end"), mlc::String("error[E068]: ? operator requires a Result type, got i32\n  --> 1:38")));
results.push_back(assert_checker_diagnostic(mlc::String("negative: binary + incompatible operands exact"), mlc::String("fn f() -> i32 = 1 + true"), mlc::String("error[E005]: incompatible operand types for +: i32 and bool\n  --> 1:19")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E014: call arity exact"), mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f()"), mlc::String("error[E014]: expected 1 arguments, got 0\n  --> 2:18")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E015: if branch mismatch exact"), mlc::String("fn f() -> i32 = if true then 1 else \"x\" end"), mlc::String("error[E015]: if branch types differ: then i32, else string\n  --> 1:37")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E016: field access on i32 exact"), mlc::String("type P = { x: i32 }\nfn f(p: i32) -> i32 = p.x"), mlc::String("error[E016]: field access on value without record fields: i32\n  --> 2:24")));
results.push_back(assert_checker_diagnostic(mlc::String("negative: monomorphic call wrong arg exact"), mlc::String("fn g(x: i32) -> i32 = x\nfn f() -> i32 = g(\"a\")"), mlc::String("error[E003]: argument expects i32, got string\n  --> 2:19")));
results.push_back(test_runner::assert_true(mlc::String("integer literal called like function - diagnostic names type"), checker_first_error_contains_nonfunction_i32(mlc::String("fn f() -> i32 = 42()"))));
results.push_back(test_runner::assert_eq_int(mlc::String("fn using its own param - 0 errors"), check_error_count(mlc::String("fn f(x: i32) -> i32 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn calling another fn - 0 errors"), check_error_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y\nfn main() -> i32 = add(1, 2)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("pipe chain of plain functions - 0 errors"), check_error_count(mlc::String("fn increment(value: i32) -> i32 = value + 1\nfn double_value(value: i32) -> i32 = value * 2\nfn main() -> i32 = 3 |> increment |> double_value")), 0));
results.push_back(test_runner::assert_true(mlc::String("undefined name - at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = undefined_name")) > 0));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("undefined ident: line and column (single line body)"), first_checker_error_line(mlc::String("fn f() -> i32 = badident")), 1, 17, mlc::String("undefined: badident"), mlc::String("E001")));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("undefined ident: line and column (next line, indented)"), first_checker_error_line(mlc::String("fn f() -> i32 =\n  badident")), 2, 3, mlc::String("undefined: badident"), mlc::String("E001")));
results.push_back(test_runner::assert_eq_str(mlc::String("undefined ident: diagnostic includes source path"), first_checker_error_line_with_path(mlc::String("fn f() -> i32 = badident"), mlc::String("unit.mlc")), mlc::String("error[E001]: undefined: badident\n  --> unit.mlc:1:17")));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl with variants - 0 errors"), check_error_count(mlc::String("type Color = Red | Green | Blue")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning bool - 0 errors"), check_error_count(mlc::String("fn f() -> bool = true")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with const binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("let const compile-time binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let const c = 42\n  c\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("mlcc2 parity corpus: do-block let mut tail — 0 errors (Ruby/mlcc checker)"), check_error_count(mlc::String("fn main() -> i32 = do\n  let mut counter = 0\n  counter\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("nested generic in array type annotation - 0 errors"), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> i32 = do\n  let items: [Result<i32, string>] = []\n  0\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("record update expression - 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> Point = Point { ...p, x: 10 }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("record update two fields — 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> Point = Point { ...p, x: 10, y: 20 }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("trait bound: param method call - 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn show<T: Display>(x: T) -> string = x.to_string()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("trait bound: call with implementing type — 0 errors"), check_error_count(mlc::String("type Display { fn show(self: Self) -> string }\nextend i32 : Display { fn show(self: i32) -> string = self.to_string() }\nfn print_it<T: Display>(x: T) -> string = x.show()\nfn main() -> string = print_it(42)")), 0));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("trait bound: non-implementing type — span on argument"), first_checker_error_line(mlc::String("type Display { fn show(self: Self) -> string }\nfn print_it<T: Display>(x: T) -> string = x.show()\nfn main() -> string = print_it(42)")), 3, 32, mlc::String("does not implement Display"), mlc::String("E033")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E030: record literal missing field exact"), mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: 1 }"), mlc::String("error[E030]: missing field \"y\" in record literal\n  --> 2:25")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E032: match guard not bool exact"), mlc::String("fn f() -> i32 = match 1 { x if x => 0 }"), mlc::String("error[E032]: match guard must have type bool\n  --> 1:32")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E033: trait bound exact"), mlc::String("type Display { fn show(self: Self) -> string }\nfn print_it<T: Display>(x: T) -> string = x.show()\nfn main() -> string = print_it(42)"), mlc::String("error[E033]: type i32 does not implement Display\n  --> 3:32")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("record update: unknown field — position on field value"), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> Point = Point { ...p, z: 1 }")), 2, 44, mlc::String("undefined field: z on Point")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("record constructor: unknown field — position on field value"), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: 1, z: 1 }")), 2, 36, mlc::String("undefined field: z on Point")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("record constructor: field type mismatch — position on field value"), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: \"wrong\", y: 0 }")), 2, 30, mlc::String("field x: expected i32, got string")));
results.push_back(test_runner::assert_eq_int(mlc::String("record constructor: correct field types — 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: 1, y: 2 }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("E4: record field default binary arithmetic — 0 errors"), check_error_count(mlc::String("type Line = { offset: i32 = 10 + 5 * 2 }\nfn main() -> i32 = Line {} .offset")), 0));
results.push_back(assert_checker_diagnostic(mlc::String("negative E043: record field default call not lowerable exact"), mlc::String("fn zero() -> i32 = 0\ntype R = { x: i32 = zero() }\nfn main() -> i32 = R {} .x"), mlc::String("error[E043]: record field default expression cannot be lowered to a C++ member initializer (literals, identifiers, + - * / %)\n  --> 2:25")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E044: record field default type mismatch exact"), mlc::String("type Bad = { x: i32 = \"wrong\" }\nfn main() -> i32 = 0"), mlc::String("error[E044]: record field default type mismatch for field \"x\"\n  --> 1:23")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E042: generic record field default exact"), mlc::String("type Box<T> = { value: i32 = 0 }\nfn main() -> i32 = 0"), mlc::String("error[E042]: record field default values are not supported when the type has generic parameters\n  --> 1:30")));
results.push_back(test_runner::assert_eq_int(mlc::String("multiline string literal in source — 0 checker errors"), check_error_count(mlc::String("fn f() -> string = \"hello\nworld\"")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("string with escaped newline in source — 0 checker errors"), check_error_count(mlc::String("fn f() -> string = \"a\\nb\"")), 0));
results.push_back(test_runner::assert_true(mlc::String("two undefined names - at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = foo + bar")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("builtin print - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  print(\"hi\")\n  0\nend")), 0));
results.push_back(test_runner::assert_true(mlc::String("builtin print wrong arity - error"), check_error_count(mlc::String("fn f() -> i32 = do\n  print()\n  0\nend")) > 0));
results.push_back(test_runner::assert_true(mlc::String("builtin print wrong argument type - error"), check_error_count(mlc::String("fn f() -> i32 = do\n  print(42)\n  0\nend")) > 0));
results.push_back(test_runner::assert_true(mlc::String("tuple variant constructor wrong payload type - error"), check_error_count(mlc::String("type Msg = Num(i32) | Empty\nfn f() -> Msg = Num(\"x\")")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with type param - 0 errors"), check_error_count(mlc::String("fn id<T>(x: T) -> T = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with type param and bound - 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn id<T: Display>(x: T) -> T = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("D3 trait-as-param: single Display parameter — 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn f(x: Display) -> unit = ()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("D3 trait-as-param: two Display parameters — 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn f(first: Display, second: Display) -> unit = ()")), 0));
results.push_back(assert_checker_diagnostic(mlc::String("negative E073: type and trait same name exact"), mlc::String("type Ambiguous { fn method(self: Self) -> unit }\ntype Ambiguous = VariantOne | VariantTwo"), mlc::String("error[E073]: the name \"Ambiguous\" is declared as both a type and a trait\n  --> 2:6")));
results.push_back(test_runner::assert_eq_int(mlc::String("D4 where clause merges trait bounds — 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\ntype EqCompare { fn same(self: Self, other: Self) -> bool }\nfn f<T>(x: T) -> unit where T: Display + EqCompare = ()")), 0));
results.push_back(test_runner::assert_true(mlc::String("D4 where clause unknown type parameter — diagnostic"), first_checker_error_line(mlc::String("fn f<T>(x: T) -> unit where U: Display = ()")).contains(mlc::String("E076"))));
results.push_back(test_runner::assert_eq_int(mlc::String("call generic fn - 0 errors"), check_error_count(mlc::String("fn id<T>(x: T) -> T = x\nfn f() -> i32 = id(42)")), 0));
results.push_back(test_runner::assert_true(mlc::String("generic fn two params same T - conflicting args error"), check_error_count(mlc::String("fn pair<T>(a: T, b: T) -> T = a\nfn f() -> i32 = pair(1, \"x\")")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("trait decl + extend impl - 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\ntype Color = Red | Green\nextend Color : Display { fn to_string(self: Color) -> string = \"Red\" }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("method call on param - 0 errors"), check_error_count(mlc::String("fn f(c: string) -> string = c.to_string()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("lambda expr - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = x => x + 1\n  g(41)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("typed lambda param — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = (x: i32) => x + 1\n  g(41)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("typed lambda two params — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = (x: i32, y: i32) => x + y\n  g(1, 2)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("untyped lambda closure — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const base = 10\n  const g = x => x + base\n  g(5)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("array literal return — 0 errors"), check_error_count(mlc::String("fn f() -> [i32] = [1, 2]")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Ok/Err/Result as known globals - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const r = Ok(42)\n  0\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Err constructor - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const e = Err(\"oops\")\n  0\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Result return type annotation - 0 errors"), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> Result<i32, string> = Ok(42)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("block: let then use binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let x = 10\n  x + 1\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("for-in: loop variable visible in body - 0 errors"), check_error_count(mlc::String("fn sum(items: [i32]) -> i32 = do\n  let mut total = 0\n  for element in items do\n    total = total + element\n  end\n  total\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("nested for-in — 0 errors"), check_error_count(mlc::String("fn f(outer: [i32], inner: [i32]) -> i32 = do\n  let mut total = 0\n  for x in outer do\n    for y in inner do\n      total = total + x + y\n    end\n  end\n  total\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let (a, b) = (1, 2) - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let (a, b) = (1, 2)\n  a + b\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let (a, b, c) = (1, 2, 3) - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let (a, b, c) = (1, 2, 3)\n  a + b + c\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("tuple swap destructure — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let pair = (1, 2)\n  let (second, first) = pair\n  first + second\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("tuple destructure from binding — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let pair = (1, 2)\n  let (left, right) = pair\n  left + right\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("record update in block — 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = do\n  let base = Point { x: 1, y: 2 }\n  Point { ...base, x: 10 }\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let {x, y} = record - 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> i32 = do\n  let p = Point { x: 1, y: 2 }\n  let { x, y } = p\n  x + y\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let [a, ...b] = array - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let a: [i32] = [1, 2, 3]\n  let [u, ...v] = a\n  u\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let Ok(x) = Ok - 0 errors"), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> i32 = do\n  let o: Result<i32, string> = Ok(1)\n  let Ok(n) = o else return 0 end\n  n\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A4: default param call with omitted args - 0 errors"), check_error_count(mlc::String("fn f(a: i32 = 1) -> i32 = a\nfn g() -> i32 = f()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A4: two defaults call with one arg - 0 errors"), check_error_count(mlc::String("fn f(a: i32, b: i32 = 2, c: i32 = 3) -> i32 = a + b + c\nfn g() -> i32 = f(0)")), 0));
results.push_back(assert_checker_diagnostic(mlc::String("negative E036: parameter defaults must be trailing exact"), mlc::String("fn f(a: i32 = 1, b: i32) -> i32 = b"), mlc::String("error[E036]: parameter defaults must be trailing\n  --> 1:35")));
results.push_back(test_runner::assert_true(mlc::String("A4: generic function default rejected"), first_checker_error_line(mlc::String("fn f<T>(a: T = 1) -> T = a")).contains(mlc::String("E037"))));
results.push_back(assert_checker_diagnostic(mlc::String("negative E040: parameter default not literal exact"), mlc::String("fn f(a: i32 = 1 + 1) -> i32 = a"), mlc::String("error[E040]: parameter default must be a literal in this version\n  --> 1:17")));
results.push_back(test_runner::assert_true(mlc::String("A4: too many call arguments with defaults"), first_checker_error_line(mlc::String("fn f(a: i32 = 1) -> i32 = a\nfn g() -> i32 = f(1, 2)")).contains(mlc::String("expected between"))));
results.push_back(test_runner::assert_true(mlc::String("A4: extern with default rejected"), first_checker_error_line(mlc::String("extern fn f(a: i32 = 1) -> i32")).contains(mlc::String("E038"))));
results.push_back(test_runner::assert_true(mlc::String("for-in: iterator must be an array"), first_checker_error_line(mlc::String("fn g() -> i32 = do\n  for x in 42 do\n    0\n  end\n  0\nend")).contains(mlc::String("for loop range must be an array"))));
results.push_back(test_runner::assert_true(mlc::String("index: subscript must be i32"), first_checker_error_line(mlc::String("fn f(items: [i32]) -> i32 = items[\"0\"]")).contains(mlc::String("array index must be i32"))));
results.push_back(test_runner::assert_eq_int(mlc::String("check_with_context: entry sees helper from full program - 0 errors"), check_with_context_error_count(mlc::String("fn main() -> i32 = helper()"), mlc::String("fn helper() -> i32 = 1\nfn main() -> i32 = helper()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("match: constructor arm binds payload - 0 errors"), check_error_count(mlc::String("type Answer = Yes(i32) | No\nfn value(answer: Answer) -> i32 = match answer | Yes(n) => n | No => 0\n")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("match guard ok path — 0 errors"), check_error_count(mlc::String("type Answer = Yes(i32) | No\nfn positive(answer: Answer) -> i32 = match answer | Yes(n) if n > 0 => n | Yes(_) => 0 | No => 0")), 0));
results.push_back(test_runner::assert_true(mlc::String("match: arm result type must match first arm"), first_checker_error_line(mlc::String("fn f(flag: bool) -> i32 = match flag | true => 1 | false => \"no\"")).contains(mlc::String("match arm result type"))));
results.push_back(test_runner::assert_true(mlc::String("if: then and else branch types must match"), first_checker_error_line(mlc::String("fn f(flag: bool) -> i32 = if flag then 1 else \"no\" end")).contains(mlc::String("if branch types differ"))));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined record field: line and column on field access"), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> i32 = p.z")), 2, 26, mlc::String("undefined field: z on Point")));
results.push_back(test_runner::assert_eq_str(mlc::String("undefined record field: message text"), first_checker_error_line(mlc::String("type R = { a: i32 }\nfn f(x: R) -> i32 = x.missing")), mlc::String("error[E016]: undefined field: missing on R\n  --> 2:22")));
results.push_back(test_runner::assert_true(mlc::String("infer: string + i32 is an error"), check_error_count(mlc::String("fn f() -> i32 = \"a\" + 1")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 + string is an error"), check_error_count(mlc::String("fn f() -> i32 = 1 + \"a\"")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: string with - is an error"), check_error_count(mlc::String("fn f() -> string = \"a\" - \"b\"")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: string + string still ok"), check_error_count(mlc::String("fn f() -> string = \"a\" + \"b\"")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: ! on i32 is an error"), check_error_count(mlc::String("fn f() -> bool = !42")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: ! on bool ok"), check_error_count(mlc::String("fn f() -> bool = !false")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 == bool is an error"), check_error_count(mlc::String("fn f() -> bool = 1 == true")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 < string is an error"), check_error_count(mlc::String("fn f() -> bool = 1 < \"a\"")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: bool && i32 is an error"), check_error_count(mlc::String("fn f() -> bool = true && 1")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: i32 ordering ok"), check_error_count(mlc::String("fn f() -> bool = 1 < 2")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: string ordering ok"), check_error_count(mlc::String("fn f() -> bool = \"a\" <= \"b\"")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: bool && ok"), check_error_count(mlc::String("fn f() -> bool = true && false")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: unary minus on string"), first_checker_error_line(mlc::String("fn f() -> i32 = -\"x\"")), 1, 17, mlc::String("unary - expects i32")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: to_i on non-string receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = 1.to_i()")), 1, 18, mlc::String("method to_i expects a string receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: join on non-array receiver"), first_checker_error_line(mlc::String("fn f() -> string = \"a\".join(\",\")")), 1, 23, mlc::String("method join expects an array receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: push on string receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = \"a\".push(1)")), 1, 20, mlc::String("method push expects an array receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: set on string receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = \"a\".set(0, 1)")), 1, 20, mlc::String("method set expects an array or Map receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: get on non-Map receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = 1.get(\"k\")")), 1, 18, mlc::String("method get expects a Map receiver")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: get and set on Map ok"), check_error_count(mlc::String("fn f(m: Map<string, i32>) -> i32 = do\n  m.set(\"k\", 1)\n  m.get(\"k\")\nend")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin map get wrong arity"), first_checker_error_line(mlc::String("fn f(m: Map<string, i32>) -> i32 = m.get()")), 1, 37, mlc::String("expected 1 arguments, got 0")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin map set wrong arity"), first_checker_error_line(mlc::String("fn f(m: Map<string, i32>) -> i32 = m.set(\"k\")")), 1, 37, mlc::String("expected 2 arguments, got 1")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin array push wrong arity"), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a.push()")), 1, 26, mlc::String("expected 1 arguments, got 0")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: user function call too few arguments — position on ( of call"), first_checker_error_line(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f()")), 2, 18, mlc::String("expected 1 arguments, got 0")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: user function call too many arguments — position on ( of call"), first_checker_error_line(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(1, 2)")), 2, 18, mlc::String("expected 1 arguments, got 2")));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("infer: user function call argument type mismatch — position on argument"), first_checker_error_line(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(\"str\")")), 2, 19, mlc::String("argument expects i32, got string"), mlc::String("E003")));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("infer: called value is not a function — position on ( of call"), first_checker_error_line(mlc::String("fn f() -> i32 = 42()")), 1, 19, mlc::String("called value is not a function (got i32)"), mlc::String("E002")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: field access on i32 — position on dot"), first_checker_error_line(mlc::String("type P = { x: i32 }\nfn f(p: i32) -> i32 = p.x")), 2, 24, mlc::String("field access on value without record fields: i32")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E027: has on i32 exact"), mlc::String("fn f() -> bool = 1.has(\"k\")"), mlc::String("error[E027]: method has expects a Map receiver, got i32\n  --> 1:19")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E021: push on i32 exact"), mlc::String("fn f() -> unit = do let mut xs: i32 = 1; xs.push(2) end"), mlc::String("error[E021]: method push expects an array receiver, got i32\n  --> 1:44")));
results.push_back(test_runner::assert_diagnostic_at_with_code(mlc::String("infer: has on sum type is an error"), first_checker_error_line(mlc::String("type Hue = Red | Green\nfn f(x: Hue) -> bool = x.has(\"k\")")), 2, 25, mlc::String("method has expects a Map receiver"), mlc::String("E027")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: has on Map ok"), check_error_count(mlc::String("fn f(m: Map<string, i32>) -> bool = m.has(\"k\")")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: ref mut Map uses inner type for builtins"), check_error_count(mlc::String("fn f(cache: ref mut Map<string, i32>) -> bool = cache.has(\"k\")")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: if branch type mismatch — position on else expression"), first_checker_error_line(mlc::String("fn f() -> i32 = if true then 1 else \"x\" end")), 1, 37, mlc::String("if branch types differ")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: if branch same type — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = if true then 1 else 2 end")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: match arm type mismatch — position on mismatched arm body"), first_checker_error_line(mlc::String("fn f() -> i32 = match 1 { _ => 0, _ => \"x\" }")), 1, 40, mlc::String("match arm result type")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: match arms same type — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = match 1 { _ => 0, _ => 1 }")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: match guard must be bool"), first_checker_error_line(mlc::String("fn f() -> i32 = match 1 { x if x => 0 }")).contains(mlc::String("match guard must have type bool"))));
results.push_back(test_runner::assert_true(mlc::String("names: undefined identifier in match guard"), first_checker_error_line(mlc::String("fn f() -> i32 = match 1 { x if missing_guard_binding => 0 }")).contains(mlc::String("undefined:"))));
results.push_back(test_runner::assert_eq_int(mlc::String("string match on string subject — 0 errors"), check_error_count(mlc::String("fn f(word: string) -> i32 = match word { | \"fn\" => 1 | _ => 0 }")), 0));
results.push_back(test_runner::assert_true(mlc::String("string match requires string subject E077"), first_checker_error_line(mlc::String("fn f(n: i32) -> i32 = match n { | \"a\" => 1 }")).contains(mlc::String("E077"))));
results.push_back(test_runner::assert_true(mlc::String("string match duplicate literal E078"), first_checker_error_line(mlc::String("fn f(word: string) -> i32 = match word { | \"a\" => 1 | \"a\" => 2 }")).contains(mlc::String("E078"))));
results.push_back(test_runner::assert_true(mlc::String("string match wildcard must be last E080"), first_checker_error_line(mlc::String("fn f(word: string) -> i32 = match word { | _ => 0 | \"a\" => 1 }")).contains(mlc::String("E080"))));
results.push_back(test_runner::assert_true(mlc::String("string match invalid arm pattern E079"), first_checker_error_line(mlc::String("fn f(word: string) -> i32 = match word { | 1 => 0 }")).contains(mlc::String("E079"))));
results.push_back(test_runner::assert_eq_int(mlc::String("string match or-pattern literals — 0 errors"), check_error_count(mlc::String("fn f(word: string) -> i32 = match word { | \"a\" | \"b\" => 1 | _ => 0 }")), 0));
results.push_back(test_runner::assert_true(mlc::String("string match duplicate in or-pattern E078"), first_checker_error_line(mlc::String("fn f(word: string) -> i32 = match word { | \"a\" | \"a\" => 1 }")).contains(mlc::String("E078"))));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: for loop range not array — position on iterator"), first_checker_error_line(mlc::String("fn f() -> i32 = do for x in 42 do end; 0 end")), 1, 29, mlc::String("for loop range must be an array")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: for loop range array — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = do for x in a do end; 0 end")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: index on non-array — position on bracket"), first_checker_error_line(mlc::String("fn f() -> i32 = 42[0]")), 1, 19, mlc::String("indexing requires an array")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: index with non-i32 — position on bracket"), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a[\"key\"]")), 1, 26, mlc::String("array index must be i32")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: valid array index — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a[0]")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: binary + type mismatch produces error"), first_checker_error_line(mlc::String("fn f() -> i32 = 1 + true")).contains(mlc::String("incompatible operand types for +"))));
results.push_back(test_runner::assert_true(mlc::String("infer: binary == type mismatch produces error"), first_checker_error_line(mlc::String("fn f() -> bool = 1 == \"x\"")).contains(mlc::String("incompatible types for =="))));
results.push_back(test_runner::assert_true(mlc::String("infer: binary < on bool produces error"), first_checker_error_line(mlc::String("fn f() -> bool = true < false")).contains(mlc::String("ordering requires i32 or string"))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: binary + string concat — 0 errors"), check_error_count(mlc::String("fn f() -> string = \"a\" + \"b\"")), 0));
results.push_back(test_runner::assert_true(mlc::String("mutation: assign to immutable binding produces error"), first_checker_error_line(mlc::String("fn f() -> i32 = do let x = 1; x = 2; x end")).contains(mlc::String("cannot assign to immutable binding: x"))));
results.push_back(test_runner::assert_eq_int(mlc::String("mutation: assign to let mut binding — 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do let mut x = 1; x = 2; x end")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("mutation: assign inside fold reducer to outer let mut — 1 error"), check_error_count(mlc::String("fn fold_mut_touch(visited: [i32]) -> i32 = do let mut progress_count = 0; let summed = visited.fold(0, (accumulator, element_under_fold) => do progress_count = progress_count + 1; accumulator + element_under_fold end); summed + progress_count end")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("mutation: assign outer let mut inside match arms — 0 errors"), check_error_count(mlc::String("type Hue = Warm | Cold\nfn match_arm_mut(counter: Hue) -> i32 = do let mut running_total = 0; let arm_value = match counter | Warm => do running_total = running_total + 10; 1 end | Cold => do running_total = running_total + 20; 2 end; running_total + arm_value end")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("mutation: while body assigns let mut counter — 0 errors"), check_error_count(mlc::String("fn spin() -> i32 = do let mut step = 0; while step < 2 do step = step + 1 end; step end")), 0));
results.push_back(test_runner::assert_true(mlc::String("question: ? on non-Result produces error"), first_checker_error_line(mlc::String("fn f() -> i32 = do let x: i32 = 42; x? end")).contains(mlc::String("E068"))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: Result map — 0 errors"), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> Result<i32, string> = Ok(1).map(x => x + 1)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: Option map — 0 errors"), check_error_count(mlc::String("type Option<T> = Some(T) | None\nfn f() -> Option<i32> = Some(1).map(x => x * 2)")), 0));
results.push_back(assert_checker_diagnostic(mlc::String("negative E059: Result and_then must return Result exact"), mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> Result<i32, string> = Ok(1).and_then(x => x + 1)"), mlc::String("error[E059]: method and_then must return a Result type\n  --> 2:48")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E064: Option filter predicate must return bool exact"), mlc::String("type Option<T> = Some(T) | None\nfn f(o: Option<i32>) -> Option<i32> = o.filter(x => x + 1)"), mlc::String("error[E064]: method filter expects a predicate returning bool\n  --> 2:48")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array map — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> [i32] = a.map(x => x + 1)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array filter — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> [i32] = a.filter(x => x > 0)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array fold — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.fold(0, (s, x) => s + x)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array flat_map — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> [i32] = a.flat_map(x => [x])")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array any — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> bool = a.any(x => x > 0)")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: array map on non-array receiver"), first_checker_error_line(mlc::String("fn f() -> [i32] = 42.map(x => x)")).contains(mlc::String("E029"))));
results.push_back(test_runner::assert_true(mlc::String("infer: array map wrong arity"), first_checker_error_line(mlc::String("fn f(a: [i32]) -> [i32] = a.map()")).contains(mlc::String("expected 1 arguments"))));
results.push_back(assert_checker_diagnostic(mlc::String("negative E046: filter predicate must return bool exact"), mlc::String("fn f(a: [i32]) -> [i32] = a.filter(x => x)"), mlc::String("error[E046]: method filter expects a predicate returning bool, got i32\n  --> 1:36")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E047: fold step return type mismatch exact"), mlc::String("fn f(a: [i32]) -> i32 = a.fold(0, (s, x) => \"bad\")"), mlc::String("error[E047]: fold step function must return the same type as the accumulator (expected i32, got string)\n  --> 1:35")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array find — 0 errors"), check_error_count(mlc::String("type Option<T> = Some(T) | None\nfn f(a: [i32]) -> Option<i32> = a.find(x => x > 0)")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: find Option element must match declared return"), first_checker_error_line(mlc::String("type Option<T> = Some(T) | None\nfn f(a: [string]) -> Option<i32> = a.find(x => false)")).contains(mlc::String("return type: expected"))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array zip — 0 errors"), check_error_count(mlc::String("fn f(a: [i32], b: [string]) -> i32 = a.zip(b).length()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array enumerate — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.enumerate().length()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array join strings — 0 errors"), check_error_count(mlc::String("fn f(a: [string]) -> string = a.join(\",\")")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array sum — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.sum()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array flat — 0 errors"), check_error_count(mlc::String("fn f(a: [[i32]]) -> [i32] = a.flat()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array group_by — 0 errors"), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.group_by(x => x % 2).length()")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: sum on non-i32 array"), first_checker_error_line(mlc::String("fn f(a: [string]) -> i32 = a.sum()")).contains(mlc::String("E052"))));
results.push_back(test_runner::assert_true(mlc::String("infer: flat on non-nested array"), first_checker_error_line(mlc::String("fn f(a: [i32]) -> [i32] = a.flat()")).contains(mlc::String("E053"))));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Display: to_string on record — 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 } derive { Display }\nfn f(p: Point) -> string = p.to_string()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Eq: == on record — 0 errors"), check_error_count(mlc::String("type Point = { x: i32, y: i32 } derive { Eq }\nfn f(a: Point, b: Point) -> bool = a == b")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Display on sum type — 0 errors"), check_error_count(mlc::String("type Color = Red | Green | Blue derive { Display }\nfn f(c: Color) -> string = c.to_string()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Hash: mixed primitive fields on record — 0 errors"), check_error_count(mlc::String("type Cell = { tag: i32, flag: bool, label: string } derive { Hash }\nfn main() -> i32 = 0")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Hash: unit sum — 0 errors"), check_error_count(mlc::String("type Hue = North | South derive { Hash }\nfn main() -> i32 = 0")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Hash: tuple and unit variants — 0 errors"), check_error_count(mlc::String("type Shape = Circle(i32) | Empty derive { Hash }\nfn main() -> i32 = 0")), 0));
results.push_back(test_runner::assert_true(mlc::String("derive: unknown trait name"), first_checker_error_line(mlc::String("type P = { x: i32 } derive { FooTrait }\nfn main() -> i32 = 0")).contains(mlc::String("E071"))));
results.push_back(assert_checker_diagnostic(mlc::String("negative E070: duplicate derive at type name span"), mlc::String("type P = { x: i32 } derive { Eq, Eq }\nfn main() -> i32 = 0"), mlc::String("error[E070]: duplicate derive trait \"Eq\"\n  --> 1:6")));
results.push_back(test_runner::assert_true(mlc::String("derive Hash: generic type rejected"), first_checker_error_line(mlc::String("type Box<T> = { value: T } derive { Hash }\nfn main() -> i32 = 0")).contains(mlc::String("E072"))));
results.push_back(test_runner::assert_true(mlc::String("derive Hash: nested named field rejected"), first_checker_error_line(mlc::String("type Inner = { x: i32 }\ntype Outer = { inner: Inner } derive { Hash }\nfn main() -> i32 = 0")).contains(mlc::String("E069"))));
results.push_back(test_runner::assert_true(mlc::String("infer: zip expects array argument"), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a.zip(1).length()")).contains(mlc::String("E048"))));
results.push_back(test_runner::assert_eq_int(mlc::String("template: plain string - 0 errors"), check_error_count(mlc::String("fn f() -> string = `hello`")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: i32 interpolation - 0 errors"), check_error_count(mlc::String("fn f(x: i32) -> string = `val ${x}`")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: string interpolation - 0 errors"), check_error_count(mlc::String("fn f(s: string) -> string = `hello ${s}`")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: bool interpolation - 0 errors"), check_error_count(mlc::String("fn f(b: bool) -> string = `${b}`")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: multi interpolation - 0 errors"), check_error_count(mlc::String("fn f(x: i32, y: i32) -> string = `${x}+${y}`")), 0));
results.push_back(test_runner::assert_true(mlc::String("template: undefined var in interpolation - error"), check_error_count(mlc::String("fn f() -> string = `${undefined_var}`")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("let-else: PatternCtor without else - 1 error"), check_error_count(mlc::String("type Opt = Some(i32) | None\nfn f(o: Opt) -> i32 = do\n  let Some(v) = o\n  v\nend")), 1));
results.push_back(test_runner::assert_true(mlc::String("let-else: PatternCtor without else reports E067"), first_checker_error_line(mlc::String("type Opt = Some(i32) | None\nfn f(o: Opt) -> i32 = do\n  let Some(v) = o\n  v\nend")).contains(mlc::String("E067"))));
results.push_back(test_runner::assert_eq_int(mlc::String("let-else: PatternCtor with else - 0 errors"), check_error_count(mlc::String("type Opt = Some(i32) | None\nfn f(o: Opt) -> i32 = do\n  let Some(v) = o else return 0 end\n  v\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("phantom type: declaration — 0 errors"), check_error_count(mlc::String("type Unvalidated\ntype Ast<Phase> = Ast { value: i32 }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("phantom type: fn with phantom param annotation — 0 errors"), check_error_count(mlc::String("type Unvalidated\ntype Ast<Phase> = Ast { value: i32 }\nfn use_ast(ast: Ast<Unvalidated>) -> i32 = ast.value")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("phantom type: wrong phantom arg at call site — 1 error"), check_error_count(mlc::String("type Unvalidated\ntype Validated\ntype Ast<Phase> = Ast { value: i32 }\nfn expect(ast: Ast<Validated>) -> i32 = 0\nfn make() -> Ast<Unvalidated> = do\n  let ast: Ast<Unvalidated> = Ast { value: 1 }\n  ast\nend\nfn wrong() -> i32 = expect(make())")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("private ctor: declaration — 0 errors"), check_error_count(mlc::String("type Email = private Email { raw: string }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("private ctor: construction outside extend — 1 error"), check_error_count(mlc::String("type Email = private Email { raw: string }\nfn f() -> Email = Email { raw: \"x\" }")), 1));
results.push_back(test_runner::assert_true(mlc::String("private ctor: construction outside extend reports E065"), first_checker_error_line(mlc::String("type Email = private Email { raw: string }\nfn f() -> Email = Email { raw: \"x\" }")).contains(mlc::String("E065"))));
results.push_back(test_runner::assert_eq_int(mlc::String("private ctor: construction inside extend — 0 errors"), check_error_count(mlc::String("type Email = private Email { raw: string }\nextend Email { fn make(s: string) -> Email = Email { raw: s } }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("private ctor: matching outside extend allowed — 0 errors"), check_error_count(mlc::String("type Email = private Email { raw: string }\nfn get_raw(e: Email) -> string = match e { Email { raw } => raw }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("named args: all-named call — 0 errors"), check_error_count(mlc::String("fn f(x: i32, y: i32) -> i32 = x + y\nfn g() -> i32 = f(x: 1, y: 2)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("named args: reverse order — 0 errors"), check_error_count(mlc::String("fn f(x: i32, y: i32) -> i32 = x + y\nfn g() -> i32 = f(y: 2, x: 1)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("named args: mixed positional + named — 0 errors"), check_error_count(mlc::String("fn f(x: i32, y: i32, z: i32) -> i32 = x\nfn g() -> i32 = f(1, y: 2, z: 3)")), 0));
results.push_back(assert_checker_diagnostic(mlc::String("negative E034: unknown named parameter exact"), mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(bad: 1)"), mlc::String("error[E034]: unknown parameter name: bad\n  --> 2:19")));
results.push_back(assert_checker_diagnostic(mlc::String("negative E035: duplicate named arg exact"), mlc::String("fn f(x: i32, y: i32) -> i32 = x\nfn g() -> i32 = f(x: 1, x: 2)"), mlc::String("error[E035]: duplicate named arg: x\n  --> 2:25")));
results.push_back(test_runner::assert_true(mlc::String("named args: unknown param name — at least 1 error"), check_error_count(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(bad: 1)")) > 0));
results.push_back(test_runner::assert_true(mlc::String("named args: type mismatch in named arg"), check_error_count(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(x: \"str\")")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: i64 annotation - 0 errors"), check_error_count(mlc::String("fn f(x: i64) -> i64 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: f64 annotation - 0 errors"), check_error_count(mlc::String("fn f(x: f64) -> f64 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: u8 annotation - 0 errors"), check_error_count(mlc::String("fn f(x: u8) -> u8 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: usize annotation - 0 errors"), check_error_count(mlc::String("fn f(x: usize) -> usize = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: char annotation - 0 errors"), check_error_count(mlc::String("fn f(x: char) -> char = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: i64 literal - 0 errors"), check_error_count(mlc::String("fn f() -> i64 = 42i64")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: char literal - 0 errors"), check_error_count(mlc::String("fn f() -> char = 'a'")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: i64 arithmetic - 0 errors"), check_error_count(mlc::String("fn f(x: i64, y: i64) -> i64 = x + y")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: char arithmetic - 0 errors"), check_error_count(mlc::String("fn f(x: char, y: char) -> char = x + y")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("c4: f64 arithmetic - 0 errors"), check_error_count(mlc::String("fn f(x: f64, y: f64) -> f64 = x + y")), 0));
results.push_back(test_runner::assert_true(mlc::String("c4: type mismatch i64 vs i32 - 1+ errors"), check_error_count(mlc::String("fn f(x: i32) -> i64 = x")) > 0));
mlc::String with_src = mlc::String("type Res = { v: i32 } fn make(x: i32) -> Res = Res { v: x } extend Res { fn drop(self: Res) -> i32 = 0 fn read(self: Res) -> i32 = self.v } fn f(x: i32) -> i32 = do with make(x) as r do r.read() end 0 end");
results.push_back(test_runner::assert_eq_int(mlc::String("c5: with — 0 errors"), check_error_count(with_src), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("d1: DeclAssocType in trait - 0 errors"), check_error_count(mlc::String("type Iter { type Item fn Iter_next(self: Iter) -> i32 }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("d1: DeclAssocBind in extend - 0 errors"), check_error_count(mlc::String("type Iter { type Item } type Box<T> = { val: T } extend Box<T>: Iter { type Item = T fn Iter_next(self: Box<T>) -> i32 = 0 }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("d1: TyAssoc in fn return type - 0 errors"), check_error_count(mlc::String("type Iter { type Item } fn first<I: Iter>(x: I) -> I.Item = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("d2: operator overload via Add trait - 0 errors"), check_error_count(mlc::String("type Vec2 = { x: i32, y: i32 } extend Vec2 : Add<Vec2> { type Output = Vec2 fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y } } fn test(a: Vec2, b: Vec2) -> Vec2 = a + b fn main() -> i32 = 0")), 0));
mlc::String codegen_invariant_tail_after_else_if_chain = mlc::String("export fn tail_after_chain(kind: string, extra: i32) -> i32 = do\n  let mut accumulator = 0\n  if kind == \"a\" then\n    accumulator = 1\n  else if kind == \"b\" then\n    accumulator = 2\n  else\n    accumulator = 3\n  end\n  accumulator + extra\nend\n\nexport fn main() -> i32 = tail_after_chain(\"a\", 100)");
results.push_back(test_runner::assert_eq_int(mlc::String("codegen invariant: do-block tail after else-if chain — 0 errors"), check_error_count(codegen_invariant_tail_after_else_if_chain), 0));
mlc::String codegen_invariant_twin_if_zip_join_style = mlc::String("export fn twin_branch(kind: string, has_arg: bool) -> i32 = do\n  let mut diagnostics_weight = 0\n  if has_arg then\n    if kind == \"zip\" then\n      diagnostics_weight = 1\n    end\n    if kind == \"join\" then\n      diagnostics_weight = diagnostics_weight + 2\n    end\n  end\n  diagnostics_weight + 50\nend\n\nexport fn main() -> i32 = twin_branch(\"zip\", true)");
results.push_back(test_runner::assert_eq_int(mlc::String("codegen invariant: sequential if branches (zip/join shape) — 0 errors"), check_error_count(codegen_invariant_twin_if_zip_join_style), 0));
mlc::String codegen_invariant_if_else_flattened = mlc::String("fn merge_unknown_inner(then_inner: i32, else_errors: i32) -> i32 = do\n  if then_inner == 0 then else_errors + 10\n  else then_inner + else_errors\n  end\nend\n\nexport fn conditional_flat(use_merge: bool, then_inner: i32, else_errors: i32) -> i32 = do\n  if !use_merge then else_errors\n  else merge_unknown_inner(then_inner, else_errors)\n  end\nend\n\nexport fn main() -> i32 = conditional_flat(true, 0, 5)");
results.push_back(test_runner::assert_eq_int(mlc::String("codegen invariant: if + else calls helper (flattened match shape) — 0 errors"), check_error_count(codegen_invariant_if_else_flattened), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("E6 partial application: let-bound thunk call — 0 errors"), check_error_count(mlc::String("fn add_numbers(left: i32, right: i32) -> i32 = left + right\nfn main() -> i32 = do\n  let bound = add_numbers(10, _)\n  bound(3)\nend")), 0));
results.push_back(test_runner::assert_true(mlc::String("E6 bare underscore expression — error"), check_error_count(mlc::String("fn main() -> i32 = _")) > 0));
mlc::String shared_node_decl = mlc::String("type Node = { name: string, value: i32 }\n");
results.push_back(test_runner::assert_eq_int(mlc::String("infer: flat_map on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("fn f(nodes: [Shared<Node>]) -> [string] = nodes.flat_map(node => [node.name])")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: any on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("fn f(nodes: [Shared<Node>]) -> bool = nodes.any(node => node.value > 0)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: all on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("fn f(nodes: [Shared<Node>]) -> bool = nodes.all(node => node.value > 0)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: none on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("fn f(nodes: [Shared<Node>]) -> bool = nodes.none(node => node.value > 0)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: find on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("type Option<T> = Some(T) | None\n") + mlc::String("fn f(nodes: [Shared<Node>]) -> Option<Shared<Node>> = nodes.find(node => node.value > 0)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: sort_by on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("fn f(nodes: [Shared<Node>]) -> [Shared<Node>] = nodes.sort_by(node => node.value)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: filter on Shared<T> with member access — 0 errors"), check_error_count(shared_node_decl + mlc::String("fn f(nodes: [Shared<Node>]) -> [Shared<Node>] = nodes.filter(node => node.value > 0)")), 0));
return results;
}

} // namespace test_checker
