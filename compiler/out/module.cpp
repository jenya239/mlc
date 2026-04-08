#include "module.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "transform.hpp"
#include "decl_index.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "decl.hpp"
#include "expr.hpp"

namespace module {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace transform;
using namespace decl_index;
using namespace context;
using namespace cpp_naming;
using namespace decl;
using namespace expr;
using namespace ast_tokens;

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SNamespaceImportAlias> items) noexcept;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept;

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;

mlc::String gen_program(ast::Program program) noexcept;

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SNamespaceImportAlias> items) noexcept{
mlc::Array<decl_index::NamespaceImportAlias> result = {};
int index = 0;
while (index < items.size()){
{
semantic_ir::SNamespaceImportAlias entry = items[index];
result.push_back(decl_index::NamespaceImportAlias{entry.alias, entry.module_path});
index = index + 1;
}
}
return result;
}

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept{return context::PrecomputedCtx{decl_index::build_field_orders(prog), decl_index::build_variant_types_from_decls(prog.decls), decl_index::build_item_index(all_items), ctor_info::build_ctor_type_infos_from_decls(prog.decls), decl_index::build_generic_variants_from_decls(prog.decls)};}

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept{
mlc::String base = cpp_naming::path_to_module_base(s_item.path);
mlc::Array<semantic_ir::SNamespaceImportAlias> namespace_aliases = s_item.namespace_import_aliases;
mlc::HashMap<mlc::String, mlc::String> qualified = decl_index::build_qualified(s_item.imports, all_items);
mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes = decl_index::build_namespace_alias_prefixes(namespace_aliases_mapped(namespace_aliases));
context::CodegenContext context = context::CodegenContext{precomp.field_orders, mlc::String(""), qualified, namespace_alias_prefixes, mlc::String(""), decl_index::build_method_owners_from_decls(full_prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.variant_types, {}, {}, precomp.generic_variants};
mlc::String module_namespace = base == mlc::String("main") ? mlc::String("mlc_main") : base;
bool is_entry = decl::decls_have_main(s_item.decls);
mlc::String std_includes = expr::standard_translation_unit_runtime_headers() + cpp_naming::include_lines(s_item.imports) + mlc::String("\n");
mlc::Array<mlc::String> type_fwds = decl::collect_decl_parts(s_item.decls, context, 0);
mlc::Array<mlc::String> type_defs = decl::collect_decl_parts(s_item.decls, context, 1);
mlc::Array<mlc::String> fn_protos = decl::collect_decl_parts(s_item.decls, context, 2);
mlc::Array<mlc::String> fn_defs = decl::collect_decl_parts(s_item.decls, context, 3);
mlc::String guard = base.upper() + mlc::String("_HPP");
mlc::String header = mlc::Array<mlc::String>{expr::include_guard_ifndef_line(guard), expr::include_guard_define_line(guard), mlc::String("\n"), std_includes, expr::namespace_open_line(module_namespace), mlc::String("\n"), type_fwds.join(mlc::String("")), type_defs.join(mlc::String("")), fn_protos.join(mlc::String("")), mlc::String("\n"), expr::namespace_close_comment_line(module_namespace), mlc::String("\n"), expr::include_guard_endif_comment_line(guard)}.join(mlc::String(""));
mlc::String cli_wrapper = is_entry ? expr::bootstrap_host_main_calling_namespaced_user_main(module_namespace) : mlc::String("");
mlc::String impl = mlc::Array<mlc::String>{expr::implementation_define_main_as_user_main_line(), expr::implementation_include_quotefile_line(base + mlc::String(".hpp")), mlc::String("\n"), expr::namespace_open_line(module_namespace), mlc::String("\n"), fn_defs.join(mlc::String("")), mlc::String("\n"), expr::namespace_close_comment_line(module_namespace), cli_wrapper}.join(mlc::String(""));
return context::GenModuleOut{header, impl};
}

mlc::String gen_program(ast::Program program) noexcept{
mlc::Array<decl_index::LoadItem> all_items = {};
context::PrecomputedCtx precomp = precompute(program, all_items);
registry::TypeRegistry registry = registry::build_registry(program);
decl_index::LoadItem single = decl_index::LoadItem{mlc::String("test_main"), program.decls, {}, {}};
mlc::Array<semantic_ir::SLoadItem> s_items = transform::transform_load_items(mlc::Array<decl_index::LoadItem>{single}, registry);
context::GenModuleOut result = gen_module(s_items[0], all_items, program, precomp);
return result.h + result.c;
}

} // namespace module
