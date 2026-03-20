#define main mlc_user_main
#include "types.hpp"

namespace types {

preds::TypeResult parse_type(preds::Parser parser) noexcept{
auto base = parse_base_type(parser);
return (preds::Parser_kind(base.parser).is_arrow() ? [&]() {
auto ret = parse_type(base.parser.advance());
auto params = mlc::Array{base.type_expr};
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn(params, ret.type_expr)), ret.parser};
}() : base);
}
preds::TypeResult parse_base_type(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
return (kind.is_lbracket() ? [&]() {
auto inner = parse_type(parser.advance());
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyArray(inner.type_expr)), inner.parser.advance()};
}() : (kind.is_ident() ? [&]() {
auto name = kind.ident();
return ((name == mlc::String("i32", 3)) ? preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyI32{}), parser.advance()} : ((name == mlc::String("string", 6)) ? preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyString{}), parser.advance()} : ((name == mlc::String("bool", 4)) ? preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyBool{}), parser.advance()} : ((name == mlc::String("unit", 4)) ? preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), parser.advance()} : ((name == mlc::String("ref", 3)) ? [&]() {
auto after_ref = (preds::Parser_kind(parser.advance()).is_mut() ? parser.advance_by(2) : parser.advance());
auto inner = parse_type(after_ref);
auto targs = mlc::Array{inner.type_expr};
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyGeneric(mlc::String("ref", 3), targs)), inner.parser};
}() : [&]() {
auto after = parser.advance();
return ((preds::Parser_kind(after).is_op() && (preds::Parser_kind(after).op_val() == mlc::String("<", 1))) ? [&]() {
auto targs = parse_type_args(after.advance());
auto type_expr = (((name == mlc::String("Shared", 6)) && (targs.types.length() == 1)) ? std::make_shared<ast::TypeExpr>(ast::TyShared(targs.types[0])) : std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, targs.types)));
return preds::TypeResult{type_expr, targs.parser};
}() : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyNamed(name)), after});
}())))));
}() : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), parser.advance_by(2)}));
}
preds::TypesResult parse_type_args(preds::Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto first = parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (preds::Parser_kind(state).is_comma()) {
auto next = parse_type(state.advance());
types.push_back(next.type_expr);
state = next.parser;
}
auto end_state = ((preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String(">", 1))) ? state.advance() : state);
return preds::TypesResult{types, end_state};
}

} // namespace types
