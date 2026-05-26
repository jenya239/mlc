#include "module.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "transform_decl.hpp"
#include "trait_param_expand.hpp"
#include "decl_index.hpp"
#include "ctor_info.hpp"
#include "context.hpp"
#include "semantic_ir.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "decl.hpp"
#include "decl_cpp.hpp"
#include "cpp_ast.hpp"
#include "cpp_ast.hpp"
#include "expr.hpp"

namespace module {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace transform_decl;
using namespace trait_param_expand;
using namespace decl_index;
using namespace ctor_info;
using namespace context;
using namespace semantic_ir;
using namespace type_gen;
using namespace cpp_naming;
using namespace decl;
using namespace decl_cpp;
using namespace cpp_ast;
using namespace cpp_ast;
using namespace expr;
using namespace ast_tokens;

struct ModuleGenerationContext {mlc::String base;mlc::String guard;mlc::String std_includes;mlc::String module_namespace;bool is_entry;decl::DeclPartsBundle decl_parts;mlc::String implementation_import_includes;mlc::String implementation_using_namespaces;mlc::String cli_wrapper;};

bool use_cpp_printer_default() noexcept;

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SNamespaceImportAlias> items) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_struct_using_lines(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept;

module::ModuleGenerationContext prepare_module_generation(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, context::PrecomputedCtx precomp) noexcept;

mlc::String assemble_header_string(module::ModuleGenerationContext parts) noexcept;

mlc::String assemble_source_string(module::ModuleGenerationContext parts) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_declarations_from_text_parts(mlc::Array<mlc::String> text_parts) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> assemble_header_cpp_declarations(module::ModuleGenerationContext parts) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> assemble_source_cpp_declarations(module::ModuleGenerationContext parts) noexcept;

std::shared_ptr<cpp_ast::CppFile> gen_module_cpp_file(module::ModuleGenerationContext parts) noexcept;

context::GenModuleOut gen_module_via_string(module::ModuleGenerationContext parts) noexcept;

context::GenModuleOut gen_module_via_cpp_printer(module::ModuleGenerationContext parts) noexcept;

context::GenModuleOut gen_module_with_printer(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp, bool use_cpp_printer) noexcept;

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;

mlc::String gen_program_with_printer(ast::Program program, bool use_cpp_printer) noexcept;

mlc::String gen_program(ast::Program program) noexcept;

bool use_cpp_printer_default() noexcept{return false;}

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

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders = type_index::build_field_orders(prog);
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos = ctor_info::build_ctor_type_infos_from_decls(prog.decls);
return context::PrecomputedCtx{field_orders, type_index::build_variant_types_from_decls(prog.decls), decl_index::build_item_index(all_items), ctor_type_infos, type_index::build_generic_variants_from_decls(prog.decls), type_index::build_method_owners_from_decls(prog.decls), decl_index::build_field_order_index(field_orders), ctor_info::build_ctor_type_info_index(ctor_type_infos)};
}

module::ModuleGenerationContext prepare_module_generation(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, context::PrecomputedCtx precomp) noexcept{
mlc::String base = cpp_naming::path_to_module_base(s_item.path);
mlc::Array<semantic_ir::SNamespaceImportAlias> namespace_aliases = s_item.namespace_import_aliases;
mlc::HashMap<mlc::String, mlc::String> qualified = decl_index::build_qualified(s_item.imports, all_items);
mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes = decl_index::build_namespace_alias_prefixes(namespace_aliases_mapped(namespace_aliases));
context::CodegenContext base_context = context::CodegenContext{precomp.field_orders, precomp.field_order_index, mlc::String(""), qualified, namespace_alias_prefixes, mlc::String(""), precomp.method_owners, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.ctor_type_info_index, precomp.variant_types, {}, {}, precomp.generic_variants, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>()};
context::CodegenContext context = context::CodegenContext_with_struct_using_lines(base_context, build_struct_using_lines(s_item.decls, base_context));
mlc::String module_namespace = base == mlc::String("main") ? mlc::String("mlc_main") : base;
bool is_entry = decl::decls_have_main(s_item.decls);
mlc::String std_includes = expr::standard_translation_unit_runtime_headers() + cpp_naming::include_lines(s_item.imports) + mlc::String("\n");
decl::DeclPartsBundle decl_parts = decl::collect_all_decl_parts(s_item.decls, context);
mlc::String guard = base.upper() + mlc::String("_HPP");
mlc::String cli_wrapper = is_entry ? expr::bootstrap_host_main_calling_namespaced_user_main(module_namespace) : mlc::String("");
return module::ModuleGenerationContext{base, guard, std_includes, module_namespace, is_entry, decl_parts, cpp_naming::include_lines(s_item.imports), cpp_naming::using_namespace_lines(s_item.imports), cli_wrapper};
}

mlc::String assemble_header_string(module::ModuleGenerationContext parts) noexcept{return mlc::Array<mlc::String>{expr::include_guard_ifndef_line(parts.guard), expr::include_guard_define_line(parts.guard), mlc::String("\n"), parts.std_includes, expr::namespace_open_line(parts.module_namespace), mlc::String("\n"), parts.decl_parts.type_fwds.join(mlc::String("")), parts.decl_parts.type_defs.join(mlc::String("")), parts.decl_parts.fn_protos.join(mlc::String("")), mlc::String("\n"), expr::namespace_close_comment_line(parts.module_namespace), mlc::String("\n"), expr::include_guard_endif_comment_line(parts.guard)}.join(mlc::String(""));}

