#include "module.hpp"

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

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace transform_decl;
using namespace decl_index;
using namespace context;
using namespace semantic_ir;
using namespace type_gen;
using namespace cpp_naming;
using namespace decl;
using namespace expr;
using namespace ast_tokens;

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SNamespaceImportAlias> items) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_struct_using_lines(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept;

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

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_struct_using_lines(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept{
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> result = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
int i = 0;
while (i < decls.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclExtend>((*decls[i]))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*decls[i])); auto [type_name, _w0, methods] = _v_sdeclextend; return [&]() -> std::tuple<> { 
  int j = 0;
  while (j < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclAssocBind>((*methods[j]))) { auto _v_sdeclassocbind = std::get<semantic_ir::SDeclAssocBind>((*methods[j])); auto [assoc_name, bound_type, _w0] = _v_sdeclassocbind; return [&]() -> std::tuple<> { 
  mlc::String line = mlc::String("using ") + assoc_name + mlc::String(" = ") + type_gen::sem_type_to_cpp(context, bound_type) + mlc::String(";\n");
  mlc::Array<mlc::String> existing = result.has(type_name) ? result.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
  existing.push_back(line);
  result.set(type_name, existing);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
j = j + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<semantic_ir::SDeclExported>((*decls[i]))) { auto _v_sdeclexported = std::get<semantic_ir::SDeclExported>((*decls[i])); auto [inner] = _v_sdeclexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclExtend>((*inner))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*inner)); auto [type_name, _w0, methods] = _v_sdeclextend; return [&]() -> std::tuple<> { 
  int j = 0;
  while (j < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclAssocBind>((*methods[j]))) { auto _v_sdeclassocbind = std::get<semantic_ir::SDeclAssocBind>((*methods[j])); auto [assoc_name, bound_type, _w0] = _v_sdeclassocbind; return [&]() -> std::tuple<> { 
  mlc::String line = mlc::String("using ") + assoc_name + mlc::String(" = ") + type_gen::sem_type_to_cpp(context, bound_type) + mlc::String(";\n");
  mlc::Array<mlc::String> existing = result.has(type_name) ? result.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
  existing.push_back(line);
  result.set(type_name, existing);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
j = j + 1;
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
i = i + 1;
}
}
return result;
}

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept{return context::PrecomputedCtx{type_index::build_field_orders(prog), type_index::build_variant_types_from_decls(prog.decls), decl_index::build_item_index(all_items), ctor_info::build_ctor_type_infos_from_decls(prog.decls), type_index::build_generic_variants_from_decls(prog.decls)};}

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept{
mlc::String base = cpp_naming::path_to_module_base(s_item.path);
mlc::Array<semantic_ir::SNamespaceImportAlias> namespace_aliases = s_item.namespace_import_aliases;
mlc::HashMap<mlc::String, mlc::String> qualified = decl_index::build_qualified(s_item.imports, all_items);
mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes = decl_index::build_namespace_alias_prefixes(namespace_aliases_mapped(namespace_aliases));
context::CodegenContext base_context = context::CodegenContext{precomp.field_orders, mlc::String(""), qualified, namespace_alias_prefixes, mlc::String(""), type_index::build_method_owners_from_decls(full_prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.variant_types, {}, {}, precomp.generic_variants, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>()};
context::CodegenContext context = context::context_with_struct_using_lines(base_context, build_struct_using_lines(s_item.decls, base_context));
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
mlc::Array<semantic_ir::SLoadItem> s_items = transform_decl::transform_load_items(mlc::Array<decl_index::LoadItem>{single}, registry);
context::GenModuleOut result = gen_module(s_items[0], all_items, program, precomp);
return result.h + result.c;
}

} // namespace module
