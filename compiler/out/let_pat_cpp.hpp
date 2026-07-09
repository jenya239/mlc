#ifndef LET_PAT_CPP_HPP
#define LET_PAT_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "print.hpp"

namespace let_pat_cpp {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct GenLetPatternCppResult {std::shared_ptr<cpp_ast::CppStatement> statement;context::CodegenContext codegen_context;};

let_pat_cpp::GenLetPatternCppResult gen_let_pattern_cpp_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;

} // namespace let_pat_cpp

#endif // LET_PAT_CPP_HPP
