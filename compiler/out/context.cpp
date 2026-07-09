#define main mlc_user_main
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

GenStmtsResult GenStmtsResult_append_stmt(GenStmtsResult self, GenStmtResult statement_parsed) noexcept;
mlc::String GenStmtsResult_joined_code(GenStmtsResult self) noexcept;
mlc::String CodegenContext_resolve(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_for_type_body(CodegenContext self, mlc::String type_name) noexcept;
CodegenContext CodegenContext_add_shared(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_shared_array(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_value(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_match_deref(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_with_struct_using_data(CodegenContext self, StructUsingData data) noexcept;
CodegenContext CodegenContext_with_struct_using_lines(CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;
CodegenContext CodegenContext_with_namespace_alias_prefixes(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;
CodegenContext CodegenContext_with_enclosing_function_return_type(CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept;
CodegenContext CodegenContext_with_param_template_type_names(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> new_param_template_type_names) noexcept;
CodegenContext CodegenContext_update_from_statement(CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;
int new_temp_name_counter() noexcept{
  return 0;
}
TempNameResult take_temp_name(CodegenContext context) noexcept{
  return TempNameResult{(mlc::String("__tmp_", 6) + mlc::to_string(context.temp_name_counter)), CodegenContext{context.field_orders, context.field_order_index, context.namespace_prefix, context.qualified, context.namespace_alias_prefixes, context.self_type, context.method_owners, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.ctor_type_infos, context.ctor_type_info_index, context.variant_types, context.sum_type_parameter_names, context.variant_used_type_parameter_names, context.value_params, context.match_deref_params, context.generic_variants, context.struct_using_entries, context.struct_using_lines, context.type_alias_annotations, context.trait_associated_type_names, (context.temp_name_counter + 1), context.enclosing_function_return_type, context.param_template_type_names}};
}
GenStmtsResult GenStmtsResult_append_stmt(GenStmtsResult self, GenStmtResult statement_parsed) noexcept;
mlc::String GenStmtsResult_joined_code(GenStmtsResult self) noexcept;
GenStmtsResult GenStmtsResult_append_stmt(GenStmtsResult self, GenStmtResult statement_parsed) noexcept{
auto parts = self.parts;
self.parts.push_back(statement_parsed.output);
return GenStmtsResult{self.parts, statement_parsed.next_try};
}
mlc::String GenStmtsResult_joined_code(GenStmtsResult self) noexcept{
return self.parts.join(mlc::String("", 0));
}
mlc::String context_resolve(CodegenContext context, mlc::String name) noexcept{
  if (context.method_owners.has(name))   {
    return cpp_naming::cpp_safe(name);
  } else if (context.qualified.has(name))   {
    return (context.qualified.get(name) + cpp_naming::cpp_safe(name));
  } else if ((context.namespace_prefix.length() > 0))   {
    return (context.namespace_prefix + cpp_naming::cpp_safe(name));
  } else   {
    return cpp_naming::cpp_safe(name);
  }
}
mlc::String type_method_namespace_prefix(mlc::String mangled_name, CodegenContext context) noexcept{
  if (context.method_owners.has(mangled_name))   {
    return mlc::String("", 0);
  } else   {
    auto underscore = (-1);
    auto index = 0;
    while (((index < mangled_name.length()) && (underscore < 0)))     {
      if ((mangled_name.char_at(index) == mlc::String("_", 1)))       {
        (underscore = index);
      }
      (index = (index + 1));
    }
    if ((underscore <= 0))     {
      return mlc::String("", 0);
    } else if (context.qualified.has(mangled_name.substring(0, underscore)))     {
      return context.qualified.get(mangled_name.substring(0, underscore));
    } else     {
      return mlc::String("", 0);
    }
  }
}
mlc::String qualify_function_callee(mlc::String name, CodegenContext context) noexcept{
  if (context.method_owners.has(name))   {
    return cpp_naming::cpp_safe(name);
  } else if (context.qualified.has(name))   {
    return (context.qualified.get(name) + cpp_naming::cpp_safe(name));
  } else   {
    return (type_method_namespace_prefix(name, context) + cpp_naming::cpp_safe(name));
  }
}
mlc::Array<mlc::String> collect_pattern_bind_names_for_context(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String>& accumulator) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [binding_name, __1] = patternIdent; return [&]() {
accumulator.push_back(binding_name);
return accumulator;
}();
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, sub_patterns, __2] = patternCtor; return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
  (accumulator = collect_pattern_bind_names_for_context(sub_patterns[index], accumulator));
  (index = (index + 1));
}
return accumulator;
}();
}
if (std::holds_alternative<ast::PatternRecord>((*__match_subject))) {
const ast::PatternRecord& patternRecord = std::get<ast::PatternRecord>((*__match_subject));
auto [__0, field_patterns, __2] = patternRecord; return [&]() {
auto index = 0;
while ((index < field_patterns.length())) {
  (accumulator = collect_pattern_bind_names_for_context(field_patterns[index], accumulator));
  (index = (index + 1));
}
return accumulator;
}();
}
if (std::holds_alternative<ast::PatternTuple>((*__match_subject))) {
const ast::PatternTuple& patternTuple = std::get<ast::PatternTuple>((*__match_subject));
auto [sub_patterns, __1] = patternTuple; return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
  (accumulator = collect_pattern_bind_names_for_context(sub_patterns[index], accumulator));
  (index = (index + 1));
}
return accumulator;
}();
}
if (std::holds_alternative<ast::PatternArray>((*__match_subject))) {
const ast::PatternArray& patternArray = std::get<ast::PatternArray>((*__match_subject));
auto [sub_patterns, rest, __2] = patternArray; return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
  (accumulator = collect_pattern_bind_names_for_context(sub_patterns[index], accumulator));
  (index = (index + 1));
}
if (((rest != mlc::String("", 0)) && (rest != mlc::String("_", 1)))) {
  accumulator.push_back(rest);
}
return accumulator;
}();
}
return accumulator;
std::abort();
}();
}
mlc::String struct_using_line_from_entry(StructUsingEntry entry) noexcept{
  return ((((mlc::String("using ", 6) + entry.alias) + mlc::String(" = ", 3)) + entry.type_cpp) + mlc::String(";\n", 2));
}
mlc::String CodegenContext_resolve(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_for_type_body(CodegenContext self, mlc::String type_name) noexcept;
CodegenContext CodegenContext_add_shared(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_shared_array(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_value(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_match_deref(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_with_struct_using_data(CodegenContext self, StructUsingData data) noexcept;
CodegenContext CodegenContext_with_struct_using_lines(CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;
CodegenContext CodegenContext_with_namespace_alias_prefixes(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;
CodegenContext CodegenContext_with_enclosing_function_return_type(CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept;
CodegenContext CodegenContext_with_param_template_type_names(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> new_param_template_type_names) noexcept;
CodegenContext CodegenContext_update_from_statement(CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;
mlc::String CodegenContext_resolve(CodegenContext self, mlc::String name) noexcept{
return context_resolve(self, name);
}
CodegenContext CodegenContext_for_type_body(CodegenContext self, mlc::String type_name) noexcept{
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, type_name, self.method_owners, {}, {}, {}, {}, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, {}, {}, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_add_shared(CodegenContext self, mlc::String name) noexcept{
auto new_shared_params = self.shared_params;
new_shared_params.push_back(name);
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, new_shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_add_shared_array(CodegenContext self, mlc::String name) noexcept{
auto new_shared_array_params = self.shared_array_params;
new_shared_array_params.push_back(name);
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, new_shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_add_value(CodegenContext self, mlc::String name) noexcept{
auto new_value_params = self.value_params;
new_value_params.push_back(name);
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, new_value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_add_match_deref(CodegenContext self, mlc::String name) noexcept{
auto new_match_deref_params = self.match_deref_params;
new_match_deref_params.push_back(name);
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, new_match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_with_struct_using_data(CodegenContext self, StructUsingData data) noexcept{
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, data.entries, data.lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_with_struct_using_lines(CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept{
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_with_namespace_alias_prefixes(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept{
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_with_enclosing_function_return_type(CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept{
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, return_type, self.param_template_type_names};
}
CodegenContext CodegenContext_with_param_template_type_names(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> new_param_template_type_names) noexcept{
return CodegenContext{self.field_orders, self.field_order_index, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.ctor_type_info_index, self.variant_types, self.sum_type_parameter_names, self.variant_used_type_parameter_names, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_entries, self.struct_using_lines, self.type_alias_annotations, self.trait_associated_type_names, self.temp_name_counter, self.enclosing_function_return_type, new_param_template_type_names};
}
CodegenContext CodegenContext_update_from_statement(CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept{
return [&]() -> CodegenContext {
auto __match_subject = statement;
if (std::holds_alternative<semantic_ir::SemanticStatementLetConst>((*__match_subject))) {
const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst = std::get<semantic_ir::SemanticStatementLetConst>((*__match_subject));
auto [binding_name, __1, __2, __3] = semanticStatementLetConst; return CodegenContext_add_value(self, binding_name);
}
if (std::holds_alternative<semantic_ir::SemanticStatementLet>((*__match_subject))) {
const semantic_ir::SemanticStatementLet& semanticStatementLet = std::get<semantic_ir::SemanticStatementLet>((*__match_subject));
auto [binding_name, __1, __2, value_type, __4] = semanticStatementLet; return [&]() -> CodegenContext {
auto __match_subject = value_type;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; return CodegenContext_add_shared(self, binding_name);
}
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner] = tArray; return [&]() -> CodegenContext {
auto __match_subject = inner;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; return CodegenContext_add_shared_array(self, binding_name);
}
return CodegenContext_add_value(self, binding_name);
std::abort();
}();
}
return CodegenContext_add_value(self, binding_name);
std::abort();
}();
}
if (std::holds_alternative<semantic_ir::SemanticStatementLetPattern>((*__match_subject))) {
const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern = std::get<semantic_ir::SemanticStatementLetPattern>((*__match_subject));
auto [pattern, __1, __2, __3, __4, __5, __6] = semanticStatementLetPattern; return [&]() {
auto names = [&]() -> auto {
  auto __mut_actual_argument_holder_1 = mlc::Array<mlc::String>{};
  return collect_pattern_bind_names_for_context(pattern, __mut_actual_argument_holder_1);
}();
auto codegen_context = self;
auto name_index = 0;
while ((name_index < names.length())) {
  (codegen_context = CodegenContext_add_value(codegen_context, names[name_index]));
  (name_index = (name_index + 1));
}
return codegen_context;
}();
}
return self;
std::abort();
}();
}
bool trait_has_associated_types(CodegenContext context, mlc::String trait_name) noexcept{
  auto bare_trait_name = registry::trait_base_name(trait_name);
  if (context.trait_associated_type_names.has(bare_trait_name))   {
    return (context.trait_associated_type_names.get(bare_trait_name).length() > 0);
  } else   {
    return false;
  }
}
CodegenContext make_body_context(CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{
  return CodegenContext{base.field_orders, base.field_order_index, base.namespace_prefix, base.qualified, base.namespace_alias_prefixes, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.ctor_type_info_index, base.variant_types, base.sum_type_parameter_names, base.variant_used_type_parameter_names, value_params, match_deref_params, base.generic_variants, base.struct_using_entries, base.struct_using_lines, base.type_alias_annotations, base.trait_associated_type_names, base.temp_name_counter, base.enclosing_function_return_type, base.param_template_type_names};
}
CodegenContext context_add_shared(CodegenContext context, mlc::String name) noexcept{
  return CodegenContext_add_shared(context, name);
}
CodegenContext context_add_shared_array(CodegenContext context, mlc::String name) noexcept{
  return CodegenContext_add_shared_array(context, name);
}
CodegenContext context_add_value(CodegenContext context, mlc::String name) noexcept{
  return CodegenContext_add_value(context, name);
}
CodegenContext context_add_match_deref(CodegenContext context, mlc::String name) noexcept{
  return CodegenContext_add_match_deref(context, name);
}
CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, CodegenContext context) noexcept{
  return CodegenContext_update_from_statement(context, statement);
}
void push_pattern_bind_names_to_value_params(std::shared_ptr<ast::Pattern> pattern, CodegenContext& context) noexcept{
  auto names = [&]() -> auto {
  auto __mut_actual_argument_holder_1 = mlc::Array<mlc::String>{};
  return collect_pattern_bind_names_for_context(pattern, __mut_actual_argument_holder_1);
}();
  auto name_index = 0;
  return [&]() {
while ((name_index < names.length())) {
context.value_params.push_back(names[name_index]);
(name_index = (name_index + 1));
}
}();
}
void mutate_context_for_let_const(mlc::String binding_name, CodegenContext& context) noexcept{
  return context.value_params.push_back(binding_name);
}
void mutate_context_for_let(mlc::String binding_name, std::shared_ptr<registry::Type> value_type, CodegenContext& context) noexcept{
  return [&]() {
auto __match_subject = value_type;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; context.shared_params.push_back(binding_name);
return;
}
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner] = tArray; [&]() {
auto __match_subject = inner;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; context.shared_array_params.push_back(binding_name);
return;
}
context.value_params.push_back(binding_name);
return;
std::abort();
}();
return;
}
context.value_params.push_back(binding_name);
return;
std::abort();
}();
}
void mutate_context_noop() noexcept{

}
void mutate_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, CodegenContext& context) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst) -> void { auto [binding_name, __1, __2, __3] = semanticStatementLetConst; mutate_context_for_let_const(binding_name, context); },
[&](const semantic_ir::SemanticStatementLet& semanticStatementLet) -> void { auto [binding_name, __1, __2, value_type, __4] = semanticStatementLet; mutate_context_for_let(binding_name, value_type, context); },
[&](const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern) -> void { auto [pattern, __1, __2, __3, __4, __5, __6] = semanticStatementLetPattern; push_pattern_bind_names_to_value_params(pattern, context); },
[&](const semantic_ir::SemanticStatementExpr& semanticStatementExpr) -> void { auto [__0, __1] = semanticStatementExpr; mutate_context_noop(); },
[&](const semantic_ir::SemanticStatementReturn& semanticStatementReturn) -> void { auto [__0, __1] = semanticStatementReturn; mutate_context_noop(); },
[&](const semantic_ir::SemanticStatementBreak& semanticStatementBreak) -> void { auto [__0] = semanticStatementBreak; mutate_context_noop(); },
[&](const semantic_ir::SemanticStatementContinue& semanticStatementContinue) -> void { auto [__0] = semanticStatementContinue; mutate_context_noop(); }
}, (*statement));
}
mlc::Array<mlc::String> lookup_fields_for_context(CodegenContext context, mlc::String name) noexcept{
  if (context.field_order_index.has(name))   {
    return context.field_order_index.get(name);
  } else   {
    return decl_index::lookup_fields(context.field_orders, name);
  }
}
std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info_for_context(CodegenContext context, mlc::String constructor_name) noexcept{
  return ctor_info::lookup_ctor_type_info_indexed(context.ctor_type_info_index, context.ctor_type_infos, constructor_name);
}
CodegenContext create_codegen_context(ast::Program program) noexcept{
  auto field_orders = type_index::build_field_orders(program);
  auto ctor_type_infos = ctor_info::build_ctor_type_infos_from_decls(program.decls);
  auto registry = registry::build_registry(program);
  return CodegenContext{field_orders, decl_index::build_field_order_index(field_orders), mlc::String("", 0), {}, {}, mlc::String("", 0), type_index::build_method_owners_from_decls(program.decls), {}, {}, {}, {}, ctor_type_infos, ctor_info::build_ctor_type_info_index(ctor_type_infos), type_index::build_variant_types_from_decls(program.decls), type_index::build_sum_type_parameter_names_from_decls(program.decls), type_index::build_variant_used_type_parameter_names_from_decls(program.decls), {}, {}, type_index::build_generic_variants_from_decls(program.decls), {}, {}, registry.type_alias_annotations, registry.adt_index.trait_assoc_types, new_temp_name_counter(), std::make_shared<registry::Type>(registry::TUnknown{}), {}};
}
CodegenContext context_with_struct_using_data(CodegenContext base, StructUsingData data) noexcept{
  return CodegenContext_with_struct_using_data(base, data);
}
CodegenContext context_with_struct_using_lines(CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept{
  return CodegenContext_with_struct_using_lines(base, struct_using_lines);
}
CodegenContext context_with_namespace_alias_prefixes(CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept{
  return CodegenContext_with_namespace_alias_prefixes(base, namespace_alias_prefixes);
}

} // namespace context
