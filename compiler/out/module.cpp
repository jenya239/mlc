#define main mlc_user_main
#include "module.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "load_item.hpp"
#include "decl_index.hpp"
#include "ctor_info.hpp"
#include "type_index.hpp"
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
using namespace load_item;
using namespace decl_index;
using namespace ctor_info;
using namespace type_index;
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

mlc::Array<load_item::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SemanticNamespaceImportAlias> items) noexcept{
  auto mapped_aliases = mlc::Array<load_item::NamespaceImportAlias>{};
  auto index = 0;
  while ((index < items.length()))   {
    auto entry = items[index];
    mapped_aliases.push_back(load_item::NamespaceImportAlias{entry.alias, entry.module_path});
    (index = mlc::arith::checked_add(index, 1));
  }
  return mapped_aliases;
}
void push_struct_using_entry(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, context::StructUsingEntry entry) noexcept{
  auto existing_entries = [&]() -> mlc::Array<context::StructUsingEntry> {
  if (entries.has(type_name))   {
    return entries.get(type_name);
  } else   {
    return [&]() {
auto empty = mlc::Array<context::StructUsingEntry>{};
return empty;
}();
  }
}();
  existing_entries.push_back(entry);
  entries.set(type_name, existing_entries);
  auto line = context::struct_using_line_from_entry(entry);
  auto existing_lines = [&]() -> mlc::Array<mlc::String> {
  if (lines.has(type_name))   {
    return lines.get(type_name);
  } else   {
    return [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}();
  }
}();
  existing_lines.push_back(line);
  return lines.set(type_name, existing_lines);
}
bool semantic_declaration_is_assoc_bind(std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> bool {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject))) {
const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject));
auto [__0, __1, __2] = semanticDeclarationAssocBind; return true;
}
return false;
std::abort();
}();
}
context::StructUsingEntry semantic_declaration_assoc_bind_entry(std::shared_ptr<semantic_ir::SemanticDeclaration> method, context::CodegenContext context) noexcept{
  return [&]() -> context::StructUsingEntry {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject))) {
const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject));
auto [assoc_name, bound_type, __2] = semanticDeclarationAssocBind; return context::StructUsingEntry{assoc_name, type_gen::sem_type_to_cpp(context, bound_type)};
}
return context::StructUsingEntry{mlc::String("", 0), mlc::String("", 0)};
std::abort();
}();
}
void add_assoc_bind_struct_using(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  auto method_index = 0;
  return [&]() {
while ((method_index < methods.length())) {
if (semantic_declaration_is_assoc_bind(methods[method_index])) {
  auto entry = semantic_declaration_assoc_bind_entry(methods[method_index], context);
  push_struct_using_entry(entries, lines, type_name, entry);
}
(method_index = mlc::arith::checked_add(method_index, 1));
}
}();
}
void add_assoc_binds_from_declaration(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return [&]() {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, __1, methods, __3] = semanticDeclarationExtend; add_assoc_bind_struct_using(entries, lines, type_name, methods, context);
return;
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner] = semanticDeclarationExported; add_assoc_binds_from_declaration(entries, lines, inner, context);
return;
}
[&]() {
std::make_tuple();
return std::make_tuple();
}();
return;
std::abort();
}();
}
context::StructUsingData build_struct_using_data(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  auto entries = mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>();
  auto lines = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    add_assoc_binds_from_declaration(entries, lines, declarations[declaration_index], context);
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  return context::StructUsingData{entries, lines};
}
context::PrecomputedCtx precompute(ast::Program program, mlc::Array<load_item::LoadItem> all_items) noexcept{
  auto field_orders = type_index::build_field_orders(program);
  auto ctor_type_infos = ctor_info::build_ctor_type_infos_from_decls(program.decls);
  auto registry = registry::build_registry(program);
  return context::PrecomputedCtx{field_orders, type_index::build_variant_types_from_decls(program.decls), type_index::build_sum_type_parameter_names_from_decls(program.decls), type_index::build_variant_used_type_parameter_names_from_decls(program.decls), decl_index::build_item_index(all_items), ctor_type_infos, type_index::build_generic_variants_from_decls(program.decls), type_index::build_method_owners_from_decls(program.decls), decl_index::build_field_order_index(field_orders), ctor_info::build_ctor_type_info_index(ctor_type_infos), registry.type_alias_annotations, registry.adt_index.trait_assoc_types};
}
ModuleGenerationContext prepare_module_generation(semantic_ir::SemanticLoadItem load_item, mlc::Array<load_item::LoadItem> all_items, context::PrecomputedCtx precomputed_context) noexcept{
  auto base = cpp_naming::path_to_module_base(load_item.path);
  auto namespace_aliases = load_item.namespace_import_aliases;
  auto module_decls = decl_index::ast_decls_for_path(load_item.path, all_items);
  auto qualified = decl_index::extend_qualified_map(decl_index::build_qualified(load_item.imports, all_items), all_items, load_item.path);
  auto namespace_alias_prefixes = decl_index::build_namespace_alias_prefixes(namespace_aliases_mapped(namespace_aliases));
  auto base_context = context::CodegenContext{precomputed_context.field_orders, precomputed_context.field_order_index, mlc::String("", 0), qualified, namespace_alias_prefixes, mlc::String("", 0), type_index::build_method_owners_from_decls(module_decls), {}, {}, {}, {}, precomputed_context.ctor_type_infos, precomputed_context.ctor_type_info_index, precomputed_context.variant_types, precomputed_context.sum_type_parameter_names, precomputed_context.variant_used_type_parameter_names, {}, {}, precomputed_context.generic_variants, {}, {}, precomputed_context.type_alias_annotations, precomputed_context.trait_associated_type_names, context::new_temp_name_counter(), std::make_shared<registry::Type>(registry::TUnknown{}), {}};
  auto context = context::CodegenContext_with_struct_using_data(base_context, build_struct_using_data(load_item.decls, base_context));
  auto module_namespace = ((base == mlc::String("main", 4)) ? (mlc::String("mlc_main", 8)) : (base));
  auto is_entry = decl::decls_have_main(load_item.decls);
  auto std_includes = mlc::arith::checked_add((expr::standard_translation_unit_runtime_headers() + cpp_naming::include_lines(load_item.imports)), mlc::String("\n", 1));
  auto ffi_includes = decl_cpp::collect_ffi_include_lines(load_item.decls);
  auto decl_parts = decl_cpp::collect_all_decl_parts_cpp(load_item.decls, context);
  auto guard = (base.upper() + mlc::String("_HPP", 4));
  return ModuleGenerationContext{base, guard, std_includes, module_namespace, is_entry, decl_parts, (cpp_naming::include_lines(load_item.imports) + ffi_includes), load_item.imports};
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_header_cpp_declarations(ModuleGenerationContext parts) noexcept{
  auto declarations = mlc::Array{module_tu_helpers::make_include_guard_ifndef_cpp_declaration(parts.guard), module_tu_helpers::make_include_guard_define_cpp_declaration(parts.guard), module_tu_helpers::make_blank_line_cpp_declaration()};
  (declarations = module_tu_helpers::append_cpp_declarations_from_include_text(declarations, parts.std_includes));
  declarations.push_back(module_tu_helpers::make_namespace_open_cpp_declaration(parts.module_namespace));
  declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
  auto epilogue = mlc::Array{module_tu_helpers::make_blank_line_cpp_declaration(), module_tu_helpers::make_namespace_close_cpp_declaration(parts.module_namespace), module_tu_helpers::make_blank_line_cpp_declaration(), module_tu_helpers::make_include_guard_endif_cpp_declaration(parts.guard)};
  return declarations.concat(parts.decl_parts.type_fwds).concat(parts.decl_parts.type_defs).concat(parts.decl_parts.fn_protos).concat(epilogue);
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_source_cpp_declarations(ModuleGenerationContext parts) noexcept{
  auto declarations = mlc::Array{module_tu_helpers::make_define_main_as_user_main_cpp_declaration(), module_tu_helpers::make_quoted_include_cpp_declaration((parts.base + mlc::String(".hpp", 4))), module_tu_helpers::make_blank_line_cpp_declaration()};
  if ((parts.implementation_import_includes.length() > 0))   {
    (declarations = module_tu_helpers::append_cpp_declarations_from_include_text(declarations, parts.implementation_import_includes));
    declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
  }
  declarations.push_back(module_tu_helpers::make_namespace_open_cpp_declaration(parts.module_namespace));
  declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
  if ((cpp_naming::using_namespace_names(parts.implementation_import_paths).length() > 0))   {
    (declarations = module_tu_helpers::append_using_namespace_declarations(declarations, parts.implementation_import_paths));
    declarations.push_back(module_tu_helpers::make_blank_line_cpp_declaration());
  }
  auto epilogue = mlc::Array{module_tu_helpers::make_blank_line_cpp_declaration(), module_tu_helpers::make_namespace_close_cpp_declaration(parts.module_namespace)};
  auto result = declarations.concat(parts.decl_parts.fn_defs).concat(epilogue);
  if (parts.is_entry)   {
    (result = result.concat(mlc::Array{module_tu_helpers::make_entry_host_main_cpp_declaration(parts.module_namespace)}));
  }
  return result;
}
std::shared_ptr<cpp_ast::CppFile> gen_module_cpp_file(ModuleGenerationContext parts) noexcept{
  return std::make_shared<cpp_ast::CppFile>(cpp_ast::CppFile{assemble_header_cpp_declarations(parts), assemble_source_cpp_declarations(parts)});
}
context::GenModuleOut gen_module_output(ModuleGenerationContext parts) noexcept{
  auto file = gen_module_cpp_file(parts);
  return context::GenModuleOut{decl_cpp::print_cpp_declarations(cpp_ast::cpp_file_header(file)), decl_cpp::print_cpp_declarations(cpp_ast::cpp_file_source(file)), {}};
}
mlc::Array<mlc::String> collect_extern_lib_names_from_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExternLib>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib = std::get<semantic_ir::SemanticDeclarationExternLib>((*__match_subject));
auto [library_name, __1] = semanticDeclarationExternLib; return [&]() {
auto names = mlc::Array<mlc::String>{};
if ((library_name.length() > 0)) {
  names.push_back(library_name);
}
return names;
}();
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner_declaration] = semanticDeclarationExported; return collect_extern_lib_names_from_declaration(semantic_ir::sdecl_inner(inner_declaration));
}
return [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}();
std::abort();
}();
}
mlc::Array<mlc::String> append_unique_library_name(mlc::Array<mlc::String> names, mlc::String library_name) noexcept{
  auto index = 0;
  while ((index < names.length()))   {
    if ((names[index] == library_name))     {
      return names;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  names.push_back(library_name);
  return names;
}
mlc::Array<mlc::String> collect_extern_lib_names(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
  auto names = mlc::Array<mlc::String>{};
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    auto found = collect_extern_lib_names_from_declaration(declarations[declaration_index]);
    auto found_index = 0;
    while ((found_index < found.length()))     {
      (names = append_unique_library_name(names, found[found_index]));
      (found_index = mlc::arith::checked_add(found_index, 1));
    }
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  return names;
}
context::GenModuleOut gen_module(semantic_ir::SemanticLoadItem load_item, mlc::Array<load_item::LoadItem> all_items, ast::Program full_program, context::PrecomputedCtx precomputed_context) noexcept{
  auto parts = prepare_module_generation(load_item, all_items, precomputed_context);
  auto output = gen_module_output(parts);
  return context::GenModuleOut{output.header, output.source, collect_extern_lib_names(load_item.decls)};
}

} // namespace module
