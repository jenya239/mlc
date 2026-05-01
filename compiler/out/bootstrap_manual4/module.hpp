#ifndef MODULE_HPP
#define MODULE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "transform_decl.hpp"
#include "decl_index.hpp"
#include "context.hpp"
#include "semantic_ir.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "decl.hpp"
#include "expr.hpp"

namespace module {

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SNamespaceImportAlias> items) noexcept;
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_struct_using_lines(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept;
context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept;
context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;
mlc::String gen_program(ast::Program program) noexcept;

} // namespace module

#endif // MODULE_HPP
