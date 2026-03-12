#ifndef DECLS_HPP
#define DECLS_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"
#include "types.hpp"
#include "exprs.hpp"

namespace decls {

preds::DeclResult parse_import_decl(preds::Parser parser) noexcept;
preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept;
preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept;
preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept;
preds::DeclResult parse_decl(preds::Parser parser) noexcept;
preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept;
preds::ParamsResult parse_params(preds::Parser parser) noexcept;
preds::ParamResult parse_param(preds::Parser parser) noexcept;
preds::DeclResult parse_type_decl(preds::Parser parser) noexcept;
bool is_decl_start(ast_tokens::TKind kind) noexcept;
bool parse_variants_continue(preds::Parser state) noexcept;
preds::VariantsResult parse_variants(preds::Parser parser) noexcept;
preds::VariantResult parse_variant(preds::Parser parser) noexcept;
preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept;
preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept;
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

} // namespace decls

#endif // DECLS_HPP
