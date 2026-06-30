#include "predicates.hpp"

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace predicates {

using namespace ast_tokens;
using namespace ast;
using namespace ast_tokens;

predicates::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

predicates::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;

predicates::ParseResult<mlc::Array<mlc::String>> bounds_parse_result(mlc::Array<mlc::String> bounds, predicates::Parser parser) noexcept;

predicates::TypeResult type_parse_result(std::shared_ptr<ast::TypeExpr> type_expression, predicates::Parser parser) noexcept;

predicates::PatternResult pattern_parse_result(std::shared_ptr<ast::Pattern> pattern, predicates::Parser parser) noexcept;

predicates::StmtResult statement_parse_result(std::shared_ptr<ast::Stmt> statement, predicates::Parser parser) noexcept;

predicates::VariantResult variant_parse_result(std::shared_ptr<ast::TypeVariant> variant, predicates::Parser parser) noexcept;

predicates::StmtsResult statements_parse_result(mlc::Array<std::shared_ptr<ast::Stmt>> statements, predicates::Parser parser) noexcept;

predicates::ExprsResult expressions_parse_result(mlc::Array<std::shared_ptr<ast::Expr>> expressions, predicates::Parser parser) noexcept;

predicates::ArmsResult arms_parse_result(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, predicates::Parser parser) noexcept;

predicates::VariantsResult variants_parse_result(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, predicates::Parser parser) noexcept;

predicates::ParamsResult parameters_parse_result(mlc::Array<std::shared_ptr<ast::Param>> parameters, predicates::Parser parser) noexcept;

predicates::FieldValsResult field_values_parse_result(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, predicates::Parser parser) noexcept;

predicates::RecordLitPartsResult record_lit_parts_parse_result(mlc::Array<ast::RecordLitPart> lit_parts, predicates::Parser parser) noexcept;

predicates::PatternsResult patterns_parse_result(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, predicates::Parser parser) noexcept;

predicates::FieldDefsResult field_definitions_parse_result(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, predicates::Parser parser) noexcept;

predicates::TypesResult types_parse_result(mlc::Array<std::shared_ptr<ast::TypeExpr>> types, predicates::Parser parser) noexcept;

predicates::DeclResult declaration_parse_result(std::shared_ptr<ast::Decl> declaration, predicates::Parser parser) noexcept;

predicates::WhereClauseParseResult where_clause_parse_result(mlc::Array<ast::WhereClauseBound> where_bounds, predicates::Parser parser) noexcept;

predicates::TraitBodyResult trait_body_parse_result(mlc::Array<std::shared_ptr<ast::Decl>> methods, predicates::Parser parser) noexcept;

predicates::ExprResult expression_parse_result(std::shared_ptr<ast::Expr> expression, predicates::Parser parser) noexcept;

predicates::TypeParamsResult type_params_parse_result(mlc::Array<mlc::String> params, mlc::Array<mlc::Array<mlc::String>> bounds, predicates::Parser parser) noexcept;

predicates::ProgramParseValue program_parse_result(ast::Program program, mlc::Array<mlc::String> errors) noexcept;

bool is_ctor_name(mlc::String name) noexcept;

bool TokenKind_is_fn(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_type(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_let(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_mut(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_const(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_if(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_unless(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_else(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_end(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_do(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_while(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_for(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_with(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_spawn(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_match(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_return(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_break(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_continue(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_true(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_false(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_extern(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_extend(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_where(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_import(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_as(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_from(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_eof(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_ident(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_int(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_str(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_template(ast_tokens::TokenKind self) noexcept;

mlc::Array<mlc::String> TokenKind_template_parts(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_equal(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_bar(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_pipe(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_colon(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_comma(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_dot(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_spread(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_arrow(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_fat_arrow(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_question(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_lparen(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_rparen(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_lbrace(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_rbrace(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_lbracket(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_rbracket(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_op(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_ident(ast_tokens::TokenKind self) noexcept;

int TokenKind_int_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_str_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_op_val(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_float(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_i64(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_u8(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_usize(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_char_lit(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_float_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_i64_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_u8_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_usize_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_char_val(ast_tokens::TokenKind self) noexcept;

ast_tokens::TokenKind Parser_kind(predicates::Parser self) noexcept;

bool Parser_lambda_shorthand_suppression_active(predicates::Parser self) noexcept;

predicates::Parser Parser_with_lambda_shorthand_suppressed(predicates::Parser self, bool value) noexcept;

predicates::Parser Parser_record_parse_error(predicates::Parser self, mlc::String message) noexcept;

bool Parser_has_parse_errors(predicates::Parser self) noexcept;

predicates::Parser Parser_advance(predicates::Parser self) noexcept;

predicates::Parser Parser_advance_by(predicates::Parser self, int count) noexcept;

bool Parser_at_eof(predicates::Parser self) noexcept;

predicates::Parser Parser_skip_semi(predicates::Parser self) noexcept;

ast::Span Parser_span_at_cursor(predicates::Parser self) noexcept;

int Parser_prev_line(predicates::Parser self) noexcept;

predicates::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{return predicates::Parser{tokens, 0, source_path, false, {}};}

predicates::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept{return parser_new_with_source_path(tokens, mlc::String(""));}

predicates::ParseResult<mlc::Array<mlc::String>> bounds_parse_result(mlc::Array<mlc::String> bounds, predicates::Parser parser) noexcept{return parse_result<mlc::Array<mlc::String>>(bounds, parser);}

predicates::TypeResult type_parse_result(std::shared_ptr<ast::TypeExpr> type_expression, predicates::Parser parser) noexcept{return parse_result<std::shared_ptr<ast::TypeExpr>>(type_expression, parser);}

predicates::PatternResult pattern_parse_result(std::shared_ptr<ast::Pattern> pattern, predicates::Parser parser) noexcept{return parse_result<std::shared_ptr<ast::Pattern>>(pattern, parser);}

predicates::StmtResult statement_parse_result(std::shared_ptr<ast::Stmt> statement, predicates::Parser parser) noexcept{return parse_result<std::shared_ptr<ast::Stmt>>(statement, parser);}

predicates::VariantResult variant_parse_result(std::shared_ptr<ast::TypeVariant> variant, predicates::Parser parser) noexcept{return parse_result<std::shared_ptr<ast::TypeVariant>>(variant, parser);}

predicates::StmtsResult statements_parse_result(mlc::Array<std::shared_ptr<ast::Stmt>> statements, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::Stmt>>>(statements, parser);}

predicates::ExprsResult expressions_parse_result(mlc::Array<std::shared_ptr<ast::Expr>> expressions, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::Expr>>>(expressions, parser);}

predicates::ArmsResult arms_parse_result(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::MatchArm>>>(arms, parser);}

predicates::VariantsResult variants_parse_result(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::TypeVariant>>>(variants, parser);}

predicates::ParamsResult parameters_parse_result(mlc::Array<std::shared_ptr<ast::Param>> parameters, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::Param>>>(parameters, parser);}

predicates::FieldValsResult field_values_parse_result(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::FieldVal>>>(field_values, parser);}

predicates::RecordLitPartsResult record_lit_parts_parse_result(mlc::Array<ast::RecordLitPart> lit_parts, predicates::Parser parser) noexcept{return parse_result<mlc::Array<ast::RecordLitPart>>(lit_parts, parser);}

predicates::PatternsResult patterns_parse_result(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::Pattern>>>(patterns, parser);}

predicates::FieldDefsResult field_definitions_parse_result(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::FieldDef>>>(field_definitions, parser);}

predicates::TypesResult types_parse_result(mlc::Array<std::shared_ptr<ast::TypeExpr>> types, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::TypeExpr>>>(types, parser);}

predicates::DeclResult declaration_parse_result(std::shared_ptr<ast::Decl> declaration, predicates::Parser parser) noexcept{return parse_result<std::shared_ptr<ast::Decl>>(declaration, parser);}

predicates::WhereClauseParseResult where_clause_parse_result(mlc::Array<ast::WhereClauseBound> where_bounds, predicates::Parser parser) noexcept{return parse_result<mlc::Array<ast::WhereClauseBound>>(where_bounds, parser);}

predicates::TraitBodyResult trait_body_parse_result(mlc::Array<std::shared_ptr<ast::Decl>> methods, predicates::Parser parser) noexcept{return parse_result<mlc::Array<std::shared_ptr<ast::Decl>>>(methods, parser);}

predicates::ExprResult expression_parse_result(std::shared_ptr<ast::Expr> expression, predicates::Parser parser) noexcept{return parse_result<std::shared_ptr<ast::Expr>>(expression, parser);}

predicates::TypeParamsResult type_params_parse_result(mlc::Array<mlc::String> params, mlc::Array<mlc::Array<mlc::String>> bounds, predicates::Parser parser) noexcept{return parse_result<predicates::TypeParamsValue>(predicates::TypeParamsValue{params, bounds}, parser);}

predicates::ProgramParseValue program_parse_result(ast::Program program, mlc::Array<mlc::String> errors) noexcept{return predicates::ProgramParseValue{program, errors};}

bool is_ctor_name(mlc::String name) noexcept{return name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");}

bool TokenKind_is_fn(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFn>(self)) {  return true; } return false; }();}

bool TokenKind_is_type(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KType>(self)) {  return true; } return false; }();}

bool TokenKind_is_let(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KLet>(self)) {  return true; } return false; }();}

bool TokenKind_is_mut(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KMut>(self)) {  return true; } return false; }();}

bool TokenKind_is_const(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KConst>(self)) {  return true; } return false; }();}

bool TokenKind_is_if(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KIf>(self)) {  return true; } return false; }();}

bool TokenKind_is_unless(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KUnless>(self)) {  return true; } return false; }();}

bool TokenKind_is_else(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KElse>(self)) {  return true; } return false; }();}

bool TokenKind_is_end(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KEnd>(self)) {  return true; } return false; }();}

bool TokenKind_is_do(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KDo>(self)) {  return true; } return false; }();}

bool TokenKind_is_while(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWhile>(self)) {  return true; } return false; }();}

bool TokenKind_is_for(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFor>(self)) {  return true; } return false; }();}

bool TokenKind_is_with(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWith>(self)) {  return true; } return false; }();}

bool TokenKind_is_spawn(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KSpawn>(self)) {  return true; } return false; }();}

bool TokenKind_is_match(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KMatch>(self)) {  return true; } return false; }();}

bool TokenKind_is_return(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KReturn>(self)) {  return true; } return false; }();}

bool TokenKind_is_break(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KBreak>(self)) {  return true; } return false; }();}

bool TokenKind_is_continue(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KContinue>(self)) {  return true; } return false; }();}

bool TokenKind_is_true(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KTrue>(self)) {  return true; } return false; }();}

bool TokenKind_is_false(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFalse>(self)) {  return true; } return false; }();}

bool TokenKind_is_extern(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KExtern>(self)) {  return true; } return false; }();}

bool TokenKind_is_extend(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KExtend>(self)) {  return true; } return false; }();}

bool TokenKind_is_where(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWhere>(self)) {  return true; } return false; }();}

bool TokenKind_is_import(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KImport>(self)) {  return true; } return false; }();}

bool TokenKind_is_as(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KAs>(self)) {  return true; } return false; }();}

bool TokenKind_is_from(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFrom>(self)) {  return true; } return false; }();}

bool TokenKind_is_eof(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Eof>(self)) {  return true; } return false; }();}

bool TokenKind_is_ident(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Ident>(self)) { auto _v_ident = std::get<ast_tokens::Ident>(self); auto [_w0] = _v_ident; return true; } return false; }();}

