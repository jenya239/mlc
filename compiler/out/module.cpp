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
#include "module_tu_helpers.hpp"
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
using namespace module_tu_helpers;
using namespace expr;
using namespace ast_tokens;

struct ModuleGenerationContext {mlc::String base;mlc::String guard;mlc::String std_includes;mlc::String module_namespace;bool is_entry;decl_cpp::DeclPartsBundleCpp decl_parts;mlc::String implementation_import_includes;mlc::Array<mlc::String> implementation_import_paths;};

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SemanticNamespaceImportAlias> items) noexcept;

void push_struct_using_entry(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, context::StructUsingEntry entry) noexcept;

void add_assoc_bind_struct_using(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

context::StructUsingData build_struct_using_data(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

context::PrecomputedCtx precompute(ast::Program program, mlc::Array<decl_index::LoadItem> all_items) noexcept;

module::ModuleGenerationContext prepare_module_generation(semantic_ir::SemanticLoadItem load_item, mlc::Array<decl_index::LoadItem> all_items, context::PrecomputedCtx precomputed_context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_header_cpp_declarations(module::ModuleGenerationContext parts) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_source_cpp_declarations(module::ModuleGenerationContext parts) noexcept;

std::shared_ptr<cpp_ast::CppFile> gen_module_cpp_file(module::ModuleGenerationContext parts) noexcept;

context::GenModuleOut gen_module_output(module::ModuleGenerationContext parts) noexcept;

context::GenModuleOut gen_module(semantic_ir::SemanticLoadItem load_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_program, context::PrecomputedCtx precomputed_context) noexcept;

mlc::String gen_program(ast::Program program) noexcept;

mlc::Array<decl_index::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SemanticNamespaceImportAlias> items) noexcept{
mlc::Array<decl_index::NamespaceImportAlias> mapped_aliases = {};
int index = 0;
while (index < items.size()){
{
semantic_ir::SemanticNamespaceImportAlias entry = items[index];
mapped_aliases.push_back(decl_index::NamespaceImportAlias{entry.alias, entry.module_path});
index = index + 1;
}
}
return mapped_aliases;
}

void push_struct_using_entry(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, context::StructUsingEntry entry) noexcept{
mlc::Array<context::StructUsingEntry> existing_entries = entries.has(type_name) ? entries.get(type_name) : [&]() -> mlc::Array<context::StructUsingEntry> { 
  mlc::Array<context::StructUsingEntry> empty = {};
  return empty;
 }();
existing_entries.push_back(entry);
entries.set(type_name, existing_entries);
mlc::String line = context::struct_using_line_from_entry(entry);
mlc::Array<mlc::String> existing_lines = lines.has(type_name) ? lines.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
existing_lines.push_back(line);
return lines.set(type_name, existing_lines);
}

void add_assoc_bind_struct_using(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
int method_index = 0;
return [&]() { 
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*methods[method_index]))) { auto _v_semanticdeclarationassocbind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*methods[method_index])); auto [assoc_name, bound_type, _w0] = _v_semanticdeclarationassocbind; return [&]() -> std::tuple<> { 
  mlc::String type_cpp = type_gen::sem_type_to_cpp(context, bound_type);
  push_struct_using_entry(entries, lines, type_name, context::StructUsingEntry{assoc_name, type_cpp});
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
 }();
}

context::StructUsingData build_struct_using_data(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>> entries = mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>();
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declarations[declaration_index]))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declarations[declaration_index])); auto [type_name, _w0, methods, _w1] = _v_semanticdeclarationextend; return [&]() -> std::tuple<> { 
  add_assoc_bind_struct_using(entries, lines, type_name, methods, context);
  return std::make_tuple();
 }(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declarations[declaration_index]))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declarations[declaration_index])); auto [inner] = _v_semanticdeclarationexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*inner))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*inner)); auto [type_name, _w0, methods, _w1] = _v_semanticdeclarationextend; return [&]() -> std::tuple<> { 
  add_assoc_bind_struct_using(entries, lines, type_name, methods, context);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
declaration_index = declaration_index + 1;
}
}
return context::StructUsingData{entries, lines};
}

context::PrecomputedCtx precompute(ast::Program program, mlc::Array<decl_index::LoadItem> all_items) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders = type_index::build_field_orders(program);
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos = ctor_info::build_ctor_type_infos_from_decls(program.decls);
registry::TypeRegistry registry = registry::build_registry(program);
return context::PrecomputedCtx{field_orders, type_index::build_variant_types_from_decls(program.decls), decl_index::build_item_index(all_items), ctor_type_infos, type_index::build_generic_variants_from_decls(program.decls), type_index::build_method_owners_from_decls(program.decls), decl_index::build_field_order_index(field_orders), ctor_info::build_ctor_type_info_index(ctor_type_infos), registry.type_alias_annotations};
}

