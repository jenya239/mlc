#define main mlc_user_main
#include "transform.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "hof_method_spec.hpp"
#include "infer_weak_method.hpp"
#include "pattern_env.hpp"
#include "substitution.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"
#include "named_args.hpp"
#include "semantic_ir.hpp"

namespace transform {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace hof_method_spec;
using namespace infer_weak_method;
using namespace pattern_env;
using namespace substitution;
using namespace infer;
using namespace check_context;
using namespace record_lit_merge;
using namespace partial_application_desugar;
using namespace named_args;
using namespace semantic_ir;
using namespace ast_tokens;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_int(TransformPass self, int value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_str(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bool(TransformPass self, bool value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unit(TransformPass self, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_float(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_i64(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_u8(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_usize(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_char(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_extern(TransformPass self, mlc::String extern_c_name, mlc::String extern_header, mlc::Array<mlc::String> concurrency_attrs, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_ident(TransformPass self, mlc::String name, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bin(TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_un(TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_call(TransformPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_method(TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_field(TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_index(TransformPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_if(TransformPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_block(TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_while(TransformPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_spawn(TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_for(TransformPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_match(TransformPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record(TransformPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record_update(TransformPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_array(TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_tuple(TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_question(TransformPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_lambda(TransformPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_named_arg(TransformPass self, std::shared_ptr<ast::Expr> inner_expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_with(TransformPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unsupported(TransformPass self, std::shared_ptr<ast::Expr> expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = receiver_semantic_type;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [shared_inner_type] = tShared; return [&]() -> mlc::String {
auto __match_subject = shared_inner_type;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [inner_name] = tNamed; return inner_name;
}
return mlc::String("", 0);
std::abort();
}();
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<int> direct_call_parameter_mutability_flags(registry::TypeRegistry registry, mlc::String callee_name, int positional_argument_count) noexcept{
  if ((callee_name == mlc::String("", 0)))   {
    auto empty_mutability_pattern = mlc::Array<int>{};
    return empty_mutability_pattern;
  } else   {
    auto stored = registry::TypeRegistry_parameter_mutability_flags_for(registry, callee_name);
    if ((stored.length() != positional_argument_count))     {
      auto empty_mutability_pattern = mlc::Array<int>{};
      return empty_mutability_pattern;
    } else     {
      return stored;
    }
  }
}
mlc::Array<int> instance_method_receiver_and_parameters_mutability_pattern(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name, int method_argument_count) noexcept{
  auto receiver_key = receiver_type_key_for_method_dispatch(receiver_semantic_type);
  if ((receiver_key == mlc::String("", 0)))   {
    auto empty_mutability_pattern = mlc::Array<int>{};
    return empty_mutability_pattern;
  } else   {
    auto mangled = ((receiver_key + mlc::String("_", 1)) + method_name);
    if ((!registry::TypeRegistry_has_fn(registry, mangled)))     {
      auto empty_mutability_pattern = mlc::Array<int>{};
      return empty_mutability_pattern;
    } else     {
      auto stored = registry::TypeRegistry_parameter_mutability_flags_for(registry, mangled);
      if ((stored.length() != (method_argument_count + 1)))       {
        auto empty_mutability_pattern = mlc::Array<int>{};
        return empty_mutability_pattern;
      } else       {
        return stored;
      }
    }
  }
}
mlc::String extend_method_mangled_name(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name) noexcept{
  auto receiver_key = receiver_type_key_for_method_dispatch(receiver_semantic_type);
  if ((receiver_key == mlc::String("", 0)))   {
    return mlc::String("", 0);
  } else   {
    auto mangled = ((receiver_key + mlc::String("_", 1)) + method_name);
    if (registry::TypeRegistry_has_fn(registry, mangled))     {
      return mangled;
    } else     {
      return mlc::String("", 0);
    }
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_extend_method_as_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments, std::shared_ptr<registry::Type> receiver_type, std::shared_ptr<registry::Type> result_type, ast::Span source_span) noexcept{
  auto mangled = extend_method_mangled_name(registry, receiver_type, method_name);
  auto call_arguments = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{};
  call_arguments.push_back(typed_object);
  auto argument_index = 0;
  while ((argument_index < typed_arguments.length()))   {
    call_arguments.push_back(typed_arguments[argument_index]);
    (argument_index = (argument_index + 1));
  }
  auto callee = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent{mangled, registry::TypeRegistry_fn_type(registry, mangled), source_span});
  auto call_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.length());
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall{callee, call_arguments, call_parameter_mutability_flags, result_type, source_span});
}
bool semantic_type_is_tarray(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [__0] = tArray; return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return false; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return false; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TUnknown& tUnknown) { return false; }
}, (*type_value));
}
std::shared_ptr<registry::Type> array_element_type_from_semantic_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*type_value));
}
mlc::String generic_type_name(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return name; },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("", 0); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); }
}, (*type_value));
}
bool callee_semantic_type_is_function(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return true; },
[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return (name == mlc::String("__ExternFn", 10)); },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return false; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return false; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TUnknown& tUnknown) { return false; }
}, (*type_value));
}
mlc::Array<std::shared_ptr<registry::Type>> function_parameter_types_from_callee_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [parameter_types, __1] = tFn; return parameter_types; },
[&](const registry::TGeneric& tGeneric) { auto [name, type_arguments] = tGeneric; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> {
  if ((name == mlc::String("__ExternFn", 10)))   {
    return type_arguments.drop(1);
  } else   {
    return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}();
  }
}(); },
[&](const registry::TI32& tI32) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TString& tString) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TBool& tBool) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUnit& tUnit) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TI64& tI64) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TF64& tF64) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TU8& tU8) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUsize& tUsize) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TChar& tChar) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUnknown& tUnknown) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); }
}, (*type_value));
}
mlc::String call_callee_ident_name(std::shared_ptr<ast::Expr> function) noexcept{
  return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return name; },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return mlc::String("", 0); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return mlc::String("", 0); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return mlc::String("", 0); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return mlc::String("", 0); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return mlc::String("", 0); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return mlc::String("", 0); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return mlc::String("", 0); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return mlc::String("", 0); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return mlc::String("", 0); },