bool TokenKind_is_int(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LInt>(self)) { auto _v_lint = std::get<ast_tokens::LInt>(self); auto [_w0] = _v_lint; return true; } return false; }();}

bool TokenKind_is_str(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LStr>(self)) { auto _v_lstr = std::get<ast_tokens::LStr>(self); auto [_w0] = _v_lstr; return true; } return false; }();}

bool TokenKind_is_template(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LTemplate>(self)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(self); auto [_w0] = _v_ltemplate; return true; } return false; }();}

mlc::Array<mlc::String> TokenKind_template_parts(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast_tokens::LTemplate>(self)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(self); auto [parts] = _v_ltemplate; return parts; } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> r = {};
  return r;
 }(); }();}

bool TokenKind_is_equal(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Equal>(self)) {  return true; } return false; }();}

bool TokenKind_is_bar(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Bar>(self)) {  return true; } return false; }();}

bool TokenKind_is_pipe(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Pipe>(self)) {  return true; } return false; }();}

bool TokenKind_is_colon(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Colon>(self)) {  return true; } return false; }();}

bool TokenKind_is_comma(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Comma>(self)) {  return true; } return false; }();}

bool TokenKind_is_dot(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Dot>(self)) {  return true; } return false; }();}

bool TokenKind_is_spread(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Spread>(self)) {  return true; } return false; }();}

