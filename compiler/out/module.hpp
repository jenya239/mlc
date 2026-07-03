#ifndef MODULE_HPP
#define MODULE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "load_item.hpp"
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

struct ModuleGenerationContext {
  mlc::String base;
  mlc::String guard;
  mlc::String std_includes;
  mlc::String module_namespace;
  bool is_entry;
  decl_cpp::DeclPartsBundleCpp decl_parts;
  mlc::String implementation_import_includes;
  mlc::Array<mlc::String> implementation_import_paths;
};
mlc::Array<load_item::NamespaceImportAlias> namespace_aliases_mapped(mlc::Array<semantic_ir::SemanticNamespaceImportAlias> items) noexcept;
void push_struct_using_entry(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, context::StructUsingEntry entry) noexcept;
void add_assoc_bind_struct_using(mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>& entries, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>& lines, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
context::StructUsingData build_struct_using_data(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
context::PrecomputedCtx precompute(ast::Program program, mlc::Array<load_item::LoadItem> all_items) noexcept;
ModuleGenerationContext prepare_module_generation(semantic_ir::SemanticLoadItem load_item, mlc::Array<load_item::LoadItem> all_items, context::PrecomputedCtx precomputed_context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_header_cpp_declarations(ModuleGenerationContext parts) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> assemble_source_cpp_declarations(ModuleGenerationContext parts) noexcept;
std::shared_ptr<cpp_ast::CppFile> gen_module_cpp_file(ModuleGenerationContext parts) noexcept;
context::GenModuleOut gen_module_output(ModuleGenerationContext parts) noexcept;
context::GenModuleOut gen_module(semantic_ir::SemanticLoadItem load_item, mlc::Array<load_item::LoadItem> all_items, ast::Program full_program, context::PrecomputedCtx precomputed_context) noexcept;

} // namespace module

#endif // MODULE_HPP
