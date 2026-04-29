#define main mlc_user_main
#include "test_parser.hpp"

namespace test_parser {

ast::Program parse_source(mlc::String source) noexcept{
return decls::parse_program(lexer::tokenize(source).tokens);
}
std::shared_ptr<ast::Expr> parse_expr_source(mlc::String source) noexcept{
return exprs::parse_expr(preds::parser_new(lexer::tokenize(source).tokens)).expr;
}
int decl_count(mlc::String source) noexcept{
return parse_source(source).decls.length();
}
std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept{
return parse_source(source).decls[0];
}
std::shared_ptr<ast::Decl> second_decl(mlc::String source) noexcept{
return parse_source(source).decls[1];
}
mlc::Array<test_runner::TestResult> parser_tests() noexcept{
auto results = mlc::Array<test_runner::TestResult>{};
results.push_back(test_runner::assert_eq_int(mlc::String("empty source - 0 decls", 22), decl_count(mlc::String("", 0)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("single fn decl - 1 decl", 23), decl_count(mlc::String("fn f() -> i32 = 42", 18)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("two fn decls - 2 decls", 22), decl_count(mlc::String("fn f() -> i32 = 1\nfn g() -> i32 = 2", 35)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl - 1 decl", 18), decl_count(mlc::String("type Color = Red | Green | Blue", 31)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with params - 1 decl", 23), decl_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y", 37)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with default param - 1 decl", 30), decl_count(mlc::String("fn f(a: i32 = 1) -> i32 = a", 27)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with do block - 1 decl", 25), decl_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend", 40)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("type + fn - 2 decls", 19), decl_count(mlc::String("type Color = Red\nfn f() -> i32 = 1", 34)), 2));
results.push_back(test_runner::assert_true(mlc::String("fn decl is DeclFn", 17), std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return true; },
[&](const auto& __v) { return false; }
}, (*first_decl(mlc::String("fn f() -> i32 = 42", 18))))));
results.push_back(test_runner::assert_true(mlc::String("type decl is DeclType", 21), std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3] = declType; return true; },
[&](const auto& __v) { return false; }
}, (*first_decl(mlc::String("type Color = Red | Green", 24))))));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with if expr - 1 decl", 24), decl_count(mlc::String("fn f(x: i32) -> i32 = if x > 0 then 1 else 0 end", 48)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with for-in - 1 decl", 23), decl_count(mlc::String("fn f(items: [i32]) -> i32 = do\n  let mut s = 0\n  for x in items do\n    s = s + x\n  end\n  s\nend", 94)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("type + fn with match pipe arms - 2 decls", 40), decl_count(mlc::String("type T = A(i32) | B\nfn f(x: T) -> i32 = match x | A(n) => n | B => 0\n", 69)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with parenthesized lambda - 1 decl", 37), decl_count(mlc::String("fn f() -> i32 = (() => 7)()", 27)), 1));
results.push_back(test_runner::assert_true(mlc::String("extend method: self with type and extra param", 45), std::visit(overloaded{[&](const ast::DeclExtend& declExtend) { auto [__0, __1, methods] = declExtend; return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, parameters, __4, __5] = declFn; return (parameters.length() == 2); },
[&](const auto& __v) { return false; }
}, (*methods[0])); },
[&](const auto& __v) { return false; }
}, (*second_decl(mlc::String("type R = { a: i32 }\nextend R {\n  fn m(self: R, x: i32) -> i32 = x\n}", 67))))));
results.push_back(test_runner::assert_eq_int(mlc::String("import star-as - 1 decl", 23), decl_count(mlc::String("import * as helpers from \"./infer_literals.mlc\"", 47)), 1));
results.push_back(test_runner::assert_true(mlc::String("import star-as: path and symbols", 32), std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [path, symbols] = declImport; return ((((path == mlc::String("./infer_literals.mlc", 20)) && (symbols.length() == 2)) && (symbols[0] == mlc::String("*", 1))) && (symbols[1] == mlc::String("helpers", 7))); },
[&](const auto& __v) { return false; }
}, (*first_decl(mlc::String("import * as helpers from \"./infer_literals.mlc\"", 47))))));
results.push_back(test_runner::assert_eq_int(mlc::String("import named list - 1 decl", 26), decl_count(mlc::String("import { foo, bar } from \"./mod.mlc\"", 36)), 1));
results.push_back(test_runner::assert_true(mlc::String("import named list: path and symbols order", 41), std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [path, symbols] = declImport; return ((((path == mlc::String("./mod.mlc", 9)) && (symbols.length() == 2)) && (symbols[0] == mlc::String("foo", 3))) && (symbols[1] == mlc::String("bar", 3))); },
[&](const auto& __v) { return false; }
}, (*first_decl(mlc::String("import { foo, bar } from \"./mod.mlc\"", 36))))));
results.push_back(test_runner::assert_true(mlc::String("import single name in braces", 28), std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [path, symbols] = declImport; return (((path == mlc::String("./one.mlc", 9)) && (symbols.length() == 1)) && (symbols[0] == mlc::String("only", 4))); },
[&](const auto& __v) { return false; }
}, (*first_decl(mlc::String("import { only } from \"./one.mlc\"", 32))))));
results.push_back(test_runner::assert_true(mlc::String("or-pattern brace-style: PatOr produced", 38), std::visit(overloaded{[&](const ast::ExprMatch& exprMatch) { auto [__0, arms, __2] = exprMatch; return ((arms.length() == 1) ? std::visit(overloaded{[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return (alts.length() == 2); },
[&](const auto& __v) { return false; }
}, (*arms[0]->pat)) : false); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("match x { A(r) | B(r) => r }", 28))))));
results.push_back(test_runner::assert_true(mlc::String("or-pattern pipe-style: PatOr produced", 37), std::visit(overloaded{[&](const ast::ExprMatch& exprMatch) { auto [__0, arms, __2] = exprMatch; return ((arms.length() == 1) ? std::visit(overloaded{[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return (alts.length() == 2); },
[&](const auto& __v) { return false; }
}, (*arms[0]->pat)) : false); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("match x | A(r) | B(r) => r end", 30))))));
results.push_back(test_runner::assert_true(mlc::String("unit or-pattern: PatOr produced", 31), std::visit(overloaded{[&](const ast::ExprMatch& exprMatch) { auto [__0, arms, __2] = exprMatch; return ((arms.length() == 2) ? std::visit(overloaded{[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return (alts.length() == 2); },
[&](const auto& __v) { return false; }
}, (*arms[0]->pat)) : false); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("match x { Red | Green => 1, Blue => 2 }", 39))))));
results.push_back(test_runner::assert_true(mlc::String("single pattern: no PatOr", 24), std::visit(overloaded{[&](const ast::ExprMatch& exprMatch) { auto [__0, arms, __2] = exprMatch; return ((arms.length() == 1) ? std::visit(overloaded{[&](const ast::PatCtor& patCtor) { auto [name, __1, __2] = patCtor; return (name == mlc::String("A", 1)); },
[&](const auto& __v) { return false; }
}, (*arms[0]->pat)) : false); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("match x { A(r) => r }", 21))))));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template literal - 1 decl", 31), decl_count(mlc::String("fn f() -> string = `hello`", 26)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("template with interpolation - 1 decl", 36), decl_count(mlc::String("fn f(x: i32) -> string = `a \\${x} b`", 36)), 1));
results.push_back(test_runner::assert_true(mlc::String("plain template parses to ExprStr", 32), std::visit(overloaded{[&](const ast::ExprStr& exprStr) { auto [s, __1] = exprStr; return (s == mlc::String("hello", 5)); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("`hello`", 7))))));
results.push_back(test_runner::assert_true(mlc::String("template with interp parses to ExprBin", 38), std::visit(overloaded{[&](const ast::ExprBin& exprBin) { auto [op, __1, __2, __3] = exprBin; return (op == mlc::String("+", 1)); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("`a \\${x}`", 9))))));
results.push_back(test_runner::assert_true(mlc::String("template with only interp parses to ExprBin", 43), std::visit(overloaded{[&](const ast::ExprBin& exprBin) { auto [op, __1, __2, __3] = exprBin; return (op == mlc::String("+", 1)); },
[&](const auto& __v) { return false; }
}, (*parse_expr_source(mlc::String("`\\${x}`", 7))))));
return results;
}

} // namespace test_parser
