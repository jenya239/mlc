#define main mlc_user_main
#include "semantic_ir.hpp"

namespace semantic_ir {

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<SExpr> expression) noexcept{
return std::visit(overloaded{[&](const SExprInt& sExprInt) { auto [__0, t, __2] = sExprInt; return t; },
[&](const SExprStr& sExprStr) { auto [__0, t, __2] = sExprStr; return t; },
[&](const SExprBool& sExprBool) { auto [__0, t, __2] = sExprBool; return t; },
[&](const SExprUnit& sExprUnit) { auto [t, __1] = sExprUnit; return t; },
[&](const SExprExtern& sExprExtern) { auto [t, __1] = sExprExtern; return t; },
[&](const SExprIdent& sExprIdent) { auto [__0, t, __2] = sExprIdent; return t; },
[&](const SExprBin& sExprBin) { auto [__0, __1, __2, t, __4] = sExprBin; return t; },
[&](const SExprUn& sExprUn) { auto [__0, __1, t, __3] = sExprUn; return t; },
[&](const SExprCall& sExprCall) { auto [__0, __1, t, __3] = sExprCall; return t; },
[&](const SExprMethod& sExprMethod) { auto [__0, __1, __2, t, __4] = sExprMethod; return t; },
[&](const SExprField& sExprField) { auto [__0, __1, t, __3] = sExprField; return t; },
[&](const SExprIndex& sExprIndex) { auto [__0, __1, t, __3] = sExprIndex; return t; },
[&](const SExprIf& sExprIf) { auto [__0, __1, __2, t, __4] = sExprIf; return t; },
[&](const SExprBlock& sExprBlock) { auto [__0, __1, t, __3] = sExprBlock; return t; },
[&](const SExprWhile& sExprWhile) { auto [__0, __1, t, __3] = sExprWhile; return t; },
[&](const SExprFor& sExprFor) { auto [__0, __1, __2, t, __4] = sExprFor; return t; },
[&](const SExprMatch& sExprMatch) { auto [__0, __1, t, __3] = sExprMatch; return t; },
[&](const SExprRecord& sExprRecord) { auto [__0, __1, t, __3] = sExprRecord; return t; },
[&](const SExprRecordUpdate& sExprRecordUpdate) { auto [__0, __1, __2, t, __4] = sExprRecordUpdate; return t; },
[&](const SExprArray& sExprArray) { auto [__0, t, __2] = sExprArray; return t; },
[&](const SExprTuple& sExprTuple) { auto [__0, t, __2] = sExprTuple; return t; },
[&](const SExprQuestion& sExprQuestion) { auto [__0, t, __2] = sExprQuestion; return t; },
[&](const SExprLambda& sExprLambda) { auto [__0, __1, t, __3] = sExprLambda; return t; }
}, (*expression));
}
ast::Span sexpr_span(std::shared_ptr<SExpr> expression) noexcept{
return std::visit(overloaded{[&](const SExprInt& sExprInt) { auto [__0, __1, s] = sExprInt; return s; },
[&](const SExprStr& sExprStr) { auto [__0, __1, s] = sExprStr; return s; },
[&](const SExprBool& sExprBool) { auto [__0, __1, s] = sExprBool; return s; },
[&](const SExprUnit& sExprUnit) { auto [__0, s] = sExprUnit; return s; },
[&](const SExprExtern& sExprExtern) { auto [__0, s] = sExprExtern; return s; },
[&](const SExprIdent& sExprIdent) { auto [__0, __1, s] = sExprIdent; return s; },
[&](const SExprBin& sExprBin) { auto [__0, __1, __2, __3, s] = sExprBin; return s; },
[&](const SExprUn& sExprUn) { auto [__0, __1, __2, s] = sExprUn; return s; },
[&](const SExprCall& sExprCall) { auto [__0, __1, __2, s] = sExprCall; return s; },
[&](const SExprMethod& sExprMethod) { auto [__0, __1, __2, __3, s] = sExprMethod; return s; },
[&](const SExprField& sExprField) { auto [__0, __1, __2, s] = sExprField; return s; },
[&](const SExprIndex& sExprIndex) { auto [__0, __1, __2, s] = sExprIndex; return s; },
[&](const SExprIf& sExprIf) { auto [__0, __1, __2, __3, s] = sExprIf; return s; },
[&](const SExprBlock& sExprBlock) { auto [__0, __1, __2, s] = sExprBlock; return s; },
[&](const SExprWhile& sExprWhile) { auto [__0, __1, __2, s] = sExprWhile; return s; },
[&](const SExprFor& sExprFor) { auto [__0, __1, __2, __3, s] = sExprFor; return s; },
[&](const SExprMatch& sExprMatch) { auto [__0, __1, __2, s] = sExprMatch; return s; },
[&](const SExprRecord& sExprRecord) { auto [__0, __1, __2, s] = sExprRecord; return s; },
[&](const SExprRecordUpdate& sExprRecordUpdate) { auto [__0, __1, __2, __3, s] = sExprRecordUpdate; return s; },
[&](const SExprArray& sExprArray) { auto [__0, __1, s] = sExprArray; return s; },
[&](const SExprTuple& sExprTuple) { auto [__0, __1, s] = sExprTuple; return s; },
[&](const SExprQuestion& sExprQuestion) { auto [__0, __1, s] = sExprQuestion; return s; },
[&](const SExprLambda& sExprLambda) { auto [__0, __1, __2, s] = sExprLambda; return s; }
}, (*expression));
}
std::shared_ptr<SDecl> sdecl_inner(std::shared_ptr<SDecl> declaration) noexcept{
return std::visit(overloaded{[&](const SDeclExported& sDeclExported) { auto [inner] = sDeclExported; return inner; },
[&](const auto& __v) { return declaration; }
}, (*declaration));
}

} // namespace semantic_ir
