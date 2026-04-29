#define main mlc_user_main
#include "test_checker.hpp"

namespace test_checker {

int check_error_count(mlc::String source) noexcept{
return std::visit(overloaded{[&](const ast::Ok<auto>& ok) { auto [__0] = ok; return 0; },
[&](const ast::Err<auto>& err) { auto [errs] = err; return errs.length(); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));
}
int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept{
return std::visit(overloaded{[&](const ast::Ok<auto>& ok) { auto [__0] = ok; return 0; },
[&](const ast::Err<auto>& err) { auto [error_list] = err; return error_list.length(); }
}, check::check_with_context(decls::parse_program(lexer::tokenize(entry_source).tokens), decls::parse_program(lexer::tokenize(full_source).tokens)));
}
mlc::String first_checker_error_line(mlc::String source) noexcept{
return std::visit(overloaded{[&](const ast::Err<auto>& err) { auto [error_lines] = err; return ((error_lines.length() > 0) ? error_lines[0] : mlc::String("", 0)); },
[&](const ast::Ok<auto>& ok) { auto [__0] = ok; return mlc::String("", 0); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));
}
mlc::String first_checker_error_line_with_path(mlc::String source, mlc::String source_path) noexcept{
return std::visit(overloaded{[&](const ast::Err<auto>& err) { auto [error_lines] = err; return ((error_lines.length() > 0) ? error_lines[0] : mlc::String("", 0)); },
[&](const ast::Ok<auto>& ok) { auto [__0] = ok; return mlc::String("", 0); }
}, check::check(decls::parse_program_with_source_path(lexer::tokenize(source).tokens, source_path)));
}
bool checker_first_error_contains_nonfunction_i32(mlc::String program_source) noexcept{
auto line = first_checker_error_line(program_source);
return (line.contains(mlc::String("called value is not a function", 30)) && line.contains(mlc::String("(got i32)", 9)));
}
mlc::Array<test_runner::TestResult> checker_tests() noexcept{
auto results = mlc::Array<test_runner::TestResult>{};
results.push_back(test_runner::assert_eq_int(mlc::String("empty program - 0 errors", 24), check_error_count(mlc::String("", 0)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning literal - 0 errors", 31), check_error_count(mlc::String("fn f() -> i32 = 42", 18)), 0));
results.push_back(test_runner::assert_true(mlc::String("fn return type mismatch — error", 31), first_checker_error_line(mlc::String("fn f() -> i32 = \"hello\"", 23)).contains(mlc::String("return type: expected i32, got string", 37))));
results.push_back(test_runner::assert_eq_int(mlc::String("fn return type matches — 0 errors", 33), check_error_count(mlc::String("fn f() -> string = \"hello\"", 26)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn generic return T — 0 errors", 30), check_error_count(mlc::String("fn id<T>(x: T) -> T = x", 23)), 0));
results.push_back(test_runner::assert_true(mlc::String("integer literal called like function - diagnostic names type", 60), checker_first_error_contains_nonfunction_i32(mlc::String("fn f() -> i32 = 42()", 20))));
results.push_back(test_runner::assert_eq_int(mlc::String("fn using its own param - 0 errors", 33), check_error_count(mlc::String("fn f(x: i32) -> i32 = x", 23)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn calling another fn - 0 errors", 32), check_error_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y\nfn main() -> i32 = add(1, 2)", 66)), 0));
results.push_back(test_runner::assert_true(mlc::String("undefined name - at least 1 error", 33), (check_error_count(mlc::String("fn f() -> i32 = undefined_name", 30)) > 0)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined ident: line and column (single line body)", 51), first_checker_error_line(mlc::String("fn f() -> i32 = badident", 24)), 1, 17, mlc::String("undefined: badident", 19)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined ident: line and column (next line, indented)", 54), first_checker_error_line(mlc::String("fn f() -> i32 =\n  badident", 26)), 2, 3, mlc::String("undefined: badident", 19)));
results.push_back(test_runner::assert_eq_str(mlc::String("undefined ident: diagnostic includes source path", 48), first_checker_error_line_with_path(mlc::String("fn f() -> i32 = badident", 24), mlc::String("unit.mlc", 8)), mlc::String("unit.mlc:1:17: error: undefined: badident", 41)));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl with variants - 0 errors", 34), check_error_count(mlc::String("type Color = Red | Green | Blue", 31)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning bool - 0 errors", 28), check_error_count(mlc::String("fn f() -> bool = true", 21)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with const binding - 0 errors", 32), check_error_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend", 40)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("let const compile-time binding - 0 errors", 41), check_error_count(mlc::String("fn f() -> i32 = do\n  let const c = 42\n  c\nend", 45)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("nested generic in array type annotation - 0 errors", 50), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> i32 = do\n  let items: [Result<i32, string>] = []\n  0\nend", 101)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("record update expression - 0 errors", 35), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> Point = Point { ...p, x: 10 }", 79)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("trait bound: param method call - 0 errors", 41), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn show<T: Display>(x: T) -> string = x.to_string()", 103)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("trait bound: call with implementing type — 0 errors", 51), check_error_count(mlc::String("type Display { fn show(self: Self) -> string }\nextend i32 : Display { fn show(self: i32) -> string = self.to_string() }\nfn print_it<T: Display>(x: T) -> string = x.show()\nfn main() -> string = print_it(42)", 205)), 0));
results.push_back(test_runner::assert_true(mlc::String("trait bound: call with non-implementing type — error", 52), first_checker_error_line(mlc::String("type Display { fn show(self: Self) -> string }\nfn print_it<T: Display>(x: T) -> string = x.show()\nfn main() -> string = print_it(42)", 132)).contains(mlc::String("does not implement Display", 26))));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("record update: unknown field — position on field value", 54), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> Point = Point { ...p, z: 1 }", 78)), 2, 44, mlc::String("undefined field: z on Point", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("record constructor: unknown field — position on field value", 59), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: 1, z: 1 }", 70)), 2, 36, mlc::String("undefined field: z on Point", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("record constructor: field type mismatch — position on field value", 65), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: \"wrong\", y: 0 }", 76)), 2, 30, mlc::String("field x: expected i32, got string", 33)));
results.push_back(test_runner::assert_eq_int(mlc::String("record constructor: correct field types — 0 errors", 50), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> Point = Point { x: 1, y: 2 }", 70)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("multiline string literal in source — 0 checker errors", 53), check_error_count(mlc::String("fn f() -> string = \"hello\nworld\"", 32)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("string with escaped newline in source — 0 checker errors", 56), check_error_count(mlc::String("fn f() -> string = \"a\\nb\"", 25)), 0));
results.push_back(test_runner::assert_true(mlc::String("two undefined names - at least 1 error", 38), (check_error_count(mlc::String("fn f() -> i32 = foo + bar", 25)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("builtin print - 0 errors", 24), check_error_count(mlc::String("fn f() -> i32 = do\n  print(\"hi\")\n  0\nend", 40)), 0));
results.push_back(test_runner::assert_true(mlc::String("builtin print wrong arity - error", 33), (check_error_count(mlc::String("fn f() -> i32 = do\n  print()\n  0\nend", 36)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("builtin print wrong argument type - error", 41), (check_error_count(mlc::String("fn f() -> i32 = do\n  print(42)\n  0\nend", 38)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("monomorphic call wrong argument type - error", 44), (check_error_count(mlc::String("fn g(x: i32) -> i32 = x\nfn f() -> i32 = g(\"a\")", 46)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("tuple variant constructor wrong payload type - error", 52), (check_error_count(mlc::String("type Msg = Num(i32) | Empty\nfn f() -> Msg = Num(\"x\")", 52)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with type param - 0 errors", 29), check_error_count(mlc::String("fn id<T>(x: T) -> T = x", 23)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with type param and bound - 0 errors", 39), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn id<T: Display>(x: T) -> T = x", 84)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("call generic fn - 0 errors", 26), check_error_count(mlc::String("fn id<T>(x: T) -> T = x\nfn f() -> i32 = id(42)", 46)), 0));
results.push_back(test_runner::assert_true(mlc::String("generic fn two params same T - conflicting args error", 53), (check_error_count(mlc::String("fn pair<T>(a: T, b: T) -> T = a\nfn f() -> i32 = pair(1, \"x\")", 60)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("trait decl + extend impl - 0 errors", 35), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\ntype Color = Red | Green\nextend Color : Display { fn to_string(self: Color) -> string = \"Red\" }", 147)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("method call on param - 0 errors", 31), check_error_count(mlc::String("fn f(c: string) -> string = c.to_string()", 41)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("lambda expr - 0 errors", 22), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = x => x + 1\n  g(41)\nend", 53)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("typed lambda param — 0 errors", 29), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = (x: i32) => x + 1\n  g(41)\nend", 60)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("typed lambda two params — 0 errors", 34), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = (x: i32, y: i32) => x + y\n  g(1, 2)\nend", 70)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Ok/Err/Result as known globals - 0 errors", 41), check_error_count(mlc::String("fn f() -> i32 = do\n  const r = Ok(42)\n  0\nend", 45)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Err constructor - 0 errors", 26), check_error_count(mlc::String("fn f() -> i32 = do\n  const e = Err(\"oops\")\n  0\nend", 50)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Result return type annotation - 0 errors", 40), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> Result<i32, string> = Ok(42)", 73)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("block: let then use binding - 0 errors", 38), check_error_count(mlc::String("fn f() -> i32 = do\n  let x = 10\n  x + 1\nend", 43)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("for-in: loop variable visible in body - 0 errors", 48), check_error_count(mlc::String("fn sum(items: [i32]) -> i32 = do\n  let mut total = 0\n  for element in items do\n    total = total + element\n  end\n  total\nend", 124)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let (a, b) = (1, 2) - 0 errors", 34), check_error_count(mlc::String("fn f() -> i32 = do\n  let (a, b) = (1, 2)\n  a + b\nend", 52)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let (a, b, c) = (1, 2, 3) - 0 errors", 40), check_error_count(mlc::String("fn f() -> i32 = do\n  let (a, b, c) = (1, 2, 3)\n  a + b + c\nend", 62)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let {x, y} = record - 0 errors", 34), check_error_count(mlc::String("type Point = { x: i32, y: i32 }\nfn f() -> i32 = do\n  let p = Point { x: 1, y: 2 }\n  let { x, y } = p\n  x + y\nend", 112)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let [a, ...b] = array - 0 errors", 36), check_error_count(mlc::String("fn f() -> i32 = do\n  let a: [i32] = [1, 2, 3]\n  let [u, ...v] = a\n  u\nend", 73)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A3: let Ok(x) = Ok - 0 errors", 29), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> i32 = do\n  let o: Result<i32, string> = Ok(1)\n  let Ok(n) = o\n  n\nend", 114)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A4: default param call with omitted args - 0 errors", 51), check_error_count(mlc::String("fn f(a: i32 = 1) -> i32 = a\nfn g() -> i32 = f()", 47)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("A4: two defaults call with one arg - 0 errors", 45), check_error_count(mlc::String("fn f(a: i32, b: i32 = 2, c: i32 = 3) -> i32 = a + b + c\nfn g() -> i32 = f(0)", 76)), 0));
results.push_back(test_runner::assert_true(mlc::String("A4: default not before required", 31), first_checker_error_line(mlc::String("fn f(a: i32 = 1, b: i32) -> i32 = b", 35)).contains(mlc::String("parameter defaults must be trailing", 35))));
results.push_back(test_runner::assert_true(mlc::String("A4: generic function default rejected", 37), first_checker_error_line(mlc::String("fn f<T>(a: T = 1) -> T = a", 26)).contains(mlc::String("default parameters are not allowed on generic", 45))));
results.push_back(test_runner::assert_true(mlc::String("A4: non-literal default rejected", 32), first_checker_error_line(mlc::String("fn f(a: i32 = 1 + 1) -> i32 = a", 31)).contains(mlc::String("literal", 7))));
results.push_back(test_runner::assert_true(mlc::String("A4: too many call arguments with defaults", 41), first_checker_error_line(mlc::String("fn f(a: i32 = 1) -> i32 = a\nfn g() -> i32 = f(1, 2)", 51)).contains(mlc::String("expected between", 16))));
results.push_back(test_runner::assert_true(mlc::String("A4: extern with default rejected", 32), first_checker_error_line(mlc::String("extern fn f(a: i32 = 1) -> i32", 30)).contains(mlc::String("extern", 6))));
results.push_back(test_runner::assert_true(mlc::String("for-in: iterator must be an array", 33), first_checker_error_line(mlc::String("fn g() -> i32 = do\n  for x in 42 do\n    0\n  end\n  0\nend", 55)).contains(mlc::String("for loop range must be an array", 31))));
results.push_back(test_runner::assert_true(mlc::String("index: subscript must be i32", 28), first_checker_error_line(mlc::String("fn f(items: [i32]) -> i32 = items[\"0\"]", 38)).contains(mlc::String("array index must be i32", 23))));
results.push_back(test_runner::assert_eq_int(mlc::String("check_with_context: entry sees helper from full program - 0 errors", 66), check_with_context_error_count(mlc::String("fn main() -> i32 = helper()", 27), mlc::String("fn helper() -> i32 = 1\nfn main() -> i32 = helper()", 50)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("match: constructor arm binds payload - 0 errors", 47), check_error_count(mlc::String("type Answer = Yes(i32) | No\nfn value(answer: Answer) -> i32 = match answer | Yes(n) => n | No => 0\n", 99)), 0));
results.push_back(test_runner::assert_true(mlc::String("match: arm result type must match first arm", 43), first_checker_error_line(mlc::String("fn f(flag: bool) -> i32 = match flag | true => 1 | false => \"no\"", 64)).contains(mlc::String("match arm result type", 21))));
results.push_back(test_runner::assert_true(mlc::String("if: then and else branch types must match", 41), first_checker_error_line(mlc::String("fn f(flag: bool) -> i32 = if flag then 1 else \"no\" end", 54)).contains(mlc::String("if branch types differ", 22))));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined record field: line and column on field access", 55), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> i32 = p.z", 59)), 2, 26, mlc::String("undefined field: z on Point", 27)));
results.push_back(test_runner::assert_eq_str(mlc::String("undefined record field: message text", 36), first_checker_error_line(mlc::String("type R = { a: i32 }\nfn f(x: R) -> i32 = x.missing", 49)), mlc::String("2:22: error: undefined field: missing on R", 42)));
results.push_back(test_runner::assert_true(mlc::String("infer: string + i32 is an error", 31), (check_error_count(mlc::String("fn f() -> i32 = \"a\" + 1", 23)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 + string is an error", 31), (check_error_count(mlc::String("fn f() -> i32 = 1 + \"a\"", 23)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("infer: string with - is an error", 32), (check_error_count(mlc::String("fn f() -> string = \"a\" - \"b\"", 28)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: string + string still ok", 31), check_error_count(mlc::String("fn f() -> string = \"a\" + \"b\"", 28)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: ! on i32 is an error", 27), (check_error_count(mlc::String("fn f() -> bool = !42", 20)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: ! on bool ok", 19), check_error_count(mlc::String("fn f() -> bool = !false", 23)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 == bool is an error", 30), (check_error_count(mlc::String("fn f() -> bool = 1 == true", 26)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 < string is an error", 31), (check_error_count(mlc::String("fn f() -> bool = 1 < \"a\"", 24)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("infer: bool && i32 is an error", 30), (check_error_count(mlc::String("fn f() -> bool = true && 1", 26)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: i32 ordering ok", 22), check_error_count(mlc::String("fn f() -> bool = 1 < 2", 22)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: string ordering ok", 25), check_error_count(mlc::String("fn f() -> bool = \"a\" <= \"b\"", 27)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: bool && ok", 17), check_error_count(mlc::String("fn f() -> bool = true && false", 30)), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: unary minus on string", 28), first_checker_error_line(mlc::String("fn f() -> i32 = -\"x\"", 20)), 1, 17, mlc::String("unary - expects i32", 19)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: to_i on non-string receiver", 34), first_checker_error_line(mlc::String("fn f() -> i32 = 1.to_i()", 24)), 1, 18, mlc::String("method to_i expects a string receiver", 37)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: join on non-array receiver", 33), first_checker_error_line(mlc::String("fn f() -> string = \"a\".join(\",\")", 32)), 1, 23, mlc::String("method join expects an array receiver", 37)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: push on string receiver", 30), first_checker_error_line(mlc::String("fn f() -> i32 = \"a\".push(1)", 27)), 1, 20, mlc::String("method push expects an array receiver", 37)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: set on string receiver", 29), first_checker_error_line(mlc::String("fn f() -> i32 = \"a\".set(0, 1)", 29)), 1, 20, mlc::String("method set expects an array or Map receiver", 43)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: get on non-Map receiver", 30), first_checker_error_line(mlc::String("fn f() -> i32 = 1.get(\"k\")", 26)), 1, 18, mlc::String("method get expects a Map receiver", 33)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: get and set on Map ok", 28), check_error_count(mlc::String("fn f(m: Map<string, i32>) -> i32 = do\n  m.set(\"k\", 1)\n  m.get(\"k\")\nend", 70)), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin map get wrong arity", 34), first_checker_error_line(mlc::String("fn f(m: Map<string, i32>) -> i32 = m.get()", 42)), 1, 37, mlc::String("expected 1 arguments, got 0", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin map set wrong arity", 34), first_checker_error_line(mlc::String("fn f(m: Map<string, i32>) -> i32 = m.set(\"k\")", 45)), 1, 37, mlc::String("expected 2 arguments, got 1", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin array push wrong arity", 37), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a.push()", 32)), 1, 26, mlc::String("expected 1 arguments, got 0", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: user function call too few arguments — position on ( of call", 67), first_checker_error_line(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f()", 43)), 2, 18, mlc::String("expected 1 arguments, got 0", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: user function call too many arguments — position on ( of call", 68), first_checker_error_line(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(1, 2)", 47)), 2, 18, mlc::String("expected 1 arguments, got 2", 27)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: user function call argument type mismatch — position on argument", 71), first_checker_error_line(mlc::String("fn f(x: i32) -> i32 = x\nfn g() -> i32 = f(\"str\")", 48)), 2, 19, mlc::String("argument expects i32, got string", 32)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: called value is not a function — position on ( of call", 61), first_checker_error_line(mlc::String("fn f() -> i32 = 42()", 20)), 1, 19, mlc::String("called value is not a function (got i32)", 40)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: field access on i32 — position on dot", 44), first_checker_error_line(mlc::String("type P = { x: i32 }\nfn f(p: i32) -> i32 = p.x", 45)), 2, 24, mlc::String("field access on value without record fields: i32", 48)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: has on non-Map receiver (i32)", 36), first_checker_error_line(mlc::String("fn f() -> bool = 1.has(\"k\")", 27)), 1, 19, mlc::String("method has expects a Map receiver", 33)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: has on sum type is an error", 34), first_checker_error_line(mlc::String("type Hue = Red | Green\nfn f(x: Hue) -> bool = x.has(\"k\")", 56)), 2, 25, mlc::String("method has expects a Map receiver", 33)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: has on Map ok", 20), check_error_count(mlc::String("fn f(m: Map<string, i32>) -> bool = m.has(\"k\")", 46)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: ref mut Map uses inner type for builtins", 47), check_error_count(mlc::String("fn f(cache: ref mut Map<string, i32>) -> bool = cache.has(\"k\")", 62)), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: if branch type mismatch — position on else expression", 60), first_checker_error_line(mlc::String("fn f() -> i32 = if true then 1 else \"x\" end", 43)), 1, 37, mlc::String("if branch types differ", 22)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: if branch same type — 0 errors", 37), check_error_count(mlc::String("fn f() -> i32 = if true then 1 else 2 end", 41)), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: match arm type mismatch — position on mismatched arm body", 64), first_checker_error_line(mlc::String("fn f() -> i32 = match 1 { _ => 0, _ => \"x\" }", 44)), 1, 40, mlc::String("match arm result type", 21)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: match arms same type — 0 errors", 38), check_error_count(mlc::String("fn f() -> i32 = match 1 { _ => 0, _ => 1 }", 42)), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: for loop range not array — position on iterator", 54), first_checker_error_line(mlc::String("fn f() -> i32 = do for x in 42 do end; 0 end", 44)), 1, 29, mlc::String("for loop range must be an array", 31)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: for loop range array — 0 errors", 38), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = do for x in a do end; 0 end", 51)), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: index on non-array — position on bracket", 47), first_checker_error_line(mlc::String("fn f() -> i32 = 42[0]", 21)), 1, 19, mlc::String("indexing requires an array", 26)));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: index with non-i32 — position on bracket", 47), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a[\"key\"]", 32)), 1, 26, mlc::String("array index must be i32", 23)));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: valid array index — 0 errors", 35), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a[0]", 28)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: binary + type mismatch produces error", 44), first_checker_error_line(mlc::String("fn f() -> i32 = 1 + true", 24)).contains(mlc::String("incompatible operand types for +", 32))));
results.push_back(test_runner::assert_true(mlc::String("infer: binary == type mismatch produces error", 45), first_checker_error_line(mlc::String("fn f() -> bool = 1 == \"x\"", 25)).contains(mlc::String("incompatible types for ==", 25))));
results.push_back(test_runner::assert_true(mlc::String("infer: binary < on bool produces error", 38), first_checker_error_line(mlc::String("fn f() -> bool = true < false", 29)).contains(mlc::String("ordering requires i32 or string", 31))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: binary + string concat — 0 errors", 40), check_error_count(mlc::String("fn f() -> string = \"a\" + \"b\"", 28)), 0));
results.push_back(test_runner::assert_true(mlc::String("mutation: assign to immutable binding produces error", 52), first_checker_error_line(mlc::String("fn f() -> i32 = do let x = 1; x = 2; x end", 42)).contains(mlc::String("cannot assign to immutable binding: x", 37))));
results.push_back(test_runner::assert_eq_int(mlc::String("mutation: assign to let mut binding — 0 errors", 46), check_error_count(mlc::String("fn f() -> i32 = do let mut x = 1; x = 2; x end", 46)), 0));
results.push_back(test_runner::assert_true(mlc::String("question: ? on non-Result produces error", 40), first_checker_error_line(mlc::String("fn f() -> i32 = do let x: i32 = 42; x? end", 42)).contains(mlc::String("? operator requires a Result type", 33))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: Result map — 0 errors", 28), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> Result<i32, string> = Ok(1).map(x => x + 1)", 88)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: Option map — 0 errors", 28), check_error_count(mlc::String("type Option<T> = Some(T) | None\nfn f() -> Option<i32> = Some(1).map(x => x * 2)", 79)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array map — 0 errors", 27), check_error_count(mlc::String("fn f(a: [i32]) -> [i32] = a.map(x => x + 1)", 43)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array filter — 0 errors", 30), check_error_count(mlc::String("fn f(a: [i32]) -> [i32] = a.filter(x => x > 0)", 46)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array fold — 0 errors", 28), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.fold(0, (s, x) => s + x)", 50)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array flat_map — 0 errors", 32), check_error_count(mlc::String("fn f(a: [i32]) -> [i32] = a.flat_map(x => [x])", 46)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array any — 0 errors", 27), check_error_count(mlc::String("fn f(a: [i32]) -> bool = a.any(x => x > 0)", 42)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: array map on non-array receiver", 38), first_checker_error_line(mlc::String("fn f() -> [i32] = 42.map(x => x)", 32)).contains(mlc::String("expects an array receiver", 25))));
results.push_back(test_runner::assert_true(mlc::String("infer: array map wrong arity", 28), first_checker_error_line(mlc::String("fn f(a: [i32]) -> [i32] = a.map()", 33)).contains(mlc::String("expected 1 arguments", 20))));
results.push_back(test_runner::assert_true(mlc::String("infer: filter predicate must return bool", 40), first_checker_error_line(mlc::String("fn f(a: [i32]) -> [i32] = a.filter(x => x)", 42)).contains(mlc::String("expects a predicate returning bool", 34))));
results.push_back(test_runner::assert_true(mlc::String("infer: fold step return must match accumulator", 46), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a.fold(0, (s, x) => \"bad\")", 50)).contains(mlc::String("fold step function must return", 30))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array find — 0 errors", 28), check_error_count(mlc::String("type Option<T> = Some(T) | None\nfn f(a: [i32]) -> Option<i32> = a.find(x => x > 0)", 82)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: find Option element must match declared return", 53), first_checker_error_line(mlc::String("type Option<T> = Some(T) | None\nfn f(a: [string]) -> Option<i32> = a.find(x => false)", 85)).contains(mlc::String("return type: expected", 21))));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array zip — 0 errors", 27), check_error_count(mlc::String("fn f(a: [i32], b: [string]) -> i32 = a.zip(b).length()", 54)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array enumerate — 0 errors", 33), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.enumerate().length()", 46)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array join strings — 0 errors", 36), check_error_count(mlc::String("fn f(a: [string]) -> string = a.join(\",\")", 41)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array sum — 0 errors", 27), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.sum()", 31)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array flat — 0 errors", 28), check_error_count(mlc::String("fn f(a: [[i32]]) -> [i32] = a.flat()", 36)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: array group_by — 0 errors", 32), check_error_count(mlc::String("fn f(a: [i32]) -> i32 = a.group_by(x => x % 2).length()", 55)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: sum on non-i32 array", 27), first_checker_error_line(mlc::String("fn f(a: [string]) -> i32 = a.sum()", 34)).contains(mlc::String("sum expects i32 array", 21))));
results.push_back(test_runner::assert_true(mlc::String("infer: flat on non-nested array", 31), first_checker_error_line(mlc::String("fn f(a: [i32]) -> [i32] = a.flat()", 34)).contains(mlc::String("flat expects an array of arrays", 31))));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Display: to_string on record — 0 errors", 46), check_error_count(mlc::String("type Point = { x: i32, y: i32 } derive { Display }\nfn f(p: Point) -> string = p.to_string()", 91)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Eq: == on record — 0 errors", 34), check_error_count(mlc::String("type Point = { x: i32, y: i32 } derive { Eq }\nfn f(a: Point, b: Point) -> bool = a == b", 87)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("derive Display on sum type — 0 errors", 37), check_error_count(mlc::String("type Color = Red | Green | Blue derive { Display }\nfn f(c: Color) -> string = c.to_string()", 91)), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: zip expects array argument", 33), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a.zip(1).length()", 41)).contains(mlc::String("expects an array argument", 25))));
results.push_back(test_runner::assert_eq_int(mlc::String("template: plain string - 0 errors", 33), check_error_count(mlc::String("fn f() -> string = `hello`", 26)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: i32 interpolation - 0 errors", 38), check_error_count(mlc::String("fn f(x: i32) -> string = `val \\${x}`", 36)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: string interpolation - 0 errors", 41), check_error_count(mlc::String("fn f(s: string) -> string = `hello \\${s}`", 41)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: bool interpolation - 0 errors", 39), check_error_count(mlc::String("fn f(b: bool) -> string = `\\${b}`", 33)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("template: multi interpolation - 0 errors", 40), check_error_count(mlc::String("fn f(x: i32, y: i32) -> string = `\\${x}+\\${y}`", 46)), 0));
results.push_back(test_runner::assert_true(mlc::String("template: undefined var in interpolation - error", 48), (check_error_count(mlc::String("fn f() -> string = `\\${undefined_var}`", 38)) > 0)));
return results;
}

} // namespace test_checker