[&](const ast::ExprBin& exprBin) { auto [__0, __1, __2, __3] = exprBin; return mlc::String("", 0); },
[&](const ast::ExprUn& exprUn) { auto [__0, __1, __2] = exprUn; return mlc::String("", 0); },
[&](const ast::ExprCall& exprCall) { auto [__0, __1, __2] = exprCall; return mlc::String("", 0); },
[&](const ast::ExprMethod& exprMethod) { auto [__0, __1, __2, __3] = exprMethod; return mlc::String("", 0); },
[&](const ast::ExprField& exprField) { auto [__0, __1, __2] = exprField; return mlc::String("", 0); },
[&](const ast::ExprIndex& exprIndex) { auto [__0, __1, __2] = exprIndex; return mlc::String("", 0); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2, __3] = exprIf; return mlc::String("", 0); },
[&](const ast::ExprBlock& exprBlock) { auto [__0, __1, __2] = exprBlock; return mlc::String("", 0); },
[&](const ast::ExprWhile& exprWhile) { auto [__0, __1, __2] = exprWhile; return mlc::String("", 0); },
[&](const ast::ExprFor& exprFor) { auto [__0, __1, __2, __3] = exprFor; return mlc::String("", 0); },
[&](const ast::ExprMatch& exprMatch) { auto [__0, __1, __2] = exprMatch; return mlc::String("", 0); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, __1, __2] = exprRecord; return mlc::String("", 0); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, __1, __2, __3] = exprRecordUpdate; return mlc::String("", 0); },
[&](const ast::ExprArray& exprArray) { auto [__0, __1] = exprArray; return mlc::String("", 0); },
[&](const ast::ExprTuple& exprTuple) { auto [__0, __1] = exprTuple; return mlc::String("", 0); },
[&](const ast::ExprQuestion& exprQuestion) { auto [__0, __1] = exprQuestion; return mlc::String("", 0); },
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return mlc::String("", 0); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, __1, __2] = exprLambda; return mlc::String("", 0); },
[&](const ast::ExprSpawn& exprSpawn) { auto [__0, __1] = exprSpawn; return mlc::String("", 0); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, __1, __2] = exprNamedArg; return mlc::String("", 0); },
[&](const ast::ExprWith& exprWith) { auto [__0, __1, __2, __3] = exprWith; return mlc::String("", 0); }
}, (*function));
}
bool call_argument_is_lambda(std::shared_ptr<ast::Expr> argument_expression) noexcept{
  return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [__0, __1, __2] = exprLambda; return true; },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return false; },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return false; },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return false; },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return false; },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return false; },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return false; },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return false; },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return false; },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return false; },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return false; },