bool TokenKind_is_arrow(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Arrow>(self)) {  return true; } return false; }();}

bool TokenKind_is_fat_arrow(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::FatArrow>(self)) {  return true; } return false; }();}

bool TokenKind_is_question(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Question>(self)) {  return true; } return false; }();}

bool TokenKind_is_lparen(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LParen>(self)) {  return true; } return false; }();}

bool TokenKind_is_rparen(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RParen>(self)) {  return true; } return false; }();}

bool TokenKind_is_lbrace(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LBrace>(self)) {  return true; } return false; }();}

bool TokenKind_is_rbrace(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RBrace>(self)) {  return true; } return false; }();}

bool TokenKind_is_lbracket(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LBracket>(self)) {  return true; } return false; }();}

bool TokenKind_is_rbracket(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RBracket>(self)) {  return true; } return false; }();}

bool TokenKind_is_op(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Op>(self)) { auto _v_op = std::get<ast_tokens::Op>(self); auto [_w0] = _v_op; return true; } return false; }();}

mlc::String TokenKind_ident(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Ident>(self)) { auto _v_ident = std::get<ast_tokens::Ident>(self); auto [name] = _v_ident; return name; } return mlc::String(""); }();}

int TokenKind_int_val(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LInt>(self)) { auto _v_lint = std::get<ast_tokens::LInt>(self); auto [n] = _v_lint; return n; } return 0; }();}

