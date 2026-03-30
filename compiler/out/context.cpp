#include "context.hpp"

#include "ast.hpp"
#include "cpp_naming.hpp"
#include "decl_index.hpp"
#include "ctor_info.hpp"
#include "param_analysis.hpp"

namespace context {

using namespace ast;
using namespace cpp_naming;
using namespace decl_index;
using namespace ctor_info;
using namespace param_analysis;
using namespace ast_tokens;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

bool expr_returns_shared_sum_visit_ptr(std::shared_ptr<ast::Expr> e) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context) noexcept;

context::CodegenContext create_codegen_context(ast::Program prog) noexcept;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept{return context.qualified.has(name) ? context.qualified.get(name) + cpp_naming::cpp_safe(name) : context.namespace_prefix.length() > 0 ? context.namespace_prefix + cpp_naming::cpp_safe(name) : cpp_naming::cpp_safe(name);}

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{return context::CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params, match_deref_params, base.generic_variants};}

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_params = context.shared_params;
new_shared_params.push_back(name);
return make_body_context(context, new_shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, context.match_deref_params);
}

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_array_params = context.shared_array_params;
new_shared_array_params.push_back(name);
return make_body_context(context, context.shared_params, new_shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, context.match_deref_params);
}

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_value_params = context.value_params;
new_value_params.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, new_value_params, context.match_deref_params);
}

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> md = context.match_deref_params;
md.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, md);
}

bool expr_returns_shared_sum_visit_ptr(std::shared_ptr<ast::Expr> e) noexcept{return [&]() { if (std::holds_alternative<ast::ExprCall>((*e)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*e)._); auto [func, _w0, _w1] = _v_exprcall; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*func)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*func)._); auto [n, _w0] = _v_exprident; return n == mlc::String("decl_inner") || n == mlc::String("find_field_val") || n == mlc::String("param_typ"); } return false; }(); } return false; }();}

context::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context) noexcept{return [&]() -> context::CodegenContext { if (std::holds_alternative<ast::StmtLet>((*stmt)._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*stmt)._); auto [binding_name, _w0, typ, value, _w1] = _v_stmtlet; return param_analysis::is_shared_type(typ) ? context_add_shared(context, binding_name) : param_analysis::is_shared_array_type(typ) ? context_add_shared_array(context, binding_name) : [&]() -> context::CodegenContext { if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [map_obj, method_name, _w0, _w1] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::ExprIdent>((*map_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_obj)._); auto [map_name, _w0] = _v_exprident; return context.shared_map_params.contains(map_name) ? context_add_shared(context, binding_name) : context_add_value(context, binding_name); } return context_add_value(context, binding_name); }() : [&]() -> context::CodegenContext { 
  context::CodegenContext vctx = context_add_value(context, binding_name);
  return expr_returns_shared_sum_visit_ptr(value) ? context_add_match_deref(vctx, binding_name) : vctx;
 }(); } return [&]() -> context::CodegenContext { 
  context::CodegenContext vctx = context_add_value(context, binding_name);
  return expr_returns_shared_sum_visit_ptr(value) ? context_add_match_deref(vctx, binding_name) : vctx;
 }(); }(); } return context; }();}

context::CodegenContext create_codegen_context(ast::Program prog) noexcept{return context::CodegenContext{decl_index::build_field_orders(prog), mlc::String(""), mlc::HashMap<mlc::String, mlc::String>(), mlc::String(""), decl_index::build_method_owners_from_decls(prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, {}, decl_index::build_variant_types_from_decls(prog.decls), {}, {}, decl_index::build_generic_variants_from_decls(prog.decls)};}

} // namespace context
