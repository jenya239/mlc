#define main mlc_user_main
#include "exprs.hpp"

#include "predicates.hpp"
#include "types.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"

namespace exprs {

using namespace predicates;
using namespace types;
using namespace comma_separated;
using namespace expect_close;
using namespace ast;
using namespace ast_tokens;
using namespace lexer;
using namespace ast_tokens;

predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_end(predicates::Parser parser) noexcept{
  auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
  auto state = parser;
  while (((!predicates::TokenKind_is_end(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    auto parsed = parse_statement(state);
    stmts.push_back(parsed.value);
    (state = predicates::Parser_skip_semi(parsed.parser));
  }
  if (predicates::TokenKind_is_end(predicates::Parser_kind(state)))   {
    return predicates::statements_parse_result(stmts, predicates::Parser_advance(state));
  }
  if (predicates::Parser_at_eof(state))   {
    return predicates::statements_parse_result(stmts, predicates::Parser_record_parse_error(state, mlc::String("parse: expected end in block", 28)));
  }
  return predicates::statements_parse_result(stmts, state);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_end_or_eof(predicates::Parser parser) noexcept{
  auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
  auto state = parser;
  while (((!predicates::TokenKind_is_end(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    auto parsed = parse_statement(state);
    stmts.push_back(parsed.value);
    (state = predicates::Parser_skip_semi(parsed.parser));
  }
  if (predicates::TokenKind_is_end(predicates::Parser_kind(state)))   {
    return predicates::statements_parse_result(stmts, predicates::Parser_advance(state));
  }
  return predicates::statements_parse_result(stmts, state);
}
bool parser_at_toplevel_declaration_start(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if ((((predicates::TokenKind_is_fn(kind) || predicates::TokenKind_is_type(kind)) || predicates::TokenKind_is_extend(kind)) || predicates::TokenKind_is_import(kind)))   {
    return true;
  }
  if ((predicates::TokenKind_is_extern(kind) && predicates::TokenKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(parser)))))   {
    return true;
  }
  if ((predicates::TokenKind_is_ident(kind) && ((predicates::TokenKind_ident(kind) == mlc::String("export", 6)) || (predicates::TokenKind_ident(kind) == mlc::String("trait", 5)))))   {
    return true;
  }
  return false;
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_end_eof_or_toplevel_decl(predicates::Parser parser) noexcept{
  auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
  auto state = parser;
  while ((((!predicates::TokenKind_is_end(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))) && (!parser_at_toplevel_declaration_start(state))))   {
    auto parsed = parse_statement(state);
    stmts.push_back(parsed.value);
    (state = predicates::Parser_skip_semi(parsed.parser));
  }
  if (predicates::TokenKind_is_end(predicates::Parser_kind(state)))   {
    return predicates::statements_parse_result(stmts, predicates::Parser_advance(state));
  }
  return predicates::statements_parse_result(stmts, state);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_else_end(predicates::Parser parser) noexcept{
  auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
  auto state = parser;
  while ((((!predicates::TokenKind_is_end(predicates::Parser_kind(state))) && (!predicates::TokenKind_is_else(predicates::Parser_kind(state)))) && (!predicates::Parser_at_eof(state))))   {
    auto parsed = parse_statement(state);
    stmts.push_back(parsed.value);
    (state = parsed.parser);
  }
  return predicates::statements_parse_result(stmts, state);
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let_const(predicates::Parser parser, ast::Span statement_span) noexcept{
  auto name_pos = predicates::Parser_advance(parser);
  auto var_name = predicates::TokenKind_ident(predicates::Parser_kind(name_pos));
  auto type_parsed = (predicates::TokenKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? (types::parse_type(predicates::Parser_advance_by(name_pos, 2))) : (predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), predicates::Parser_advance(name_pos))));
  auto type_expr = type_parsed.value;
  auto value_parsed = parse_expr(predicates::Parser_advance(type_parsed.parser));
  auto expression = value_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLetConst{var_name, type_expr, expression, statement_span}), value_parsed.parser);
}
ast_tokens::TokenKind parser_next_kind(predicates::Parser p) noexcept{
  if (((p.position + 1) >= p.tokens.length()))   {
    return ast_tokens::Eof{};
  } else   {
    return p.tokens[(p.position + 1)].kind;
  }
}
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_after_let_pattern(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_colon(predicates::Parser_kind(parser)))   {
    auto type_parsed = types::parse_type(predicates::Parser_advance(parser));
    auto type_expr = type_parsed.value;
    if (predicates::TokenKind_is_equal(predicates::Parser_kind(type_parsed.parser)))     {
      return predicates::type_parse_result(type_expr, predicates::Parser_advance(type_parsed.parser));
    } else     {
      return predicates::type_parse_result(type_expr, type_parsed.parser);
    }
  } else if (predicates::TokenKind_is_equal(predicates::Parser_kind(parser)))   {
    return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), predicates::Parser_advance(parser));
  } else   {
    return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_array_pattern(predicates::Parser parser, ast::Span bracket_span) noexcept{
  if (predicates::TokenKind_is_rbracket(predicates::Parser_kind(parser)))   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternArray{{}, mlc::String("", 0), bracket_span}), predicates::Parser_advance(parser));
  } else   {
    auto first = parse_pattern(parser);
    auto patterns = mlc::Array<std::shared_ptr<ast::Pattern>>{};
    patterns.push_back(first.value);
    auto parser_state = first.parser;
    auto rest = mlc::String("", 0);
    while (predicates::TokenKind_is_comma(predicates::Parser_kind(parser_state)))     {
      (parser_state = predicates::Parser_advance(parser_state));
      if (predicates::TokenKind_is_spread(predicates::Parser_kind(parser_state)))       {
        (parser_state = predicates::Parser_advance(parser_state));
        if (predicates::TokenKind_is_ident(predicates::Parser_kind(parser_state)))         {
          (rest = predicates::TokenKind_ident(predicates::Parser_kind(parser_state)));
          (parser_state = predicates::Parser_advance(parser_state));
        }
        break;
      } else       {
        auto next = parse_pattern(parser_state);
        patterns.push_back(next.value);
        (parser_state = next.parser);
      }
    }
    if (predicates::TokenKind_is_rbracket(predicates::Parser_kind(parser_state)))     {
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternArray{patterns, rest, bracket_span}), predicates::Parser_advance(parser_state));
    } else     {
      auto error_parser = predicates::Parser_record_parse_error(parser_state, mlc::String("parse: expected ] in array pattern", 34));
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternArray{patterns, rest, bracket_span}), error_parser);
    }
  }
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let_pat_finish(std::shared_ptr<ast::Pattern> pattern, bool is_mut, predicates::Parser after_pattern, ast::Span statement_span) noexcept{
  auto type_parsed = parse_after_let_pattern(after_pattern);
  auto type_expr = type_parsed.value;
  auto value_parsed = parse_expr(type_parsed.parser);
  auto expression = value_parsed.value;
  auto has_else = predicates::TokenKind_is_else(predicates::Parser_kind(value_parsed.parser));
  auto else_parsed = [&]() -> predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> {
  if (has_else)   {
    return parse_statements_until_end(predicates::Parser_advance(value_parsed.parser));
  } else   {
    return [&]() {
auto empty_statements = mlc::Array<std::shared_ptr<ast::Stmt>>{};
return predicates::statements_parse_result(empty_statements, value_parsed.parser);
}();
  }
}();
  auto block_statements = else_parsed.value;
  auto else_body = std::make_shared<ast::Expr>(ast::ExprBlock{block_statements, std::make_shared<ast::Expr>(ast::ExprUnit{statement_span}), statement_span});
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLetPattern{pattern, is_mut, type_expr, expression, has_else, else_body, statement_span}), else_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let_mutable_body(predicates::Parser name_pos, bool is_mut, ast::Span statement_span) noexcept{
  auto k0 = predicates::Parser_kind(name_pos);
  if (predicates::TokenKind_is_lparen(k0))   {
    auto paren_span = predicates::Parser_span_at_cursor(name_pos);
    auto inner = parse_pattern_args(predicates::Parser_advance(name_pos));
    auto pattern = std::make_shared<ast::Pattern>(ast::PatternTuple{inner.value, paren_span});
    return parse_statement_let_pat_finish(pattern, is_mut, inner.parser, statement_span);
  }
  if (predicates::TokenKind_is_lbracket(k0))   {
    auto bracket_span = predicates::Parser_span_at_cursor(name_pos);
    auto array_pattern_parsed = parse_array_pattern(predicates::Parser_advance(name_pos), bracket_span);
    return parse_statement_let_pat_finish(array_pattern_parsed.value, is_mut, array_pattern_parsed.parser, statement_span);
  }
  if (predicates::TokenKind_is_lbrace(k0))   {
    auto brace_span = predicates::Parser_span_at_cursor(name_pos);
    auto record_fields_parsed = parse_record_pattern_fields(predicates::Parser_advance(name_pos));
    auto pattern = std::make_shared<ast::Pattern>(ast::PatternRecord{mlc::String("", 0), record_fields_parsed.value, brace_span});
    return parse_statement_let_pat_finish(pattern, is_mut, record_fields_parsed.parser, statement_span);
  }
  if (((predicates::TokenKind_is_ident(k0) && predicates::is_ctor_name(predicates::TokenKind_ident(k0))) && predicates::TokenKind_is_lparen(parser_next_kind(name_pos))))   {
    auto pattern_branch_parsed = parse_pattern_identifier_branch(name_pos, predicates::TokenKind_ident(k0));
    return parse_statement_let_pat_finish(pattern_branch_parsed.value, is_mut, pattern_branch_parsed.parser, statement_span);
  }
  if (predicates::TokenKind_is_ident(k0))   {
    auto variable_name = predicates::TokenKind_ident(k0);
    auto after_name = predicates::Parser_advance(name_pos);
    if (predicates::TokenKind_is_colon(predicates::Parser_kind(after_name)))     {
      auto type_parsed = types::parse_type(predicates::Parser_advance(after_name));
      auto value_parsed = parse_expr(predicates::Parser_advance(type_parsed.parser));
      return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLet{variable_name, is_mut, type_parsed.value, value_parsed.value, statement_span}), value_parsed.parser);
    }
    auto value_parsed = parse_expr(predicates::Parser_advance(after_name));
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLet{variable_name, is_mut, std::make_shared<ast::TypeExpr>(ast::TyUnit{}), value_parsed.value, statement_span}), value_parsed.parser);
  }
  auto error_parser = predicates::Parser_record_parse_error(name_pos, mlc::String("parse: expected pattern or identifier after let", 47));
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{std::make_shared<ast::Expr>(ast::ExprUnit{predicates::Parser_span_at_cursor(name_pos)}), statement_span}), error_parser);
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let(predicates::Parser parser) noexcept{
  auto statement_span = predicates::Parser_span_at_cursor(parser);
  auto after_let = predicates::Parser_advance(parser);
  if (predicates::TokenKind_is_const(predicates::Parser_kind(after_let)))   {
    return parse_statement_let_const(after_let, statement_span);
  }
  auto is_mut = predicates::TokenKind_is_mut(predicates::Parser_kind(after_let));
  auto name_pos = (is_mut ? (predicates::Parser_advance(after_let)) : (after_let));
  return parse_statement_let_mutable_body(name_pos, is_mut, statement_span);
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_const(predicates::Parser parser) noexcept{
  auto statement_span = predicates::Parser_span_at_cursor(parser);
  auto name_pos = predicates::Parser_advance(parser);
  auto var_name = predicates::TokenKind_ident(predicates::Parser_kind(name_pos));
  auto type_parsed = (predicates::TokenKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? (types::parse_type(predicates::Parser_advance_by(name_pos, 2))) : (predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), predicates::Parser_advance(name_pos))));
  auto type_expr = type_parsed.value;
  auto value_parsed = parse_expr(predicates::Parser_advance(type_parsed.parser));
  auto expression = value_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLet{var_name, false, type_expr, expression, statement_span}), value_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_return(predicates::Parser parser) noexcept{
  auto statement_span = predicates::Parser_span_at_cursor(parser);
  auto value_parsed = parse_expr(predicates::Parser_advance(parser));
  auto expression = value_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtReturn{expression, statement_span}), value_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_expression_or_assign(predicates::Parser parser) noexcept{
  auto statement_span = predicates::Parser_span_at_cursor(parser);
  auto left_parsed = parse_expr(parser);
  auto expression = left_parsed.value;
  if (predicates::TokenKind_is_equal(predicates::Parser_kind(left_parsed.parser)))   {
    auto assign_span = predicates::Parser_span_at_cursor(left_parsed.parser);
    auto right_parsed = parse_expr(predicates::Parser_advance(left_parsed.parser));
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("=", 1), expression, right_parsed.value, assign_span}), statement_span}), right_parsed.parser);
  } else   {
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{expression, statement_span}), left_parsed.parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if (predicates::TokenKind_is_let(kind))   {
    return parse_statement_let(parser);
  } else if (predicates::TokenKind_is_const(kind))   {
    return parse_statement_const(parser);
  } else if (predicates::TokenKind_is_break(kind))   {
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtBreak{predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_continue(kind))   {
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtContinue{predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_return(kind))   {
    return parse_statement_return(parser);
  } else if (predicates::TokenKind_is_while(kind))   {
    auto header_span = predicates::Parser_span_at_cursor(parser);
    auto while_parsed = parse_while_expr(predicates::Parser_advance(parser), header_span);
    auto expression = while_parsed.value;
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{expression, header_span}), while_parsed.parser);
  } else if (predicates::TokenKind_is_for(kind))   {
    auto header_span = predicates::Parser_span_at_cursor(parser);
    auto for_parsed = parse_for_expr(predicates::Parser_advance(parser), header_span);
    auto expression = for_parsed.value;
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{expression, header_span}), for_parsed.parser);
  } else if (predicates::TokenKind_is_do(kind))   {
    auto header_span = predicates::Parser_span_at_cursor(parser);
    auto block_parsed = parse_block(predicates::Parser_advance(parser), header_span);
    auto expression = block_parsed.value;
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{expression, header_span}), block_parsed.parser);
  } else if (predicates::TokenKind_is_with(kind))   {
    auto header_span = predicates::Parser_span_at_cursor(parser);
    auto with_parsed = parse_with_expr(predicates::Parser_advance(parser), header_span);
    auto expression = with_parsed.value;
    return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr{expression, header_span}), with_parsed.parser);
  } else   {
    return parse_statement_expression_or_assign(parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_identifier_branch(predicates::Parser parser, mlc::String name) noexcept{
  auto binding_span = predicates::Parser_span_at_cursor(parser);
  if ((name == mlc::String("_", 1)))   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild{binding_span}), predicates::Parser_advance(parser));
  } else if (predicates::is_ctor_name(name))   {
    auto after = predicates::Parser_advance(parser);
    if (predicates::TokenKind_is_lparen(predicates::Parser_kind(after)))     {
      auto sub_patterns = parse_pattern_args(predicates::Parser_advance(after));
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternCtor{name, sub_patterns.value, binding_span}), sub_patterns.parser);
    } else if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(after)))     {
      auto fields_parsed = parse_record_pattern_fields(predicates::Parser_advance(after));
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternRecord{name, fields_parsed.value, binding_span}), fields_parsed.parser);
    } else     {
      return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternCtor{name, {}, binding_span}), after);
    }
  } else   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternIdent{name, binding_span}), predicates::Parser_advance(parser));
  }
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_integer(predicates::Parser parser, int value) noexcept{
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternInt{value, predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_string(predicates::Parser parser, mlc::String value) noexcept{
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternStringLit{value, predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_boolean(predicates::Parser parser, bool value) noexcept{
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternBool{value, predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_fallback_wildcard(predicates::Parser parser) noexcept{
  auto error_parser = predicates::Parser_record_parse_error(parser, mlc::String("parse: unexpected token in pattern", 34));
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild{predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(error_parser));
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_or_pattern(predicates::Parser parser) noexcept{
  auto first = parse_pattern(parser);
  auto span = predicates::Parser_span_at_cursor(parser);
  auto alts = mlc::Array<std::shared_ptr<ast::Pattern>>{first.value};
  auto state = first.parser;
  while (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))   {
    auto after_bar = predicates::Parser_advance(state);
    if ((predicates::Parser_at_eof(after_bar) || predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(after_bar))))     {
      (state = after_bar);
    } else     {
      auto next = parse_pattern(after_bar);
      alts.push_back(next.value);
      (state = next.parser);
    }
  }
  if ((alts.length() == 1))   {
    return first;
  } else   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternOr{alts, span}), state);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if (predicates::TokenKind_is_ident(kind))   {
    return parse_pattern_identifier_branch(parser, predicates::TokenKind_ident(kind));
  } else if (predicates::TokenKind_is_int(kind))   {
    return parse_pattern_integer(parser, predicates::TokenKind_int_val(kind));
  } else if (predicates::TokenKind_is_str(kind))   {
    return parse_pattern_string(parser, predicates::TokenKind_str_val(kind));
  } else if (predicates::TokenKind_is_true(kind))   {
    return parse_pattern_boolean(parser, true);
  } else if (predicates::TokenKind_is_false(kind))   {
    return parse_pattern_boolean(parser, false);
  } else if (predicates::TokenKind_is_else(kind))   {
    return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild{predicates::Parser_span_at_cursor(parser)}), predicates::Parser_advance(parser));
  } else   {
    return parse_pattern_fallback_wildcard(parser);
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> parse_pattern_args(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)))   {
    auto empty_patterns = mlc::Array<std::shared_ptr<ast::Pattern>>{};
    return predicates::patterns_parse_result(empty_patterns, predicates::Parser_advance(parser));
  } else   {
    auto suffix = comma_separated::comma_separated_pats_suffix_from_first(parse_pattern(parser), parse_pattern);
    return predicates::patterns_parse_result(suffix.value, expect_close::parser_expect_rparen(suffix.parser, mlc::String("pattern list", 12)));
  }
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> parse_record_pattern_fields(predicates::Parser parser) noexcept{
  auto patterns = mlc::Array<std::shared_ptr<ast::Pattern>>{};
  auto state = parser;
  while (((!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)))     {
      patterns.push_back(std::make_shared<ast::Pattern>(ast::PatternIdent{predicates::TokenKind_ident(predicates::Parser_kind(state)), predicates::Parser_span_at_cursor(state)}));
    }
    (state = predicates::Parser_advance(state));
    if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
    }
  }
  return predicates::patterns_parse_result(patterns, expect_close::parser_expect_rbrace(state, mlc::String("record pattern fields", 21)));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_expr(predicates::Parser parser) noexcept{
  return parse_or(parser);
}
std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expression, std::shared_ptr<ast::Expr> right_expression) noexcept{
  return [&]() -> std::shared_ptr<ast::Expr> {
auto __match_subject = right_expression;
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, existing_args, __2] = exprCall; return std::make_shared<ast::Expr>(ast::ExprCall{callee, mlc::Array<std::shared_ptr<ast::Expr>>{left_expression}.concat(existing_args), ast::span_unknown()});
}
return [&]() {
auto call_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expression};
return std::make_shared<ast::Expr>(ast::ExprCall{right_expression, call_args, ast::span_unknown()});
}();
std::abort();
}();
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_pipe(predicates::Parser parser) noexcept{
  auto left = parse_comparison_relational(parser);
  auto expression = left.value;
  auto state = left.parser;
  while (predicates::TokenKind_is_pipe(predicates::Parser_kind(state)))   {
    auto right = parse_comparison_relational(predicates::Parser_advance(state));
    (expression = pipe_desugar(expression, right.value));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_or(predicates::Parser parser) noexcept{
  auto left = parse_and(parser);
  auto expression = left.value;
  auto state = left.parser;
  while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("||", 2))))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_and(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("||", 2), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_and(predicates::Parser parser) noexcept{
  auto left = parse_bitwise_or(parser);
  auto expression = left.value;
  auto state = left.parser;
  while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("&&", 2))))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_bitwise_or(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("&&", 2), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_and_no_pipe(predicates::Parser parser) noexcept{
  auto left = parse_bitwise_xor(parser);
  auto expression = left.value;
  auto state = left.parser;
  while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("&&", 2))))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_bitwise_xor(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("&&", 2), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_or_no_pipe(predicates::Parser parser) noexcept{
  auto left = parse_and_no_pipe(parser);
  auto expression = left.value;
  auto state = left.parser;
  while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("||", 2))))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_and_no_pipe(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("||", 2), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_bitwise_or(predicates::Parser parser) noexcept{
  auto left = parse_bitwise_xor(parser);
  auto expression = left.value;
  auto state = left.parser;
  while (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_bitwise_xor(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("|", 1), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_bitwise_xor(predicates::Parser parser) noexcept{
  auto left = parse_bitwise_and(parser);
  auto expression = left.value;
  auto state = left.parser;
  while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("^", 1))))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_bitwise_and(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("^", 1), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_bitwise_and(predicates::Parser parser) noexcept{
  auto left = parse_equality(parser);
  auto expression = left.value;
  auto state = left.parser;
  while ((predicates::TokenKind_is_op(predicates::Parser_kind(state)) && (predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("&", 1))))   {
    auto operator_span = predicates::Parser_span_at_cursor(state);
    auto right = parse_equality(predicates::Parser_advance(state));
    (expression = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("&", 1), expression, right.value, operator_span}));
    (state = right.parser);
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_equality(predicates::Parser parser) noexcept{
  auto left = parse_pipe(parser);
  auto expression = left.value;
  auto state = left.parser;
  auto go = true;
  while (go)   {
    auto kind = predicates::Parser_kind(state);
    if ((predicates::TokenKind_is_op(kind) && ((predicates::TokenKind_op_val(kind) == mlc::String("==", 2)) || (predicates::TokenKind_op_val(kind) == mlc::String("!=", 2)))))     {
      auto operator_span = predicates::Parser_span_at_cursor(state);
      auto operator_value = predicates::TokenKind_op_val(kind);
      auto right = parse_pipe(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprBin{operator_value, expression, right.value, operator_span}));
      (state = right.parser);
    } else     {
      (go = false);
    }
  }
  return predicates::expression_parse_result(expression, state);
}
bool is_relational_comparison_operator(mlc::String operator_) noexcept{
  return ((((operator_ == mlc::String("<", 1)) || (operator_ == mlc::String(">", 1))) || (operator_ == mlc::String("<=", 2))) || (operator_ == mlc::String(">=", 2)));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_comparison_relational(predicates::Parser parser) noexcept{
  auto left = parse_shift(parser);
  auto expression = left.value;
  auto state = left.parser;
  auto go = true;
  while (go)   {
    auto kind = predicates::Parser_kind(state);
    if ((predicates::TokenKind_is_op(kind) && is_relational_comparison_operator(predicates::TokenKind_op_val(kind))))     {
      auto operator_span = predicates::Parser_span_at_cursor(state);
      auto operator_value = predicates::TokenKind_op_val(kind);
      auto right = parse_shift(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprBin{operator_value, expression, right.value, operator_span}));
      (state = right.parser);
    } else     {
      (go = false);
    }
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_shift(predicates::Parser parser) noexcept{
  auto left = parse_add(parser);
  auto expression = left.value;
  auto state = left.parser;
  auto go = true;
  while (go)   {
    auto kind = predicates::Parser_kind(state);
    if ((predicates::TokenKind_is_op(kind) && ((predicates::TokenKind_op_val(kind) == mlc::String("<<", 2)) || (predicates::TokenKind_op_val(kind) == mlc::String(">>", 2)))))     {
      auto operator_span = predicates::Parser_span_at_cursor(state);
      auto operator_value = predicates::TokenKind_op_val(kind);
      auto right = parse_add(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprBin{operator_value, expression, right.value, operator_span}));
      (state = right.parser);
    } else     {
      (go = false);
    }
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_add(predicates::Parser parser) noexcept{
  auto left = parse_mul(parser);
  auto expression = left.value;
  auto state = left.parser;
  auto go = true;
  while (go)   {
    auto kind = predicates::Parser_kind(state);
    if ((predicates::TokenKind_is_op(kind) && ((predicates::TokenKind_op_val(kind) == mlc::String("+", 1)) || (predicates::TokenKind_op_val(kind) == mlc::String("-", 1)))))     {
      auto operator_span = predicates::Parser_span_at_cursor(state);
      auto right = parse_mul(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprBin{predicates::TokenKind_op_val(kind), expression, right.value, operator_span}));
      (state = right.parser);
    } else     {
      (go = false);
    }
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_mul(predicates::Parser parser) noexcept{
  auto left = parse_unary(parser);
  auto expression = left.value;
  auto state = left.parser;
  auto go = true;
  while (go)   {
    auto kind = predicates::Parser_kind(state);
    if ((predicates::TokenKind_is_op(kind) && (((predicates::TokenKind_op_val(kind) == mlc::String("*", 1)) || (predicates::TokenKind_op_val(kind) == mlc::String("/", 1))) || (predicates::TokenKind_op_val(kind) == mlc::String("%", 1)))))     {
      auto operator_span = predicates::Parser_span_at_cursor(state);
      auto right = parse_unary(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprBin{predicates::TokenKind_op_val(kind), expression, right.value, operator_span}));
      (state = right.parser);
    } else     {
      (go = false);
    }
  }
  return predicates::expression_parse_result(expression, state);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_unary(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if (predicates::TokenKind_is_move(kind))   {
    auto operator_span = predicates::Parser_span_at_cursor(parser);
    auto inner = parse_unary(predicates::Parser_advance(parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUn{mlc::String("move", 4), inner.value, operator_span}), inner.parser);
  } else if ((predicates::TokenKind_is_op(kind) && ((((predicates::TokenKind_op_val(kind) == mlc::String("!", 1)) || (predicates::TokenKind_op_val(kind) == mlc::String("-", 1))) || (predicates::TokenKind_op_val(kind) == mlc::String("~", 1))) || (predicates::TokenKind_op_val(kind) == mlc::String("+", 1)))))   {
    auto operator_span = predicates::Parser_span_at_cursor(parser);
    auto inner = parse_unary(predicates::Parser_advance(parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUn{predicates::TokenKind_op_val(kind), inner.value, operator_span}), inner.parser);
  } else   {
    return parse_postfix(parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_postfix_conditional(predicates::Parser parser, std::shared_ptr<ast::Expr> expression) noexcept{
  auto expression_line_number = ast::expr_span(expression).line;
  auto kind = predicates::Parser_kind(parser);
  if ((!(predicates::TokenKind_is_if(kind) || predicates::TokenKind_is_unless(kind))))   {
    return predicates::expression_parse_result(expression, parser);
  }
  if ((predicates::Parser_span_at_cursor(parser).line != expression_line_number))   {
    return predicates::expression_parse_result(expression, parser);
  }
  auto header_span = predicates::Parser_span_at_cursor(parser);
  auto is_negated = predicates::TokenKind_is_unless(kind);
  auto condition_parsed = parse_or(predicates::Parser_advance(parser));
  auto condition = (is_negated ? (std::make_shared<ast::Expr>(ast::ExprUn{mlc::String("!", 1), condition_parsed.value, ast::span_unknown()})) : (condition_parsed.value));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf{condition, expression, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), header_span}), condition_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_postfix(predicates::Parser parser) noexcept{
  auto base = parse_primary(parser);
  auto expression = base.value;
  auto state = base.parser;
  auto go = true;
  while (go)   {
    auto kind = predicates::Parser_kind(state);
    if (predicates::TokenKind_is_dot(kind))     {
      auto dot_span = predicates::Parser_span_at_cursor(state);
      auto field_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(state)));
      if ((predicates::TokenKind_is_lparen(predicates::Parser_kind(predicates::Parser_advance_by(state, 2))) && (predicates::Parser_span_at_cursor(predicates::Parser_advance_by(state, 2)).line == predicates::Parser_span_at_cursor(predicates::Parser_advance_by(state, 1)).line)))       {
        auto margs = parse_arg_list(predicates::Parser_advance_by(state, 3));
        (expression = std::make_shared<ast::Expr>(ast::ExprMethod{expression, field_name, margs.value, dot_span}));
        (state = margs.parser);
      } else       {
        (expression = std::make_shared<ast::Expr>(ast::ExprField{expression, field_name, dot_span}));
        (state = predicates::Parser_advance_by(state, 2));
      }
    } else if ((predicates::TokenKind_is_lparen(kind) && (predicates::Parser_span_at_cursor(state).line == predicates::Parser_prev_line(state))))     {
      auto call_span = predicates::Parser_span_at_cursor(state);
      auto call_args = parse_arg_list(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprCall{expression, call_args.value, call_span}));
      (state = call_args.parser);
    } else if (predicates::TokenKind_is_lbracket(kind))     {
      auto bracket_span = predicates::Parser_span_at_cursor(state);
      auto index = parse_expr(predicates::Parser_advance(state));
      (expression = std::make_shared<ast::Expr>(ast::ExprIndex{expression, index.value, bracket_span}));
      (state = expect_close::parser_expect_rbracket(index.parser, mlc::String("index expression", 16)));
    } else if (predicates::TokenKind_is_question(kind))     {
      auto question_span = predicates::Parser_span_at_cursor(state);
      (expression = std::make_shared<ast::Expr>(ast::ExprQuestion{expression, question_span}));
      (state = predicates::Parser_advance(state));
    } else     {
      (go = false);
    }
  }
  return parse_postfix_conditional(state, expression);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> parse_arg_list(predicates::Parser parser) noexcept{
  auto expressions = mlc::Array<std::shared_ptr<ast::Expr>>{};
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)))   {
    return predicates::expressions_parse_result(expressions, predicates::Parser_advance(parser));
  } else   {
    auto state = parser;
    while (((!predicates::TokenKind_is_rparen(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))     {
      auto arg_span = predicates::Parser_span_at_cursor(state);
      auto argument = [&]() -> std::shared_ptr<ast::Expr> {
  if ((predicates::TokenKind_is_ident(predicates::Parser_kind(state)) && predicates::TokenKind_is_colon(parser_next_kind(state))))   {
    return [&]() {
auto name = predicates::TokenKind_ident(predicates::Parser_kind(state));
auto value = parse_expr(predicates::Parser_advance_by(state, 2));
(state = value.parser);
return std::make_shared<ast::Expr>(ast::ExprNamedArg{name, value.value, arg_span});
}();
  } else   {
    return [&]() {
auto value_parsed = parse_expr(state);
(state = value_parsed.parser);
return value_parsed.value;
}();
  }
}();
      expressions.push_back(argument);
      if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))       {
        (state = predicates::Parser_advance(state));
      }
    }
    (state = expect_close::parser_expect_rparen(state, mlc::String("argument list", 13)));
    return predicates::expressions_parse_result(expressions, state);
  }
}
std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept{
  auto result = std::make_shared<ast::Expr>(ast::ExprStr{mlc::String("", 0), ast::span_unknown()});
  auto pi = 0;
  while ((pi < parts.length()))   {
    auto part_expr = (((pi % 2) == 0) ? (std::make_shared<ast::Expr>(ast::ExprStr{parts[pi], ast::span_unknown()})) : ([&]() -> std::shared_ptr<ast::Expr> {
  if ((parts[pi].length() == 0))   {
    return std::make_shared<ast::Expr>(ast::ExprStr{mlc::String("", 0), ast::span_unknown()});
  } else   {
    return [&]() {
auto sub_lex = lexer::tokenize(parts[pi]);
auto sub_parsed = parse_expr(predicates::parser_new(sub_lex.tokens));
return std::make_shared<ast::Expr>(ast::ExprMethod{sub_parsed.value, mlc::String("to_string", 9), {}, ast::span_unknown()});
}();
  }
}()));
    if ((pi == 0))     {
      (result = part_expr);
    } else     {
      (result = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("+", 1), result, part_expr, ast::span_unknown()}));
    }
    (pi = (pi + 1));
  }
  return result;
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_integer_literal(predicates::Parser parser, int value) noexcept{
  auto source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprInt{value, source_span}), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_string_literal(predicates::Parser parser, mlc::String value) noexcept{
  auto source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprStr{value, source_span}), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_template_literal(predicates::Parser parser, mlc::Array<mlc::String> parts) noexcept{
  return predicates::expression_parse_result(build_template_expr(parts), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_boolean_literal(predicates::Parser parser, bool value) noexcept{
  auto source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprBool{value, source_span}), predicates::Parser_advance(parser));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_parenthesized(predicates::Parser parser) noexcept{
  auto open_paren_span = predicates::Parser_span_at_cursor(parser);
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(predicates::Parser_advance(parser))))   {
    auto after_close = predicates::Parser_advance_by(parser, 2);
    if ((predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(after_close)) && (!predicates::Parser_lambda_shorthand_suppression_active(parser))))     {
      auto params = mlc::Array<mlc::String>{};
      auto body = parse_expr(predicates::Parser_advance_by(parser, 3));
      return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda{params, body.value, open_paren_span}), body.parser);
    } else     {
      return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUnit{open_paren_span}), after_close);
    }
  } else if (((!predicates::Parser_lambda_shorthand_suppression_active(parser)) && looks_like_typed_lambda_params(predicates::Parser_advance(parser))))   {
    auto params_parsed = parse_typed_lambda_params(predicates::Parser_advance(parser));
    auto body = parse_expr(predicates::Parser_advance(params_parsed.parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda{params_parsed.value, body.value, open_paren_span}), body.parser);
  } else if (((!predicates::Parser_lambda_shorthand_suppression_active(parser)) && looks_like_lambda_params(predicates::Parser_advance(parser))))   {
    auto params_parsed = parse_lambda_params(predicates::Parser_advance(parser));
    auto body = parse_expr(predicates::Parser_advance(params_parsed.parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda{params_parsed.value, body.value, open_paren_span}), body.parser);
  } else   {
    auto first_expression_parsed = parse_expr(predicates::Parser_advance(parser));
    if (predicates::TokenKind_is_comma(predicates::Parser_kind(first_expression_parsed.parser)))     {
      auto suffix = comma_separated::comma_separated_exprs_suffix_from_first(first_expression_parsed, parse_expr);
      if (predicates::TokenKind_is_rparen(predicates::Parser_kind(suffix.parser)))       {
        return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprTuple{suffix.value, open_paren_span}), predicates::Parser_advance(suffix.parser));
      } else       {
        auto error_parser = predicates::Parser_record_parse_error(suffix.parser, mlc::String("parse: expected ) in tuple literal", 34));
        return predicates::expression_parse_result(first_expression_parsed.value, error_parser);
      }
    } else     {
      auto closed_parser = expect_close::parser_expect_rparen(first_expression_parsed.parser, mlc::String("parenthesized expression", 24));
      return predicates::expression_parse_result(first_expression_parsed.value, closed_parser);
    }
  }
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_if_or_unless(predicates::Parser parser) noexcept{
  return parse_if_expr(parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_do_block(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  return parse_block(predicates::Parser_advance(parser), header_span);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_while_loop(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  return parse_while_expr(predicates::Parser_advance(parser), header_span);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_for_loop(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  return parse_for_expr(predicates::Parser_advance(parser), header_span);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_spawn_expr(predicates::Parser parser, ast::Span header_span) noexcept{
  if (predicates::TokenKind_is_do(predicates::Parser_kind(parser)))   {
    auto body_parsed = parse_statements_until_end(predicates::Parser_advance(parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprSpawn{body_parsed.value, header_span}), body_parsed.parser);
  } else   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUnit{header_span}), predicates::Parser_record_parse_error(parser, mlc::String("spawn: expected do", 18)));
  }
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_spawn(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  return parse_spawn_expr(predicates::Parser_advance(parser), header_span);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_match(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  return parse_match_expr(predicates::Parser_advance(parser), header_span);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_return_as_block(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  auto value = parse_expr(predicates::Parser_advance(parser));
  auto statements = mlc::Array<std::shared_ptr<ast::Stmt>>{};
  statements.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn{value.value, header_span}));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprBlock{statements, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), header_span}), value.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_identifier(predicates::Parser parser, mlc::String name) noexcept{
  auto source_span = predicates::Parser_span_at_cursor(parser);
  auto after_name = predicates::Parser_advance(parser);
  if ((predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(after_name)) && (!predicates::Parser_lambda_shorthand_suppression_active(parser))))   {
    auto parameters = mlc::Array<mlc::String>{name};
    auto body = parse_expr(predicates::Parser_advance(after_name));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda{parameters, body.value, source_span}), body.parser);
  } else if ((predicates::is_ctor_name(name) && predicates::TokenKind_is_lbrace(predicates::Parser_kind(after_name))))   {
    auto brace_span = predicates::Parser_span_at_cursor(after_name);
    return parse_record_expr(predicates::Parser_advance(after_name), name, brace_span);
  } else   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIdent{name, source_span}), after_name);
  }
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_unit_fallback(predicates::Parser parser) noexcept{
  auto source_span = predicates::Parser_span_at_cursor(parser);
  auto error_parser = predicates::Parser_record_parse_error(parser, mlc::String("parse: unexpected token in expression", 37));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUnit{source_span}), predicates::Parser_advance(error_parser));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if (predicates::TokenKind_is_float(kind))   {
    auto source_span = predicates::Parser_span_at_cursor(parser);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprFloat{predicates::TokenKind_float_val(kind), source_span}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_i64(kind))   {
    auto source_span = predicates::Parser_span_at_cursor(parser);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprI64{predicates::TokenKind_i64_val(kind), source_span}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_u8(kind))   {
    auto source_span = predicates::Parser_span_at_cursor(parser);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprU8{predicates::TokenKind_u8_val(kind), source_span}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_usize(kind))   {
    auto source_span = predicates::Parser_span_at_cursor(parser);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUsize{predicates::TokenKind_usize_val(kind), source_span}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_char_lit(kind))   {
    auto source_span = predicates::Parser_span_at_cursor(parser);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprChar{predicates::TokenKind_char_val(kind), source_span}), predicates::Parser_advance(parser));
  } else if (predicates::TokenKind_is_int(kind))   {
    return parse_primary_integer_literal(parser, predicates::TokenKind_int_val(kind));
  } else if (predicates::TokenKind_is_str(kind))   {
    return parse_primary_string_literal(parser, predicates::TokenKind_str_val(kind));
  } else if (predicates::TokenKind_is_template(kind))   {
    return parse_primary_template_literal(parser, predicates::TokenKind_template_parts(kind));
  } else if (predicates::TokenKind_is_true(kind))   {
    return parse_primary_boolean_literal(parser, true);
  } else if (predicates::TokenKind_is_false(kind))   {
    return parse_primary_boolean_literal(parser, false);
  } else if (predicates::TokenKind_is_lparen(kind))   {
    return parse_primary_parenthesized(parser);
  } else if (predicates::TokenKind_is_lbracket(kind))   {
    auto bracket_span = predicates::Parser_span_at_cursor(parser);
    return parse_array_lit(predicates::Parser_advance(parser), bracket_span);
  } else if ((predicates::TokenKind_is_if(kind) || predicates::TokenKind_is_unless(kind)))   {
    return parse_primary_if_or_unless(parser);
  } else if (predicates::TokenKind_is_do(kind))   {
    return parse_primary_do_block(parser);
  } else if (predicates::TokenKind_is_while(kind))   {
    return parse_primary_while_loop(parser);
  } else if (predicates::TokenKind_is_for(kind))   {
    return parse_primary_for_loop(parser);
  } else if (predicates::TokenKind_is_with(kind))   {
    auto header_span = predicates::Parser_span_at_cursor(parser);
    return parse_with_expr(predicates::Parser_advance(parser), header_span);
  } else if (predicates::TokenKind_is_match(kind))   {
    return parse_primary_match(parser);
  } else if (predicates::TokenKind_is_spawn(kind))   {
    return parse_primary_spawn(parser);
  } else if (predicates::TokenKind_is_return(kind))   {
    return parse_primary_return_as_block(parser);
  } else if (predicates::TokenKind_is_ident(kind))   {
    return parse_primary_identifier(parser, predicates::TokenKind_ident(kind));
  } else   {
    return parse_primary_unit_fallback(parser);
  }
}
bool looks_like_typed_lambda_params(predicates::Parser parser) noexcept{
  return (predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && predicates::TokenKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(parser))));
}
predicates::ParseResult<mlc::Array<mlc::String>> parse_typed_lambda_params(predicates::Parser parser) noexcept{
  auto names = mlc::Array<mlc::String>{};
  auto state = parser;
  names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
  (state = types::parse_type(predicates::Parser_advance_by(state, 2)).parser);
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    (state = predicates::Parser_advance(state));
    names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
    (state = types::parse_type(predicates::Parser_advance_by(state, 2)).parser);
  }
  return predicates::bounds_parse_result(names, predicates::Parser_advance(state));
}
bool looks_like_lambda_params(predicates::Parser parser) noexcept{
  auto state = parser;
  auto ok = true;
  if ((!predicates::TokenKind_is_ident(predicates::Parser_kind(state))))   {
    (ok = false);
  } else   {
    (state = predicates::Parser_advance(state));
    while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
      if ((!predicates::TokenKind_is_ident(predicates::Parser_kind(state))))       {
        (ok = false);
        (state = predicates::Parser_advance(state));
      } else       {
        (state = predicates::Parser_advance(state));
      }
    }
    if ((!predicates::TokenKind_is_rparen(predicates::Parser_kind(state))))     {
      (ok = false);
    } else     {
      (ok = predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(predicates::Parser_advance(state))));
    }
  }
  return ok;
}
predicates::ParseResult<mlc::Array<mlc::String>> parse_lambda_params(predicates::Parser parser) noexcept{
  auto names = mlc::Array<mlc::String>{};
  auto state = parser;
  names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
  (state = predicates::Parser_advance(state));
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    (state = predicates::Parser_advance(state));
    names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
    (state = predicates::Parser_advance(state));
  }
  return predicates::bounds_parse_result(names, predicates::Parser_advance(state));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_array_lit(predicates::Parser parser, ast::Span header_span) noexcept{
  if (predicates::TokenKind_is_rbracket(predicates::Parser_kind(parser)))   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprArray{{}, header_span}), predicates::Parser_advance(parser));
  } else   {
    auto suffix = comma_separated::comma_separated_exprs_suffix_from_first(parse_expr(parser), parse_expr);
    auto closed_parser = expect_close::parser_expect_rbracket(suffix.parser, mlc::String("array literal", 13));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprArray{suffix.value, header_span}), closed_parser);
  }
}
std::shared_ptr<ast::Expr> if_expression_condition(bool is_negated, std::shared_ptr<ast::Expr> expression) noexcept{
  if (is_negated)   {
    return std::make_shared<ast::Expr>(ast::ExprUn{mlc::String("!", 1), expression, ast::span_unknown()});
  } else   {
    return expression;
  }
}
predicates::Parser parser_after_optional_end(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_end(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_if_expr(predicates::Parser parser) noexcept{
  auto header_span = predicates::Parser_span_at_cursor(parser);
  auto is_negated = predicates::TokenKind_is_unless(predicates::Parser_kind(parser));
  auto cond_parsed = parse_expr(predicates::Parser_advance(parser));
  auto expression = cond_parsed.value;
  auto condition = if_expression_condition(is_negated, expression);
  auto then_parsed = parse_statements_until_else_end(predicates::Parser_advance(cond_parsed.parser));
  auto then_expr = statements_result_to_block_expr(then_parsed, header_span);
  if ((!predicates::TokenKind_is_else(predicates::Parser_kind(then_parsed.parser))))   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), header_span}), predicates::Parser_advance(then_parsed.parser));
  }
  auto after_else = predicates::Parser_advance(then_parsed.parser);
  if ((predicates::TokenKind_is_if(predicates::Parser_kind(after_else)) || predicates::TokenKind_is_unless(predicates::Parser_kind(after_else))))   {
    auto else_parsed = parse_if_expr(after_else);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, else_parsed.value, header_span}), else_parsed.parser);
  }
  if (predicates::TokenKind_is_do(predicates::Parser_kind(after_else)))   {
    auto else_block_parsed = parse_block(predicates::Parser_advance(after_else), header_span);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, else_block_parsed.value, header_span}), parser_after_optional_end(else_block_parsed.parser));
  }
  auto else_keyword_line = predicates::Parser_prev_line(after_else);
  auto else_body_start_line = predicates::Parser_span_at_cursor(after_else).line;
  if ((else_body_start_line > else_keyword_line))   {
    auto else_span = predicates::Parser_span_at_cursor(after_else);
    auto else_stmts_parsed = parse_statements_until_end_eof_or_toplevel_decl(after_else);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, statements_result_to_block_expr(else_stmts_parsed, else_span), header_span}), else_stmts_parsed.parser);
  }
  auto else_parsed = parse_expr(after_else);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, else_parsed.value, header_span}), parser_after_optional_end(else_parsed.parser));
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_block(predicates::Parser parser, ast::Span header_span) noexcept{
  auto statements_parsed = parse_statements_until_end(parser);
  return predicates::expression_parse_result(statements_result_to_block_expr(statements_parsed, header_span), statements_parsed.parser);
}
bool is_unit_expr(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return true;
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [__0, result, __2] = exprBlock; return is_unit_expr(result);
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [__0, __1, else_expression, __3] = exprIf; return is_unit_expr(else_expression);
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [op, __1, __2, __3] = exprBin; return (op == mlc::String("=", 1));
}
return false;
std::abort();
}();
}
bool is_stmt_expr(std::shared_ptr<ast::Stmt> statement) noexcept{
  return [&]() -> bool {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [__0, __1] = stmtExpr; return true;
}
return false;
std::abort();
}();
}
std::shared_ptr<ast::Expr> stmt_expr_expression(std::shared_ptr<ast::Stmt> statement) noexcept{
  return [&]() -> std::shared_ptr<ast::Expr> {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return expression;
}
return std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()});
std::abort();
}();
}
std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
  if ((stmts.length() == 0))   {
    return std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()});
  }
  auto last = stmts[(stmts.length() - 1)];
  if ((!is_stmt_expr(last)))   {
    return std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()});
  }
  auto expression = stmt_expr_expression(last);
  if (is_unit_expr(expression))   {
    return std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()});
  }
  return expression;
}
mlc::Array<std::shared_ptr<ast::Stmt>> block_body_without_last_statement(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
  auto result = mlc::Array<std::shared_ptr<ast::Stmt>>{};
  auto index = 0;
  while ((index < (stmts.length() - 1)))   {
    result.push_back(stmts[index]);
    (index = (index + 1));
  }
  return result;
}
mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
  if ((stmts.length() == 0))   {
    return stmts;
  }
  auto last = stmts[(stmts.length() - 1)];
  if ((!is_stmt_expr(last)))   {
    return stmts;
  }
  auto expression = stmt_expr_expression(last);
  if (is_unit_expr(expression))   {
    return stmts;
  }
  return block_body_without_last_statement(stmts);
}
std::shared_ptr<ast::Expr> statements_result_to_block_expr(predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> statements_result, ast::Span header_span) noexcept{
  return std::make_shared<ast::Expr>(ast::ExprBlock{block_body(statements_result.value), block_result(statements_result.value), header_span});
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_while_expr(predicates::Parser parser, ast::Span header_span) noexcept{
  auto cond_parsed = parse_expr(parser);
  auto expression = cond_parsed.value;
  auto body_parsed = parse_statements_until_end(predicates::Parser_advance(cond_parsed.parser));
  auto block_statements = body_parsed.value;
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprWhile{expression, block_statements, header_span}), body_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_for_expr(predicates::Parser parser, ast::Span header_span) noexcept{
  auto var_name = predicates::TokenKind_ident(predicates::Parser_kind(parser));
  auto iter_parsed = parse_expr(predicates::Parser_advance_by(parser, 2));
  auto expression = iter_parsed.value;
  auto body_parsed = parse_statements_until_end(predicates::Parser_advance(iter_parsed.parser));
  auto block_statements = body_parsed.value;
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprFor{var_name, expression, block_statements, header_span}), body_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_with_expr(predicates::Parser parser, ast::Span header_span) noexcept{
  auto resource_parsed = parse_expr(parser);
  auto expression = resource_parsed.value;
  auto after_as = predicates::Parser_advance(resource_parsed.parser);
  auto binder = predicates::TokenKind_ident(predicates::Parser_kind(after_as));
  auto body_parsed = parse_statements_until_end(predicates::Parser_advance(predicates::Parser_advance(after_as)));
  auto block_statements = body_parsed.value;
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprWith{expression, binder, block_statements, header_span}), body_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_match_scrutinee(predicates::Parser parser) noexcept{
  return parse_comparison_relational(parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_match_expr(predicates::Parser parser, ast::Span header_span) noexcept{
  auto subject_parsed = parse_match_scrutinee(parser);
  auto expression = subject_parsed.value;
  auto next = predicates::Parser_kind(subject_parsed.parser);
  if (predicates::TokenKind_is_lbrace(next))   {
    auto arms_parsed = parse_arms_brace(predicates::Parser_advance(subject_parsed.parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprMatch{expression, arms_parsed.value, header_span}), arms_parsed.parser);
  } else if (predicates::TokenKind_is_do(next))   {
    auto arms_parsed = parse_arms_do_delimited(predicates::Parser_advance(subject_parsed.parser));
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprMatch{expression, arms_parsed.value, header_span}), arms_parsed.parser);
  } else   {
    auto arms_parsed = parse_arms_pipe(subject_parsed.parser);
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprMatch{expression, arms_parsed.value, header_span}), arms_parsed.parser);
  }
}
predicates::ParseResult<std::shared_ptr<ast::MatchArm>> match_arm_parse_result(std::shared_ptr<ast::MatchArm> arm, predicates::Parser parser) noexcept{
  return predicates::parse_result(arm, parser);
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_match_arm_body(predicates::Parser parser) noexcept{
  auto parsed = parse_or_no_pipe(parser);
  return parse_postfix_conditional(parsed.parser, parsed.value);
}
predicates::ParseResult<std::shared_ptr<ast::MatchArm>> parse_match_arm(predicates::ParseResult<std::shared_ptr<ast::Pattern>> pat_parsed) noexcept{
  auto after_pattern = pat_parsed.parser;
  auto has_guard = false;
  auto guard_expression = std::make_shared<ast::Expr>(ast::ExprBool{true, ast::span_unknown()});
  if (predicates::TokenKind_is_if(predicates::Parser_kind(after_pattern)))   {
    auto guard_parsed = parse_or_no_pipe(predicates::Parser_with_lambda_shorthand_suppressed(predicates::Parser_advance(after_pattern), true));
    auto expression = guard_parsed.value;
    (guard_expression = expression);
    (has_guard = true);
    (after_pattern = guard_parsed.parser);
  }
  auto after_fat_arrow = predicates::Parser_advance(after_pattern);
  auto body_parsed = parse_match_arm_body(after_fat_arrow);
  auto expression = body_parsed.value;
  return match_arm_parse_result(std::make_shared<ast::MatchArm>(ast::MatchArm{pat_parsed.value, has_guard, guard_expression, expression}), body_parsed.parser);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms_brace(predicates::Parser parser) noexcept{
  auto arms = mlc::Array<std::shared_ptr<ast::MatchArm>>{};
  auto state = parser;
  while (((!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    if (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
    }
    auto pat_parsed = parse_or_pattern(state);
    auto arm_parsed = parse_match_arm(pat_parsed);
    arms.push_back(arm_parsed.value);
    (state = arm_parsed.parser);
    if (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
    }
  }
  (state = expect_close::parser_expect_rbrace(state, mlc::String("match arms", 10)));
  return predicates::arms_parse_result(arms, state);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms_do_delimited(predicates::Parser parser) noexcept{
  auto arms = mlc::Array<std::shared_ptr<ast::MatchArm>>{};
  auto state = predicates::Parser_skip_semi(parser);
  while (((!predicates::TokenKind_is_end(predicates::Parser_kind(state))) && (!predicates::Parser_at_eof(state))))   {
    if (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))     {
      (state = predicates::Parser_advance(state));
    }
    auto pat_parsed = parse_or_pattern(state);
    auto arm_parsed = parse_match_arm(pat_parsed);
    arms.push_back(arm_parsed.value);
    (state = predicates::Parser_skip_semi(arm_parsed.parser));
  }
  return predicates::arms_parse_result(arms, parser_after_optional_end(state));
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms_pipe(predicates::Parser parser) noexcept{
  auto arms = mlc::Array<std::shared_ptr<ast::MatchArm>>{};
  auto state = parser;
  while (predicates::TokenKind_is_bar(predicates::Parser_kind(state)))   {
    auto pat_parsed = parse_or_pattern(predicates::Parser_advance(state));
    auto arm_parsed = parse_match_arm(pat_parsed);
    arms.push_back(arm_parsed.value);
    (state = arm_parsed.parser);
  }
  if (predicates::TokenKind_is_end(predicates::Parser_kind(state)))   {
    (state = predicates::Parser_advance(state));
  }
  return predicates::arms_parse_result(arms, state);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms(predicates::Parser parser) noexcept{
  return parse_arms_brace(parser);
}
predicates::Parser skip_comma_if_present(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_comma(predicates::Parser_kind(parser)))   {
    return predicates::Parser_advance(parser);
  } else   {
    return parser;
  }
}
mlc::Array<ast::RecordLitPart> lit_parts_flush_fields(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals) noexcept{
  if ((field_vals.length() == 0))   {
    return lit_parts;
  } else   {
    return lit_parts.concat(mlc::Array<ast::RecordLitPart>{ast::RecordLitFields{field_vals}});
  }
}
mlc::Array<std::shared_ptr<ast::FieldVal>> empty_record_lit_field_vals() noexcept{
  auto empty_field_vals = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
  return empty_field_vals;
}
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_field_with_value(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, predicates::Parser after_colon) noexcept{
  auto field_value_parsed = parse_expr(after_colon);
  auto field_vals_after = current_field_vals.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, field_value_parsed.value})});
  return parse_record_lit_parts_loop(skip_comma_if_present(field_value_parsed.parser), lit_parts, field_vals_after);
}
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_field_shorthand(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, ast::Span field_span, predicates::Parser after_field_name) noexcept{
  auto field_vals_after = current_field_vals.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent{field_name, field_span})})});
  return parse_record_lit_parts_loop(skip_comma_if_present(after_field_name), lit_parts, field_vals_after);
}
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_field(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept{
  auto field_span = predicates::Parser_span_at_cursor(state);
  auto field_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
  auto after_field_name = predicates::Parser_advance(state);
  if (predicates::TokenKind_is_colon(predicates::Parser_kind(after_field_name)))   {
    return parse_record_lit_field_with_value(state, lit_parts, current_field_vals, field_name, predicates::Parser_advance(after_field_name));
  } else   {
    return parse_record_lit_field_shorthand(state, lit_parts, current_field_vals, field_name, field_span, after_field_name);
  }
}
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_parts_loop(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept{
  if ((predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) || predicates::Parser_at_eof(state)))   {
    auto flushed_lit_parts = lit_parts_flush_fields(lit_parts, current_field_vals);
    auto closed_parser = expect_close::parser_expect_rbrace(state, mlc::String("record literal", 14));
    return predicates::record_lit_parts_parse_result(flushed_lit_parts, closed_parser);
  } else if (predicates::TokenKind_is_spread(predicates::Parser_kind(state)))   {
    auto lit_parts_after_flush = lit_parts_flush_fields(lit_parts, current_field_vals);
    auto spread_parsed = parse_expr(predicates::Parser_advance(state));
    auto lit_parts_after_spread = lit_parts_after_flush.concat(mlc::Array<ast::RecordLitPart>{ast::RecordLitSpread{spread_parsed.value}});
    return parse_record_lit_parts_loop(skip_comma_if_present(spread_parsed.parser), lit_parts_after_spread, empty_record_lit_field_vals());
  } else   {
    return parse_record_lit_field(state, lit_parts, current_field_vals);
  }
}
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_parts(predicates::Parser parser) noexcept{
  auto empty_lit_parts = mlc::Array<ast::RecordLitPart>{};
  return parse_record_lit_parts_loop(parser, empty_lit_parts, empty_record_lit_field_vals());
}
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_record_expr(predicates::Parser parser, mlc::String record_name, ast::Span record_span) noexcept{
  auto parsed_lit_parts = parse_record_lit_parts(parser);
  auto lit_parts = parsed_lit_parts.value;
  auto after_record_body = parsed_lit_parts.parser;
  if ((lit_parts.length() == 2))   {
    auto first_part = lit_parts[0];
    auto second_part = lit_parts[1];
    return std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) -> predicates::ParseResult<std::shared_ptr<ast::Expr>> { auto [base_expression] = recordLitSpread; return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) -> predicates::ParseResult<std::shared_ptr<ast::Expr>> { auto [override_fields] = recordLitFields; return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecordUpdate{record_name, base_expression, override_fields, record_span}), after_record_body); },
[&](const ast::RecordLitSpread& recordLitSpread) -> predicates::ParseResult<std::shared_ptr<ast::Expr>> { auto [__0] = recordLitSpread; return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecord{record_name, lit_parts, record_span}), after_record_body); }
}, second_part); },
[&](const ast::RecordLitFields& recordLitFields) -> predicates::ParseResult<std::shared_ptr<ast::Expr>> { auto [__0] = recordLitFields; return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecord{record_name, lit_parts, record_span}), after_record_body); }
}, first_part);
  } else   {
    return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecord{record_name, lit_parts, record_span}), after_record_body);
  }
}

} // namespace exprs
