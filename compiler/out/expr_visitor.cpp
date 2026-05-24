#include "expr_visitor.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"

namespace expr_visitor {

using namespace registry;
using namespace semantic_ir;

template<typename VisitorResult>
struct ExprVisitor {
  std::function<VisitorResult(int, std::shared_ptr<Type>)> visit_int;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_str;
  std::function<VisitorResult(bool, std::shared_ptr<Type>)> visit_bool;
  std::function<VisitorResult(std::shared_ptr<Type>)> visit_unit;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_float;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_i64;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_u8;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_usize;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_char;
  std::function<VisitorResult(std::shared_ptr<Type>)> visit_extern;
  std::function<VisitorResult(mlc::String, std::shared_ptr<Type>)> visit_ident;
  std::function<VisitorResult(mlc::String, std::shared_ptr<SExpr>, std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_bin;
  std::function<VisitorResult(mlc::String, std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_un;
  std::function<VisitorResult(std::shared_ptr<SExpr>, mlc::Array<std::shared_ptr<SExpr>>, mlc::Array<int>, std::shared_ptr<Type>)> visit_call;
  std::function<VisitorResult(std::shared_ptr<SExpr>, mlc::String, mlc::Array<std::shared_ptr<SExpr>>, mlc::Array<int>, std::shared_ptr<Type>)> visit_method;
  std::function<VisitorResult(std::shared_ptr<SExpr>, mlc::String, std::shared_ptr<Type>)> visit_field;
  std::function<VisitorResult(std::shared_ptr<SExpr>, std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_index;
  std::function<VisitorResult(std::shared_ptr<SExpr>, std::shared_ptr<SExpr>, std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_if;
  std::function<VisitorResult(mlc::Array<std::shared_ptr<SStmt>>, std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_block;
  std::function<VisitorResult(std::shared_ptr<SExpr>, mlc::Array<std::shared_ptr<SStmt>>, std::shared_ptr<Type>)> visit_while;
  std::function<VisitorResult(mlc::String, std::shared_ptr<SExpr>, mlc::Array<std::shared_ptr<SStmt>>, std::shared_ptr<Type>)> visit_for;
  std::function<VisitorResult(std::shared_ptr<SExpr>, mlc::Array<std::shared_ptr<SMatchArm>>, std::shared_ptr<Type>)> visit_match;
  std::function<VisitorResult(mlc::String, mlc::Array<std::shared_ptr<SFieldVal>>, std::shared_ptr<Type>)> visit_record;
  std::function<VisitorResult(mlc::String, std::shared_ptr<SExpr>, mlc::Array<std::shared_ptr<SFieldVal>>, std::shared_ptr<Type>)> visit_record_update;
  std::function<VisitorResult(mlc::Array<std::shared_ptr<SExpr>>, std::shared_ptr<Type>)> visit_array;
  std::function<VisitorResult(mlc::Array<std::shared_ptr<SExpr>>, std::shared_ptr<Type>)> visit_tuple;
  std::function<VisitorResult(std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_question;
  std::function<VisitorResult(mlc::Array<mlc::String>, std::shared_ptr<SExpr>, std::shared_ptr<Type>)> visit_lambda;
  std::function<VisitorResult(std::shared_ptr<SExpr>, mlc::String, mlc::Array<std::shared_ptr<SStmt>>, std::shared_ptr<Type>)> visit_with;
  std::function<VisitorResult(std::shared_ptr<SExpr>)> visit_unsupported;
};

} // namespace expr_visitor
