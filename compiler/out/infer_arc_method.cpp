#define main mlc_user_main
#include "infer_arc_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "send_safe.hpp"
#include "type_diagnostics.hpp"
#include "diagnostic_codes.hpp"

namespace infer_arc_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace send_safe;
using namespace type_diagnostics;
using namespace diagnostic_codes;
using namespace ast_tokens;

bool is_arc_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept{
  return [&]() -> bool {
auto __match_subject = object;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return ((name == mlc::String("Arc", 3)) && (method_name == mlc::String("new", 3)));
}
return false;
std::abort();
}();
}

} // namespace infer_arc_method
