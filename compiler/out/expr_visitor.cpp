#define main mlc_user_main
#include "expr_visitor.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"

namespace expr_visitor {

using namespace registry;
using namespace semantic_ir;

template<typename VisitorResult>
struct ExprVisitor_vtable {
  std::function<VisitorResult(int, std::shared_ptr<registry::Type>)> visit_int;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_str;
  std::function<VisitorResult(bool, std::shared_ptr<registry::Type>)> visit_bool;
  std::function<VisitorResult(std::shared_ptr<registry::Type>)> visit_unit;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_float;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_i64;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_u8;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_usize;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_char;
  std::function<VisitorResult(std::shared_ptr<registry::Type>)> visit_extern;
  std::function<VisitorResult(mlc::String, std::shared_ptr<registry::Type>)> visit_ident;
  std::function<VisitorResult(mlc::String, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_bin;
  std::function<VisitorResult(mlc::String, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_un;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>, mlc::Array<int>, std::shared_ptr<registry::Type>)> visit_call;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, mlc::String, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>, mlc::Array<int>, std::shared_ptr<registry::Type>)> visit_method;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, mlc::String, std::shared_ptr<registry::Type>)> visit_field;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_index;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_if;
  std::function<VisitorResult(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_block;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, std::shared_ptr<registry::Type>)> visit_while;
  std::function<VisitorResult(mlc::String, std::shared_ptr<semantic_ir::SemanticExpression>, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, std::shared_ptr<registry::Type>)> visit_for;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>, std::shared_ptr<registry::Type>)> visit_match;
  std::function<VisitorResult(mlc::String, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>, std::shared_ptr<registry::Type>)> visit_record;
  std::function<VisitorResult(mlc::String, std::shared_ptr<semantic_ir::SemanticExpression>, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>, std::shared_ptr<registry::Type>)> visit_record_update;
  std::function<VisitorResult(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>, std::shared_ptr<registry::Type>)> visit_array;
  std::function<VisitorResult(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>, std::shared_ptr<registry::Type>)> visit_tuple;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_question;
  std::function<VisitorResult(mlc::Array<mlc::String>, std::shared_ptr<semantic_ir::SemanticExpression>, std::shared_ptr<registry::Type>)> visit_lambda;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>, mlc::String, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, std::shared_ptr<registry::Type>)> visit_with;
  std::function<VisitorResult(std::shared_ptr<semantic_ir::SemanticExpression>)> visit_unsupported;
};

} // namespace expr_visitor
