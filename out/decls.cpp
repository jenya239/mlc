#define main mlc_user_main
#include "decls.hpp"

namespace decls {

preds::BoundsResult parse_trait_bound_ident_list(preds::Parser parser_state) noexcept{
auto state = parser_state;
auto bounds = mlc::Array<mlc::String>{};
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
bounds.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
[&]() {
while ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("+", 1)))) {
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
bounds.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
}
}();
}
return preds::BoundsResult{bounds, state};
}
preds::BoundsResult parse_one_param_bounds(preds::Parser state) noexcept{
if (preds::TKind_is_colon(preds::Parser_kind(state))) {
return parse_trait_bound_ident_list(preds::Parser_advance(state));
} else {
return preds::BoundsResult{{}, state};
}
}
preds::WhereClauseParseResult parse_where_clause_opt(preds::Parser parser_state) noexcept{
if ((!preds::TKind_is_where(preds::Parser_kind(parser_state)))) {
return preds::WhereClauseParseResult{{}, parser_state};
} else {
auto state = preds::Parser_advance(parser_state);
auto collected = mlc::Array<ast::WhereClauseBound>{};
while (preds::TKind_is_ident(preds::Parser_kind(state))) {
auto parameter_name = preds::TKind_ident(preds::Parser_kind(state));
state = preds::Parser_advance(state);
if (preds::TKind_is_colon(preds::Parser_kind(state))) {
auto trait_result = parse_trait_bound_ident_list(preds::Parser_advance(state));
state = trait_result.parser;
collected.push_back(ast::WhereClauseBound{parameter_name, trait_result.bounds});
}
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::WhereClauseParseResult{collected, state};
}
}
bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
auto index = 0;
while ((index < haystack.length())) {
if ((haystack[index] == needle)) {
return true;
}
index = (index + 1);
}
return false;
}
mlc::Array<mlc::String> clone_string_row(mlc::Array<mlc::String> source) noexcept{
auto copy = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < source.length())) {
copy.push_back(source[index]);
index = (index + 1);
}
return copy;
}
mlc::Array<mlc::Array<mlc::String>> clone_trait_bounds_matrix(mlc::Array<mlc::Array<mlc::String>> source) noexcept{
auto matrix = mlc::Array<mlc::Array<mlc::String>>{};
auto row_index = 0;
while ((row_index < source.length())) {
matrix.push_back(clone_string_row(source[row_index]));
row_index = (row_index + 1);
}
return matrix;
}
mlc::Array<mlc::String> union_trait_string_rows(mlc::Array<mlc::String> existing_row, mlc::Array<mlc::String> incoming_traits) noexcept{
auto combined = clone_string_row(existing_row);
auto incoming_index = 0;
while ((incoming_index < incoming_traits.length())) {
auto trait_name = incoming_traits[incoming_index];
if ((!string_list_contains(combined, trait_name))) {
combined.push_back(trait_name);
}
incoming_index = (incoming_index + 1);
}
return combined;
}
int type_parameter_row_index(mlc::Array<mlc::String> type_parameter_names, mlc::String parameter_name) noexcept{
auto index = 0;
while ((index < type_parameter_names.length())) {
if ((type_parameter_names[index] == parameter_name)) {
return index;
}
index = (index + 1);
}
return (-1);
}
mlc::Array<mlc::Array<mlc::String>> replace_trait_bounds_row_at(mlc::Array<mlc::Array<mlc::String>> matrix, int row_index, mlc::Array<mlc::String> new_row) noexcept{
auto output_matrix = mlc::Array<mlc::Array<mlc::String>>{};
auto matrix_row_index = 0;
while ((matrix_row_index < matrix.length())) {
if ((matrix_row_index == row_index)) {
output_matrix.push_back(clone_string_row(new_row));
} else {
output_matrix.push_back(clone_string_row(matrix[matrix_row_index]));
}
matrix_row_index = (matrix_row_index + 1);
}
return output_matrix;
}
mlc::Array<mlc::Array<mlc::String>> merge_where_clause_trait_bounds(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<ast::WhereClauseBound> where_entries) noexcept{
auto merged = clone_trait_bounds_matrix(trait_bounds_rows);
while ((merged.length() < type_parameter_names.length())) {
auto empty_row = mlc::Array<mlc::String>{};
merged.push_back(empty_row);
}
auto entry_index = 0;
while ((entry_index < where_entries.length())) {
auto entry = where_entries[entry_index];
auto row_index = type_parameter_row_index(type_parameter_names, entry.parameter_name);
if ((row_index >= 0)) {
auto updated_row = union_trait_string_rows(merged[row_index], entry.traits);
merged = replace_trait_bounds_row_at(merged, row_index, updated_row);
}
entry_index = (entry_index + 1);
}
return merged;
}
preds::TypeParamsResult parse_type_params_opt(preds::Parser parser) noexcept{
if ((preds::TKind_is_op(preds::Parser_kind(parser)) && (preds::TKind_op_val(preds::Parser_kind(parser)) == mlc::String("<", 1)))) {
auto state = preds::Parser_advance(parser);
auto params = mlc::Array<mlc::String>{};
auto bounds_list = mlc::Array<mlc::Array<mlc::String>>{};
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
params.push_back(preds::TKind_ident(preds::Parser_kind(state)));
auto res = parse_one_param_bounds(preds::Parser_advance(state));
bounds_list.push_back(res.bounds);
state = res.parser;
[&]() {
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
params.push_back(preds::TKind_ident(preds::Parser_kind(state)));
auto res2 = parse_one_param_bounds(preds::Parser_advance(state));
bounds_list.push_back(res2.bounds);
state = res2.parser;
}
}
}();
}
state = [&]() -> preds::Parser {
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1)))) {
return preds::Parser_advance(state);
} else {
return state;
}
}();
return preds::TypeParamsResult{params, bounds_list, state};
} else {
return preds::TypeParamsResult{{}, {}, parser};
}
}
preds::DeclResult parse_import_decl(preds::Parser parser) noexcept{
auto state = parser;
auto symbols = mlc::Array<mlc::String>{};
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*", 1)))) {
symbols.push_back(mlc::String("*", 1));
state = preds::Parser_advance(state);
if (preds::TKind_is_as(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
symbols.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
}
} else if (preds::TKind_is_lbrace(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
while ((preds::TKind_is_ident(preds::Parser_kind(state)) || (preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*", 1))))) {
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*", 1)))) {
symbols.push_back(mlc::String("*", 1));
state = preds::Parser_advance(state);
} else {
symbols.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
state = [&]() -> preds::Parser {
if (preds::TKind_is_rbrace(preds::Parser_kind(state))) {
return preds::Parser_advance(state);
} else {
return state;
}
}();
}

state = [&]() -> preds::Parser {
if (preds::TKind_is_from(preds::Parser_kind(state))) {
return preds::Parser_advance(state);
} else {
return state;
}
}();
auto path = (preds::TKind_is_str(preds::Parser_kind(state)) ? (preds::TKind_str_val(preds::Parser_kind(state))) : (mlc::String("", 0)));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclImport{path, symbols}), preds::Parser_advance(state)};
}
preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept{
auto type_name = preds::TKind_ident(preds::Parser_kind(parser));
auto state = preds::Parser_advance(parser);
auto trait_name = mlc::String("", 0);
if (preds::TKind_is_colon(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
trait_name = preds::TKind_ident(preds::Parser_kind(state));
state = preds::Parser_advance(state);
}
}
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("<", 1)))) {
state = preds::Parser_advance(state);
while (((!preds::Parser_at_eof(state)) && (!(preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1)))))) {
state = preds::Parser_advance(state);
}
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1)))) {
state = preds::Parser_advance(state);
}
}
auto methods_state = [&]() -> preds::Parser {
if (preds::TKind_is_lbrace(preds::Parser_kind(state))) {
return preds::Parser_advance(state);
} else {
return state;
}
}();
auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
while (((!preds::TKind_is_rbrace(preds::Parser_kind(methods_state))) && (!preds::Parser_at_eof(methods_state)))) {
if (preds::TKind_is_fn(preds::Parser_kind(methods_state))) {
auto method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else if ((preds::TKind_is_extern(preds::Parser_kind(methods_state)) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(methods_state))))) {
auto method_result = parse_extend_extern_method(preds::Parser_advance(methods_state), type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else if (preds::TKind_is_type(preds::Parser_kind(methods_state))) {
auto after_type = preds::Parser_advance(methods_state);
auto assoc_span = preds::Parser_span_at_cursor(after_type);
auto assoc_name = preds::TKind_ident(preds::Parser_kind(after_type));
auto after_name = preds::Parser_advance(after_type);
if (preds::TKind_is_equal(preds::Parser_kind(after_name))) {
auto type_result = types::parse_type(preds::Parser_advance(after_name));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocBind{assoc_name, type_result.type_expr, assoc_span}));
methods_state = type_result.parser;
} else {
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType{assoc_name, assoc_span}));
methods_state = after_name;
}
} else {
methods_state = preds::Parser_advance(methods_state);
}


}
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExtend{type_name, trait_name, methods}), preds::Parser_advance(methods_state)};
}
preds::Parser advance_past_optional_self_type(preds::Parser parser_after_self) noexcept{
if (preds::TKind_is_colon(preds::Parser_kind(parser_after_self))) {
return types::parse_type(preds::Parser_advance(parser_after_self)).parser;
} else {
return parser_after_self;
}
}
preds::ParamsResult parse_extend_extern_no_self_params(preds::Parser after_lparen) noexcept{
if (preds::TKind_is_rparen(preds::Parser_kind(after_lparen))) {
return preds::ParamsResult{{}, preds::Parser_advance(after_lparen)};
} else {
return parse_params(after_lparen);
}
}
preds::ParamsResult parse_extend_extern_rest_params(preds::Parser after_lparen, bool leading_self) noexcept{
if (leading_self) {
auto after_self_type = advance_past_optional_self_type(preds::Parser_advance(after_lparen));
if (preds::TKind_is_rparen(preds::Parser_kind(after_self_type))) {
return preds::ParamsResult{{}, preds::Parser_advance(after_self_type)};
} else {
return parse_params([&]() -> preds::Parser {
if (preds::TKind_is_comma(preds::Parser_kind(after_self_type))) {
return preds::Parser_advance(after_self_type);
} else {
return after_self_type;
}
}());
}
} else {
return parse_extend_extern_no_self_params(after_lparen);
}
}
preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept{
auto extern_span = preds::Parser_span_at_cursor(parser);
auto fn_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(parser)));
auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
auto after_lparen = preds::Parser_advance_by(parser, 3);
auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed{type_name}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), std::make_shared<ast::Pat>(ast::PatUnit{ast::span_unknown()})});
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
auto leading_self = (preds::TKind_is_ident(preds::Parser_kind(after_lparen)) && (preds::TKind_ident(preds::Parser_kind(after_lparen)) == mlc::String("self", 4)));
if (leading_self) {
params.push_back(self_param);
}
auto rest_params = parse_extend_extern_rest_params(after_lparen, leading_self);
auto index = 0;
while ((index < rest_params.params.length())) {
auto rp = rest_params.params[index];
if ((ast::param_name(rp) != mlc::String("self", 4))) {
params.push_back(rp);
}
index = (index + 1);
}
auto after_rparen = preds::Parser_advance(rest_params.parser);
auto type_parser = [&]() -> preds::Parser {
if (preds::TKind_is_arrow(preds::Parser_kind(after_rparen))) {
return preds::Parser_advance(after_rparen);
} else {
return after_rparen;
}
}();
auto ret_type_result = types::parse_type(type_parser);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn{mangled_name, {}, {}, params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{extern_span}), {}}), ret_type_result.parser};
}
preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept{
auto fn_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(parser)));
auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
auto after_lparen = preds::Parser_advance_by(parser, 3);
auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed{type_name}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), std::make_shared<ast::Pat>(ast::PatUnit{ast::span_unknown()})});
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
params.push_back(self_param);
auto rest_start = [&]() -> preds::Parser {
if ((preds::TKind_is_ident(preds::Parser_kind(after_lparen)) && (preds::TKind_ident(preds::Parser_kind(after_lparen)) == mlc::String("self", 4)))) {
return advance_past_optional_self_type(preds::Parser_advance(after_lparen));
} else {
return after_lparen;
}
}();
auto rest_params = [&]() -> preds::ParamsResult {
if (preds::TKind_is_rparen(preds::Parser_kind(rest_start))) {
return preds::ParamsResult{{}, preds::Parser_advance(rest_start)};
} else {
return parse_params([&]() -> preds::Parser {
if (preds::TKind_is_comma(preds::Parser_kind(rest_start))) {
return preds::Parser_advance(rest_start);
} else {
return rest_start;
}
}());
}
}();
auto index = 0;
while ((index < rest_params.params.length())) {
auto rp = rest_params.params[index];
if ((ast::param_name(rp) != mlc::String("self", 4))) {
params.push_back(rp);
}
index = (index + 1);
}
auto after_rparen = preds::Parser_advance(rest_params.parser);
auto type_parser = [&]() -> preds::Parser {
if (preds::TKind_is_arrow(preds::Parser_kind(after_rparen))) {
return preds::Parser_advance(after_rparen);
} else {
return after_rparen;
}
}();
auto ret_type_result = types::parse_type(type_parser);
auto after_eq = preds::Parser_advance(ret_type_result.parser);
auto body_result = [&]() -> preds::ExprResult {
if (preds::TKind_is_extern(preds::Parser_kind(after_eq))) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern{preds::Parser_span_at_cursor(after_eq)}), preds::Parser_advance(after_eq)};
} else {
return [&]() {
auto parsed = exprs::parse_expr(after_eq);
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return [&]() -> preds::ExprResult {
if ((name == mlc::String("extern", 6))) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern{ast::expr_span(parsed.expr)}), parsed.parser};
} else {
return parsed;
}
}(); },
[&](const auto& __v) { return parsed; }
}, (*parsed.expr));
}();
}
}();
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn{mangled_name, {}, {}, params, ret_type_result.type_expr, body_result.expr, {}}), body_result.parser};
}
preds::DeclResult parse_decl(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if ((preds::TKind_is_ident(kind) && (preds::TKind_ident(kind) == mlc::String("export", 6)))) {
auto inner = parse_decl(preds::Parser_advance(parser));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExported{inner.decl}), inner.parser};
} else if ((preds::TKind_is_fn(kind) || (preds::TKind_is_extern(kind) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(parser)))))) {
return parse_fn_decl(parser);
} else if (preds::TKind_is_type(kind)) {
return parse_type_decl(preds::Parser_advance(parser));
} else if (preds::TKind_is_extend(kind)) {
return parse_extend_decl(preds::Parser_advance(parser));
} else if (preds::TKind_is_import(kind)) {
return parse_import_decl(preds::Parser_advance(parser));
} else {
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn{mlc::String("__skip__", 8), {}, {}, {}, std::make_shared<ast::TypeExpr>(ast::TyUnit{}), std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), {}}), preds::Parser_advance(parser)};
}




}
preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept{
auto is_extern = preds::TKind_is_extern(preds::Parser_kind(parser));
auto extern_keyword_span = [&]() -> ast::Span {
if (is_extern) {
return preds::Parser_span_at_cursor(parser);
} else {
return ast::span_unknown();
}
}();
auto fn_start = [&]() -> preds::Parser {
if (is_extern) {
return preds::Parser_advance(parser);
} else {
return parser;
}
}();
auto after_name = preds::Parser_advance(fn_start);
auto fn_name = preds::TKind_ident(preds::Parser_kind(after_name));
auto type_params_res = parse_type_params_opt(preds::Parser_advance(after_name));
auto params_result = parse_params(preds::Parser_advance(type_params_res.parser));
auto after_rparen = preds::Parser_advance(params_result.parser);
auto type_parser = [&]() -> preds::Parser {
if (preds::TKind_is_arrow(preds::Parser_kind(after_rparen))) {
return preds::Parser_advance(after_rparen);
} else {
return after_rparen;
}
}();
auto ret_type_result = types::parse_type(type_parser);
auto type_bounds = type_params_res.bounds;
auto where_parse = parse_where_clause_opt(ret_type_result.parser);
auto merged_trait_bounds = merge_where_clause_trait_bounds(type_params_res.params, type_bounds, where_parse.where_bounds);
if (is_extern) {
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn{fn_name, type_params_res.params, merged_trait_bounds, params_result.params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{extern_keyword_span}), where_parse.where_bounds}), where_parse.parser};
} else {
auto body_result = exprs::parse_expr(preds::Parser_advance(where_parse.parser));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn{fn_name, type_params_res.params, merged_trait_bounds, params_result.params, ret_type_result.type_expr, body_result.expr, where_parse.where_bounds}), body_result.parser};
}
}
preds::PatResult parse_tuple_parameter_binding_element(preds::Parser binding_parser) noexcept{
auto binding_span = preds::Parser_span_at_cursor(binding_parser);
auto binding_kind = preds::Parser_kind(binding_parser);
if (preds::TKind_is_ident(binding_kind)) {
auto binding_label = preds::TKind_ident(binding_kind);
if ((binding_label == mlc::String("_", 1))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{binding_span}), preds::Parser_advance(binding_parser)};
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatIdent{binding_label, binding_span}), preds::Parser_advance(binding_parser)};
}
} else if ((preds::TKind_is_op(binding_kind) && (preds::TKind_op_val(binding_kind) == mlc::String("_", 1)))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{binding_span}), preds::Parser_advance(binding_parser)};
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{binding_span}), preds::Parser_advance(binding_parser)};
}

}
preds::PatResult parse_tuple_parameter_pattern(ast::Span tuple_span, preds::Parser parser_after_open_paren) noexcept{
auto cursor = parser_after_open_paren;
auto tuple_elements = mlc::Array<std::shared_ptr<ast::Pat>>{};
if (preds::TKind_is_rparen(preds::Parser_kind(cursor))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatTuple{tuple_elements, tuple_span}), preds::Parser_advance(cursor)};
} else {
auto tuple_parse_finished = false;
while ((!tuple_parse_finished)) {
auto bound_element = parse_tuple_parameter_binding_element(cursor);
tuple_elements.push_back(bound_element.pat);
cursor = bound_element.parser;
if (preds::TKind_is_comma(preds::Parser_kind(cursor))) {
cursor = preds::Parser_advance(cursor);
if (preds::TKind_is_rparen(preds::Parser_kind(cursor))) {
tuple_parse_finished = true;
}
} else if (preds::TKind_is_rparen(preds::Parser_kind(cursor))) {
tuple_parse_finished = true;
} else {
tuple_parse_finished = true;
}

}
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatTuple{tuple_elements, tuple_span}), preds::Parser_advance(cursor)};
}
}
std::shared_ptr<ast::Pat> plain_identifier_parameter_pattern_marker() noexcept{
return std::make_shared<ast::Pat>(ast::PatUnit{ast::span_unknown()});
}
preds::ParamResult parse_param(preds::Parser parser_for_parameter, int parameter_slot_index) noexcept{
auto mutable_binding_requested = preds::TKind_is_mut(preds::Parser_kind(parser_for_parameter));
auto cursor_after_mutability_keyword = [&]() -> preds::Parser {
if (mutable_binding_requested) {
return preds::Parser_advance(parser_for_parameter);
} else {
return parser_for_parameter;
}
}();
if (preds::TKind_is_lbrace(preds::Parser_kind(cursor_after_mutability_keyword))) {
auto record_pattern_span = preds::Parser_span_at_cursor(cursor_after_mutability_keyword);
auto shorthand_record_fields = exprs::parse_record_pat_fields(preds::Parser_advance(cursor_after_mutability_keyword));
auto cursor_after_record_fields = shorthand_record_fields.parser;
auto record_field_type_parse = types::parse_type(preds::Parser_advance(cursor_after_record_fields));
auto remainder_after_record_type = record_field_type_parse.parser;
auto synthetic_aggregate_identifier = (mlc::String("__mlc_ds", 8) + mlc::to_string(parameter_slot_index));
auto record_destructure_pattern = std::make_shared<ast::Pat>(ast::PatRecord{mlc::String("", 0), shorthand_record_fields.pats, record_pattern_span});
if (preds::TKind_is_equal(preds::Parser_kind(remainder_after_record_type))) {
auto parsed_default_expression = exprs::parse_expr(preds::Parser_advance(remainder_after_record_type));
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.type_expr, true, parsed_default_expression.expr, record_destructure_pattern}), parsed_default_expression.parser};
} else {
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), record_destructure_pattern}), remainder_after_record_type};
}
} else if (preds::TKind_is_lparen(preds::Parser_kind(cursor_after_mutability_keyword))) {
auto tuple_pattern_span = preds::Parser_span_at_cursor(cursor_after_mutability_keyword);
auto tuple_pattern_parse = parse_tuple_parameter_pattern(tuple_pattern_span, preds::Parser_advance(cursor_after_mutability_keyword));
auto cursor_after_tuple_pattern = tuple_pattern_parse.parser;
auto tuple_parameter_type_parse = types::parse_type(preds::Parser_advance(cursor_after_tuple_pattern));
auto remainder_after_tuple_type = tuple_parameter_type_parse.parser;
auto synthetic_tuple_aggregate_identifier = (mlc::String("__mlc_ds", 8) + mlc::to_string(parameter_slot_index));
auto tuple_destructure_pattern = tuple_pattern_parse.pat;
if (preds::TKind_is_equal(preds::Parser_kind(remainder_after_tuple_type))) {
auto parsed_tuple_default_expression = exprs::parse_expr(preds::Parser_advance(remainder_after_tuple_type));
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.type_expr, true, parsed_tuple_default_expression.expr, tuple_destructure_pattern}), parsed_tuple_default_expression.parser};
} else {
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), tuple_destructure_pattern}), remainder_after_tuple_type};
}
} else {
auto binding_identifier_token_position = cursor_after_mutability_keyword;
auto binding_name = preds::TKind_ident(preds::Parser_kind(binding_identifier_token_position));
auto after_binding_name = preds::Parser_advance(binding_identifier_token_position);
auto plain_binding_type_parse = types::parse_type(preds::Parser_advance(after_binding_name));
auto remainder_after_plain_type = plain_binding_type_parse.parser;
if (preds::TKind_is_equal(preds::Parser_kind(remainder_after_plain_type))) {
auto parsed_plain_default_expression = exprs::parse_expr(preds::Parser_advance(remainder_after_plain_type));
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.type_expr, true, parsed_plain_default_expression.expr, plain_identifier_parameter_pattern_marker()}), parsed_plain_default_expression.parser};
} else {
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), plain_identifier_parameter_pattern_marker()}), remainder_after_plain_type};
}
}

}
preds::ParamsResult comma_separated_params_suffix_from_first(preds::ParamResult first) noexcept{
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
params.push_back(first.param);
auto state = first.parser;
auto parameter_slot_index = 1;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_param(preds::Parser_advance(state), parameter_slot_index);
params.push_back(next.param);
state = next.parser;
parameter_slot_index = (parameter_slot_index + 1);
}
return preds::ParamsResult{params, state};
}
preds::ParamsResult parse_params(preds::Parser parser) noexcept{
if (preds::TKind_is_rparen(preds::Parser_kind(parser))) {
return preds::ParamsResult{{}, preds::Parser_advance(parser)};
} else {
auto suffix = comma_separated_params_suffix_from_first(parse_param(parser, 0));
return preds::ParamsResult{suffix.params, preds::Parser_advance(suffix.parser)};
}
}
DeriveResult parse_derive_clause(preds::Parser parser) noexcept{
if ((preds::TKind_is_ident(preds::Parser_kind(parser)) && (preds::TKind_ident(preds::Parser_kind(parser)) == mlc::String("derive", 6)))) {
auto state = preds::Parser_advance(parser);
auto traits = mlc::Array<mlc::String>{};
if (preds::TKind_is_lbrace(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
while (preds::TKind_is_ident(preds::Parser_kind(state))) {
traits.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
if (preds::TKind_is_rbrace(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return DeriveResult{traits, state};
} else {
return DeriveResult{{}, parser};
}
}
preds::DeclResult parse_type_decl(preds::Parser parser) noexcept{
auto type_name = preds::TKind_ident(preds::Parser_kind(parser));
auto state = preds::Parser_advance(parser);
auto type_params_res = parse_type_params_opt(state);
state = type_params_res.parser;
if (preds::TKind_is_lbrace(preds::Parser_kind(state))) {
auto trait_result = parse_trait_body(preds::Parser_advance(state), type_name, type_params_res.params);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclTrait{type_name, type_params_res.params, trait_result.methods}), trait_result.parser};
} else if (preds::TKind_is_equal(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
auto first_kind = preds::Parser_kind(state);
if (preds::TKind_is_lbrace(first_kind)) {
auto field_defs_result = parse_field_defs(preds::Parser_advance(state));
auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarRecord{type_name, field_defs_result.field_defs, false}));
auto derive_res = parse_derive_clause(field_defs_result.parser);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType{type_name, type_params_res.params, variants, derive_res.traits}), derive_res.parser};
} else {
auto variants_result = parse_variants(state);
auto derive_res = parse_derive_clause(variants_result.parser);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType{type_name, type_params_res.params, variants_result.variants, derive_res.traits}), derive_res.parser};
}
} else {
auto variants_result = parse_variants(state);
auto derive_res = parse_derive_clause(variants_result.parser);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType{type_name, type_params_res.params, variants_result.variants, derive_res.traits}), derive_res.parser};
}

}
bool is_decl_start(ast_tokens::TKind kind) noexcept{
return ((((preds::TKind_is_type(kind) || preds::TKind_is_fn(kind)) || preds::TKind_is_extend(kind)) || preds::TKind_is_import(kind)) || (preds::TKind_is_ident(kind) && (((((preds::TKind_ident(kind) == mlc::String("export", 6)) || (preds::TKind_ident(kind) == mlc::String("type", 4))) || (preds::TKind_ident(kind) == mlc::String("fn", 2))) || (preds::TKind_ident(kind) == mlc::String("extend", 6))) || (preds::TKind_ident(kind) == mlc::String("import", 6)))));
}
bool parse_variants_continue(preds::Parser state) noexcept{
if (preds::Parser_at_eof(state)) {
return false;
} else if (is_decl_start(preds::Parser_kind(state))) {
return false;
} else if ((preds::TKind_is_ident(preds::Parser_kind(state)) && (preds::TKind_ident(preds::Parser_kind(state)) == mlc::String("derive", 6)))) {
return false;
} else {
return (preds::TKind_is_bar(preds::Parser_kind(state)) || preds::TKind_is_ident(preds::Parser_kind(state)));
}


}
preds::VariantsResult parse_variants(preds::Parser parser) noexcept{
auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
auto state = [&]() -> preds::Parser {
if (preds::TKind_is_bar(preds::Parser_kind(parser))) {
return preds::Parser_advance(parser);
} else {
return parser;
}
}();
while (parse_variants_continue(state)) {
auto variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (preds::TKind_is_bar(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::VariantsResult{variants, state};
}
preds::VariantResult parse_variant(preds::Parser parser) noexcept{
auto is_private = (preds::TKind_is_ident(preds::Parser_kind(parser)) && (preds::TKind_ident(preds::Parser_kind(parser)) == mlc::String("private", 7)));
auto after_private = [&]() -> preds::Parser {
if (is_private) {
return preds::Parser_advance(parser);
} else {
return parser;
}
}();
auto variant_name = preds::TKind_ident(preds::Parser_kind(after_private));
auto after_name = preds::Parser_advance(after_private);
if (preds::TKind_is_lparen(preds::Parser_kind(after_name))) {
auto types_result = parse_tuple_types(preds::Parser_advance(after_name));
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarTuple{variant_name, types_result.types, is_private}), types_result.parser};
} else if (preds::TKind_is_lbrace(preds::Parser_kind(after_name))) {
auto field_defs_result = parse_field_defs(preds::Parser_advance(after_name));
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarRecord{variant_name, field_defs_result.field_defs, is_private}), field_defs_result.parser};
} else {
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarUnit{variant_name, is_private}), after_name};
}

}
preds::TypesResult comma_separated_type_suffix_from_first(preds::TypeResult first) noexcept{
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
types.push_back(first.type_expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = types::parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
return preds::TypesResult{types, state};
}
preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept{
auto suffix = comma_separated_type_suffix_from_first(types::parse_type(parser));
return preds::TypesResult{suffix.types, preds::Parser_advance(suffix.parser)};
}
preds::TraitBodyResult parse_trait_body(preds::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
if (preds::TKind_is_fn(preds::Parser_kind(state))) {
auto after_fn = preds::Parser_advance(state);
auto fn_name = preds::TKind_ident(preds::Parser_kind(after_fn));
auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
auto params_result = parse_params(preds::Parser_advance(preds::Parser_advance(after_fn)));
auto after_rparen = preds::Parser_advance(params_result.parser);
auto type_parser = [&]() -> preds::Parser {
if (preds::TKind_is_arrow(preds::Parser_kind(after_rparen))) {
return preds::Parser_advance(after_rparen);
} else {
return after_rparen;
}
}();
auto ret_result = types::parse_type(type_parser);
auto initial_trait_bounds = mlc::Array<mlc::Array<mlc::String>>{};
/* unit */;
auto bi = 0;
/* unit */;
while ((bi < type_params.length())) {
initial_trait_bounds.push_back({});
bi = (bi + 1);
}
auto where_parse = parse_where_clause_opt(ret_result.parser);
auto merged_trait_bounds = merge_where_clause_trait_bounds(type_params, initial_trait_bounds, where_parse.where_bounds);
auto body = (preds::TKind_is_equal(preds::Parser_kind(where_parse.parser)) ? ([&]() {
auto body_res = exprs::parse_expr(preds::Parser_advance(where_parse.parser));
state = body_res.parser;
return body_res.expr;
}()) : ([&]() {
auto extern_method_span = preds::Parser_span_at_cursor(where_parse.parser);
state = where_parse.parser;
return std::make_shared<ast::Expr>(ast::ExprExtern{extern_method_span});
}()));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn{mangled, type_params, merged_trait_bounds, params_result.params, ret_result.type_expr, body, where_parse.where_bounds}));
} else if (preds::TKind_is_extern(preds::Parser_kind(state))) {
auto parser_after_extern = preds::Parser_advance(state);
if (preds::TKind_is_fn(preds::Parser_kind(parser_after_extern))) {
auto parser_after_fn_keyword = preds::Parser_advance(parser_after_extern);
auto fn_name = preds::TKind_ident(preds::Parser_kind(parser_after_fn_keyword));
auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
auto params_result = parse_params(preds::Parser_advance(preds::Parser_advance(parser_after_fn_keyword)));
auto after_rparen = preds::Parser_advance(params_result.parser);
auto type_parser = [&]() -> preds::Parser {
if (preds::TKind_is_arrow(preds::Parser_kind(after_rparen))) {
return preds::Parser_advance(after_rparen);
} else {
return after_rparen;
}
}();
auto ret_result = types::parse_type(type_parser);
auto initial_trait_bounds_extern = mlc::Array<mlc::Array<mlc::String>>{};
/* unit */;
auto be = 0;
/* unit */;
while ((be < type_params.length())) {
initial_trait_bounds_extern.push_back({});
be = (be + 1);
}
auto where_parse_extern = parse_where_clause_opt(ret_result.parser);
auto merged_trait_bounds_extern = merge_where_clause_trait_bounds(type_params, initial_trait_bounds_extern, where_parse_extern.where_bounds);
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn{mangled, type_params, merged_trait_bounds_extern, params_result.params, ret_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{ast::span_unknown()}), where_parse_extern.where_bounds}));
state = where_parse_extern.parser;
} else {
state = parser_after_extern;
}
} else if (preds::TKind_is_type(preds::Parser_kind(state))) {
auto after_type = preds::Parser_advance(state);
auto assoc_span = preds::Parser_span_at_cursor(after_type);
auto assoc_name = preds::TKind_ident(preds::Parser_kind(after_type));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType{assoc_name, assoc_span}));
state = preds::Parser_advance(after_type);
} else {
state = preds::Parser_advance(state);
}


}
return preds::TraitBodyResult{methods, preds::Parser_advance(state)};
}
preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept{
auto field_defs = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
auto field_name = preds::TKind_ident(preds::Parser_kind(state));
auto type_result = types::parse_type(preds::Parser_advance_by(state, 2));
auto remainder_after_field_type = type_result.parser;
if (preds::TKind_is_equal(preds::Parser_kind(remainder_after_field_type))) {
auto default_parse_result = exprs::parse_expr(preds::Parser_advance(remainder_after_field_type));
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr, true, default_parse_result.expr}));
state = default_parse_result.parser;
} else {
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})}));
state = remainder_after_field_type;
}
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::FieldDefsResult{field_defs, preds::Parser_advance(state)};
}
ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
auto decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto state = preds::parser_new_with_source_path(tokens, source_path);
while ((!preds::Parser_at_eof(state))) {
auto result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
return ast::Program{decls};
}
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{
return parse_program_with_source_path(tokens, mlc::String("", 0));
}

} // namespace decls