mlc::String TokenKind_str_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LStr>(self)) { auto _v_lstr = std::get<ast_tokens::LStr>(self); auto [s] = _v_lstr; return s; } return mlc::String(""); }();}

mlc::String TokenKind_op_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Op>(self)) { auto _v_op = std::get<ast_tokens::Op>(self); auto [s] = _v_op; return s; } return mlc::String(""); }();}

bool TokenKind_is_float(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LFloat>(self)) { auto _v_lfloat = std::get<ast_tokens::LFloat>(self); auto [_w0] = _v_lfloat; return true; } return false; }();}

bool TokenKind_is_i64(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LI64>(self)) { auto _v_li64 = std::get<ast_tokens::LI64>(self); auto [_w0] = _v_li64; return true; } return false; }();}

bool TokenKind_is_u8(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LU8>(self)) { auto _v_lu8 = std::get<ast_tokens::LU8>(self); auto [_w0] = _v_lu8; return true; } return false; }();}

bool TokenKind_is_usize(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LUsize>(self)) { auto _v_lusize = std::get<ast_tokens::LUsize>(self); auto [_w0] = _v_lusize; return true; } return false; }();}

bool TokenKind_is_char_lit(ast_tokens::TokenKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LChar>(self)) { auto _v_lchar = std::get<ast_tokens::LChar>(self); auto [_w0] = _v_lchar; return true; } return false; }();}

