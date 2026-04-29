#ifndef DECLS_HPP
#define DECLS_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "ast.hpp"

namespace decls {

struct DeriveResult {mlc::Array<mlc::String> traits;preds::Parser parser;};
preds::BoundsResult parse_one_param_bounds(preds::Parser state) noexcept;
preds::TypeParamsResult parse_type_params_opt(preds::Parser parser) noexcept;
preds::DeclResult parse_import_decl(preds::Parser parser) noexcept;
preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept;
preds::Parser advance_past_optional_self_type(preds::Parser parser_after_self) noexcept;
preds::ParamsResult parse_extend_extern_no_self_params(preds::Parser after_lparen) noexcept;
preds::ParamsResult parse_extend_extern_rest_params(preds::Parser after_lparen, bool leading_self) noexcept;
preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept;
preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept;
preds::DeclResult parse_decl(preds::Parser parser) noexcept;
preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept;
preds::ParamsResult parse_params(preds::Parser parser) noexcept;
preds::ParamResult parse_param(preds::Parser parser) noexcept;
DeriveResult parse_derive_clause(preds::Parser parser) noexcept;
preds::DeclResult parse_type_decl(preds::Parser parser) noexcept;
bool is_decl_start(ast_tokens::TKind kind) noexcept;
bool parse_variants_continue(preds::Parser state) noexcept;
preds::VariantsResult parse_variants(preds::Parser parser) noexcept;
preds::VariantResult parse_variant(preds::Parser parser) noexcept;
preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept;
preds::TraitBodyResult parse_trait_body(preds::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept;
preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept;
ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

} // namespace decls

#endif // DECLS_HPP
