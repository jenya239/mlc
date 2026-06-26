#include "context.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_naming.hpp"
#include "decl_index.hpp"
#include "decl_index.hpp"
#include "load_item.hpp"
#include "type_index.hpp"
#include "ctor_info.hpp"
#include "param_analysis.hpp"

namespace context {

using namespace ast;
using namespace registry;
using namespace semantic_ir;
using namespace registry;
using namespace cpp_naming;
using namespace decl_index;
using namespace decl_index;
using namespace load_item;
using namespace type_index;
using namespace ctor_info;
using namespace param_analysis;
using namespace ast_tokens;

int new_temp_name_counter() noexcept;

context::TempNameResult take_temp_name(context::CodegenContext context) noexcept;

context::GenStmtsResult GenStmtsResult_append_stmt(context::GenStmtsResult self, context::GenStmtResult statement_parsed) noexcept;

mlc::String GenStmtsResult_joined_code(context::GenStmtsResult self) noexcept;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

mlc::String type_method_namespace_prefix(mlc::String mangled_name, context::CodegenContext context) noexcept;

mlc::String qualify_function_callee(mlc::String name, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> collect_pattern_bind_names_for_context(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String>& accumulator) noexcept;

mlc::String struct_using_line_from_entry(context::StructUsingEntry entry) noexcept;

mlc::String CodegenContext_resolve(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept;

context::CodegenContext CodegenContext_add_shared(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_shared_array(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_value(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_match_deref(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_with_struct_using_data(context::CodegenContext self, context::StructUsingData data) noexcept;

context::CodegenContext CodegenContext_with_struct_using_lines(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;

context::CodegenContext CodegenContext_with_namespace_alias_prefixes(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;

context::CodegenContext CodegenContext_with_enclosing_function_return_type(context::CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept;

context::CodegenContext CodegenContext_update_from_statement(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context) noexcept;

void mutate_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext& context) noexcept;

mlc::Array<mlc::String> lookup_fields_for_context(context::CodegenContext context, mlc::String name) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info_for_context(context::CodegenContext context, mlc::String constructor_name) noexcept;

context::CodegenContext create_codegen_context(ast::Program program) noexcept;

context::CodegenContext context_with_struct_using_data(context::CodegenContext base, context::StructUsingData data) noexcept;

context::CodegenContext context_with_struct_using_lines(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept;

context::CodegenContext context_with_namespace_alias_prefixes(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept;

int new_temp_name_counter() noexcept{return 0;}

context::TempNameResult take_temp_name(context::CodegenContext context) noexcept{
return context::TempNameResult{mlc::String("__tmp_") + mlc::to_string(context.temp_name_counter), context::CodegenContext{context.field_orders, context.field_order_index, context.namespace_prefix, context.qualified, context.namespace_alias_prefixes, context.self_type, context.method_owners, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.ctor_type_infos, context.ctor_type_info_index, context.variant_types, context.value_params, context.match_deref_params, context.generic_variants, context.struct_using_entries, context.struct_using_lines, context.type_alias_annotations, context.temp_name_counter + 1, context.enclosing_function_return_type}};
}

context::GenStmtsResult GenStmtsResult_append_stmt(context::GenStmtsResult self, context::GenStmtResult statement_parsed) noexcept{
mlc::Array<mlc::String> parts = self.parts;
parts.push_back(statement_parsed.output);
return context::GenStmtsResult{parts, statement_parsed.next_try};
}

mlc::String GenStmtsResult_joined_code(context::GenStmtsResult self) noexcept{return self.parts.join(mlc::String(""));}

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept{return context.qualified.has(name) ? context.qualified.get(name) + cpp_naming::cpp_safe(name) : context.namespace_prefix.length() > 0 ? context.namespace_prefix + cpp_naming::cpp_safe(name) : cpp_naming::cpp_safe(name);}

mlc::String type_method_namespace_prefix(mlc::String mangled_name, context::CodegenContext context) noexcept{
int underscore = -1;
int index = 0;
while (index < mangled_name.length() && underscore < 0){
{
if (mangled_name.char_at(index) == mlc::String("_")){
{
underscore = index;
}
}
index = index + 1;
}
}
return underscore <= 0 ? mlc::String("") : context.qualified.has(mangled_name.substring(0, underscore)) ? context.qualified.get(mangled_name.substring(0, underscore)) : mlc::String("");
}

mlc::String qualify_function_callee(mlc::String name, context::CodegenContext context) noexcept{return context.qualified.has(name) ? context.qualified.get(name) + cpp_naming::cpp_safe(name) : type_method_namespace_prefix(name, context) + cpp_naming::cpp_safe(name);}

mlc::Array<mlc::String> collect_pattern_bind_names_for_context(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String>& accumulator) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [binding_name, _w0] = _v_patternident; return [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(binding_name);
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, sub_patterns, _w1] = _v_patternctor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bind_names_for_context(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [_w0, field_patterns, _w1] = _v_patternrecord; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < field_patterns.size()){
{
accumulator = collect_pattern_bind_names_for_context(field_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [sub_patterns, _w0] = _v_patterntuple; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bind_names_for_context(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [sub_patterns, rest, _w0] = _v_patternarray; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bind_names_for_context(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return rest != mlc::String("") && rest != mlc::String("_") ? [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(rest);
  return accumulator;
 }() : accumulator;
 }(); } return accumulator; }();
}

mlc::String struct_using_line_from_entry(context::StructUsingEntry entry) noexcept{return mlc::String("using ") + entry.alias + mlc::String(" = ") + entry.type_cpp + mlc::String(";\n");}

mlc::String CodegenContext_resolve(context::CodegenContext self, mlc::String name) noexcept{return context_resolve(self, name);}

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept{return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, type_name, self.method_owners, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, {}, {}, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};}

context::CodegenContext CodegenContext_add_shared(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_params = self.shared_params;
new_shared_params.push_back(name);
return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, new_shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};
}

context::CodegenContext CodegenContext_add_shared_array(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_array_params = self.shared_array_params;
new_shared_array_params.push_back(name);
return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, new_shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};
}

context::CodegenContext CodegenContext_add_value(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_value_params = self.value_params;
new_value_params.push_back(name);
return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, new_value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};
}

context::CodegenContext CodegenContext_add_match_deref(context::CodegenContext self, mlc::String name) noexcept{
mlc::Array<mlc::String> new_match_deref_params = self.match_deref_params;
new_match_deref_params.push_back(name);
return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, new_match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};
}

context::CodegenContext CodegenContext_with_struct_using_data(context::CodegenContext self, context::StructUsingData data) noexcept{return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, data.entries, data.lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};}

context::CodegenContext CodegenContext_with_struct_using_lines(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept{return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};}

context::CodegenContext CodegenContext_with_namespace_alias_prefixes(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept{return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, self.enclosing_function_return_type};}

context::CodegenContext CodegenContext_with_enclosing_function_return_type(context::CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept{return context::CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.temp_name_counter, return_type};}

context::CodegenContext CodegenContext_update_from_statement(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept{return [&]() -> context::CodegenContext { if (std::holds_alternative<semantic_ir::SemanticStatementLetConst>((*statement)._)) { auto _v_semanticstatementletconst = std::get<semantic_ir::SemanticStatementLetConst>((*statement)._); auto [binding_name, _w0, _w1, _w2] = _v_semanticstatementletconst; return CodegenContext_add_value(self, binding_name); } if (std::holds_alternative<semantic_ir::SemanticStatementLet>((*statement)._)) { auto _v_semanticstatementlet = std::get<semantic_ir::SemanticStatementLet>((*statement)._); auto [binding_name, _w0, _w1, value_type, _w2] = _v_semanticstatementlet; return [&]() -> context::CodegenContext { if (std::holds_alternative<registry::TShared>((*value_type))) { auto _v_tshared = std::get<registry::TShared>((*value_type)); auto [_w0] = _v_tshared; return CodegenContext_add_shared(self, binding_name); } if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inner] = _v_tarray; return [&]() -> context::CodegenContext { if (std::holds_alternative<registry::TShared>((*inner))) { auto _v_tshared = std::get<registry::TShared>((*inner)); auto [_w0] = _v_tshared; return CodegenContext_add_shared_array(self, binding_name); } return CodegenContext_add_value(self, binding_name); }(); } return CodegenContext_add_value(self, binding_name); }(); } if (std::holds_alternative<semantic_ir::SemanticStatementLetPattern>((*statement)._)) { auto _v_semanticstatementletpattern = std::get<semantic_ir::SemanticStatementLetPattern>((*statement)._); auto [pattern, _w0, _w1, _w2, _w3, _w4, _w5] = _v_semanticstatementletpattern; return [&]() -> context::CodegenContext { 
  mlc::Array<mlc::String> names = [&]() { mlc::Array<mlc::String> __tmp_9 = {}; return collect_pattern_bind_names_for_context(pattern, __tmp_9); }();
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

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{return context::CodegenContext{base.field_orders, base.field_order_index, base.namespace_prefix, base.qualified, base.namespace_alias_prefixes, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.ctor_type_info_index, base.variant_types, value_params, match_deref_params, base.generic_variants, base.struct_using_entries, base.struct_using_lines, base.type_alias_annotations, base.temp_name_counter, base.enclosing_function_return_type};}

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_shared(context, name);}

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_shared_array(context, name);}

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_value(context, name);}

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept{return CodegenContext_add_match_deref(context, name);}

context::CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context) noexcept{return CodegenContext_update_from_statement(context, statement);}

void mutate_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext& context) noexcept{
return [&]() -> void { if (std::holds_alternative<semantic_ir::SemanticStatementLetConst>((*statement)._)) { auto _v_semanticstatementletconst = std::get<semantic_ir::SemanticStatementLetConst>((*statement)._); auto [binding_name, _w0, _w1, _w2] = _v_semanticstatementletconst; return context.value_params.push_back(binding_name); } if (std::holds_alternative<semantic_ir::SemanticStatementLet>((*statement)._)) { auto _v_semanticstatementlet = std::get<semantic_ir::SemanticStatementLet>((*statement)._); auto [binding_name, _w0, _w1, value_type, _w2] = _v_semanticstatementlet; return [&]() -> void { if (std::holds_alternative<registry::TShared>((*value_type))) { auto _v_tshared = std::get<registry::TShared>((*value_type)); auto [_w0] = _v_tshared; return context.shared_params.push_back(binding_name); } if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inner] = _v_tarray; return [&]() -> void { if (std::holds_alternative<registry::TShared>((*inner))) { auto _v_tshared = std::get<registry::TShared>((*inner)); auto [_w0] = _v_tshared; return context.shared_array_params.push_back(binding_name); } return context.value_params.push_back(binding_name); }(); } return context.value_params.push_back(binding_name); }(); } if (std::holds_alternative<semantic_ir::SemanticStatementLetPattern>((*statement)._)) { auto _v_semanticstatementletpattern = std::get<semantic_ir::SemanticStatementLetPattern>((*statement)._); auto [pattern, _w0, _w1, _w2, _w3, _w4, _w5] = _v_semanticstatementletpattern; return [&]() { 
  mlc::Array<mlc::String> names = [&]() { mlc::Array<mlc::String> __tmp_10 = {}; return collect_pattern_bind_names_for_context(pattern, __tmp_10); }();
  int name_index = 0;
  return [&]() { 
  while (name_index < names.size()){
{
context.value_params.push_back(names[name_index]);
name_index = name_index + 1;
}
}
 }();
 }(); } return; }();
}

mlc::Array<mlc::String> lookup_fields_for_context(context::CodegenContext context, mlc::String name) noexcept{return context.field_order_index.has(name) ? context.field_order_index.get(name) : decl_index::lookup_fields(context.field_orders, name);}

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info_for_context(context::CodegenContext context, mlc::String constructor_name) noexcept{return ctor_info::lookup_ctor_type_info_indexed(context.ctor_type_info_index, context.ctor_type_infos, constructor_name);}

context::CodegenContext create_codegen_context(ast::Program program) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders = type_index::build_field_orders(program);
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos = ctor_info::build_ctor_type_infos_from_decls(program.decls);
registry::TypeRegistry registry = registry::build_registry(program);
return context::CodegenContext{field_orders, decl_index::build_field_order_index(field_orders), mlc::String(""), mlc::HashMap<mlc::String, mlc::String>(), mlc::HashMap<mlc::String, mlc::String>(), mlc::String(""), type_index::build_method_owners_from_decls(program.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, ctor_type_infos, ctor_info::build_ctor_type_info_index(ctor_type_infos), type_index::build_variant_types_from_decls(program.decls), {}, {}, type_index::build_generic_variants_from_decls(program.decls), mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), registry.type_alias_annotations, new_temp_name_counter(), std::make_shared<registry::Type>((registry::TUnknown{}))};
}

context::CodegenContext context_with_struct_using_data(context::CodegenContext base, context::StructUsingData data) noexcept{return CodegenContext_with_struct_using_data(base, data);}

context::CodegenContext context_with_struct_using_lines(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept{return CodegenContext_with_struct_using_lines(base, struct_using_lines);}

context::CodegenContext context_with_namespace_alias_prefixes(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept{return CodegenContext_with_namespace_alias_prefixes(base, namespace_alias_prefixes);}

} // namespace context