module::ModuleGenerationContext prepare_module_generation(semantic_ir::SemanticLoadItem load_item, mlc::Array<decl_index::LoadItem> all_items, context::PrecomputedCtx precomputed_context) noexcept{
mlc::String base = cpp_naming::path_to_module_base(load_item.path);
mlc::Array<semantic_ir::SemanticNamespaceImportAlias> namespace_aliases = load_item.namespace_import_aliases;
mlc::HashMap<mlc::String, mlc::String> qualified = decl_index::build_qualified(load_item.imports, all_items);
mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes = decl_index::build_namespace_alias_prefixes(namespace_aliases_mapped(namespace_aliases));
context::CodegenContext base_context = context::CodegenContext{precomputed_context.field_orders, precomputed_context.field_order_index, mlc::String(""), qualified, namespace_alias_prefixes, mlc::String(""), precomputed_context.method_owners, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomputed_context.ctor_type_infos, precomputed_context.ctor_type_info_index, precomputed_context.variant_types, {}, {}, precomputed_context.generic_variants, mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), precomputed_context.type_alias_annotations, context::new_temp_name_counter()};
context::CodegenContext context = context::CodegenContext_with_struct_using_data(base_context, build_struct_using_data(load_item.decls, base_context));
mlc::String module_namespace = base == mlc::String("main") ? mlc::String("mlc_main") : base;
bool is_entry = decl::decls_have_main(load_item.decls);
mlc::String std_includes = expr::standard_translation_unit_runtime_headers() + cpp_naming::include_lines(load_item.imports) + mlc::String("\n");
decl_cpp::DeclPartsBundleCpp decl_parts = decl_cpp::collect_all_decl_parts_cpp(load_item.decls, context);
mlc::String guard = base.upper() + mlc::String("_HPP");
return module::ModuleGenerationContext{base, guard, std_includes, module_namespace, is_entry, decl_parts, cpp_naming::include_lines(load_item.imports), load_item.imports};
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_header_cpp_declarations(module::ModuleGenerationContext parts) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{module_tu_helpers::make_include_guard_ifndef_cpp_declaration(parts.guard), module_tu_helpers::make_include_guard_define_cpp_declaration(parts.guard), module_tu_helpers::make_blank_line_cpp_declaration()};
declarations = module_tu_helpers::append_cpp_declarations_from_include_text(declarations, parts.std_includes);
declarations.push_back(module_tu_helpers::make_namespace_open_cpp_declaration(parts.module_namespace));
declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> epilogue = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{module_tu_helpers::make_blank_line_cpp_declaration(), module_tu_helpers::make_namespace_close_cpp_declaration(parts.module_namespace), module_tu_helpers::make_blank_line_cpp_declaration(), module_tu_helpers::make_include_guard_endif_cpp_declaration(parts.guard)};
return declarations.concat(parts.decl_parts.type_fwds).concat(parts.decl_parts.type_defs).concat(parts.decl_parts.fn_protos).concat(epilogue);
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_source_cpp_declarations(module::ModuleGenerationContext parts) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{module_tu_helpers::make_define_main_as_user_main_cpp_declaration(), module_tu_helpers::make_quoted_include_cpp_declaration(parts.base + mlc::String(".hpp")), module_tu_helpers::make_blank_line_cpp_declaration()};
if (parts.implementation_import_includes.length() > 0){
{
declarations = module_tu_helpers::append_cpp_declarations_from_include_text(declarations, parts.implementation_import_includes);
declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
}
}
declarations.push_back(module_tu_helpers::make_namespace_open_cpp_declaration(parts.module_namespace));
declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
if (cpp_naming::using_namespace_names(parts.implementation_import_paths).size() > 0){
{
declarations = module_tu_helpers::append_using_namespace_declarations(declarations, parts.implementation_import_paths);
declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
}
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> epilogue = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{module_tu_helpers::make_blank_line_cpp_declaration(), module_tu_helpers::make_namespace_close_cpp_declaration(parts.module_namespace)};
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = declarations.concat(parts.decl_parts.fn_defs).concat(epilogue);
if (parts.is_entry){
{
result = result.concat(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{module_tu_helpers::make_entry_host_main_cpp_declaration(parts.module_namespace)});
}
}
return result;
}

std::shared_ptr<cpp_ast::CppFile> gen_module_cpp_file(module::ModuleGenerationContext parts) noexcept{return std::make_shared<cpp_ast::CppFile>(cpp_ast::CppFile{assemble_header_cpp_declarations(parts), assemble_source_cpp_declarations(parts)});}

context::GenModuleOut gen_module_output(module::ModuleGenerationContext parts) noexcept{
std::shared_ptr<cpp_ast::CppFile> file = gen_module_cpp_file(parts);
return context::GenModuleOut{decl_cpp::print_cpp_declarations(cpp_ast::cpp_file_header(file)), decl_cpp::print_cpp_declarations(cpp_ast::cpp_file_source(file))};
}

context::GenModuleOut gen_module(semantic_ir::SemanticLoadItem load_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_program, context::PrecomputedCtx precomputed_context) noexcept{
module::ModuleGenerationContext parts = prepare_module_generation(load_item, all_items, precomputed_context);
return gen_module_output(parts);
}

mlc::String gen_program(ast::Program program) noexcept{
mlc::Array<decl_index::LoadItem> all_items = {};
context::PrecomputedCtx precomputed_context = precompute(program, all_items);
registry::TypeRegistry registry = registry::build_registry(program);
decl_index::LoadItem single_load_item = decl_index::LoadItem{mlc::String("test_main"), program.decls, {}, {}};
mlc::Array<semantic_ir::SemanticLoadItem> semantic_load_items = transform_decl::transform_load_items(mlc::Array<decl_index::LoadItem>{single_load_item}, registry, trait_param_expand::build_trait_nominal_maps(program));
context::GenModuleOut module_parsed = gen_module(semantic_load_items[0], all_items, program, precomputed_context);
return module_parsed.header + module_parsed.source;
}

} // namespace module
