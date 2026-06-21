#ifndef TESTS_MAIN_HPP
#define TESTS_MAIN_HPP

#include "mlc.hpp"
#include <variant>

#include "test_lexer.hpp"
#include "test_parser.hpp"
#include "test_checker.hpp"
#include "test_codegen.hpp"
#include "test_pipe_and_record_update.hpp"
#include "test_decl_gen.hpp"
#include "test_partial_application.hpp"
#include "test_merge_imports.hpp"
#include "test_pass.hpp"
#include "test_compile_commands.hpp"
#include "test_expr_visitor.hpp"
#include "test_visitor_pass_parity.hpp"
#include "test_cpp_printer.hpp"
#include "test_cpp_lexer.hpp"
#include "test_cpp_ast.hpp"
#include "test_cpp_parser.hpp"
#include "test_cpp_types.hpp"
#include "test_formatter.hpp"
#include "test_fuzz.hpp"
#include "test_runner.hpp"

namespace tests_main {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

} // namespace tests_main

#endif // TESTS_MAIN_HPP
