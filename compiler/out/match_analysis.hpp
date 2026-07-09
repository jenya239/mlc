#ifndef MATCH_ANALYSIS_HPP
#define MATCH_ANALYSIS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"

namespace match_analysis {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool subject_needs_deref(std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SemanticMatchArm> first_arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> pattern_binding_name_list(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept;

mlc::String pattern_bind_names(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept;

bool subject_is_string_type(std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;

bool subject_is_bool_type(std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;

bool should_use_string_match_if_chain(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;

} // namespace match_analysis

#endif // MATCH_ANALYSIS_HPP