mlc::String TokenKind_float_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LFloat>(self)) { auto _v_lfloat = std::get<ast_tokens::LFloat>(self); auto [s] = _v_lfloat; return s; } return mlc::String(""); }();}

mlc::String TokenKind_i64_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LI64>(self)) { auto _v_li64 = std::get<ast_tokens::LI64>(self); auto [s] = _v_li64; return s; } return mlc::String(""); }();}

mlc::String TokenKind_u8_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LU8>(self)) { auto _v_lu8 = std::get<ast_tokens::LU8>(self); auto [s] = _v_lu8; return s; } return mlc::String(""); }();}

mlc::String TokenKind_usize_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LUsize>(self)) { auto _v_lusize = std::get<ast_tokens::LUsize>(self); auto [s] = _v_lusize; return s; } return mlc::String(""); }();}

mlc::String TokenKind_char_val(ast_tokens::TokenKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LChar>(self)) { auto _v_lchar = std::get<ast_tokens::LChar>(self); auto [s] = _v_lchar; return s; } return mlc::String(""); }();}

ast_tokens::TokenKind Parser_kind(predicates::Parser self) noexcept{return self.position >= self.tokens.size() ? ast_tokens::TokenKind((ast_tokens::Eof{})) : ast_tokens::TokenKind([&]() -> ast_tokens::TokenKind { 
  ast_tokens::Token current_token = self.tokens[self.position];
  return current_token.kind;
 }());}

bool Parser_lambda_shorthand_suppression_active(predicates::Parser self) noexcept{return self.suppress_lambda_shorthand;}

predicates::Parser Parser_with_lambda_shorthand_suppressed(predicates::Parser self, bool value) noexcept{return predicates::Parser{self.tokens, self.position, self.source_path, value, self.errors};}

predicates::Parser Parser_record_parse_error(predicates::Parser self, mlc::String message) noexcept{
mlc::Array<mlc::String> updated_errors = self.errors;
updated_errors.push_back(message);
return predicates::Parser{self.tokens, self.position, self.source_path, self.suppress_lambda_shorthand, updated_errors};
}

bool Parser_has_parse_errors(predicates::Parser self) noexcept{return self.errors.size() > 0;}

predicates::Parser Parser_advance(predicates::Parser self) noexcept{return predicates::Parser{self.tokens, self.position + 1, self.source_path, self.suppress_lambda_shorthand, self.errors};}

predicates::Parser Parser_advance_by(predicates::Parser self, int count) noexcept{return predicates::Parser{self.tokens, self.position + count, self.source_path, self.suppress_lambda_shorthand, self.errors};}

bool Parser_at_eof(predicates::Parser self) noexcept{return self.position >= self.tokens.size() ? true : [&]() -> bool { 
  ast_tokens::Token current_token = self.tokens[self.position];
  return TokenKind_is_eof(current_token.kind);
 }();}

predicates::Parser Parser_skip_semi(predicates::Parser self) noexcept{return self.position >= self.tokens.size() ? self : [&]() -> predicates::Parser { 
  ast_tokens::Token current_token = self.tokens[self.position];
  return [&]() -> predicates::Parser { if (std::holds_alternative<ast_tokens::Semicolon>(current_token.kind)) {  return Parser_advance(self); } return self; }();
 }();}

ast::Span Parser_span_at_cursor(predicates::Parser self) noexcept{return self.position >= self.tokens.size() ? ast::span_make(self.source_path, 0, 0) : [&]() -> ast::Span { 
  ast_tokens::Token current_token = self.tokens[self.position];
  return ast::span_make(self.source_path, current_token.line, current_token.column);
 }();}

int Parser_prev_line(predicates::Parser self) noexcept{
return self.position > 0 ? [&]() -> int { 
  ast_tokens::Token current_token = self.tokens[self.position - 1];
  return current_token.line;
 }() : 0;
}

} // namespace predicates
