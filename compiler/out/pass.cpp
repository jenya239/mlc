#include "pass.hpp"

#include "ast.hpp"

namespace pass {

using namespace ast;
using namespace ast_tokens;

struct CompilerPass {
  std::function<Result<Output, mlc::Array<Diagnostic>>(Input)> run;
};

struct ExprVisitor {
  std::function<VisitorResult(int, Shared<Type>)> visit_int;
  std::function<VisitorResult(mlc::String, Shared<Type>)> visit_str;
  std::function<VisitorResult(mlc::String, Shared<Type>)> visit_ident;
  std::function<VisitorResult(Shared<SExpr>)> visit_unsupported;
};

inline ExprVisitor UnitVisitor_as_ExprVisitor(test_expr_visitor::UnitVisitor self) noexcept {
  return ExprVisitor{
  .visit_int = [](UnitVisitor _p0, int _p1, Shared<Type> _p2) noexcept { UnitVisitor_visit_int(_p0, _p1, _p2); },
  .visit_str = [](UnitVisitor _p0, mlc::String _p1, Shared<Type> _p2) noexcept { UnitVisitor_visit_str(_p0, _p1, _p2); },
  .visit_ident = [](UnitVisitor _p0, mlc::String _p1, Shared<Type> _p2) noexcept { UnitVisitor_visit_ident(_p0, _p1, _p2); },
  .visit_unsupported = [](UnitVisitor _p0, Shared<SExpr> _p1) noexcept { UnitVisitor_visit_unsupported(_p0, _p1); }
  };
}

} // namespace pass