[&](const ast::ExprBin& exprBin) { auto [__0, __1, __2, __3] = exprBin; return false; },
[&](const ast::ExprUn& exprUn) { auto [__0, __1, __2] = exprUn; return false; },
[&](const ast::ExprCall& exprCall) { auto [__0, __1, __2] = exprCall; return false; },
[&](const ast::ExprMethod& exprMethod) { auto [__0, __1, __2, __3] = exprMethod; return false; },
[&](const ast::ExprField& exprField) { auto [__0, __1, __2] = exprField; return false; },
[&](const ast::ExprIndex& exprIndex) { auto [__0, __1, __2] = exprIndex; return false; },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2, __3] = exprIf; return false; },
[&](const ast::ExprBlock& exprBlock) { auto [__0, __1, __2] = exprBlock; return false; },
[&](const ast::ExprWhile& exprWhile) { auto [__0, __1, __2] = exprWhile; return false; },
[&](const ast::ExprFor& exprFor) { auto [__0, __1, __2, __3] = exprFor; return false; },
[&](const ast::ExprMatch& exprMatch) { auto [__0, __1, __2] = exprMatch; return false; },
[&](const ast::ExprRecord& exprRecord) { auto [__0, __1, __2] = exprRecord; return false; },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, __1, __2, __3] = exprRecordUpdate; return false; },
[&](const ast::ExprArray& exprArray) { auto [__0, __1] = exprArray; return false; },
[&](const ast::ExprTuple& exprTuple) { auto [__0, __1] = exprTuple; return false; },
[&](const ast::ExprQuestion& exprQuestion) { auto [__0, __1] = exprQuestion; return false; },
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return false; },
[&](const ast::ExprSpawn& exprSpawn) { auto [__0, __1] = exprSpawn; return false; },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, __1, __2] = exprNamedArg; return false; },
[&](const ast::ExprWith& exprWith) { auto [__0, __1, __2, __3] = exprWith; return false; }
}, (*argument_expression));
}
std::shared_ptr<registry::Type> binary_result_type_for_operator(registry::TypeRegistry registry, std::shared_ptr<registry::Type> left_type, mlc::String operation, mlc::String method) noexcept{
  auto from_registry = registry::method_return_type_from_object(left_type, method, registry);
  if (semantic_type_structure::type_is_unknown(from_registry))   {
    return semantic_type_structure::binary_operation_result_type(operation, left_type);
  } else   {
    return from_registry;
  }
}
std::shared_ptr<registry::Type> method_result_type_for_dispatch(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, registry::TypeRegistry registry) noexcept{
  if (infer_weak_method::is_shared_weak_method_on_receiver(receiver_type, method_name))   {
    return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Weak", 4), mlc::Array<std::shared_ptr<registry::Type>>{semantic_type_structure::shared_pointer_inner_type(receiver_type)}});
  } else if (infer_weak_method::is_weak_upgrade_method_on_receiver(receiver_type, method_name))   {
    return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = receiver_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, type_arguments] = tGeneric; return ((type_arguments.length() == 1) ? (std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>(registry::TShared{type_arguments[0]})}})) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
  } else if (semantic_type_structure::type_is_unknown(semantic_type_structure::builtin_method_return_type(method_name)))   {
    return registry::method_return_type_from_object(receiver_type, method_name, registry);
  } else   {
    return semantic_type_structure::builtin_method_return_type(method_name);
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_shared_static_new_call(mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, TransformContext transform_context, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto typed_arguments = transform_exprs(method_arguments, transform_context, stmts_fn);
  auto argument_type = ((typed_arguments.length() == 1) ? (semantic_ir::sexpr_type(typed_arguments[0])) : (std::make_shared<registry::Type>(registry::TUnknown{})));
  auto result_type = std::make_shared<registry::Type>(registry::TShared{argument_type});
  auto typed_receiver = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent{mlc::String("Shared", 6), std::make_shared<registry::Type>(registry::TUnknown{}), source_span});
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod{typed_receiver, mlc::String("new", 3), typed_arguments, mlc::Array<int>{0}, result_type, source_span});
}
std::shared_ptr<registry::Type> merge_conditional_expression_types(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
  auto then_type = semantic_ir::sexpr_type(typed_then);
  auto else_type = semantic_ir::sexpr_type(typed_else);
  if (semantic_type_is_tarray(then_type))   {
    if (semantic_type_is_tarray(else_type))     {
      auto then_element_type = array_element_type_from_semantic_type(then_type);
      auto else_element_type = array_element_type_from_semantic_type(else_type);
      if (semantic_type_structure::type_is_unknown(then_element_type))       {
        return std::make_shared<registry::Type>(registry::TArray{else_element_type});
      } else       {
        return std::make_shared<registry::Type>(registry::TArray{then_element_type});
      }
    } else     {
      return then_type;
    }
  } else   {
    return then_type;
  }
}
std::shared_ptr<registry::Type> array_element_type_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> typed_expression) noexcept{
  auto expression_type = semantic_ir::sexpr_type(typed_expression);
  if (semantic_type_is_tarray(expression_type))   {
    return array_element_type_from_semantic_type(expression_type);
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
mlc::Array<std::shared_ptr<registry::Type>> type_arguments_from_generic_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, type_arguments] = tGeneric; return type_arguments; },
[&](const registry::TI32& tI32) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TString& tString) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TBool& tBool) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUnit& tUnit) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TI64& tI64) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TF64& tF64) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TU8& tU8) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUsize& tUsize) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TChar& tChar) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUnknown& tUnknown) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); }
}, (*type_value));
}
std::shared_ptr<registry::Type> question_unwrapped_type_from_inner(std::shared_ptr<registry::Type> inner_expression_type) noexcept{
  auto type_arguments = type_arguments_from_generic_type(inner_expression_type);
  if ((type_arguments.length() > 0))   {
    return type_arguments[0];
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_result_option_hof_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, TransformContext transform_context, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto typed_arguments = transform_exprs(method_arguments, transform_context, stmts_fn);
  auto argument_types = typed_arguments.map([=](std::shared_ptr<semantic_ir::SemanticExpression> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
  auto result_type = hof_method_spec::hof_call_result_type(receiver_type, method_name, argument_types);
  auto method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.length());
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod{typed_object, method_name, typed_arguments, method_parameter_mutability_flags, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_array_hof_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, TransformContext transform_context, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto typed_arguments = transform_array_hof_method_arguments(typed_object, method_name, method_arguments, transform_context, stmts_fn);
  auto argument_types = typed_arguments.map([=](std::shared_ptr<semantic_ir::SemanticExpression> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
  auto result_type = hof_method_spec::hof_call_result_type(receiver_type, method_name, argument_types);
  auto method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.length());
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod{typed_object, method_name, typed_arguments, method_parameter_mutability_flags, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_regular_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, TransformContext transform_context, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto typed_arguments = transform_exprs(method_arguments, transform_context, stmts_fn);
  auto result_type = method_result_type_for_dispatch(receiver_type, method_name, registry);
  auto mangled = extend_method_mangled_name(registry, receiver_type, method_name);
  if ((mangled != mlc::String("", 0)))   {
    return transform_extend_method_as_call(registry, typed_object, method_name, typed_arguments, receiver_type, result_type, source_span);
  } else   {
    auto method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.length());
    return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod{typed_object, method_name, typed_arguments, method_parameter_mutability_flags, result_type, source_span});
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<registry::Type> then_element_type, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
  auto else_type = semantic_ir::sexpr_type(typed_else);
  if ((semantic_type_is_tarray(else_type) && semantic_type_structure::type_is_unknown(array_element_type_from_semantic_type(else_type))))   {
    return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray{elements, std::make_shared<registry::Type>(registry::TArray{then_element_type}), span_else});
  } else   {
    return typed_else;
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
  auto then_type = semantic_ir::sexpr_type(typed_then);
  if (semantic_type_is_tarray(then_type))   {
    return coerce_unknown_else_array_when_then_known_inner(elements, span_else, array_element_type_from_semantic_type(then_type), typed_else);
  } else   {
    return typed_else;
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
  if ((elements.length() > 0))   {
    return typed_else;
  } else   {
    return conditional_else_coerce_empty_array_using_then_type(typed_then, elements, span_else, typed_else);
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [elements, __1, span_else] = semanticExpressionArray; return array_literal_else_maybe_coerce(typed_then, elements, span_else, typed_else); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return typed_else; },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return typed_else; },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return typed_else; },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return typed_else; },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return typed_else; },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return typed_else; },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return typed_else; },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return typed_else; },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return typed_else; },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return typed_else; },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return typed_else; },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return typed_else; },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return typed_else; },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return typed_else; },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return typed_else; },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return typed_else; },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return typed_else; },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return typed_else; },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [__0, __1, __2, __3] = semanticExpressionBlock; return typed_else; },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return typed_else; },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return typed_else; },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return typed_else; },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return typed_else; },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return typed_else; },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return typed_else; },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return typed_else; },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return typed_else; },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return typed_else; }
}, (*typed_else));
}
TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
  return TransformContext{type_env, registry, {}};
}
TransformContext empty_transform_context() noexcept{
  return TransformContext{{}, registry::empty_registry(), {}};
}
TransformContext transform_context_with_env(TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{
  return TransformContext{type_env, base.registry, base.lambda_parameter_types};
}
TransformContext transform_context_with_lambda_parameter_types(TransformContext base, mlc::Array<std::shared_ptr<registry::Type>> lambda_parameter_types) noexcept{
  return TransformContext{base.type_env, base.registry, lambda_parameter_types};
}
std::shared_ptr<semantic_ir::SemanticExpression> coerce_array_semantic_expression_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  if ((elements.length() == 0))   {
    return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray{elements, target_type, source_span});
  } else   {
    return expression;
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> coerce_record_semantic_expression_to_type(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> fields, std::shared_ptr<registry::Type> expr_type, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  auto target_generic_name = generic_type_name(target_type);
  auto expression_generic_name = generic_type_name(expr_type);
  if (((target_generic_name != mlc::String("", 0)) && (target_generic_name == expression_generic_name)))   {
    return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecord{type_name, fields, target_type, source_span});
  } else   {
    return expression;
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> coerce_block_semantic_expression_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, ast::Span span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  if (semantic_type_is_tarray(target_type))   {
    auto coerced_result = coerce_expr_to_type(result_expression, target_type);
    return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock{statements, coerced_result, semantic_ir::sexpr_type(coerced_result), span});
  } else   {
    return expression;
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> coerce_if_semantic_expression_to_type(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_branch, std::shared_ptr<semantic_ir::SemanticExpression> else_branch, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  if (semantic_type_is_tarray(target_type))   {
    return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf{condition, coerce_expr_to_type(then_branch, target_type), coerce_expr_to_type(else_branch, target_type), target_type, source_span});
  } else   {
    return expression;
  }
}
std::shared_ptr<semantic_ir::SemanticMatchArm> coerce_match_arm_to_type(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<registry::Type> target_type) noexcept{
  return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{arm->pattern, arm->has_guard, arm->when_condition, coerce_expr_to_type(arm->body, target_type)});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> coerce_match_arms_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> target_type) noexcept{
  return arms.map([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return coerce_match_arm_to_type(arm, target_type); });
}
std::shared_ptr<semantic_ir::SemanticExpression> coerce_expr_to_type(std::shared_ptr<semantic_ir::SemanticExpression> expression, std::shared_ptr<registry::Type> target_type) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [elements, __1, source_span] = semanticExpressionArray; return coerce_array_semantic_expression_to_type(elements, source_span, target_type, expression); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [type_name, fields, expr_type, source_span] = semanticExpressionRecord; return coerce_record_semantic_expression_to_type(type_name, fields, expr_type, source_span, target_type, expression); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [statements, result_expression, __2, span] = semanticExpressionBlock; return coerce_block_semantic_expression_to_type(statements, result_expression, span, target_type, expression); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [condition, then_branch, else_branch, __3, source_span] = semanticExpressionIf; return coerce_if_semantic_expression_to_type(condition, then_branch, else_branch, source_span, target_type, expression); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return expression; },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return expression; },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return expression; },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return expression; },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return expression; },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return expression; },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return expression; },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return expression; },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return expression; },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return expression; },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return expression; },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return expression; },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return expression; },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [function, arguments, flags, __3, span] = semanticExpressionCall; return [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> {
auto __match_subject = target_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, __1] = tGeneric; return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall{function, arguments, flags, target_type, span});
}
return expression;
std::abort();
}(); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return expression; },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return expression; },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return expression; },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return expression; },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return expression; },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return expression; },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return expression; },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return expression; },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return expression; },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return expression; },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return expression; }
}, (*expression));
}
std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept{
  return std::make_shared<registry::Type>(registry::TUnknown{});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return expressions.map([=](std::shared_ptr<ast::Expr> expression_under_transform) mutable { return transform_expr(expression_under_transform, transform_context, stmts_fn); });
}
std::shared_ptr<registry::Type> expected_call_argument_type_at_index(int argument_index, mlc::Array<std::shared_ptr<registry::Type>> expected_formal_parameter_types) noexcept{
  if ((argument_index < expected_formal_parameter_types.length()))   {
    return expected_formal_parameter_types[argument_index];
  } else   {
    return standalone_unknown_cell();
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_lambda_call_argument(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> lambda_body, ast::Span lambda_span, std::shared_ptr<registry::Type> expected_formal_parameter_type, std::shared_ptr<ast::Expr> argument_expression_shared, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  if (callee_semantic_type_is_function(expected_formal_parameter_type))   {
    return transform_expr_lambda_with_param_types(parameter_names, function_parameter_types_from_callee_type(expected_formal_parameter_type), lambda_body, lambda_span, transform_context, stmts_fn);
  } else   {
    return transform_expr(argument_expression_shared, transform_context, stmts_fn);
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto argument_partially_desugared_shared_expression = partial_application_desugar::partial_application_desugar_expr(argument_expression_shared);
  if (call_argument_is_lambda(argument_partially_desugared_shared_expression))   {
    return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, lambda_body, lambda_span] = exprLambda; return transform_lambda_call_argument(parameter_names, lambda_body, lambda_span, expected_formal_parameter_type, argument_expression_shared, transform_context, stmts_fn); },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprBin& exprBin) { auto [__0, __1, __2, __3] = exprBin; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprUn& exprUn) { auto [__0, __1, __2] = exprUn; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprCall& exprCall) { auto [__0, __1, __2] = exprCall; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprMethod& exprMethod) { auto [__0, __1, __2, __3] = exprMethod; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprField& exprField) { auto [__0, __1, __2] = exprField; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprIndex& exprIndex) { auto [__0, __1, __2] = exprIndex; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2, __3] = exprIf; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprBlock& exprBlock) { auto [__0, __1, __2] = exprBlock; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprWhile& exprWhile) { auto [__0, __1, __2] = exprWhile; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprFor& exprFor) { auto [__0, __1, __2, __3] = exprFor; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprMatch& exprMatch) { auto [__0, __1, __2] = exprMatch; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, __1, __2] = exprRecord; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, __1, __2, __3] = exprRecordUpdate; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprArray& exprArray) { auto [__0, __1] = exprArray; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprTuple& exprTuple) { auto [__0, __1] = exprTuple; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprQuestion& exprQuestion) { auto [__0, __1] = exprQuestion; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprSpawn& exprSpawn) { auto [__0, __1] = exprSpawn; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, __1, __2] = exprNamedArg; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
