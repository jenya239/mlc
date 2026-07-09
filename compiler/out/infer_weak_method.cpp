#define main mlc_user_main
#include "infer_weak_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "type_diagnostics.hpp"

namespace infer_weak_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace type_diagnostics;
using namespace ast_tokens;

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return ((name == mlc::String("Weak", 4)) && (type_arguments.length() == 1));
}
return false;
std::abort();
}();
}
std::shared_ptr<registry::Type> weak_pointer_inner_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, type_arguments] = tGeneric; return ((type_arguments.length() == 1) ? (type_arguments[0]) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
bool is_shared_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept{
  return [&]() -> bool {
auto __match_subject = object;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return ((name == mlc::String("Shared", 6)) && (method_name == mlc::String("new", 3)));
}
return false;
std::abort();
}();
}
bool is_shared_weak_method(mlc::String method_name) noexcept{
  return ((method_name == mlc::String("weak", 4)) || (method_name == mlc::String("downgrade", 9)));
}
bool is_weak_upgrade_method(mlc::String method_name) noexcept{
  return ((method_name == mlc::String("upgrade", 7)) || (method_name == mlc::String("lock", 4)));
}
bool is_shared_weak_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
  return (semantic_type_structure::type_is_shared_pointer(receiver_type) && is_shared_weak_method(method_name));
}
bool is_weak_upgrade_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
  return (type_is_weak_pointer(receiver_type) && is_weak_upgrade_method(method_name));
}

} // namespace infer_weak_method
