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

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

context::StructUsingData build_struct_using_data(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

context::PrecomputedCtx precompute(ast::Program program, mlc::Array<load_item::LoadItem> all_items) noexcept;

context::GenModuleOut gen_module(semantic_ir::SemanticLoadItem load_item, mlc::Array<load_item::LoadItem> all_items, ast::Program full_program, context::PrecomputedCtx precomputed_context) noexcept;

} // namespace module

#endif // MODULE_HPP
