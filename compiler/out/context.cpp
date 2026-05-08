#include "context.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_naming.hpp"
#include "decl_index.hpp"
#include "type_index.hpp"
#include "ctor_info.hpp"
#include "param_analysis.hpp"

namespace context {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace cpp_naming;
using namespace decl_index;
using namespace type_index;
using namespace ctor_info;
using namespace param_analysis;
using namespace ast_tokens;

context::GenStmtsResult GenStmtsResult_append_stmt(context::GenStmtsResult self, context::GenStmtResult statement_result) noexcept;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

mlc::Array<mlc::String> collect_pat_bind_names_for_context(std::shared_ptr<ast::Pat> pat, mlc::Array<mlc::String> accumulator) noexcept;

mlc::String CodegenContext_resolve(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept;

context::CodegenContext CodegenContext_add_shared(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_shared_array(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_value(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_match_deref(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_with_struct_using_lines(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;

context::CodegenContext CodegenContext_with_namespace_alias_prefixes(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;

context::CodegenContext CodegenContext_update_from_statement(context::CodegenContext self, std::shared_ptr<semantic_ir::SStmt> statement) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context) noexcept;

context::CodegenContext create_codegen_context(ast::Program prog) noexcept;

context::CodegenContext context_with_struct_using_lines(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept;

context::CodegenContext context_with_namespace_alias_prefixes(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept;

context::GenStmtsResult GenStmtsResult_append_stmt(context::GenStmtsResult self, context::GenStmtResult statement_result) noexcept{return context::GenStmtsResult{self.code + statement_result.output, statement_result.next_try};}

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept{return context.qualified.has(name) ? context.qualified.get(name) + cpp_naming::cpp_safe(name) : context.namespace_prefix.length() > 0 ? context.namespace_prefix + cpp_naming::cpp_safe(name) : cpp_naming::cpp_safe(name);}

mlc::Array<mlc::String> collect_pat_bind_names_for_context(std::shared_ptr<ast::Pat> pat, mlc::Array<mlc::String> accumulator) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatIdent>((*pat))) { auto _v_patident = std::get<ast::PatIdent>((*pat)); auto [n, _w0] = _v_patident; return [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(n);
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pat))) { auto _v_patctor = std::get<ast::PatCtor>((*pat)); auto [_w0, subs, _w1] = _v_patctor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < subs.size()){
{
accumulator = collect_pat_bind_names_for_context(subs[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatRecord>((*pat))) { auto _v_patrecord = std::get<ast::PatRecord>((*pat)); auto [_w0, field_patterns, _w1] = _v_patrecord; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < field_patterns.size()){
{
accumulator = collect_pat_bind_names_for_context(field_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatTuple>((*pat))) { auto _v_pattuple = std::get<ast::PatTuple>((*pat)); auto [subs, _w0] = _v_pattuple; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < subs.size()){
{
accumulator = collect_pat_bind_names_for_context(subs[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatArray>((*pat))) { auto _v_patarray = std::get<ast::PatArray>((*pat)); auto [subs, rest, _w0] = _v_patarray; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < subs.size()){
{
accumulator = collect_pat_bind_names_for_context(subs[index], accumulator);
index = index + 1;
}
}
  return rest != mlc::String("") && rest != mlc::String("_") ? [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(rest);
  return accumulator;
 }() : accumulator;
 }(); } return accumulator; }();
}

mlc::String CodegenContext_resolve(context::CodegenContext self, mlc::String name) noexcept{return context_resolve(self, name);}

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept{return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, type_name, self.method_owners, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, self.ctor_type_infos, self.variant_types, {}, {}, self.generic_variants, self.struct_using_lines};}

context::CodegenContext CodegenContext_add_shared(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_params = self.shared_params;
new_shared_params.push_back(name);
return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, new_shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}

context::CodegenContext CodegenContext_add_shared_array(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_array_params = self.shared_array_params;
new_shared_array_params.push_back(name);
return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, new_shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}

context::CodegenContext CodegenContext_add_value(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_value_params = self.value_params;
new_value_params.push_back(name);
return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, new_value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}

context::CodegenContext CodegenContext_add_match_deref(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_match_deref_params = self.match_deref_params;
new_match_deref_params.push_back(name);
return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, new_match_deref_params, self.generic_variants, self.struct_using_lines};
}

context::CodegenContext CodegenContext_with_struct_using_lines(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept{return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, lines};}

context::CodegenContext CodegenContext_with_namespace_alias_prefixes(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept{return context::CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};}

context::CodegenContext CodegenContext_update_from_statement(context::CodegenContext self, std::shared_ptr<semantic_ir::SStmt> statement) noexcept{return [&]() -> context::CodegenContext { if (std::holds_alternative<semantic_ir::SStmtLetConst>((*statement)._)) { auto _v_sstmtletconst = std::get<semantic_ir::SStmtLetConst>((*statement)._); auto [binding_name, _w0, _w1, _w2] = _v_sstmtletconst; return CodegenContext_add_value(self, binding_name); } if (std::holds_alternative<semantic_ir::SStmtLet>((*statement)._)) { auto _v_sstmtlet = std::get<semantic_ir::SStmtLet>((*statement)._); auto [binding_name, _w0, _w1, value_type, _w2] = _v_sstmtlet; return [&]() -> context::CodegenContext { if (std::holds_alternative<registry::TShared>((*value_type))) { auto _v_tshared = std::get<registry::TShared>((*value_type)); auto [_w0] = _v_tshared; return CodegenContext_add_shared(self, binding_name); } if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inner] = _v_tarray; return [&]() -> context::CodegenContext { if (std::holds_alternative<registry::TShared>((*inner))) { auto _v_tshared = std::get<registry::TShared>((*inner)); auto [_w0] = _v_tshared; return CodegenContext_add_shared_array(self, binding_name); } return CodegenContext_add_value(self, binding_name); }(); } return CodegenContext_add_value(self, binding_name); }(); } if (std::holds_alternative<semantic_ir::SStmtLetPat>((*statement)._)) { auto _v_sstmtletpat = std::get<semantic_ir::SStmtLetPat>((*statement)._); auto [pattern, _w0, _w1, _w2, _w3, _w4, _w5] = _v_sstmtletpat; return [&]() -> context::CodegenContext { 
  mlc::Array<mlc::String> names = collect_pat_bind_names_for_context(pattern, {});
  context::CodegenContext codegen_context = std::move(self);
  int name_index = 0;
  while (name_index < names.size()){
{
codegen_context = CodegenContext_add_value(codegen_context, names[name_index]);
name_index = name_index + 1;
}
}
  return codegen_context;
 }(); } return self; }();}

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{return context::CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, base.namespace_alias_prefixes, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params, match_deref_params, base.generic_variants, base.struct_using_lines};}

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_shared(context, name);}

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_shared_array(context, name);}

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_value(context, name);}

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_match_deref(context, name);}

context::CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context) noexcept{return CodegenContext_update_from_statement(context, statement);}

context::CodegenContext create_codegen_context(ast::Program prog) noexcept{return context::CodegenContext{type_index::build_field_orders(prog), mlc::String(""), mlc::HashMap<mlc::String, mlc::String>(), mlc::HashMap<mlc::String, mlc::String>(), mlc::String(""), type_index::build_method_owners_from_decls(prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, {}, type_index::build_variant_types_from_decls(prog.decls), {}, {}, type_index::build_generic_variants_from_decls(prog.decls), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>()};}

context::CodegenContext context_with_struct_using_lines(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept{return CodegenContext_with_struct_using_lines(base, struct_using_lines);}

context::CodegenContext context_with_namespace_alias_prefixes(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept{return CodegenContext_with_namespace_alias_prefixes(base, namespace_alias_prefixes);}

} // namespace context
