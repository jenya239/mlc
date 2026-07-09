#define main mlc_user_main
#include "decls.hpp"

#include "predicates.hpp"
#include "ast_tokens.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"

namespace decls {

using namespace predicates;
using namespace ast_tokens;
using namespace types;
using namespace exprs;
using namespace comma_separated;
using namespace expect_close;
using namespace ast;
using namespace ast_tokens;

mlc::String type_expression_display(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return mlc::String("i32", 3); },
[&](const ast::TyString& tyString) { return mlc::String("string", 6); },
[&](const ast::TyBool& tyBool) { return mlc::String("bool", 4); },
[&](const ast::TyUnit& tyUnit) { return mlc::String("unit", 4); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return name; },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return ((mlc::String("[", 1) + type_expression_display(inner)) + mlc::String("]", 1)); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return ((mlc::String("Shared<", 7) + type_expression_display(inner)) + mlc::String(">", 1)); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return (((name + mlc::String("<", 1)) + type_arguments_display(type_arguments)) + mlc::String(">", 1)); },
[&](const ast::TyFn& tyFn) { auto [parameters, return_type] = tyFn; return (((mlc::String("(", 1) + type_arguments_display(parameters)) + mlc::String(") -> ", 5)) + type_expression_display(return_type)); },
[&](const ast::TyAssoc& tyAssoc) { auto [base_name, associated_name] = tyAssoc; return ((base_name + mlc::String(".", 1)) + associated_name); }
}, (*type_expression));
}
mlc::String type_arguments_display(mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept{
  return type_arguments.map(type_expression_display).join(mlc::String(", ", 2));
}
predicates::Parser parser_expect_generic_close(predicates::Parser state, bool opened) noexcept{
  if ((!opened))   {
    return state;
  } else if (types::parser_at_generic_close(state))   {
    return types::parser_advance_generic_close(state);
  } else   {
    return predicates::Parser_record_parse_error(state, mlc::String("parse: unclosed generic parameter list", 38));
  }
}
predicates::Parser parser_expect_ident(predicates::Parser state, mlc::String context) noexcept{
  if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))   {
    return state;
  } else   {
    return predicates::Parser_record_parse_error(state, (mlc::String("parse: expected identifier in ", 30) + context));
  }
}
bool decl_is_parse_skip(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, __1, __2, __3, __4, __5, __6] = declFn; return (name == mlc::String("__skip__", 8));
}
return false;
std::abort();
}();
}
predicates::ParseResult<mlc::Array<mlc::String>> parse_trait_bound_ident_list(predicates::Parser parser_state) noexcept{
  auto state = parser_state;
  auto bounds = mlc::Array<mlc::String>{};
  if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))   {
    bounds.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
    (state = predicates::Parser_advance(state));
    [&]() {
while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("+", 1)))) {
(state = predicates::Parser_advance(state));
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))) {
  bounds.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
  (state = predicates::Parser_advance(state));
}
}
}();
  }
  return predicates::bounds_parse_result(bounds, state);
}
predicates::ParseResult<mlc::Array<mlc::String>> parse_one_param_bounds(predicates::Parser state) noexcept{
  if (predicates::TokenKind_is_colon(predicates::Parser_kind(state)))   {
    return parse_trait_bound_ident_list(predicates::Parser_advance(state));
  } else   {
    return predicates::bounds_parse_result(mlc::Array<mlc::String>{}, state);
  }
}
predicates::ParseResult<mlc::Array<ast::WhereClauseBound>> parse_where_clause_opt(predicates::Parser parser_state) noexcept{
  if ((!predicates::TokenKind_is_where(predicates::Parser_kind(parser_state))))   {
    auto empty_where_bounds = mlc::Array<ast::WhereClauseBound>{};
    return predicates::where_clause_parse_result(empty_where_bounds, parser_state);
  } else   {
    auto state = predicates::Parser_advance(parser_state);
    auto collected = mlc::Array<ast::WhereClauseBound>{};
    while (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))     {
      auto parameter_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
      (state = predicates::Parser_advance(state));
      if (predicates::TokenKind_is_colon(predicates::Parser_kind(state)))       {
        auto trait_parsed = parse_trait_bound_ident_list(predicates::Parser_advance(state));
        (state = trait_parsed.parser);
        collected.push_back(ast::WhereClauseBound{parameter_name, trait_parsed.value});
      }
      if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))       {
        (state = predicates::Parser_advance(state));
      }
    }
    return predicates::where_clause_parse_result(collected, state);
  }
}
bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
  auto index = 0;
  while ((index < haystack.length()))   {
    if ((haystack[index] == needle))     {
      return true;
    }
    (index = (index + 1));
  }
  return false;
}
mlc::Array<mlc::String> clone_string_row(mlc::Array<mlc::String> source) noexcept{
  auto copy = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < source.length()))   {
    copy.push_back(source[index]);
    (index = (index + 1));
  }
  return copy;
}
mlc::Array<mlc::Array<mlc::String>> clone_trait_bounds_matrix(mlc::Array<mlc::Array<mlc::String>> source) noexcept{
  auto matrix = mlc::Array<mlc::Array<mlc::String>>{};
  auto row_index = 0;
  while ((row_index < source.length()))   {
    matrix.push_back(clone_string_row(source[row_index]));
    (row_index = (row_index + 1));
  }
  return matrix;
}
mlc::Array<mlc::String> union_trait_string_rows(mlc::Array<mlc::String> existing_row, mlc::Array<mlc::String> incoming_traits) noexcept{
  auto combined = clone_string_row(existing_row);
  auto incoming_index = 0;
  while ((incoming_index < incoming_traits.length()))   {
    auto trait_name = incoming_traits[incoming_index];
    if ((!string_list_contains(combined, trait_name)))     {
      combined.push_back(trait_name);
    }
    (incoming_index = (incoming_index + 1));
  }
  return combined;
}
int type_parameter_row_index(mlc::Array<mlc::String> type_parameter_names, mlc::String parameter_name) noexcept{
  auto index = 0;
  while ((index < type_parameter_names.length()))   {
    if ((type_parameter_names[index] == parameter_name))     {
      return index;
    }
    (index = (index + 1));
  }
  return (-1);
}
mlc::Array<mlc::Array<mlc::String>> replace_trait_bounds_row_at(mlc::Array<mlc::Array<mlc::String>> matrix, int row_index, mlc::Array<mlc::String> new_row) noexcept{
  auto output_matrix = mlc::Array<mlc::Array<mlc::String>>{};
  auto matrix_row_index = 0;
  while ((matrix_row_index < matrix.length()))   {
    if ((matrix_row_index == row_index))     {
      output_matrix.push_back(clone_string_row(new_row));
    } else     {
      output_matrix.push_back(clone_string_row(matrix[matrix_row_index]));
    }
    (matrix_row_index = (matrix_row_index + 1));
  }
  return output_matrix;
}
mlc::Array<mlc::Array<mlc::String>> merge_where_clause_trait_bounds(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<ast::WhereClauseBound> where_entries) noexcept{
  auto merged = clone_trait_bounds_matrix(trait_bounds_rows);
  while ((merged.length() < type_parameter_names.length()))   {
    auto empty_row = mlc::Array<mlc::String>{};
    merged.push_back(empty_row);
  }
  auto entry_index = 0;
  while ((entry_index < where_entries.length()))   {
    auto entry = where_entries[entry_index];
    auto row_index = type_parameter_row_index(type_parameter_names, entry.parameter_name);
    if ((row_index >= 0))     {
      auto updated_row = union_trait_string_rows(merged[row_index], entry.traits);
      (merged = replace_trait_bounds_row_at(merged, row_index, updated_row));
    }
    (entry_index = (entry_index + 1));
  }
  return merged;
}
predicates::ParseResult<predicates::TypeParamsValue> parse_type_params_opt(predicates::Parser parser) noexcept{
  if ((predicates::TokenKind_is_op(predicates::Parser_kind(parser)) && (predicates::TokenKind_op_val(predicates::Parser_kind(parser)) == mlc::String("<", 1))))   {
    auto state = predicates::Parser_advance(parser);
    auto params = mlc::Array<mlc::String>{};
    auto bounds_list = mlc::Array<mlc::Array<mlc::String>>{};
    if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))     {
      params.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
      auto bounds_parsed = parse_one_param_bounds(predicates::Parser_advance(state));
      bounds_list.push_back(bounds_parsed.value);
      (state = bounds_parsed.parser);
      [&]() {
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))) {
(state = predicates::Parser_advance(state));
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))) {
  params.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
  auto bounds_parsed_loop = parse_one_param_bounds(predicates::Parser_advance(state));
  bounds_list.push_back(bounds_parsed_loop.value);
  (state = bounds_parsed_loop.parser);
}
}
}();
    }
    (state = parser_expect_generic_close(state, true));
    return predicates::type_params_parse_result(params, bounds_list, state);
  } else   {
    return predicates::type_params_parse_result(mlc::Array<mlc::String>{}, mlc::Array<mlc::Array<mlc::String>>{}, parser);
  }
}
predicates::Parser parser_after_optional_rbrace(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_rbrace(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser parser_after_optional_from(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_from(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
mlc::String import_path_from_parser(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_str(predicates::Parser_kind(parser)))   {
    return predicates::TokenKind_str_val(predicates::Parser_kind(parser));
  } else   {
    return mlc::String("", 0);
  }
}
predicates::Parser parser_after_optional_lbrace(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser parser_after_optional_comma(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_comma(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser parser_after_optional_arrow(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_arrow(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser parser_after_optional_mut(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_mut(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser parser_after_optional_bar(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_bar(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser parser_after_optional_private_keyword(predicates::Parser parser) noexcept{
  if ((predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && (predicates::TokenKind_ident(predicates::Parser_kind(parser)) == mlc::String("private", 7))))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
ast::Span extern_keyword_span_for_parser(predicates::Parser parser, bool is_extern) noexcept{
  if (is_extern)   {
    return predicates::Parser_span_at_cursor(parser);
  } else   {
    return ast::span_unknown();
  }
}
predicates::Parser parser_after_optional_extern(predicates::Parser parser, bool is_extern) noexcept{
  if (is_extern)   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::Parser trait_body_start_parser(predicates::Parser body_parser) noexcept{
  if ((!predicates::TokenKind_is_lbrace(predicates::Parser_kind(body_parser))))   {
    return predicates::Parser_record_parse_error(body_parser, mlc::String("parse: expected { after trait header", 36));
  }
  return predicates::Parser_advance(body_parser);
}
predicates::Parser parser_record_error_if_eof_after_equal(predicates::Parser state) noexcept{
  if (predicates::Parser_at_eof(state))   {
    return predicates::Parser_record_parse_error(state, mlc::String("parse: expected type body after =", 33));
  } else   {
    return state;
  }
}
predicates::Parser parser_record_error_if_eof_expect_equal(predicates::Parser state) noexcept{
  if (predicates::Parser_at_eof(state))   {
    return predicates::Parser_record_parse_error(state, mlc::String("parse: expected = after type name", 33));
  } else   {
    return state;
  }
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_trait_method_body(predicates::Parser where_parser) noexcept{
  if (predicates::TokenKind_is_equal(predicates::Parser_kind(where_parser)))   {
    return exprs::parse_expr(predicates::Parser_advance(where_parser));
  } else   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprExtern{predicates::Parser_span_at_cursor(where_parser)}), where_parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_import_decl(predicates::Parser parser) noexcept{
  auto state = parser;
  auto symbols = mlc::Array<mlc::String>{};
  if ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("*", 1))))   {
    symbols.push_back(mlc::String("*", 1));
    (state = predicates::Parser_advance(state));
    if (predicates::TokenKind_is_as(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
      if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))       {
        symbols.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
        (state = predicates::Parser_advance(state));
      }
    }
  } else if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(state)))   {
    (state = predicates::Parser_advance(state));
    while ((predicates::TokenKind_is_ident(predicates::Parser_kind(state)) || (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("*", 1)))))     {
      if ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("*", 1))))       {
        symbols.push_back(mlc::String("*", 1));
        (state = predicates::Parser_advance(state));
      } else       {
        symbols.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
        (state = predicates::Parser_advance(state));
      }
      if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))       {
        (state = predicates::Parser_advance(state));
      }
    }
    (state = parser_after_optional_rbrace(state));
  }
  (state = parser_after_optional_from(state));
  auto path = import_path_from_parser(state);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclImport{path, symbols}), predicates::Parser_advance(state));
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extend_decl(predicates::Parser parser) noexcept{
  auto type_name_span = predicates::Parser_span_at_cursor(parser);
  auto type_name = predicates::TokenKind_ident(predicates::Parser_kind(parser));
  auto state = predicates::Parser_advance(parser);
  if ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("<", 1))))   {
    auto type_name_arguments_parsed = types::parse_type_args(predicates::Parser_advance(state));
    (state = type_name_arguments_parsed.parser);
  }
  auto trait_name = mlc::String("", 0);
  if (predicates::TokenKind_is_colon(predicates::Parser_kind(state)))   {
    (state = predicates::Parser_advance(state));
    if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))     {
      (trait_name = predicates::TokenKind_ident(predicates::Parser_kind(state)));
      (state = predicates::Parser_advance(state));
    }
    if ((((trait_name.length() > 0) && predicates::TokenKind_is_op(predicates::Parser_kind(state))) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("<", 1))))     {
      auto trait_type_arguments_parsed = types::parse_type_args(predicates::Parser_advance(state));
      (trait_name = (((trait_name + mlc::String("<", 1)) + type_arguments_display(trait_type_arguments_parsed.value)) + mlc::String(">", 1)));
      (state = trait_type_arguments_parsed.parser);
    }
  }
  auto methods_state = parser_after_optional_lbrace(state);
  auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
  while (((!predicates::TokenKind_is_rbrace(predicates::Parser_kind(methods_state))) && (!predicates::Parser_at_eof(methods_state))))   {
    if (predicates::TokenKind_is_fn(predicates::Parser_kind(methods_state)))     {
      auto parsed = parse_extend_method(methods_state, type_name);
      methods.push_back(parsed.value);
      (methods_state = parsed.parser);
    } else if ((predicates::TokenKind_is_extern(predicates::Parser_kind(methods_state)) && predicates::TokenKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(methods_state)))))     {
      auto parsed = parse_extend_extern_method(predicates::Parser_advance(methods_state), type_name);
      methods.push_back(parsed.value);
      (methods_state = parsed.parser);
    } else if (predicates::TokenKind_is_type(predicates::Parser_kind(methods_state)))     {
      auto after_type = predicates::Parser_advance(methods_state);
      auto assoc_span = predicates::Parser_span_at_cursor(after_type);
      auto assoc_name = predicates::TokenKind_ident(predicates::Parser_kind(after_type));
      auto after_name = predicates::Parser_advance(after_type);
      if (predicates::TokenKind_is_equal(predicates::Parser_kind(after_name)))       {
        auto type_parsed = types::parse_type(predicates::Parser_advance(after_name));
        methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocBind{assoc_name, type_parsed.value, assoc_span}));
        (methods_state = type_parsed.parser);
      } else       {
        methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType{assoc_name, assoc_span}));
        (methods_state = after_name);
      }
    } else     {
      (methods_state = predicates::Parser_advance(methods_state));
    }
  }
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclExtend{type_name, trait_name, methods, type_name_span}), predicates::Parser_advance(methods_state));
}
predicates::Parser advance_past_optional_self_type(predicates::Parser parser_after_self) noexcept{
  if (predicates::TokenKind_is_colon(predicates::Parser_kind(parser_after_self)))   {
    return types::parse_type(predicates::Parser_advance(parser_after_self)).parser;
  } else   {
    return parser_after_self;
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_extend_extern_no_self_params(predicates::Parser after_lparen) noexcept{
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(after_lparen)))   {
    auto empty_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
    return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(after_lparen));
  } else   {
    return parse_params(after_lparen);
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_extend_extern_rest_params(predicates::Parser after_lparen, bool leading_self) noexcept{
  if (leading_self)   {
    auto after_self_type = advance_past_optional_self_type(predicates::Parser_advance(after_lparen));
    if (predicates::TokenKind_is_rparen(predicates::Parser_kind(after_self_type)))     {
      auto empty_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
      return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(after_self_type));
    } else     {
      return parse_params(parser_after_optional_comma(after_self_type));
    }
  } else   {
    return parse_extend_extern_no_self_params(after_lparen);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extend_extern_method(predicates::Parser parser, mlc::String type_name) noexcept{
  auto extern_span = predicates::Parser_span_at_cursor(parser);
  auto fn_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(parser)));
  auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
  auto after_lparen = predicates::Parser_advance_by(parser, 3);
  auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed{type_name}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), std::make_shared<ast::Pattern>(ast::PatternUnit{ast::span_unknown()})});
  auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
  auto leading_self = (predicates::TokenKind_is_ident(predicates::Parser_kind(after_lparen)) && (predicates::TokenKind_ident(predicates::Parser_kind(after_lparen)) == mlc::String("self", 4)));
  if (leading_self)   {
    params.push_back(self_param);
  }
  auto rest_parsed = parse_extend_extern_rest_params(after_lparen, leading_self);
  auto index = 0;
  while ((index < rest_parsed.value.length()))   {
    auto rest_parameter = rest_parsed.value[index];
    if ((ast::param_name(rest_parameter) != mlc::String("self", 4)))     {
      params.push_back(rest_parameter);
    }
    (index = (index + 1));
  }
  auto after_rparen = predicates::Parser_advance(rest_parsed.parser);
  auto return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn{mangled_name, {}, {}, params, return_type_parsed.value, std::make_shared<ast::Expr>(ast::ExprExtern{extern_span}), {}}), return_type_parsed.parser);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_extend_method_rest_params(predicates::Parser rest_start) noexcept{
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(rest_start)))   {
    auto empty_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
    return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(rest_start));
  } else   {
    return parse_params(parser_after_optional_comma(rest_start));
  }
}
bool is_extern_ident_expr(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return (name == mlc::String("extern", 6));
}
return false;
std::abort();
}();
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_extend_method_body(predicates::Parser after_eq) noexcept{
  if (predicates::TokenKind_is_extern(predicates::Parser_kind(after_eq)))   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprExtern{predicates::Parser_span_at_cursor(after_eq)}), predicates::Parser_advance(after_eq));
  }
  auto body_expr_parsed = exprs::parse_expr(after_eq);
  if (is_extern_ident_expr(body_expr_parsed.value))   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprExtern{ast::expr_span(body_expr_parsed.value)}), body_expr_parsed.parser);
  }
  return body_expr_parsed;
}
predicates::Parser extend_method_rest_start(predicates::Parser after_lparen) noexcept{
  if ((predicates::TokenKind_is_ident(predicates::Parser_kind(after_lparen)) && (predicates::TokenKind_ident(predicates::Parser_kind(after_lparen)) == mlc::String("self", 4))))   {
    return advance_past_optional_self_type(predicates::Parser_advance(after_lparen));
  } else   {
    return after_lparen;
  }
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extend_method(predicates::Parser parser, mlc::String type_name) noexcept{
  auto fn_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(parser)));
  auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
  auto after_lparen = predicates::Parser_advance_by(parser, 3);
  auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed{type_name}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), std::make_shared<ast::Pattern>(ast::PatternUnit{ast::span_unknown()})});
  auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
  params.push_back(self_param);
  auto rest_parsed = parse_extend_method_rest_params(extend_method_rest_start(after_lparen));
  auto index = 0;
  while ((index < rest_parsed.value.length()))   {
    auto rest_parameter = rest_parsed.value[index];
    if ((ast::param_name(rest_parameter) != mlc::String("self", 4)))     {
      params.push_back(rest_parameter);
    }
    (index = (index + 1));
  }
  auto after_rparen = predicates::Parser_advance(rest_parsed.parser);
  auto return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
  auto body_parsed = parse_extend_method_body(predicates::Parser_advance(return_type_parsed.parser));
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn{mangled_name, {}, {}, params, return_type_parsed.value, body_parsed.value, {}}), body_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_trait_decl(predicates::Parser parser) noexcept{
  auto after_trait = predicates::Parser_advance(parser);
  auto name_parser = parser_expect_ident(after_trait, mlc::String("trait name", 10));
  auto trait_name = predicates::TokenKind_ident(predicates::Parser_kind(name_parser));
  auto trait_name_span = predicates::Parser_span_at_cursor(name_parser);
  auto type_parameters_parsed = parse_type_params_opt(predicates::Parser_advance(name_parser));
  auto trait_parsed = parse_trait_body(trait_body_start_parser(type_parameters_parsed.parser), trait_name, type_parameters_parsed.value.params);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclTrait{trait_name, type_parameters_parsed.value.params, trait_parsed.value, trait_name_span}), trait_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_declaration(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if ((predicates::TokenKind_is_ident(kind) && (predicates::TokenKind_ident(kind) == mlc::String("export", 6))))   {
    auto inner_parsed = parse_declaration(predicates::Parser_advance(parser));
    return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclExported{inner_parsed.value}), inner_parsed.parser);
  } else if ((predicates::TokenKind_is_ident(kind) && (predicates::TokenKind_ident(kind) == mlc::String("trait", 5))))   {
    return parse_trait_decl(parser);
  } else if ((predicates::TokenKind_is_fn(kind) || (predicates::TokenKind_is_extern(kind) && predicates::TokenKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(parser))))))   {
    return parse_function_declaration(parser);
  } else if (predicates::TokenKind_is_type(kind))   {
    return parse_type_decl(predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_extend(kind))   {
    return parse_extend_decl(predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_import(kind))   {
    return parse_import_decl(predicates::Parser_advance(parser));
  } else   {
    auto error_parser = predicates::Parser_record_parse_error(parser, mlc::String("parse: unexpected token at declaration", 38));
    return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn{mlc::String("__skip__", 8), {}, {}, {}, std::make_shared<ast::TypeExpr>(ast::TyUnit{}), std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), {}}), predicates::Parser_advance(error_parser));
  }
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_function_declaration(predicates::Parser parser) noexcept{
  auto is_extern = predicates::TokenKind_is_extern(predicates::Parser_kind(parser));
  auto extern_keyword_span = extern_keyword_span_for_parser(parser, is_extern);
  auto fn_start = parser_after_optional_extern(parser, is_extern);
  auto name_parser = parser_expect_ident(predicates::Parser_advance(fn_start), mlc::String("function name", 13));
  auto fn_name = predicates::TokenKind_ident(predicates::Parser_kind(name_parser));
  auto type_parameters_parsed = parse_type_params_opt(predicates::Parser_advance(name_parser));
  auto params_parsed = parse_params(predicates::Parser_advance(type_parameters_parsed.parser));
  auto after_rparen = predicates::Parser_advance(params_parsed.parser);
  auto return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
  auto type_bounds = type_parameters_parsed.value.bounds;
  auto where_parse = parse_where_clause_opt(return_type_parsed.parser);
  auto merged_trait_bounds = merge_where_clause_trait_bounds(type_parameters_parsed.value.params, type_bounds, where_parse.value);
  if (is_extern)   {
    return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn{fn_name, type_parameters_parsed.value.params, merged_trait_bounds, params_parsed.value, return_type_parsed.value, std::make_shared<ast::Expr>(ast::ExprExtern{extern_keyword_span}), where_parse.value}), where_parse.parser);
  } else   {
    auto body_parsed = exprs::parse_expr(predicates::Parser_advance(where_parse.parser));
    return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn{fn_name, type_parameters_parsed.value.params, merged_trait_bounds, params_parsed.value, return_type_parsed.value, body_parsed.value, where_parse.value}), body_parsed.parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_tuple_parameter_binding_element(predicates::Parser binding_parser) noexcept{
  auto binding_span = predicates::Parser_span_at_cursor(binding_parser);
  auto binding_kind = predicates::Parser_kind(binding_parser);
  if (predicates::TokenKind_is_ident(binding_kind))   {
    auto binding_label = predicates::TokenKind_ident(binding_kind);
    if ((binding_label == mlc::String("_", 1)))     {
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild{binding_span}), predicates::Parser_advance(binding_parser));
    } else     {
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternIdent{binding_label, binding_span}), predicates::Parser_advance(binding_parser));
    }
  } else if ((predicates::TokenKind_is_op(binding_kind) && (predicates::TokenKind_op_val(binding_kind) == mlc::String("_", 1))))   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild{binding_span}), predicates::Parser_advance(binding_parser));
  } else   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild{binding_span}), predicates::Parser_advance(binding_parser));
  }
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_tuple_parameter_pattern(ast::Span tuple_span, predicates::Parser parser_after_open_paren) noexcept{
  auto cursor = parser_after_open_paren;
  auto tuple_elements = mlc::Array<std::shared_ptr<ast::Pattern>>{};
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(cursor)))   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternTuple{tuple_elements, tuple_span}), predicates::Parser_advance(cursor));
  } else   {
    auto tuple_parse_finished = false;
    while ((!tuple_parse_finished))     {
      auto bound_element = parse_tuple_parameter_binding_element(cursor);
      tuple_elements.push_back(bound_element.value);
      (cursor = bound_element.parser);
      if (predicates::TokenKind_is_comma(predicates::Parser_kind(cursor)))       {
        (cursor = predicates::Parser_advance(cursor));
        if (predicates::TokenKind_is_rparen(predicates::Parser_kind(cursor)))         {
          (tuple_parse_finished = true);
        }
      } else if (predicates::TokenKind_is_rparen(predicates::Parser_kind(cursor)))       {
        (tuple_parse_finished = true);
      } else       {
        (tuple_parse_finished = true);
      }
    }
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternTuple{tuple_elements, tuple_span}), predicates::Parser_advance(cursor));
  }
}
std::shared_ptr<ast::Pattern> plain_identifier_parameter_pattern_marker() noexcept{
  return std::make_shared<ast::Pattern>(ast::PatternUnit{ast::span_unknown()});
}
predicates::ParseResult<std::shared_ptr<ast::Param>> parse_param(predicates::Parser parser_for_parameter, int parameter_slot_index) noexcept{
  auto mutable_binding_requested = predicates::TokenKind_is_mut(predicates::Parser_kind(parser_for_parameter));
  auto cursor_after_mutability_keyword = parser_after_optional_mut(parser_for_parameter);
  if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(cursor_after_mutability_keyword)))   {
    auto record_pattern_span = predicates::Parser_span_at_cursor(cursor_after_mutability_keyword);
    auto shorthand_record_fields = exprs::parse_record_pattern_fields(predicates::Parser_advance(cursor_after_mutability_keyword));
    auto cursor_after_record_fields = shorthand_record_fields.parser;
    auto record_field_type_parse = types::parse_type(predicates::Parser_advance(cursor_after_record_fields));
    auto remainder_after_record_type = record_field_type_parse.parser;
    auto synthetic_aggregate_identifier = (mlc::String("__mlc_ds", 8) + mlc::to_string(parameter_slot_index));
    auto record_destructure_pattern = std::make_shared<ast::Pattern>(ast::PatternRecord{mlc::String("", 0), shorthand_record_fields.value, record_pattern_span});
    if (predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_record_type)))     {
      auto parsed_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_record_type));
      return predicates::parse_result(std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.value, true, parsed_default_expression.value, record_destructure_pattern}), parsed_default_expression.parser);
    } else     {
      return predicates::parse_result(std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.value, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), record_destructure_pattern}), remainder_after_record_type);
    }
  } else if (predicates::TokenKind_is_lparen(predicates::Parser_kind(cursor_after_mutability_keyword)))   {
    auto tuple_pattern_span = predicates::Parser_span_at_cursor(cursor_after_mutability_keyword);
    auto tuple_pattern_parse = parse_tuple_parameter_pattern(tuple_pattern_span, predicates::Parser_advance(cursor_after_mutability_keyword));
    auto cursor_after_tuple_pattern = tuple_pattern_parse.parser;
    auto tuple_parameter_type_parse = types::parse_type(predicates::Parser_advance(cursor_after_tuple_pattern));
    auto remainder_after_tuple_type = tuple_parameter_type_parse.parser;
    auto synthetic_tuple_aggregate_identifier = (mlc::String("__mlc_ds", 8) + mlc::to_string(parameter_slot_index));
    auto tuple_destructure_pattern = tuple_pattern_parse.value;
    if (predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_tuple_type)))     {
      auto parsed_tuple_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_tuple_type));
      return predicates::parse_result(std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.value, true, parsed_tuple_default_expression.value, tuple_destructure_pattern}), parsed_tuple_default_expression.parser);
    } else     {
      return predicates::parse_result(std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.value, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), tuple_destructure_pattern}), remainder_after_tuple_type);
    }
  } else   {
    auto binding_identifier_token_position = cursor_after_mutability_keyword;
    auto binding_name = predicates::TokenKind_ident(predicates::Parser_kind(binding_identifier_token_position));
    auto after_binding_name = predicates::Parser_advance(binding_identifier_token_position);
    auto plain_binding_type_parse = types::parse_type(predicates::Parser_advance(after_binding_name));
    auto remainder_after_plain_type = plain_binding_type_parse.parser;
    if (predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_plain_type)))     {
      auto parsed_plain_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_plain_type));
      return predicates::parse_result(std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.value, true, parsed_plain_default_expression.value, plain_identifier_parameter_pattern_marker()}), parsed_plain_default_expression.parser);
    } else     {
      return predicates::parse_result(std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.value, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), plain_identifier_parameter_pattern_marker()}), remainder_after_plain_type);
    }
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_params(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)))   {
    auto empty_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
    return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(parser));
  } else   {
    auto suffix = comma_separated::comma_separated_params_suffix_from_first(parse_param(parser, 0), 1, parse_param);
    return predicates::parameters_parse_result(suffix.value, predicates::Parser_advance(suffix.parser));
  }
}
predicates::ParseResult<mlc::Array<mlc::String>> parse_derive_clause(predicates::Parser parser) noexcept{
  if ((predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && (predicates::TokenKind_ident(predicates::Parser_kind(parser)) == mlc::String("derive", 6))))   {
    auto state = predicates::Parser_advance(parser);
    auto traits = mlc::Array<mlc::String>{};
    if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
      while (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))       {
        traits.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
        (state = predicates::Parser_advance(state));
        if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))         {
          (state = predicates::Parser_advance(state));
        }
      }
      if (predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)))       {
        (state = predicates::Parser_advance(state));
      }
    }
    return predicates::bounds_parse_result(traits, state);
  } else   {
    return predicates::bounds_parse_result(mlc::Array<mlc::String>{}, parser);
  }
}
bool type_decl_after_equal_is_sum_variant(predicates::Parser parser) noexcept{
  auto state = parser;
  if ((predicates::TokenKind_is_ident(predicates::Parser_kind(state)) && (predicates::TokenKind_ident(predicates::Parser_kind(state)) == mlc::String("private", 7))))   {
    (state = predicates::Parser_advance(state));
  }
  if (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))   {
    return true;
  } else if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))   {
    auto after_ident = predicates::Parser_advance(state);
    return ((predicates::TokenKind_is_bar(predicates::Parser_kind(after_ident)) || predicates::TokenKind_is_lparen(predicates::Parser_kind(after_ident))) || predicates::TokenKind_is_lbrace(predicates::Parser_kind(after_ident)));
  } else   {
    return false;
  }
}
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_type_decl(predicates::Parser parser) noexcept{
  auto name_parser = parser_expect_ident(parser, mlc::String("type name", 9));
  auto type_name = predicates::TokenKind_ident(predicates::Parser_kind(name_parser));
  auto type_name_span = predicates::Parser_span_at_cursor(name_parser);
  auto state = predicates::Parser_advance(name_parser);
  auto type_parameters_parsed = parse_type_params_opt(state);
  (state = type_parameters_parsed.parser);
  if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(state)))   {
    auto trait_parsed = parse_trait_body(predicates::Parser_advance(state), type_name, type_parameters_parsed.value.params);
    return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclTrait{type_name, type_parameters_parsed.value.params, trait_parsed.value, type_name_span}), trait_parsed.parser);
  } else if (predicates::TokenKind_is_equal(predicates::Parser_kind(state)))   {
    (state = parser_record_error_if_eof_after_equal(predicates::Parser_advance(state)));
    auto first_kind = predicates::Parser_kind(state);
    if (predicates::TokenKind_is_lbrace(first_kind))     {
      auto field_defs_parsed = parse_field_defs(predicates::Parser_advance(state));
      auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
      variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarRecord{type_name, field_defs_parsed.value, false}));
      auto derive_parsed = parse_derive_clause(field_defs_parsed.parser);
      return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclType{type_name, type_parameters_parsed.value.params, variants, derive_parsed.value, type_name_span}), derive_parsed.parser);
    } else if (type_decl_after_equal_is_sum_variant(state))     {
      auto variants_parsed = parse_variants(state);
      auto derive_parsed = parse_derive_clause(variants_parsed.parser);
      return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclType{type_name, type_parameters_parsed.value.params, variants_parsed.value, derive_parsed.value, type_name_span}), derive_parsed.parser);
    } else     {
      auto type_parsed = types::parse_type(state);
      return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclTypeAlias{type_name, type_parameters_parsed.value.params, type_parsed.value, type_name_span}), type_parsed.parser);
    }
  } else   {
    auto variants_parsed = parse_variants(parser_record_error_if_eof_expect_equal(state));
    auto derive_parsed = parse_derive_clause(variants_parsed.parser);
    return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclType{type_name, type_parameters_parsed.value.params, variants_parsed.value, derive_parsed.value, type_name_span}), derive_parsed.parser);
  }
}
bool is_decl_start(ast_tokens::TokenKind kind) noexcept{
  return ((((predicates::TokenKind_is_type(kind) || predicates::TokenKind_is_fn(kind)) || predicates::TokenKind_is_extend(kind)) || predicates::TokenKind_is_import(kind)) || (predicates::TokenKind_is_ident(kind) && ((((((predicates::TokenKind_ident(kind) == mlc::String("export", 6)) || (predicates::TokenKind_ident(kind) == mlc::String("trait", 5))) || (predicates::TokenKind_ident(kind) == mlc::String("type", 4))) || (predicates::TokenKind_ident(kind) == mlc::String("fn", 2))) || (predicates::TokenKind_ident(kind) == mlc::String("extend", 6))) || (predicates::TokenKind_ident(kind) == mlc::String("import", 6)))));
}
bool parse_variants_continue(predicates::Parser state) noexcept{
  if (predicates::Parser_at_eof(state))   {
    return false;
  } else if (is_decl_start(predicates::Parser_kind(state)))   {
    return false;
  } else if ((predicates::TokenKind_is_ident(predicates::Parser_kind(state)) && (predicates::TokenKind_ident(predicates::Parser_kind(state)) == mlc::String("derive", 6))))   {
    return false;
  } else   {
    return (predicates::TokenKind_is_bar(predicates::Parser_kind(state)) || predicates::TokenKind_is_ident(predicates::Parser_kind(state)));
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeVariant>>> parse_variants(predicates::Parser parser) noexcept{
  auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
  auto state = parser_after_optional_bar(parser);
  while (parse_variants_continue(state))   {
    auto parsed = parse_variant(state);
    variants.push_back(parsed.value);
    (state = parsed.parser);
    if (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
    }
  }
  return predicates::variants_parse_result(variants, state);
}
predicates::ParseResult<std::shared_ptr<ast::TypeVariant>> parse_variant(predicates::Parser parser) noexcept{
  auto is_private = (predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && (predicates::TokenKind_ident(predicates::Parser_kind(parser)) == mlc::String("private", 7)));
  auto after_private = parser_after_optional_private_keyword(parser);
  auto variant_name = predicates::TokenKind_ident(predicates::Parser_kind(after_private));
  auto after_name = predicates::Parser_advance(after_private);
  if (predicates::TokenKind_is_lparen(predicates::Parser_kind(after_name)))   {
    auto types_parsed = parse_tuple_types(predicates::Parser_advance(after_name));
    return predicates::variant_parse_result(std::make_shared<ast::TypeVariant>(ast::VarTuple{variant_name, types_parsed.value, is_private}), types_parsed.parser);
  } else if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(after_name)))   {
    auto field_defs_parsed = parse_field_defs(predicates::Parser_advance(after_name));
    return predicates::variant_parse_result(std::make_shared<ast::TypeVariant>(ast::VarRecord{variant_name, field_defs_parsed.value, is_private}), field_defs_parsed.parser);
  } else   {
    return predicates::variant_parse_result(std::make_shared<ast::TypeVariant>(ast::VarUnit{variant_name, is_private}), after_name);
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> parse_tuple_types(predicates::Parser parser) noexcept{
  auto suffix = comma_separated::comma_separated_types_suffix_from_first(types::parse_type(parser), types::parse_type);
  return predicates::types_parse_result(suffix.value, predicates::Parser_advance(suffix.parser));
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Decl>>> parse_trait_body(predicates::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
  auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto state = parser;
  while (((!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    if (predicates::TokenKind_is_fn(predicates::Parser_kind(state)))     {
      auto after_fn = predicates::Parser_advance(state);
      auto fn_name = predicates::TokenKind_ident(predicates::Parser_kind(after_fn));
      auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
      auto params_parsed = parse_params(predicates::Parser_advance(predicates::Parser_advance(after_fn)));
      auto after_rparen = predicates::Parser_advance(params_parsed.parser);
      auto return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
      auto initial_trait_bounds = mlc::Array<mlc::Array<mlc::String>>{};
      std::make_tuple();
      auto bounds_index = 0;
      std::make_tuple();
      while ((bounds_index < type_params.length()))       {
        initial_trait_bounds.push_back({});
        (bounds_index = (bounds_index + 1));
      }
      auto where_parse = parse_where_clause_opt(return_type_parsed.parser);
      auto merged_trait_bounds = merge_where_clause_trait_bounds(type_params, initial_trait_bounds, where_parse.value);
      auto body_parsed = parse_trait_method_body(where_parse.parser);
      (state = body_parsed.parser);
      methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn{mangled, type_params, merged_trait_bounds, params_parsed.value, return_type_parsed.value, body_parsed.value, where_parse.value}));
    } else if (predicates::TokenKind_is_extern(predicates::Parser_kind(state)))     {
      auto parser_after_extern = predicates::Parser_advance(state);
      if (predicates::TokenKind_is_fn(predicates::Parser_kind(parser_after_extern)))       {
        auto parser_after_fn_keyword = predicates::Parser_advance(parser_after_extern);
        auto fn_name = predicates::TokenKind_ident(predicates::Parser_kind(parser_after_fn_keyword));
        auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
        auto params_parsed = parse_params(predicates::Parser_advance(predicates::Parser_advance(parser_after_fn_keyword)));
        auto after_rparen = predicates::Parser_advance(params_parsed.parser);
        auto return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
        auto initial_trait_bounds_extern = mlc::Array<mlc::Array<mlc::String>>{};
        std::make_tuple();
        auto bounds_index_extern = 0;
        std::make_tuple();
        while ((bounds_index_extern < type_params.length()))         {
          initial_trait_bounds_extern.push_back({});
          (bounds_index_extern = (bounds_index_extern + 1));
        }
        auto where_parse_extern = parse_where_clause_opt(return_type_parsed.parser);
        auto merged_trait_bounds_extern = merge_where_clause_trait_bounds(type_params, initial_trait_bounds_extern, where_parse_extern.value);
        auto extern_method_span = predicates::Parser_span_at_cursor(where_parse_extern.parser);
        methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn{mangled, type_params, merged_trait_bounds_extern, params_parsed.value, return_type_parsed.value, std::make_shared<ast::Expr>(ast::ExprExtern{extern_method_span}), where_parse_extern.value}));
        (state = where_parse_extern.parser);
      } else       {
        (state = predicates::Parser_record_parse_error(parser_after_extern, mlc::String("parse: expected fn after extern in trait body", 45)));
      }
    } else if (predicates::TokenKind_is_type(predicates::Parser_kind(state)))     {
      auto after_type = predicates::Parser_advance(state);
      auto assoc_span = predicates::Parser_span_at_cursor(after_type);
      auto assoc_name = predicates::TokenKind_ident(predicates::Parser_kind(after_type));
      methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType{assoc_name, assoc_span}));
      (state = predicates::Parser_advance(after_type));
    } else     {
      (state = predicates::Parser_advance(predicates::Parser_record_parse_error(state, mlc::String("parse: expected fn or type in trait body", 40))));
    }
  }
  (state = expect_close::parser_expect_rbrace(state, mlc::String("trait body", 10)));
  return predicates::trait_body_parse_result(methods, state);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::FieldDef>>> parse_field_defs(predicates::Parser parser) noexcept{
  auto field_defs = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
  auto state = parser;
  while (((!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    auto field_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
    auto type_parsed = types::parse_type(predicates::Parser_advance_by(state, 2));
    auto remainder_after_field_type = type_parsed.parser;
    if (predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_field_type)))     {
      auto default_parsed = exprs::parse_expr(predicates::Parser_advance(remainder_after_field_type));
      field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_parsed.value, true, default_parsed.value}));
      (state = default_parsed.parser);
    } else     {
      field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_parsed.value, false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})}));
      (state = remainder_after_field_type);
    }
    if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
    }
  }
  (state = expect_close::parser_expect_rbrace(state, mlc::String("record type fields", 18)));
  return predicates::field_definitions_parse_result(field_defs, state);
}
ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
  auto declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto state = predicates::parser_new_with_source_path(tokens, source_path);
  while ((!predicates::Parser_at_eof(state)))   {
    auto parsed = parse_declaration(state);
    if ((!decl_is_parse_skip(parsed.value)))     {
      declarations.push_back(parsed.value);
    }
    (state = parsed.parser);
  }
  return ast::Program{declarations};
}
predicates::ProgramParseValue parse_program_with_errors(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
  auto declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto state = predicates::parser_new_with_source_path(tokens, source_path);
  while ((!predicates::Parser_at_eof(state)))   {
    auto parsed = parse_declaration(state);
    if ((!decl_is_parse_skip(parsed.value)))     {
      declarations.push_back(parsed.value);
    }
    (state = parsed.parser);
  }
  return predicates::program_parse_result(ast::Program{declarations}, state.errors);
}
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{
  return parse_program_with_source_path(tokens, mlc::String("", 0));
}

} // namespace decls