[&](const ast::ExprWith& exprWith) { auto [__0, __1, __2, __3] = exprWith; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); }
}, (*argument_partially_desugared_shared_expression));
  } else   {
    return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type);
  }
}
std::shared_ptr<registry::Type> function_return_type_from_callee_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, return_type] = tFn; return return_type; },
[&](const registry::TGeneric& tGeneric) { auto [name, type_arguments] = tGeneric; return (((name == mlc::String("__ExternFn", 10)) && (type_arguments.length() >= 1)) ? (type_arguments[0]) : (std::make_shared<registry::Type>(registry::TUnknown{}))); },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*type_value));
}
Transform_call_arguments_fold_state transform_call_arguments_fold_step(Transform_call_arguments_fold_state state, std::shared_ptr<ast::Expr> argument_expression, mlc::Array<std::shared_ptr<registry::Type>> expected_formal_parameter_types, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return Transform_call_arguments_fold_state{state.transformed_arguments.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{transform_one_call_argument_using_optional_expected_type(argument_expression, expected_call_argument_type_at_index(state.next_argument_index, expected_formal_parameter_types), transform_context, stmts_fn)}), (state.next_argument_index + 1)};
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SemanticExpression> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto callee_type = semantic_ir::sexpr_type(callee_semantic_expression);
  if (callee_semantic_type_is_function(callee_type))   {
    return argument_expressions_under_call.fold(Transform_call_arguments_fold_state{{}, 0}, [=](Transform_call_arguments_fold_state state, std::shared_ptr<ast::Expr> argument_expression) mutable { return transform_call_arguments_fold_step(state, argument_expression, function_parameter_types_from_callee_type(callee_type), transform_context, stmts_fn); }).transformed_arguments;
  } else   {
    return transform_exprs(argument_expressions_under_call, transform_context, stmts_fn);
  }
}
std::shared_ptr<semantic_ir::SemanticFieldVal> semantic_field_val_from_source_field_after_transform(std::shared_ptr<ast::FieldVal> field_value_under_transform, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{field_value_under_transform->name, transform_expr(field_value_under_transform->value, transform_context, stmts_fn)});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> transform_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return field_values.map([=](std::shared_ptr<ast::FieldVal> field_value_under_transform) mutable { return semantic_field_val_from_source_field_after_transform(field_value_under_transform, transform_context, stmts_fn); });
}
std::shared_ptr<semantic_ir::SemanticMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> match_arm, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto arm_environment_after_pattern = pattern_env::env_for_pattern_substituted(transform_context.type_env, match_arm->pattern, transform_context.registry, substitution, scrutinee_type);
  auto arm_transform_context = transform_context_with_env(transform_context, arm_environment_after_pattern);
  auto typed_body_expression = transform_expr(match_arm->body, arm_transform_context, stmts_fn);
  auto typed_guard_expression = (match_arm->has_guard ? (transform_expr(match_arm->when_condition, arm_transform_context, stmts_fn)) : (std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool{true, std::make_shared<registry::Type>(registry::TBool{}), ast::pattern_span(match_arm->pattern)})));
  return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{match_arm->pattern, match_arm->has_guard, typed_guard_expression, typed_body_expression});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return match_arms.map([=](std::shared_ptr<ast::MatchArm> match_arm_under_transform) mutable { return transform_single_match_arm(match_arm_under_transform, transform_context, substitution, scrutinee_type, stmts_fn); });
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept{
  type_environment_map.set(parameter_binding_name, standalone_unknown_cell());
  return type_environment_map;
}
Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept{
  auto resolved_parameter_type = ((state.next_explicit_type_position < optional_explicit_parameter_types.length()) ? (optional_explicit_parameter_types[state.next_explicit_type_position]) : (standalone_unknown_cell()));
  state.type_environment.set(parameter_binding_name, resolved_parameter_type);
  return Transform_lambda_parameter_types_fold_state{state.type_environment, state.parameter_type_vector.concat(mlc::Array<std::shared_ptr<registry::Type>>{resolved_parameter_type}), (state.next_explicit_type_position + 1)};
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto lambda_environment = transform_context.type_env;
  auto lambda_parameter_accumulator_after_bind = parameter_names.fold(Transform_lambda_parameter_types_fold_state{lambda_environment, {}, 0}, [=](Transform_lambda_parameter_types_fold_state accumulator, mlc::String parameter_binding_name) mutable { return transform_lambda_parameter_types_environment_fold_step(accumulator, parameter_binding_name, parameter_types); });
  auto lambda_context = transform_context_with_lambda_parameter_types(transform_context_with_env(transform_context, lambda_parameter_accumulator_after_bind.type_environment), mlc::Array<std::shared_ptr<registry::Type>>{});
  auto typed_body = transform_expr(body, lambda_context, stmts_fn);
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda{parameter_names, typed_body, std::make_shared<registry::Type>(registry::TFn{lambda_parameter_accumulator_after_bind.parameter_type_vector, semantic_ir::sexpr_type(typed_body)}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_expr_with_lambda_parameter_types(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return transform_expr(expression, transform_context_with_lambda_parameter_types(transform_context, expected_param_types), stmts_fn);
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> semantic_expression_list_singleton(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  auto empty_transformed_arguments = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{};
  return empty_transformed_arguments.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{expression});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto element_type = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
  auto empty_transformed_arguments = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{};
  if ((method_name == mlc::String("fold", 4)))   {
    if ((method_arguments.length() == 0))     {
      return empty_transformed_arguments;
    } else if ((method_arguments.length() == 1))     {
      return semantic_expression_list_singleton(transform_expr(method_arguments[0], transform_context, stmts_fn));
    } else     {
      auto first_transformed = transform_expr(method_arguments[0], transform_context, stmts_fn);
      auto second_transformed = transform_expr_with_lambda_parameter_types(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{semantic_ir::sexpr_type(first_transformed), element_type}, transform_context, stmts_fn);
      return semantic_expression_list_singleton(first_transformed).concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{second_transformed});
    }
  } else if (((method_name == mlc::String("zip", 3)) || (method_name == mlc::String("join", 4))))   {
    if ((method_arguments.length() > 0))     {
      return semantic_expression_list_singleton(transform_expr(method_arguments[0], transform_context, stmts_fn));
    } else     {
      return empty_transformed_arguments;
    }
  } else if (((method_name == mlc::String("take", 4)) || (method_name == mlc::String("drop", 4))))   {
    if ((method_arguments.length() > 0))     {
      return semantic_expression_list_singleton(transform_expr(method_arguments[0], transform_context, stmts_fn));
    } else     {
      return empty_transformed_arguments;
    }
  } else if ((((method_name == mlc::String("enumerate", 9)) || (method_name == mlc::String("sum", 3))) || (method_name == mlc::String("flat", 4))))   {
    return empty_transformed_arguments;
  } else if ((method_arguments.length() > 0))   {
    return semantic_expression_list_singleton(transform_expr_with_lambda_parameter_types(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, transform_context, stmts_fn));
  } else   {
    return empty_transformed_arguments;
  }
}
mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return mlc::Array<std::shared_ptr<registry::Type>>{infer::infer_expr(spread_expression, inference_context_for_spread).inferred_type}; },
[&](const ast::RecordLitFields& recordLitFields) { auto [__0] = recordLitFields; return [&]() {
auto empty_types = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty_types;
}(); }
}, literal_record_part);
}
TransformPass transform_pass_new(TransformContext transform_context) noexcept{
  return TransformPass{transform_context, 0};
}
std::shared_ptr<semantic_ir::SemanticExpression> dispatch_transform_pass(TransformPass transform_pass, std::shared_ptr<ast::Expr> expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  return [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [value, source_span] = exprInt; return TransformPass_visit_int(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [value, source_span] = exprStr; return TransformPass_visit_str(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [value, source_span] = exprBool; return TransformPass_visit_bool(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [source_span] = exprUnit; return TransformPass_visit_unit(transform_pass, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprFloat>((*__match_subject))) {
const ast::ExprFloat& exprFloat = std::get<ast::ExprFloat>((*__match_subject));
auto [value, source_span] = exprFloat; return TransformPass_visit_float(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprI64>((*__match_subject))) {
const ast::ExprI64& exprI64 = std::get<ast::ExprI64>((*__match_subject));
auto [value, source_span] = exprI64; return TransformPass_visit_i64(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprU8>((*__match_subject))) {
const ast::ExprU8& exprU8 = std::get<ast::ExprU8>((*__match_subject));
auto [value, source_span] = exprU8; return TransformPass_visit_u8(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprUsize>((*__match_subject))) {
const ast::ExprUsize& exprUsize = std::get<ast::ExprUsize>((*__match_subject));
auto [value, source_span] = exprUsize; return TransformPass_visit_usize(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprChar>((*__match_subject))) {
const ast::ExprChar& exprChar = std::get<ast::ExprChar>((*__match_subject));
auto [value, source_span] = exprChar; return TransformPass_visit_char(transform_pass, value, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [extern_c_name, extern_header, concurrency_attrs, source_span] = exprExtern; return TransformPass_visit_extern(transform_pass, extern_c_name, extern_header, concurrency_attrs, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, source_span] = exprIdent; return TransformPass_visit_ident(transform_pass, name, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [operation, left, right, source_span] = exprBin; return TransformPass_visit_bin(transform_pass, operation, left, right, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [operation, inner_expression, source_span] = exprUn; return TransformPass_visit_un(transform_pass, operation, inner_expression, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [function, call_arguments, source_span] = exprCall; return TransformPass_visit_call(transform_pass, function, call_arguments, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [object, method_name, method_arguments, source_span] = exprMethod; return TransformPass_visit_method(transform_pass, object, method_name, method_arguments, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, field_name, source_span] = exprField; return TransformPass_visit_field(transform_pass, object, field_name, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, source_span] = exprIndex; return TransformPass_visit_index(transform_pass, object, index_expression, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_expression, else_expression, source_span] = exprIf; return TransformPass_visit_if(transform_pass, condition, then_expression, else_expression, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, source_span] = exprBlock; return TransformPass_visit_block(transform_pass, statements, result_expression, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprWhile>((*__match_subject))) {
const ast::ExprWhile& exprWhile = std::get<ast::ExprWhile>((*__match_subject));
auto [condition, statements, source_span] = exprWhile; return TransformPass_visit_while(transform_pass, condition, statements, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprFor>((*__match_subject))) {
const ast::ExprFor& exprFor = std::get<ast::ExprFor>((*__match_subject));
auto [variable_name, iterator, statements, source_span] = exprFor; return TransformPass_visit_for(transform_pass, variable_name, iterator, statements, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprMatch>((*__match_subject))) {
const ast::ExprMatch& exprMatch = std::get<ast::ExprMatch>((*__match_subject));
auto [subject, match_arms, source_span] = exprMatch; return TransformPass_visit_match(transform_pass, subject, match_arms, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprRecord>((*__match_subject))) {
const ast::ExprRecord& exprRecord = std::get<ast::ExprRecord>((*__match_subject));
auto [type_name, lit_parts, source_span] = exprRecord; return TransformPass_visit_record(transform_pass, type_name, lit_parts, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprRecordUpdate>((*__match_subject))) {
const ast::ExprRecordUpdate& exprRecordUpdate = std::get<ast::ExprRecordUpdate>((*__match_subject));
auto [type_name, base, field_values, source_span] = exprRecordUpdate; return TransformPass_visit_record_update(transform_pass, type_name, base, field_values, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, source_span] = exprArray; return TransformPass_visit_array(transform_pass, elements, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, source_span] = exprTuple; return TransformPass_visit_tuple(transform_pass, elements, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner_expression, source_span] = exprQuestion; return TransformPass_visit_question(transform_pass, inner_expression, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, source_span] = exprLambda; return TransformPass_visit_lambda(transform_pass, parameter_names, body, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, inner_expression, __2] = exprNamedArg; return TransformPass_visit_named_arg(transform_pass, inner_expression, stmts_fn);
}
if (std::holds_alternative<ast::ExprWith>((*__match_subject))) {
const ast::ExprWith& exprWith = std::get<ast::ExprWith>((*__match_subject));
auto [resource, binder, statements, source_span] = exprWith; return TransformPass_visit_with(transform_pass, resource, binder, statements, source_span, stmts_fn);
}
if (std::holds_alternative<ast::ExprSpawn>((*__match_subject))) {
const ast::ExprSpawn& exprSpawn = std::get<ast::ExprSpawn>((*__match_subject));
auto [statements, source_span] = exprSpawn; return TransformPass_visit_spawn(transform_pass, statements, source_span, stmts_fn);
}
return TransformPass_visit_unsupported(transform_pass, expression, stmts_fn);
std::abort();
}();
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_int(TransformPass self, int value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_str(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bool(TransformPass self, bool value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unit(TransformPass self, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_float(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_i64(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_u8(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_usize(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_char(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_extern(TransformPass self, mlc::String extern_c_name, mlc::String extern_header, mlc::Array<mlc::String> concurrency_attrs, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_ident(TransformPass self, mlc::String name, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bin(TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_un(TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_call(TransformPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_method(TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_field(TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_index(TransformPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_if(TransformPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_block(TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_while(TransformPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_spawn(TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_for(TransformPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_match(TransformPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record(TransformPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record_update(TransformPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_array(TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_tuple(TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_question(TransformPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_lambda(TransformPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_named_arg(TransformPass self, std::shared_ptr<ast::Expr> inner_expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_with(TransformPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unsupported(TransformPass self, std::shared_ptr<ast::Expr> expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_int(TransformPass self, int value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt{value, std::make_shared<registry::Type>(registry::TI32{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_str(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionStr{value, std::make_shared<registry::Type>(registry::TString{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bool(TransformPass self, bool value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool{value, std::make_shared<registry::Type>(registry::TBool{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unit(TransformPass self, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit{std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_float(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionFloat{value, std::make_shared<registry::Type>(registry::TF64{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_i64(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionI64{value, std::make_shared<registry::Type>(registry::TI64{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_u8(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionU8{value, std::make_shared<registry::Type>(registry::TU8{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_usize(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUsize{value, std::make_shared<registry::Type>(registry::TUsize{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_char(TransformPass self, mlc::String value, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionChar{value, std::make_shared<registry::Type>(registry::TChar{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_extern(TransformPass self, mlc::String extern_c_name, mlc::String extern_header, mlc::Array<mlc::String> concurrency_attrs, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionExtern{std::make_shared<registry::Type>(registry::TUnit{}), extern_c_name, extern_header, concurrency_attrs, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_ident(TransformPass self, mlc::String name, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto resolved_type = (self.transform_context.type_env.has(name) ? (self.transform_context.type_env.get(name)) : ((registry::TypeRegistry_has_fn(self.transform_context.registry, name) ? (registry::TypeRegistry_fn_type(self.transform_context.registry, name)) : ((registry::TypeRegistry_has_ctor(self.transform_context.registry, name) ? (registry::TypeRegistry_ctor_type(self.transform_context.registry, name)) : (std::make_shared<registry::Type>(registry::TUnknown{})))))));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent{name, resolved_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bin(TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_left = dispatch_transform_pass(self, left, stmts_fn);
auto typed_right = dispatch_transform_pass(self, right, stmts_fn);
auto left_type = semantic_ir::sexpr_type(typed_left);
auto method = semantic_type_structure::operator_method_for(operation);
auto result_type = ((method != mlc::String("", 0)) ? (binary_result_type_for_operator(self.transform_context.registry, left_type, operation, method)) : (semantic_type_structure::binary_operation_result_type(operation, left_type)));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin{operation, typed_left, typed_right, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_un(TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_inner = dispatch_transform_pass(self, inner_expression, stmts_fn);
auto result_type = ((operation == mlc::String("!", 1)) ? (std::make_shared<registry::Type>(registry::TBool{})) : (semantic_ir::sexpr_type(typed_inner)));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUn{operation, typed_inner, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_call(TransformPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto callee_name = call_callee_ident_name(function);
auto resolved_call_args = named_args::resolve_named_call_arguments_for_transform(call_arguments, callee_name, self.transform_context.registry);
auto typed_function = dispatch_transform_pass(self, function, stmts_fn);
auto typed_arguments = transform_call_arguments_using_callee_semantic_type(typed_function, resolved_call_args, self.transform_context, stmts_fn);
auto result_type = (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_ctor(self.transform_context.registry, callee_name)) ? (registry::TypeRegistry_ctor_type(self.transform_context.registry, callee_name)) : (function_return_type_from_callee_type(semantic_ir::sexpr_type(typed_function))));
auto call_parameter_mutability_flags = direct_call_parameter_mutability_flags(self.transform_context.registry, callee_name, typed_arguments.length());
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall{typed_function, typed_arguments, call_parameter_mutability_flags, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_method(TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
if (infer_weak_method::is_shared_static_new(object, method_name)) {
  return transform_shared_static_new_call(method_arguments, self.transform_context, source_span, stmts_fn);
} else {
  return transform_method_call_after_object(self.transform_context.registry, object, method_name, method_arguments, self, source_span, stmts_fn);
}
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_field(TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_object = dispatch_transform_pass(self, object, stmts_fn);
auto result_type = registry::field_type_from_object(semantic_ir::sexpr_type(typed_object), field_name, self.transform_context.registry);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField{typed_object, field_name, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_index(TransformPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_object = dispatch_transform_pass(self, object, stmts_fn);
auto typed_index = dispatch_transform_pass(self, index_expression, stmts_fn);
auto element_type = array_element_type_from_semantic_expression(typed_object);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIndex{typed_object, typed_index, element_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_if(TransformPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_condition = dispatch_transform_pass(self, condition, stmts_fn);
auto typed_then = dispatch_transform_pass(self, then_expression, stmts_fn);
auto typed_else_raw = dispatch_transform_pass(self, else_expression, stmts_fn);
auto typed_else = conditional_else_empty_unknown_array_coerced_to_then_array_element(typed_then, typed_else_raw);
auto merged_conditional_type = merge_conditional_expression_types(typed_then, typed_else);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf{typed_condition, typed_then, typed_else, merged_conditional_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_block(TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto statements_parsed = stmts_fn(statements, self.transform_context);
auto body_context = transform_context_with_env(self.transform_context, statements_parsed.type_env);
auto body_pass = transform_pass_new(body_context);
auto result_parsed = dispatch_transform_pass(body_pass, result_expression, stmts_fn);
auto result_type = semantic_ir::sexpr_type(result_parsed);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock{statements_parsed.statements, result_parsed, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_while(TransformPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_condition = dispatch_transform_pass(self, condition, stmts_fn);
auto statements_parsed = stmts_fn(statements, self.transform_context);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWhile{typed_condition, statements_parsed.statements, std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_spawn(TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto statements_parsed = stmts_fn(statements, self.transform_context);
auto result_parsed = dispatch_transform_pass(self, ast::expr_spawn_body_result(statements), stmts_fn);
auto result_type = semantic_ir::sexpr_type(result_parsed);
auto block = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock{statements_parsed.statements, result_parsed, result_type, source_span});
auto lambda_type = std::make_shared<registry::Type>(registry::TFn{{}, result_type});
auto lambda = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda{{}, block, lambda_type, source_span});
auto spawn_callee = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent{mlc::String("spawn_task", 10), std::make_shared<registry::Type>(registry::TUnknown{}), source_span});
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall{spawn_callee, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{lambda}, mlc::Array<int>{0}, std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Task", 4), mlc::Array<std::shared_ptr<registry::Type>>{result_type}}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_for(TransformPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_iterator = dispatch_transform_pass(self, iterator, stmts_fn);
auto element_type = array_element_type_from_semantic_expression(typed_iterator);
auto loop_body_environment = self.transform_context.type_env;
loop_body_environment.set(variable_name, element_type);
auto loop_context = transform_context_with_env(self.transform_context, loop_body_environment);
auto statements_parsed = stmts_fn(statements, loop_context);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionFor{variable_name, typed_iterator, statements_parsed.statements, std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_match(TransformPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_subject = dispatch_transform_pass(self, subject, stmts_fn);
auto substitution = substitution::substitution_from_generic(semantic_ir::sexpr_type(typed_subject), self.transform_context.registry);
auto typed_arms = transform_match_arms(match_arms, self.transform_context, substitution, semantic_ir::sexpr_type(typed_subject), stmts_fn);
auto result_type = ((typed_arms.length() > 0) ? (semantic_ir::sexpr_type(typed_arms[0]->body)) : (std::make_shared<registry::Type>(registry::TUnknown{})));
auto coerced_arms = coerce_match_arms_to_type(typed_arms, result_type);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch{typed_subject, coerced_arms, result_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record(TransformPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto merged_field_values_for_transform = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
if ((!record_lit_merge::record_literal_contains_spread(lit_parts))) {
  (merged_field_values_for_transform = record_lit_merge::collect_explicit_field_values_flat(lit_parts));
} else {
  auto inference_context_for_literal_spreads = check_context::check_context_new(self.transform_context.type_env, self.transform_context.registry);
  auto spread_types_for_merge = lit_parts.flat_map([=](ast::RecordLitPart part_under_merge) mutable { return inferred_types_from_record_literal_part_for_merge(part_under_merge, inference_context_for_literal_spreads); });
  (merged_field_values_for_transform = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, self.transform_context.registry, spread_types_for_merge));
}
(merged_field_values_for_transform = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values_for_transform, self.transform_context.registry));
auto typed_fields = transform_field_values(merged_field_values_for_transform, self.transform_context, stmts_fn);
auto phantom = registry::TypeRegistry_phantom_type_params_for(self.transform_context.registry, type_name);
auto all_params = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(self.transform_context.registry, type_name);
auto expr_type = (((all_params.length() > 0) && (phantom.length() == all_params.length())) ? (std::make_shared<registry::Type>(registry::TGeneric{type_name, all_params.map([=](mlc::String _algebraic_type_parameter_placeholder) mutable { return standalone_unknown_cell(); })})) : (std::make_shared<registry::Type>(registry::TNamed{type_name})));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecord{type_name, typed_fields, expr_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record_update(TransformPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_base = dispatch_transform_pass(self, base, stmts_fn);
auto typed_fields = transform_field_values(field_values, self.transform_context, stmts_fn);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecordUpdate{type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed{type_name}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_array(TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_elements = transform_exprs(elements, self.transform_context, stmts_fn);
auto element_type = ((typed_elements.length() > 0) ? (semantic_ir::sexpr_type(typed_elements[0])) : (std::make_shared<registry::Type>(registry::TUnknown{})));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray{typed_elements, std::make_shared<registry::Type>(registry::TArray{element_type}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_tuple(TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
if ((elements.length() < 2)) {
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit{std::make_shared<registry::Type>(registry::TUnit{}), source_span});
} else {
  auto typed = transform_exprs(elements, self.transform_context, stmts_fn);
  auto tuple_types = typed.map([=](std::shared_ptr<semantic_ir::SemanticExpression> typed_tuple_member) mutable { return semantic_ir::sexpr_type(typed_tuple_member); });
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionTuple{typed, std::make_shared<registry::Type>(registry::TTuple{tuple_types}), source_span});
}
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_question(TransformPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_inner = dispatch_transform_pass(self, inner_expression, stmts_fn);
auto inner_expression_type = semantic_ir::sexpr_type(typed_inner);
auto unwrapped_type = question_unwrapped_type_from_inner(inner_expression_type);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion{typed_inner, unwrapped_type, source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_lambda(TransformPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
if ((self.transform_context.lambda_parameter_types.length() > 0)) {
  return transform_expr_lambda_with_param_types(parameter_names, self.transform_context.lambda_parameter_types, body, source_span, transform_context_with_lambda_parameter_types(self.transform_context, mlc::Array<std::shared_ptr<registry::Type>>{}), stmts_fn);
} else {
  auto lambda_environment = self.transform_context.type_env;
  auto parameter_types = parameter_names.map([=](mlc::String parameter_binding_placeholder) mutable { return standalone_unknown_cell(); });
  parameter_names.fold(lambda_environment, [=](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
  auto lambda_context = transform_context_with_env(self.transform_context, lambda_environment);
  auto lambda_pass = transform_pass_new(lambda_context);
  auto typed_body = dispatch_transform_pass(lambda_pass, body, stmts_fn);
  auto function_type = std::make_shared<registry::Type>(registry::TFn{parameter_types, semantic_ir::sexpr_type(typed_body)});
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda{parameter_names, typed_body, function_type, source_span});
}
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_named_arg(TransformPass self, std::shared_ptr<ast::Expr> inner_expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return dispatch_transform_pass(self, inner_expression, stmts_fn);
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_with(TransformPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto typed_resource = dispatch_transform_pass(self, resource, stmts_fn);
auto with_body_environment = self.transform_context.type_env;
with_body_environment.set(binder, semantic_ir::sexpr_type(typed_resource));
auto with_body_context = transform_context_with_env(self.transform_context, with_body_environment);
auto statements_parsed = stmts_fn(statements, with_body_context);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWith{typed_resource, binder, statements_parsed.statements, std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}
std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unsupported(TransformPass self, std::shared_ptr<ast::Expr> expression, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit{std::make_shared<registry::Type>(registry::TUnit{}), ast::expr_span(expression)});
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_method_call_after_object(registry::TypeRegistry registry, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, TransformPass transform_pass, ast::Span source_span, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto typed_object = dispatch_transform_pass(transform_pass, object, stmts_fn);
  auto receiver_type = semantic_ir::sexpr_type(typed_object);
  if (hof_method_spec::is_result_option_hof_method(receiver_type, method_name))   {
    return transform_result_option_hof_method_call(registry, typed_object, method_name, method_arguments, receiver_type, transform_pass.transform_context, source_span, stmts_fn);
  } else if (hof_method_spec::is_array_hof_method_on_receiver(receiver_type, method_name))   {
    return transform_array_hof_method_call(registry, typed_object, method_name, method_arguments, receiver_type, transform_pass.transform_context, source_span, stmts_fn);
  } else   {
    return transform_regular_method_call(registry, typed_object, method_name, method_arguments, receiver_type, transform_pass.transform_context, source_span, stmts_fn);
  }
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_expr(std::shared_ptr<ast::Expr> expression, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
  auto expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
  return dispatch_transform_pass(transform_pass_new(transform_context), expression_partial_application, stmts_fn);
}

} // namespace transform
