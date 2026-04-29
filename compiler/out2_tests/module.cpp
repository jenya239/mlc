#define main mlc_user_main
#include "module.hpp"

namespace module {

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SNamespaceImportAlias> items) noexcept{
auto result = mlc::Array<decl_index::NamespaceImportAlias>{};
auto index = 0;
while ((index < items.length())) {
auto entry = items[index];
result.push_back(decl_index::NamespaceImportAlias{entry.alias, entry.module_path});
index = (index + 1);
}
return result;
}
context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept{
return context::PrecomputedCtx{type_index::build_field_orders(prog), type_index::build_variant_types_from_decls(prog.decls), decl_index::build_item_index(all_items), ctor_info::build_ctor_type_infos_from_decls(prog.decls), type_index::build_generic_variants_from_decls(prog.decls)};
}
context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept{
auto base = cpp_naming::path_to_module_base(s_item.path);
auto namespace_aliases = s_item.namespace_import_aliases;
auto qualified = decl_index::build_qualified(s_item.imports, all_items);
auto namespace_alias_prefixes = decl_index::build_namespace_alias_prefixes(namespace_aliases_mapped(namespace_aliases));
auto context = context::CodegenContext{precomp.field_orders, mlc::String("", 0), qualified, namespace_alias_prefixes, mlc::String("", 0), type_index::build_method_owners_from_decls(full_prog.decls), {}, {}, {}, {}, precomp.ctor_type_infos, precomp.variant_types, {}, {}, precomp.generic_variants};
auto module_namespace = ((base == mlc::String("main", 4)) ? mlc::String("mlc_main", 8) : base);
auto is_entry = decl::decls_have_main(s_item.decls);
auto std_includes = ((expr::standard_translation_unit_runtime_headers() + cpp_naming::include_lines(s_item.imports)) + mlc::String("\n", 1));
auto type_fwds = decl::collect_decl_parts(s_item.decls, context, 0);
auto type_defs = decl::collect_decl_parts(s_item.decls, context, 1);
auto fn_protos = decl::collect_decl_parts(s_item.decls, context, 2);
auto fn_defs = decl::collect_decl_parts(s_item.decls, context, 3);
auto guard = (base.upper() + mlc::String("_HPP", 4));
auto header = mlc::Array{expr::include_guard_ifndef_line(guard), expr::include_guard_define_line(guard), mlc::String("\n", 1), std_includes, expr::namespace_open_line(module_namespace), mlc::String("\n", 1), type_fwds.join(mlc::String("", 0)), type_defs.join(mlc::String("", 0)), fn_protos.join(mlc::String("", 0)), mlc::String("\n", 1), expr::namespace_close_comment_line(module_namespace), mlc::String("\n", 1), expr::include_guard_endif_comment_line(guard)}.join(mlc::String("", 0));
auto cli_wrapper = (is_entry ? expr::bootstrap_host_main_calling_namespaced_user_main(module_namespace) : mlc::String("", 0));
auto impl = mlc::Array{expr::implementation_define_main_as_user_main_line(), expr::implementation_include_quotefile_line((base + mlc::String(".hpp", 4))), mlc::String("\n", 1), expr::namespace_open_line(module_namespace), mlc::String("\n", 1), fn_defs.join(mlc::String("", 0)), mlc::String("\n", 1), expr::namespace_close_comment_line(module_namespace), cli_wrapper}.join(mlc::String("", 0));
return context::GenModuleOut{header, impl};
}
mlc::String gen_program(ast::Program program) noexcept{
auto all_items = mlc::Array<decl_index::LoadItem>{};
auto precomp = precompute(program, all_items);
auto registry = registry::build_registry(program);
auto single = decl_index::LoadItem{mlc::String("test_main", 9), program.decls, {}, {}};
auto s_items = transform_decl::transform_load_items(mlc::Array{single}, registry);
auto result = gen_module(s_items[0], all_items, program, precomp);
return (result.h + result.c);
}

} // namespace module
