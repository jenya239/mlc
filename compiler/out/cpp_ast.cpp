#include "cpp_ast.hpp"

namespace cpp_ast {

mlc::String cpp_capture_name(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept;

bool cpp_capture_by_reference(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept;

mlc::String cpp_param_name(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

std::shared_ptr<cpp_ast::CppType> cpp_param_type(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

mlc::String cpp_field_type(std::shared_ptr<cpp_ast::CppField> field) noexcept;

mlc::String cpp_field_name(std::shared_ptr<cpp_ast::CppField> field) noexcept;

mlc::String cpp_variant_arm_name(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppType>> cpp_variant_arm_types(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_file_header(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_file_source(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String printer_indent_unit() noexcept;

mlc::Array<int> zero_based_indices(int count) noexcept;

mlc::String indent_text(int depth) noexcept;

mlc::String escape_cpp_character(mlc::String character) noexcept;

mlc::String escape_cpp_string_content(mlc::String input) noexcept;

mlc::String formatted_block(mlc::String statements_code, int depth) noexcept;

mlc::String print_comma_separated_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept;

mlc::String print_comma_separated_types(mlc::Array<std::shared_ptr<cpp_ast::CppType>> types) noexcept;

mlc::String print_comma_separated_strings(mlc::Array<mlc::String> strings) noexcept;

mlc::String type_template(mlc::String name, mlc::String arguments_code) noexcept;

mlc::String type_reference(mlc::String inner_code) noexcept;

mlc::String type_rvalue_reference(mlc::String inner_code) noexcept;

mlc::String type_pointer(mlc::String inner_code) noexcept;

mlc::String type_const_prefix(mlc::String inner_code) noexcept;

mlc::String print_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept;

mlc::String render_ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept;

mlc::String render_parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept;

mlc::String render_parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept;

mlc::String render_index_subscript(mlc::String object_code, mlc::String index_code) noexcept;

mlc::String render_function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept;

mlc::String pointer_field_access(mlc::String object_code, mlc::String field_name) noexcept;

mlc::String value_field_access(mlc::String object_code, mlc::String field_name) noexcept;

mlc::String init_list_braces(mlc::String element_list) noexcept;

mlc::String aggregate_initializer_expression(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> elements) noexcept;

mlc::String std_visit_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept;

mlc::String print_visit_arm_wild(bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept;

mlc::String print_visit_arm_binding(mlc::String parameter_name, bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept;

mlc::String print_visit_arm_constructed(mlc::String parameter_declaration, mlc::String binding_prefix, bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept;

mlc::String print_visit_arm_constructed_generic(mlc::String type_name, mlc::String binding_name, mlc::String binding_prefix, bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept;

mlc::String cast_expression(mlc::String kind_prefix, mlc::String type_code, mlc::String operand_code) noexcept;

mlc::String print_cast_kind_prefix(cpp_ast::CppCastKind kind) noexcept;

mlc::String print_comma_separated_expressions_multiline(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept;

mlc::String print_capture_entry(std::shared_ptr<cpp_ast::CppCapture> capture_entry) noexcept;

mlc::String print_capture_list(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures) noexcept;

mlc::String print_parameter_entry(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

mlc::String print_parameter_list(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept;

mlc::String block_braces(mlc::String body_code) noexcept;

mlc::String print_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, int depth) noexcept;

mlc::String print_statement_at_depth(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept;

mlc::String print_statement_body(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept;

mlc::String if_statement(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept;

mlc::String while_statement(mlc::String condition_code, mlc::String body_code) noexcept;

mlc::String range_for_statement(mlc::String variable_name, mlc::String range_code, mlc::String body_code) noexcept;

mlc::String auto_declaration(mlc::String name, mlc::String initializer_code) noexcept;

mlc::String structured_binding_declaration(mlc::Array<mlc::String> binding_names, mlc::String initializer_code) noexcept;

mlc::String constexpr_auto_declaration(mlc::String name, mlc::String initializer_code) noexcept;

mlc::String const_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept;

mlc::String variable_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept;

mlc::String include_angle(mlc::String path) noexcept;

mlc::String include_quoted(mlc::String path) noexcept;

mlc::String ifndef_directive(mlc::String macro) noexcept;

mlc::String define_macro_directive(mlc::String macro, mlc::String value) noexcept;

mlc::String endif_directive(mlc::String comment) noexcept;

mlc::String undef_directive(mlc::String macro) noexcept;

mlc::String namespace_begin_directive(mlc::String name) noexcept;

mlc::String namespace_end_directive(mlc::String name) noexcept;

mlc::String using_alias(mlc::String alias, mlc::String type_code) noexcept;

mlc::String using_namespace_directive(mlc::String namespace_identifier) noexcept;

mlc::String host_entry_main_epilogue(mlc::String qualified_namespace) noexcept;

mlc::String struct_forward_declaration(mlc::String template_prefix, mlc::String name) noexcept;

mlc::String class_forward_declaration(mlc::String name) noexcept;

mlc::String enum_forward_declaration(mlc::String name) noexcept;

mlc::String print_forward_declaration(mlc::String kind, mlc::String name) noexcept;

mlc::String print_struct_empty_definition(mlc::String template_prefix, mlc::String name) noexcept;

mlc::String struct_definition(mlc::String template_prefix, mlc::String name, mlc::String fields_code, bool forward_only) noexcept;

mlc::String struct_field_line(mlc::String type_code, mlc::String field_name) noexcept;

mlc::String function_prototype(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code) noexcept;

mlc::String compact_noexcept_function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String return_expression_code) noexcept;

mlc::String function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String body_code) noexcept;

mlc::String print_function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body, int body_statement_depth) noexcept;

mlc::String namespace_block(mlc::String name, mlc::String contents_code) noexcept;

mlc::String variant_type_alias(mlc::String template_prefix, mlc::String name, mlc::String types_code) noexcept;

mlc::String concept_requires_definition(mlc::String template_prefix, mlc::String trait_name, mlc::String self_parameter_name, mlc::Array<mlc::String> requires_expressions) noexcept;

mlc::String static_assert_declaration(mlc::String concept_cpp, mlc::Array<mlc::String> type_arguments, mlc::String message) noexcept;

mlc::String print_hash_specialization_body(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

mlc::String std_hash_specialization_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept;

mlc::String print_struct_field_line(std::shared_ptr<cpp_ast::CppField> field_entry) noexcept;

mlc::String print_struct_fields(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept;

mlc::Array<mlc::String> variant_arm_type_strings(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::String print_variant_types(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept;

bool is_empty_cpp_block(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String conditional_if_statement(mlc::String condition_code, mlc::String then_code, std::shared_ptr<cpp_ast::CppStatement> else_branch, int depth) noexcept;

mlc::String print_statement_node(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept;

mlc::String print_decl_sequence(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

mlc::String print_decl_node(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_decls(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

mlc::String print_file_node(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String invoked_while_expression(std::shared_ptr<cpp_ast::CppExpression> condition, mlc::String body_statements) noexcept;

mlc::String invoked_for_expression(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpression> iterator, mlc::String body_statements) noexcept;

mlc::String invoked_block_expression(mlc::String body_statements) noexcept;

mlc::String invoked_block_with_return_expression(mlc::String return_type, mlc::String body_statements) noexcept;

mlc::String question_try_expression(std::shared_ptr<cpp_ast::CppExpression> inner) noexcept;

mlc::String with_block_expression(std::shared_ptr<cpp_ast::CppExpression> resource, mlc::String binder, mlc::String body_statements) noexcept;

mlc::String lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String return_type_code, mlc::String body_code) noexcept;

mlc::String print_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body) noexcept;

mlc::String mutable_lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String body_expression_code) noexcept;

mlc::String print_mutable_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppExpression> body) noexcept;

mlc::String print_member_expression(std::shared_ptr<cpp_ast::CppExpression> object, mlc::String member_name, bool is_pointer) noexcept;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String cpp_capture_name(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept{return capture->name;}

bool cpp_capture_by_reference(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept{return capture->by_reference;}

mlc::String cpp_param_name(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept{return parameter->name;}

std::shared_ptr<cpp_ast::CppType> cpp_param_type(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept{return parameter->parameter_type;}

mlc::String cpp_field_type(std::shared_ptr<cpp_ast::CppField> field) noexcept{return field->type_value;}

mlc::String cpp_field_name(std::shared_ptr<cpp_ast::CppField> field) noexcept{return field->name;}

mlc::String cpp_variant_arm_name(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept{return arm->name;}

mlc::Array<std::shared_ptr<cpp_ast::CppType>> cpp_variant_arm_types(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept{return arm->types;}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_file_header(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return file->header;}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_file_source(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return file->source;}

mlc::String printer_indent_unit() noexcept{return mlc::String("  ");}

mlc::Array<int> zero_based_indices(int count) noexcept{return count <= 0 ? mlc::Array<int>{} : zero_based_indices(count - 1).concat(mlc::Array<int>{count - 1});}

mlc::String indent_text(int depth) noexcept{return zero_based_indices(depth).map([](int _) mutable { return printer_indent_unit(); }).join(mlc::String(""));}

mlc::String escape_cpp_character(mlc::String character) noexcept{return character == mlc::String("\\") ? mlc::String("\\\\") : character == mlc::String("\"") ? mlc::String("\\\"") : character == mlc::String("\n") ? mlc::String("\\n") : character == mlc::String("\r") ? mlc::String("\\r") : character == mlc::String("\t") ? mlc::String("\\t") : character == mlc::String("\0", 1) ? mlc::String("\\0") : character;}

mlc::String escape_cpp_string_content(mlc::String input) noexcept{return zero_based_indices(input.length()).map([input](int index) mutable { return escape_cpp_character(input.char_at(index)); }).join(mlc::String(""));}

mlc::String formatted_block(mlc::String statements_code, int depth) noexcept{return indent_text(depth) + mlc::String("{\n") + statements_code + mlc::String("\n") + indent_text(depth) + mlc::String("}");}

mlc::String print_comma_separated_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{return expressions.map([](std::shared_ptr<cpp_ast::CppExpression> expression) mutable { return print_expr(expression); }).join(mlc::String(", "));}

mlc::String print_comma_separated_types(mlc::Array<std::shared_ptr<cpp_ast::CppType>> types) noexcept{return types.map([](std::shared_ptr<cpp_ast::CppType> type_node) mutable { return print_type(type_node); }).join(mlc::String(", "));}

mlc::String print_comma_separated_strings(mlc::Array<mlc::String> strings) noexcept{return strings.join(mlc::String(", "));}

mlc::String type_template(mlc::String name, mlc::String arguments_code) noexcept{return name + mlc::String("<") + arguments_code + mlc::String(">");}

mlc::String type_reference(mlc::String inner_code) noexcept{return inner_code + mlc::String("&");}

mlc::String type_rvalue_reference(mlc::String inner_code) noexcept{return inner_code + mlc::String("&&");}

mlc::String type_pointer(mlc::String inner_code) noexcept{return inner_code + mlc::String("*");}

mlc::String type_const_prefix(mlc::String inner_code) noexcept{return mlc::String("const ") + inner_code;}

mlc::String print_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept{return std::visit(overloaded{
  [&](const CppTypeName& cpptypename) -> mlc::String { auto [name] = cpptypename; return name; },
  [&](const CppTypeTemplate& cpptypetemplate) -> mlc::String { auto [name, arguments] = cpptypetemplate; return type_template(name, print_comma_separated_types(arguments)); },
  [&](const CppTypeRef& cpptyperef) -> mlc::String { auto [inner] = cpptyperef; return type_reference(print_type(inner)); },
  [&](const CppTypeRRef& cpptyperref) -> mlc::String { auto [inner] = cpptyperref; return type_rvalue_reference(print_type(inner)); },
  [&](const CppTypePtr& cpptypeptr) -> mlc::String { auto [inner] = cpptypeptr; return type_pointer(print_type(inner)); },
  [&](const CppTypeConst& cpptypeconst) -> mlc::String { auto [inner] = cpptypeconst; return type_const_prefix(print_type(inner)); }
}, (*type_node));}

mlc::String render_ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{return mlc::String("(") + condition_code + mlc::String(" ? (") + then_code + mlc::String(") : (") + else_code + mlc::String("))");}

mlc::String render_parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept{return mlc::String("(") + left_code + mlc::String(" ") + operation + mlc::String(" ") + right_code + mlc::String(")");}

mlc::String render_parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept{return mlc::String("(") + operation + inner_code + mlc::String(")");}

mlc::String render_index_subscript(mlc::String object_code, mlc::String index_code) noexcept{return object_code + mlc::String("[") + index_code + mlc::String("]");}

mlc::String render_function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept{return callee_code + mlc::String("(") + argument_list + mlc::String(")");}

mlc::String pointer_field_access(mlc::String object_code, mlc::String field_name) noexcept{return object_code + mlc::String("->") + field_name;}

mlc::String value_field_access(mlc::String object_code, mlc::String field_name) noexcept{return object_code + mlc::String(".") + field_name;}

mlc::String init_list_braces(mlc::String element_list) noexcept{return element_list.length() == 0 ? mlc::String("{}") : mlc::String("{") + element_list + mlc::String("}");}

mlc::String aggregate_initializer_expression(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> elements) noexcept{return type_name + mlc::String("{") + print_comma_separated_expressions(elements) + mlc::String("}");}

mlc::String std_visit_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept{return mlc::String("std::visit(overloaded{") + arm_lambdas + mlc::String("\n}, ") + visit_subject + mlc::String(")");}

mlc::String print_visit_arm_wild(bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{return void_return ? mlc::String("[&](const auto& __v) -> void { ") + print_expr(body) + mlc::String("; }") : mlc::String("[&](const auto& __v) { return ") + print_expr(body) + mlc::String("; }");}

mlc::String print_visit_arm_binding(mlc::String parameter_name, bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{return void_return ? mlc::String("[&](const auto& ") + parameter_name + mlc::String(") -> void { ") + print_expr(body) + mlc::String("; }") : mlc::String("[&](const auto& ") + parameter_name + mlc::String(") { return ") + print_expr(body) + mlc::String("; }");}

mlc::String print_visit_arm_constructed(mlc::String parameter_declaration, mlc::String binding_prefix, bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{return void_return ? mlc::String("[&](") + parameter_declaration + mlc::String(") -> void { ") + binding_prefix + print_expr(body) + mlc::String("; }") : mlc::String("[&](") + parameter_declaration + mlc::String(") { ") + binding_prefix + mlc::String("return ") + print_expr(body) + mlc::String("; }");}

mlc::String print_visit_arm_constructed_generic(mlc::String type_name, mlc::String binding_name, mlc::String binding_prefix, bool void_return, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{return void_return ? mlc::String("[&]<typename __mlcT>(const ") + type_name + mlc::String("<__mlcT>& ") + binding_name + mlc::String(") -> void { ") + binding_prefix + print_expr(body) + mlc::String("; }") : mlc::String("[&]<typename __mlcT>(const ") + type_name + mlc::String("<__mlcT>& ") + binding_name + mlc::String(") { ") + binding_prefix + mlc::String("return ") + print_expr(body) + mlc::String("; }");}

mlc::String cast_expression(mlc::String kind_prefix, mlc::String type_code, mlc::String operand_code) noexcept{return kind_prefix + mlc::String("<") + type_code + mlc::String(">(") + operand_code + mlc::String(")");}

mlc::String print_cast_kind_prefix(cpp_ast::CppCastKind kind) noexcept{return std::visit(overloaded{
  [&](const CppCastStatic& cppcaststatic) -> mlc::String { return mlc::String("static_cast"); },
  [&](const CppCastDynamic& cppcastdynamic) -> mlc::String { return mlc::String("dynamic_cast"); },
  [&](const CppCastReinterpret& cppcastreinterpret) -> mlc::String { return mlc::String("reinterpret_cast"); },
  [&](const CppCastConst& cppcastconst) -> mlc::String { return mlc::String("const_cast"); }
}, kind);}

mlc::String print_comma_separated_expressions_multiline(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{return expressions.map([](std::shared_ptr<cpp_ast::CppExpression> expression) mutable { return print_expr(expression); }).join(mlc::String(",\n"));}

mlc::String print_capture_entry(std::shared_ptr<cpp_ast::CppCapture> capture_entry) noexcept{return cpp_capture_by_reference(capture_entry) ? mlc::String("&") + cpp_capture_name(capture_entry) : cpp_capture_name(capture_entry);}

mlc::String print_capture_list(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures) noexcept{return mlc::String("[") + captures.map([](std::shared_ptr<cpp_ast::CppCapture> capture_entry) mutable { return print_capture_entry(capture_entry); }).join(mlc::String(", ")) + mlc::String("]");}

mlc::String print_parameter_entry(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept{return print_type(cpp_param_type(parameter)) + mlc::String(" ") + cpp_param_name(parameter);}

mlc::String print_parameter_list(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept{return parameters.map([](std::shared_ptr<cpp_ast::CppParam> parameter) mutable { return print_parameter_entry(parameter); }).join(mlc::String(", "));}

mlc::String block_braces(mlc::String body_code) noexcept{return mlc::String("{") + body_code + mlc::String("}");}

mlc::String print_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, int depth) noexcept{return statements.map([depth](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return print_statement_at_depth(statement, depth); }).join(mlc::String("\n"));}

mlc::String print_statement_at_depth(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppStatementFragment>((*statement)._)) { auto _v_cppstatementfragment = std::get<cpp_ast::CppStatementFragment>((*statement)._); auto [fragment] = _v_cppstatementfragment; return fragment; } return indent_text(depth) + print_statement_node(statement, depth); }();}

mlc::String print_statement_body(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppBlock>((*statement)._)) { auto _v_cppblock = std::get<cpp_ast::CppBlock>((*statement)._); auto [statements] = _v_cppblock; return formatted_block(print_statements(statements, depth + 1), depth); } if (std::holds_alternative<cpp_ast::CppIf>((*statement)._)) { auto _v_cppif = std::get<cpp_ast::CppIf>((*statement)._); auto [_w0, _w1, _w2] = _v_cppif; return print_statement_node(statement, depth); } return formatted_block(indent_text(depth + 1) + print_statement_node(statement, depth + 1), depth); }();}

mlc::String if_statement(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{return mlc::String("if (") + condition_code + mlc::String(") ") + then_code + mlc::String(" else ") + else_code;}

mlc::String while_statement(mlc::String condition_code, mlc::String body_code) noexcept{return mlc::String("while (") + condition_code + mlc::String(") ") + body_code;}

mlc::String range_for_statement(mlc::String variable_name, mlc::String range_code, mlc::String body_code) noexcept{return mlc::String("for (auto ") + variable_name + mlc::String(" : ") + range_code + mlc::String(") ") + body_code;}

mlc::String auto_declaration(mlc::String name, mlc::String initializer_code) noexcept{return mlc::String("auto ") + name + mlc::String(" = ") + initializer_code + mlc::String(";");}

mlc::String structured_binding_declaration(mlc::Array<mlc::String> binding_names, mlc::String initializer_code) noexcept{return mlc::String("auto [") + print_comma_separated_strings(binding_names) + mlc::String("] = ") + initializer_code + mlc::String(";");}

mlc::String constexpr_auto_declaration(mlc::String name, mlc::String initializer_code) noexcept{return mlc::String("constexpr auto ") + name + mlc::String(" = ") + initializer_code + mlc::String(";");}

mlc::String const_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept{return mlc::String("const ") + type_code + mlc::String(" ") + name + mlc::String(" = ") + initializer_code + mlc::String(";");}

mlc::String variable_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept{return type_code + mlc::String(" ") + name + mlc::String(" = ") + initializer_code + mlc::String(";");}

mlc::String include_angle(mlc::String path) noexcept{return mlc::String("#include <") + path + mlc::String(">\n");}

mlc::String include_quoted(mlc::String path) noexcept{return mlc::String("#include ") + mlc::String("\"") + path + mlc::String("\"") + mlc::String("\n");}

mlc::String ifndef_directive(mlc::String macro) noexcept{return mlc::String("#ifndef ") + macro + mlc::String("\n");}

mlc::String define_macro_directive(mlc::String macro, mlc::String value) noexcept{return value.length() == 0 ? mlc::String("#define ") + macro + mlc::String("\n") : mlc::String("#define ") + macro + mlc::String(" ") + value + mlc::String("\n");}

mlc::String endif_directive(mlc::String comment) noexcept{return mlc::String("#endif // ") + comment + mlc::String("\n");}

mlc::String undef_directive(mlc::String macro) noexcept{return mlc::String("#undef ") + macro + mlc::String("\n");}

mlc::String namespace_begin_directive(mlc::String name) noexcept{return mlc::String("namespace ") + name + mlc::String(" {\n");}

mlc::String namespace_end_directive(mlc::String name) noexcept{return mlc::String("} // namespace ") + name + mlc::String("\n");}

mlc::String using_alias(mlc::String alias, mlc::String type_code) noexcept{return mlc::String("using ") + alias + mlc::String(" = ") + type_code + mlc::String(";");}

mlc::String using_namespace_directive(mlc::String namespace_identifier) noexcept{return mlc::String("using namespace ") + namespace_identifier + mlc::String(";\n");}

mlc::String host_entry_main_epilogue(mlc::String qualified_namespace) noexcept{return mlc::String("\n#undef main\n\n") + mlc::String("static void mlc_cli_set_args(int argc, char** argv) {\n") + mlc::String("  std::vector<mlc::String> arguments;\n") + mlc::String("  arguments.reserve(argc > 0 ? argc - 1 : 0);\n") + mlc::String("  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n") + mlc::String("  mlc::io::set_args(std::move(arguments));\n") + mlc::String("}\n\n") + mlc::String("int main(int argc, char** argv) {\n") + mlc::String("  mlc_cli_set_args(argc, argv);\n") + mlc::String("  return ::") + qualified_namespace + mlc::String("::mlc_user_main(argc, argv);\n") + mlc::String("}\n");}

mlc::String struct_forward_declaration(mlc::String template_prefix, mlc::String name) noexcept{return template_prefix + mlc::String("struct ") + name + mlc::String(";\n");}

mlc::String class_forward_declaration(mlc::String name) noexcept{return mlc::String("class ") + name + mlc::String(";\n");}

mlc::String enum_forward_declaration(mlc::String name) noexcept{return mlc::String("enum ") + name + mlc::String(";\n");}

mlc::String print_forward_declaration(mlc::String kind, mlc::String name) noexcept{return kind == mlc::String("class") ? class_forward_declaration(name) : kind == mlc::String("enum") ? enum_forward_declaration(name) : struct_forward_declaration(mlc::String(""), name);}

mlc::String print_struct_empty_definition(mlc::String template_prefix, mlc::String name) noexcept{return template_prefix + mlc::String("struct ") + name + mlc::String(" {};\n");}

mlc::String struct_definition(mlc::String template_prefix, mlc::String name, mlc::String fields_code, bool forward_only) noexcept{return fields_code.length() == 0 ? forward_only ? struct_forward_declaration(template_prefix, name) : print_struct_empty_definition(template_prefix, name) : template_prefix + mlc::String("struct ") + name + mlc::String(" {\n") + fields_code + mlc::String("\n};\n");}

mlc::String struct_field_line(mlc::String type_code, mlc::String field_name) noexcept{return type_code + mlc::String(" ") + field_name + mlc::String(";");}

mlc::String function_prototype(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code) noexcept{return template_prefix + return_type + mlc::String(" ") + name + mlc::String("(") + parameters_code + mlc::String(") noexcept;\n");}

mlc::String compact_noexcept_function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String return_expression_code) noexcept{return template_prefix + return_type + mlc::String(" ") + name + mlc::String("(") + parameters_code + mlc::String(") noexcept { return ") + return_expression_code + mlc::String("; }\n");}

mlc::String function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String body_code) noexcept{return template_prefix + return_type + mlc::String(" ") + name + mlc::String("(") + parameters_code + mlc::String(") noexcept{\n") + body_code + mlc::String("\n}\n");}

mlc::String print_function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body, int body_statement_depth) noexcept{return body_statement_depth == -1 && body.size() == 1 ? [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppReturn>((*body[0])._)) { auto _v_cppreturn = std::get<cpp_ast::CppReturn>((*body[0])._); auto [expression] = _v_cppreturn; return compact_noexcept_function_definition(template_prefix, return_type, name, parameters_code, print_expr(expression)); } return function_definition(template_prefix, return_type, name, parameters_code, print_statements(body, 1)); }() : function_definition(template_prefix, return_type, name, parameters_code, print_statements(body, body_statement_depth));}

mlc::String namespace_block(mlc::String name, mlc::String contents_code) noexcept{return mlc::String("namespace ") + name + mlc::String(" {\n") + contents_code + mlc::String("\n}");}

mlc::String variant_type_alias(mlc::String template_prefix, mlc::String name, mlc::String types_code) noexcept{return template_prefix + mlc::String("using ") + name + mlc::String(" = std::variant<") + types_code + mlc::String(">;\n");}

mlc::String concept_requires_definition(mlc::String template_prefix, mlc::String trait_name, mlc::String self_parameter_name, mlc::Array<mlc::String> requires_expressions) noexcept{return template_prefix + mlc::String("concept ") + trait_name + mlc::String(" = requires(const ") + self_parameter_name + mlc::String("& self) { ") + requires_expressions.join(mlc::String("; ")) + mlc::String("; };\n");}

mlc::String static_assert_declaration(mlc::String concept_cpp, mlc::Array<mlc::String> type_arguments, mlc::String message) noexcept{return mlc::String("static_assert(") + concept_cpp + mlc::String("<") + print_comma_separated_strings(type_arguments) + mlc::String(">, ") + mlc::String("\"") + message + mlc::String("\"") + mlc::String(");\n");}

mlc::String print_hash_specialization_body(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{return statements.map([](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return mlc::String("    ") + print_statement(statement); }).join(mlc::String("\n"));}

mlc::String std_hash_specialization_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{return mlc::String("namespace std {\n") + mlc::String("template<>\n") + mlc::String("struct hash<") + type_name + mlc::String("> {\n") + mlc::String("  size_t operator()(const ") + type_name + mlc::String("& self) const noexcept {\n") + print_hash_specialization_body(body_statements) + mlc::String("\n") + mlc::String("  }\n") + mlc::String("};\n") + mlc::String("}\n");}

mlc::String print_struct_field_line(std::shared_ptr<cpp_ast::CppField> field_entry) noexcept{return indent_text(1) + struct_field_line(cpp_field_type(field_entry), cpp_field_name(field_entry));}

mlc::String print_struct_fields(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept{return fields.map([](std::shared_ptr<cpp_ast::CppField> field_entry) mutable { return print_struct_field_line(field_entry); }).join(mlc::String("\n"));}

mlc::Array<mlc::String> variant_arm_type_strings(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppType>> types = cpp_variant_arm_types(arm);
return types.size() == 0 ? mlc::Array<mlc::String>{cpp_variant_arm_name(arm)} : types.map([](std::shared_ptr<cpp_ast::CppType> type_node) mutable { return print_type(type_node); });
}

mlc::String print_variant_types(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept{return mlc::collections::flat_map(arms, [](std::shared_ptr<cpp_ast::CppVariantArm> arm) mutable { return variant_arm_type_strings(arm); }).join(mlc::String(", "));}

bool is_empty_cpp_block(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppBlock>((*statement)._)) { auto _v_cppblock = std::get<cpp_ast::CppBlock>((*statement)._); auto [statements] = _v_cppblock; return statements.size() == 0; } return false; }();}

mlc::String conditional_if_statement(mlc::String condition_code, mlc::String then_code, std::shared_ptr<cpp_ast::CppStatement> else_branch, int depth) noexcept{return is_empty_cpp_block(else_branch) ? mlc::String("if (") + condition_code + mlc::String(") ") + then_code : if_statement(condition_code, then_code, print_statement_body(else_branch, depth));}

mlc::String print_statement_node(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept{return std::visit(overloaded{
  [&](const CppAutoDecl& cppautodecl) -> mlc::String { auto [name, initializer] = cppautodecl; return auto_declaration(name, print_expr(initializer)); },
  [&](const CppVarDecl& cppvardecl) -> mlc::String { auto [name, type_node, initializer] = cppvardecl; return variable_declaration(print_type(type_node), name, print_expr(initializer)); },
  [&](const CppConstDecl& cppconstdecl) -> mlc::String { auto [name, type_node, initializer] = cppconstdecl; return const_declaration(print_type(type_node), name, print_expr(initializer)); },
  [&](const CppConstexprAutoDecl& cppconstexprautodecl) -> mlc::String { auto [name, initializer] = cppconstexprautodecl; return constexpr_auto_declaration(name, print_expr(initializer)); },
  [&](const CppReturn& cppreturn) -> mlc::String { auto [expression] = cppreturn; return mlc::String("return ") + print_expr(expression) + mlc::String(";"); },
  [&](const CppExpressionStatement& cppexpressionstatement) -> mlc::String { auto [expression] = cppexpressionstatement; return print_expr(expression) + mlc::String(";"); },
  [&](const CppBlock& cppblock) -> mlc::String { auto [statements] = cppblock; return formatted_block(print_statements(statements, depth + 1), depth); },
  [&](const CppIf& cppif) -> mlc::String { auto [condition, then_branch, else_branch] = cppif; return conditional_if_statement(print_expr(condition), print_statement_body(then_branch, depth), else_branch, depth); },
  [&](const CppWhile& cppwhile) -> mlc::String { auto [condition, body] = cppwhile; return while_statement(print_expr(condition), print_statement_body(body, depth)); },
  [&](const CppFor& cppfor) -> mlc::String { auto [variable_name, range_expression, body] = cppfor; return range_for_statement(variable_name, print_expr(range_expression), formatted_block(print_statements(body, depth + 1), depth)); },
  [&](const CppStructuredBinding& cppstructuredbinding) -> mlc::String { auto [binding_names, initializer] = cppstructuredbinding; return structured_binding_declaration(binding_names, print_expr(initializer)); },
  [&](const CppStatementFragment& cppstatementfragment) -> mlc::String { auto [fragment] = cppstatementfragment; return fragment; }
}, (*statement)._);}

mlc::String print_decl_sequence(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{return declarations.map([](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return print_decl_node(declaration); }).join(mlc::String(""));}

mlc::String print_decl_node(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const CppInclude& cppinclude) -> mlc::String { auto [is_angle, path] = cppinclude; return is_angle ? include_angle(path) : include_quoted(path); },
  [&](const CppIfndef& cppifndef) -> mlc::String { auto [macro] = cppifndef; return ifndef_directive(macro); },
  [&](const CppDefineMacro& cppdefinemacro) -> mlc::String { auto [macro, value] = cppdefinemacro; return define_macro_directive(macro, value); },
  [&](const CppEndif& cppendif) -> mlc::String { auto [comment] = cppendif; return endif_directive(comment); },
  [&](const CppUndef& cppundef) -> mlc::String { auto [macro] = cppundef; return undef_directive(macro); },
  [&](const CppNamespaceBegin& cppnamespacebegin) -> mlc::String { auto [name] = cppnamespacebegin; return namespace_begin_directive(name); },
  [&](const CppNamespaceEnd& cppnamespaceend) -> mlc::String { auto [name] = cppnamespaceend; return namespace_end_directive(name); },
  [&](const CppUsing& cppusing) -> mlc::String { auto [alias, type_code] = cppusing; return using_alias(alias, type_code); },
  [&](const CppUsingNamespace& cppusingnamespace) -> mlc::String { auto [namespace_identifier] = cppusingnamespace; return using_namespace_directive(namespace_identifier); },
  [&](const CppStruct& cppstruct) -> mlc::String { auto [template_prefix, name, fields, forward_only] = cppstruct; return struct_definition(template_prefix, name, print_struct_fields(fields), forward_only); },
  [&](const CppForwardDecl& cppforwarddecl) -> mlc::String { auto [kind, name] = cppforwarddecl; return print_forward_declaration(kind, name); },
  [&](const CppFnProto& cppfnproto) -> mlc::String { auto [template_prefix, return_type, name, parameters] = cppfnproto; return function_prototype(template_prefix, return_type, name, print_comma_separated_strings(parameters)); },
  [&](const CppFnDef& cppfndef) -> mlc::String { auto [template_prefix, return_type, name, parameters, body, body_statement_depth] = cppfndef; return print_function_definition(template_prefix, return_type, name, print_comma_separated_strings(parameters), body, body_statement_depth); },
  [&](const CppNamespace& cppnamespace) -> mlc::String { auto [name, declarations] = cppnamespace; return namespace_block(name, print_decls(declarations)); },
  [&](const CppVariant& cppvariant) -> mlc::String { auto [template_prefix, name, arms] = cppvariant; return variant_type_alias(template_prefix, name, print_variant_types(arms)); },
  [&](const CppConceptRequires& cppconceptrequires) -> mlc::String { auto [template_prefix, trait_name, self_parameter_name, requires_expressions] = cppconceptrequires; return concept_requires_definition(template_prefix, trait_name, self_parameter_name, requires_expressions); },
  [&](const CppStaticAssert& cppstaticassert) -> mlc::String { auto [concept_cpp, type_arguments, message] = cppstaticassert; return static_assert_declaration(concept_cpp, type_arguments, message); },
  [&](const CppStdHashSpecialization& cppstdhashspecialization) -> mlc::String { auto [type_name, body_statements] = cppstdhashspecialization; return std_hash_specialization_definition(type_name, body_statements); },
  [&](const CppDeclarationSequence& cppdeclarationsequence) -> mlc::String { auto [declarations] = cppdeclarationsequence; return print_decl_sequence(declarations); },
  [&](const CppDeclarationEmpty& cppdeclarationempty) -> mlc::String { return mlc::String(""); },
  [&](const CppBlankLine& cppblankline) -> mlc::String { return mlc::String("\n"); },
  [&](const CppHostEntryMain& cpphostentrymain) -> mlc::String { auto [qualified_namespace] = cpphostentrymain; return host_entry_main_epilogue(qualified_namespace); },
  [&](const CppDeclarationFragment& cppdeclarationfragment) -> mlc::String { auto [code] = cppdeclarationfragment; return code; }
}, (*declaration));}

mlc::String print_decls(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{return declarations.map([](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return print_decl_node(declaration); }).join(mlc::String("\n\n"));}

mlc::String print_file_node(std::shared_ptr<cpp_ast::CppFile> file) noexcept{
mlc::String header_code = print_decls(cpp_file_header(file));
mlc::String source_code = print_decls(cpp_file_source(file));
return header_code.length() == 0 ? source_code : source_code.length() == 0 ? header_code : header_code + mlc::String("\n\n") + source_code;
}

mlc::String invoked_while_expression(std::shared_ptr<cpp_ast::CppExpression> condition, mlc::String body_statements) noexcept{return mlc::String("[&]() {\nwhile (") + print_expr(condition) + mlc::String(") {\n") + body_statements + mlc::String("}\n}()");}

mlc::String invoked_for_expression(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpression> iterator, mlc::String body_statements) noexcept{return mlc::String("[&]() {\nfor (auto ") + variable_name + mlc::String(" : ") + print_expr(iterator) + mlc::String(") {\n") + body_statements + mlc::String("}\n}()");}

mlc::String invoked_block_expression(mlc::String body_statements) noexcept{return mlc::String("[&]() {\n") + body_statements + mlc::String("}()");}

mlc::String invoked_block_with_return_expression(mlc::String return_type, mlc::String body_statements) noexcept{return mlc::String("[&]() -> ") + return_type + mlc::String(" {\n") + body_statements + mlc::String("}()");}

mlc::String question_try_expression(std::shared_ptr<cpp_ast::CppExpression> inner) noexcept{return mlc::String("({ auto __q = ") + print_expr(inner) + mlc::String("; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })");}

mlc::String with_block_expression(std::shared_ptr<cpp_ast::CppExpression> resource, mlc::String binder, mlc::String body_statements) noexcept{return mlc::String("{\nauto ") + binder + mlc::String(" = (") + print_expr(resource) + mlc::String(");\n") + body_statements + binder + mlc::String(".drop();\n}\n");}

mlc::String lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String return_type_code, mlc::String body_code) noexcept{return capture_prefix + mlc::String("(") + parameters_code + mlc::String(") -> ") + return_type_code + mlc::String(" {\n") + body_code + mlc::String("\n}");}

mlc::String print_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body) noexcept{
mlc::String capture_prefix = print_capture_list(captures);
mlc::String parameters_code = print_parameter_list(parameters);
mlc::String return_type_code = print_type(return_type);
mlc::String body_code = print_statements(body, 1);
return lambda_expression(capture_prefix, parameters_code, return_type_code, body_code);
}

mlc::String mutable_lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String body_expression_code) noexcept{return capture_prefix + mlc::String("(") + parameters_code + mlc::String(") mutable { return ") + body_expression_code + mlc::String("; }");}

mlc::String print_mutable_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{
mlc::String capture_prefix = print_capture_list(captures);
mlc::String parameters_code = print_parameter_list(parameters);
return mutable_lambda_expression(capture_prefix, parameters_code, print_expr(body));
}

mlc::String print_member_expression(std::shared_ptr<cpp_ast::CppExpression> object, mlc::String member_name, bool is_pointer) noexcept{return is_pointer ? pointer_field_access(print_expr(object), member_name) : value_field_access(print_expr(object), member_name);}

mlc::String print_integer_literal(int integer_value) noexcept{return mlc::to_string(integer_value);}

mlc::String print_string_literal(mlc::String string_value) noexcept{return mlc::String("\"") + escape_cpp_string_content(string_value) + mlc::String("\"");}

mlc::String print_boolean_literal(bool boolean_value) noexcept{return boolean_value ? mlc::String("true") : mlc::String("false");}

mlc::String print_identifier(mlc::String name) noexcept{return name;}

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{return std::visit(overloaded{
  [&](const CppInt& cppint) -> mlc::String { auto [integer_value] = cppint; return print_integer_literal(integer_value); },
  [&](const CppStr& cppstr) -> mlc::String { auto [string_value] = cppstr; return print_string_literal(string_value); },
  [&](const CppBool& cppbool) -> mlc::String { auto [boolean_value] = cppbool; return print_boolean_literal(boolean_value); },
  [&](const CppIdent& cppident) -> mlc::String { auto [name] = cppident; return print_identifier(name); },
  [&](const CppCall& cppcall) -> mlc::String { auto [callee, arguments] = cppcall; return render_function_call_parentheses(print_expr(callee), print_comma_separated_expressions(arguments)); },
  [&](const CppMember& cppmember) -> mlc::String { auto [object, member_name, is_pointer] = cppmember; return print_member_expression(object, member_name, is_pointer); },
  [&](const CppIndex& cppindex) -> mlc::String { auto [object, index] = cppindex; return render_index_subscript(print_expr(object), print_expr(index)); },
  [&](const CppBinary& cppbinary) -> mlc::String { auto [operation, left, right] = cppbinary; return render_parenthesized_binary(print_expr(left), operation, print_expr(right)); },
  [&](const CppUnary& cppunary) -> mlc::String { auto [operation, inner] = cppunary; return render_parenthesized_unary(operation, print_expr(inner)); },
  [&](const CppTernary& cppternary) -> mlc::String { auto [condition, then_branch, else_branch] = cppternary; return render_ternary_conditional(print_expr(condition), print_expr(then_branch), print_expr(else_branch)); },
  [&](const CppInitList& cppinitlist) -> mlc::String { auto [elements] = cppinitlist; return init_list_braces(print_comma_separated_expressions(elements)); },
  [&](const CppAggregateInit& cppaggregateinit) -> mlc::String { auto [type_name, elements] = cppaggregateinit; return aggregate_initializer_expression(type_name, elements); },
  [&](const CppStdVisit& cppstdvisit) -> mlc::String { auto [subject, handlers] = cppstdvisit; return std_visit_expression(print_comma_separated_expressions_multiline(handlers), print_expr(subject)); },
  [&](const CppVisitArmWild& cppvisitarmwild) -> mlc::String { auto [void_return, body] = cppvisitarmwild; return print_visit_arm_wild(void_return, body); },
  [&](const CppVisitArmBinding& cppvisitarmbinding) -> mlc::String { auto [parameter_name, void_return, body] = cppvisitarmbinding; return print_visit_arm_binding(parameter_name, void_return, body); },
  [&](const CppVisitArmConstructed& cppvisitarmconstructed) -> mlc::String { auto [parameter_declaration, binding_prefix, void_return, body] = cppvisitarmconstructed; return print_visit_arm_constructed(parameter_declaration, binding_prefix, void_return, body); },
  [&](const CppVisitArmConstructedGeneric& cppvisitarmconstructedgeneric) -> mlc::String { auto [type_name, binding_name, binding_prefix, void_return, body] = cppvisitarmconstructedgeneric; return print_visit_arm_constructed_generic(type_name, binding_name, binding_prefix, void_return, body); },
  [&](const CppCast& cppcast) -> mlc::String { auto [kind, target_type, operand] = cppcast; return cast_expression(print_cast_kind_prefix(kind), print_type(target_type), print_expr(operand)); },
  [&](const CppLambda& cpplambda) -> mlc::String { auto [captures, parameters, return_type, body] = cpplambda; return print_lambda_expression(captures, parameters, return_type, body); },
  [&](const CppMutableLambda& cppmutablelambda) -> mlc::String { auto [captures, parameters, body] = cppmutablelambda; return print_mutable_lambda_expression(captures, parameters, body); },
  [&](const CppInvokedWhile& cppinvokedwhile) -> mlc::String { auto [condition, body_statements] = cppinvokedwhile; return invoked_while_expression(condition, body_statements); },
  [&](const CppInvokedFor& cppinvokedfor) -> mlc::String { auto [variable_name, iterator, body_statements] = cppinvokedfor; return invoked_for_expression(variable_name, iterator, body_statements); },
  [&](const CppInvokedBlock& cppinvokedblock) -> mlc::String { auto [body_statements] = cppinvokedblock; return invoked_block_expression(body_statements); },
  [&](const CppInvokedBlockWithReturn& cppinvokedblockwithreturn) -> mlc::String { auto [return_type, body_statements] = cppinvokedblockwithreturn; return invoked_block_with_return_expression(return_type, body_statements); },
  [&](const CppQuestionTry& cppquestiontry) -> mlc::String { auto [inner] = cppquestiontry; return question_try_expression(inner); },
  [&](const CppWithBlock& cppwithblock) -> mlc::String { auto [resource, binder, body_statements] = cppwithblock; return with_block_expression(resource, binder, body_statements); }
}, (*expression)._);}

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return print_statement_at_depth(statement, 0);}

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return print_decl_node(declaration);}

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return print_file_node(file);}

} // namespace cpp_ast
