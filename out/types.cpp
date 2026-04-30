#define main mlc_user_main
#include "types.hpp"

namespace types {

preds::TypeResult parse_type(preds::Parser parser) noexcept{
auto base = parse_base_type(parser);
if (preds::TKind_is_arrow(preds::Parser_kind(base.parser))) {
auto ret = parse_type(preds::Parser_advance(base.parser));
auto params = mlc::Array{base.type_expr};
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn{params, ret.type_expr}), ret.parser};
} else {
return base;
}
}
preds::TypesResult parse_paren_types(preds::Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
if (preds::TKind_is_rparen(preds::Parser_kind(parser))) {
return preds::TypesResult{types, preds::Parser_advance(parser)};
} else {
auto first = parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
return preds::TypesResult{types, (preds::TKind_is_rparen(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state)};
}
}
preds::TypeResult parse_base_type(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_lparen(kind)) {
auto paren_types = parse_paren_types(preds::Parser_advance(parser));
if (preds::TKind_is_arrow(preds::Parser_kind(paren_types.parser))) {
auto ret = parse_type(preds::Parser_advance(paren_types.parser));
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn{paren_types.types, ret.type_expr}), ret.parser};
} else {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), paren_types.parser};
}
} else if (preds::TKind_is_lbracket(kind)) {
auto inner = parse_type(preds::Parser_advance(parser));
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyArray{inner.type_expr}), preds::Parser_advance(inner.parser)};
} else if (preds::TKind_is_ident(kind)) {
auto name = preds::TKind_ident(kind);
if ((name == mlc::String("i32", 3))) {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyI32{}), preds::Parser_advance(parser)};
} else if ((name == mlc::String("string", 6))) {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyString{}), preds::Parser_advance(parser)};
} else if ((name == mlc::String("bool", 4))) {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyBool{}), preds::Parser_advance(parser)};
} else if ((name == mlc::String("unit", 4))) {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(parser)};
} else if ((name == mlc::String("ref", 3))) {
auto after_ref = (preds::TKind_is_mut(preds::Parser_kind(preds::Parser_advance(parser))) ? preds::Parser_advance_by(parser, 2) : preds::Parser_advance(parser));
auto inner = parse_type(after_ref);
auto targs = mlc::Array{inner.type_expr};
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyGeneric{mlc::String("ref", 3), targs}), inner.parser};
} else {
auto after = preds::Parser_advance(parser);
if ((preds::TKind_is_op(preds::Parser_kind(after)) && (preds::TKind_op_val(preds::Parser_kind(after)) == mlc::String("<", 1)))) {
auto targs = parse_type_args(preds::Parser_advance(after));
auto type_expr = (((name == mlc::String("Shared", 6)) && (targs.types.length() == 1)) ? std::make_shared<ast::TypeExpr>(ast::TyShared{targs.types[0]}) : std::make_shared<ast::TypeExpr>(ast::TyGeneric{name, targs.types}));
return preds::TypeResult{type_expr, targs.parser};
} else if ((preds::TKind_is_dot(preds::Parser_kind(after)) && preds::TKind_is_ident(preds::Parser_kind(preds::Parser_advance(after))))) {
auto assoc_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(after)));
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyAssoc{name, assoc_name}), preds::Parser_advance_by(after, 2)};
} else {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyNamed{name}), after};
}

}




} else {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance_by(parser, 2)};
}


}
preds::TypesResult parse_type_args(preds::Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto first = parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
auto end_state = ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1))) ? preds::Parser_advance(state) : state);
return preds::TypesResult{types, end_state};
}

} // namespace types
