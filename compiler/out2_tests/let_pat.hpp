#ifndef LET_PAT_HPP
#define LET_PAT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"

namespace let_pat {

mlc::String gen_let_array_rest_slice(mlc::String temp, int from_index, mlc::String element_cpp) noexcept;
mlc::String gen_let_pattern_statement(std::shared_ptr<ast::Pat> pat, std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> eval_fn) noexcept;

} // namespace let_pat

#endif // LET_PAT_HPP