mlc::String assemble_source_string(module::ModuleGenerationContext parts) noexcept{return mlc::Array<mlc::String>{expr::implementation_define_main_as_user_main_line(), expr::implementation_include_quotefile_line(parts.base + mlc::String(".hpp")), mlc::String("\n"), parts.implementation_import_includes, parts.implementation_import_includes.length() > 0 ? mlc::String("\n") : mlc::String(""), expr::namespace_open_line(parts.module_namespace), mlc::String("\n"), parts.implementation_using_namespaces, parts.implementation_using_namespaces.length() > 0 ? mlc::String("\n") : mlc::String(""), parts.decl_parts.fn_defs.join(mlc::String("")), mlc::String("\n"), expr::namespace_close_comment_line(parts.module_namespace), parts.cli_wrapper}.join(mlc::String(""));}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_declarations_from_text_parts(mlc::Array<mlc::String> text_parts) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations = {};
int index = 0;
while (index < text_parts.size()){
{
if (text_parts[index].length() > 0){
{
declarations.push_back(decl_cpp::cpp_decl_from_string_output(text_parts[index]));
}
}
index = index + 1;
}
}
return declarations;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> assemble_header_cpp_declarations(module::ModuleGenerationContext parts) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations = cpp_declarations_from_text_parts(mlc::Array<mlc::String>{expr::include_guard_ifndef_line(parts.guard), expr::include_guard_define_line(parts.guard), mlc::String("\n"), parts.std_includes, expr::namespace_open_line(parts.module_namespace), mlc::String("\n")});
return declarations.concat(decl_cpp::cpp_decls_from_string_parts(parts.decl_parts.type_fwds)).concat(decl_cpp::cpp_decls_from_string_parts(parts.decl_parts.type_defs)).concat(decl_cpp::cpp_decls_from_string_parts(parts.decl_parts.fn_protos)).concat(cpp_declarations_from_text_parts(mlc::Array<mlc::String>{mlc::String("\n"), expr::namespace_close_comment_line(parts.module_namespace), mlc::String("\n"), expr::include_guard_endif_comment_line(parts.guard)}));
}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> assemble_source_cpp_declarations(module::ModuleGenerationContext parts) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations = cpp_declarations_from_text_parts(mlc::Array<mlc::String>{expr::implementation_define_main_as_user_main_line(), expr::implementation_include_quotefile_line(parts.base + mlc::String(".hpp")), mlc::String("\n"), parts.implementation_import_includes, parts.implementation_import_includes.length() > 0 ? mlc::String("\n") : mlc::String(""), expr::namespace_open_line(parts.module_namespace), mlc::String("\n"), parts.implementation_using_namespaces, parts.implementation_using_namespaces.length() > 0 ? mlc::String("\n") : mlc::String("")});
return declarations.concat(decl_cpp::cpp_decls_from_string_parts(parts.decl_parts.fn_defs)).concat(cpp_declarations_from_text_parts(mlc::Array<mlc::String>{mlc::String("\n"), expr::namespace_close_comment_line(parts.module_namespace), parts.cli_wrapper}));
}

std::shared_ptr<cpp_ast::CppFile> gen_module_cpp_file(module::ModuleGenerationContext parts) noexcept{return std::make_shared<cpp_ast::CppFile>(cpp_ast::CppFile{assemble_header_cpp_declarations(parts), assemble_source_cpp_declarations(parts)});}

context::GenModuleOut gen_module_via_string(module::ModuleGenerationContext parts) noexcept{return context::GenModuleOut{assemble_header_string(parts), assemble_source_string(parts)};}

context::GenModuleOut gen_module_via_cpp_printer(module::ModuleGenerationContext parts) noexcept{
std::shared_ptr<cpp_ast::CppFile> file = gen_module_cpp_file(parts);
return context::GenModuleOut{decl_cpp::print_cpp_declarations(cpp_ast::cpp_file_header(file)), decl_cpp::print_cpp_declarations(cpp_ast::cpp_file_source(file))};
}

context::GenModuleOut gen_module_with_printer(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp, bool use_cpp_printer) noexcept{
module::ModuleGenerationContext parts = prepare_module_generation(s_item, all_items, precomp);
return use_cpp_printer ? gen_module_via_cpp_printer(parts) : gen_module_via_string(parts);
}

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept{return gen_module_with_printer(s_item, all_items, full_prog, precomp, use_cpp_printer_default());}

mlc::String gen_program_with_printer(ast::Program program, bool use_cpp_printer) noexcept{
mlc::Array<decl_index::LoadItem> all_items = {};
context::PrecomputedCtx precomp = precompute(program, all_items);
registry::TypeRegistry registry = registry::build_registry(program);
decl_index::LoadItem single = decl_index::LoadItem{mlc::String("test_main"), program.decls, {}, {}};
mlc::Array<semantic_ir::SLoadItem> s_items = transform_decl::transform_load_items(mlc::Array<decl_index::LoadItem>{single}, registry, trait_param_expand::build_trait_nominal_maps(program));
context::GenModuleOut result = gen_module_with_printer(s_items[0], all_items, program, precomp, use_cpp_printer);
return result.h + result.c;
}

mlc::String gen_program(ast::Program program) noexcept{return gen_program_with_printer(program, use_cpp_printer_default());}

} // namespace module
