#ifndef TEST_DECL_GEN_HPP
#define TEST_DECL_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "transform_decl.hpp"
#include "context.hpp"
#include "module.hpp"
#include "decl_extend.hpp"
#include "decl.hpp"
#include "derive_methods_cpp.hpp"
#include "trait_struct_cpp.hpp"
#include "cpp_ast.hpp"
#include "decl_cpp.hpp"
#include "type_gen.hpp"
#include "expr.hpp"
#include "ast_builders.hpp"
#include "codegen_test_helpers.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "return_body.hpp"
#include "stmt_cpp.hpp"

namespace test_decl_gen {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept;

} // namespace test_decl_gen

#endif // TEST_DECL_GEN_HPP
