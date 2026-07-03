#define main mlc_user_main
#include "type_diagnostics.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "diagnostic_codes.hpp"

namespace type_diagnostics {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace array_method_types;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
  if ((((operation == mlc::String("-", 1)) && (!semantic_type_structure::type_is_unknown(inner_type))) && (!semantic_type_structure::type_is_i32(inner_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("unary - expects i32, got ", 25) + semantic_type_structure::type_description(inner_type)), source_span, diagnostic_codes::diagnostic_code_e012())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
  if ((((operation == mlc::String("!", 1)) && (!semantic_type_structure::type_is_unknown(inner_type))) && (!semantic_type_structure::type_is_bool(inner_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("operator ! expects bool, got ", 29) + semantic_type_structure::type_description(inner_type)), source_span, diagnostic_codes::diagnostic_code_e013())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept{
  if ((expected_length != actual_length))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("expected ", 9) + mlc::to_string(expected_length)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(actual_length)), call_source_span, diagnostic_codes::diagnostic_code_e014())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
int builtin_method_expected_argument_count_for_name(mlc::String method_name) noexcept{
  if (array_method_types::is_array_hof_method(method_name))   {
    return array_method_types::array_hof_expected_argument_count(method_name);
  } else   {
    return semantic_type_structure::builtin_method_expected_argument_count(method_name);
  }
}
mlc::Array<ast::Diagnostic> infer_builtin_method_arity_diagnostics(mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
  auto expected = builtin_method_expected_argument_count_for_name(method_name);
  if ((expected < 0))   {
    return mlc::Array<ast::Diagnostic>{};
  } else if ((argument_count != expected))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("expected ", 9) + mlc::to_string(expected)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(argument_count)), method_span, diagnostic_codes::diagnostic_code_e014())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept{
  if ((expected_count != actual_count))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("expected ", 9) + mlc::to_string(expected_count)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(actual_count)), call_source_span, diagnostic_codes::diagnostic_code_e014())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> call_arity_range_diagnostics(int min_count, int max_count, int actual_count, ast::Span call_source_span) noexcept{
  if (((actual_count < min_count) || (actual_count > max_count)))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((((mlc::String("expected between ", 17) + mlc::to_string(min_count)) + mlc::String(" and ", 5)) + mlc::to_string(max_count)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(actual_count)), call_source_span, diagnostic_codes::diagnostic_code_e014())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> method_arity_after_receiver(mlc::Array<ast::Diagnostic> receiver_errors, mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
  if ((receiver_errors.length() > 0))   {
    return mlc::Array<ast::Diagnostic>{};
  } else   {
    return infer_builtin_method_arity_diagnostics(method_name, argument_count, method_span);
  }
}
mlc::Array<ast::Diagnostic> index_not_array_diagnostic(std::shared_ptr<registry::Type> receiver_type, ast::Span bracket_source_span) noexcept{
  if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_array(receiver_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("indexing requires an array, got ", 32) + semantic_type_structure::type_description(receiver_type)), bracket_source_span, diagnostic_codes::diagnostic_code_e017())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> index_not_i32_diagnostic(std::shared_ptr<registry::Type> index_type, ast::Span bracket_source_span) noexcept{
  if (((!semantic_type_structure::type_is_unknown(index_type)) && (!semantic_type_structure::type_is_i32(index_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("array index must be i32, got ", 29) + semantic_type_structure::type_description(index_type)), bracket_source_span, diagnostic_codes::diagnostic_code_e018())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> if_branch_mismatch_diagnostic(bool branches_mismatch, std::shared_ptr<registry::Type> then_type, std::shared_ptr<registry::Type> else_type, std::shared_ptr<ast::Expr> else_expression) noexcept{
  if (branches_mismatch)   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("if branch types differ: then ", 29) + semantic_type_structure::type_description(then_type)) + mlc::String(", else ", 7)) + semantic_type_structure::type_description(else_type)), ast::expr_span(else_expression), diagnostic_codes::diagnostic_code_e015())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> for_loop_range_diagnostic(std::shared_ptr<registry::Type> iterator_type, std::shared_ptr<ast::Expr> iterator) noexcept{
  if (((!semantic_type_structure::type_is_unknown(iterator_type)) && (!semantic_type_structure::type_is_array(iterator_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("for loop range must be an array, got ", 37) + semantic_type_structure::type_description(iterator_type)), ast::expr_span(iterator), diagnostic_codes::diagnostic_code_e019())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
mlc::Array<ast::Diagnostic> match_arm_type_mismatch_diagnostic(int arm_index, std::shared_ptr<registry::Type> first_arm_type, std::shared_ptr<registry::Type> arm_result_type, std::shared_ptr<ast::Expr> arm_body) noexcept{
  if (((((arm_index > 0) && (!semantic_type_structure::type_is_unknown(first_arm_type))) && (!semantic_type_structure::type_is_unknown(arm_result_type))) && (!semantic_type_structure::types_structurally_equal(first_arm_type, arm_result_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("match arm result type ", 22) + semantic_type_structure::type_description(arm_result_type)) + mlc::String(" does not match first arm type ", 31)) + semantic_type_structure::type_description(first_arm_type)), ast::expr_span(arm_body), diagnostic_codes::diagnostic_code_e020())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
std::shared_ptr<registry::Type> inner_type_from_object_type(std::shared_ptr<registry::Type> object_type) noexcept{
  return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [inner] = tShared; return inner; },
[&](const registry::TI32& tI32) { return object_type; },
[&](const registry::TString& tString) { return object_type; },
[&](const registry::TBool& tBool) { return object_type; },
[&](const registry::TUnit& tUnit) { return object_type; },
[&](const registry::TI64& tI64) { return object_type; },
[&](const registry::TF64& tF64) { return object_type; },
[&](const registry::TU8& tU8) { return object_type; },
[&](const registry::TUsize& tUsize) { return object_type; },
[&](const registry::TChar& tChar) { return object_type; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return object_type; },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return object_type; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return object_type; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return object_type; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return object_type; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return object_type; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return object_type; },
[&](const registry::TUnknown& tUnknown) { return object_type; }
}, (*object_type));
}
mlc::String field_lookup_type_name_from_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return name; },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); }
}, (*type_value));
}
mlc::Array<ast::Diagnostic> infer_expr_field_diagnostics(std::shared_ptr<registry::Type> object_type, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept{
  auto inner_object_type = inner_type_from_object_type(object_type);
  auto type_name = field_lookup_type_name_from_type(inner_object_type);
  if (((type_name != mlc::String("", 0)) && registry::TypeRegistry_has_fields(registry, type_name)))   {
    if ((!registry::TypeRegistry_fields_for(registry, type_name).has(field_name)))     {
      return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("undefined field: ", 17) + field_name) + mlc::String(" on ", 4)) + type_name), field_source_span, diagnostic_codes::diagnostic_code_e016())};
    } else     {
      return mlc::Array<ast::Diagnostic>{};
    }
  } else if ((!semantic_type_structure::type_is_unknown(object_type)))   {
    if (((type_name != mlc::String("", 0)) && (!registry::TypeRegistry_has_fields(registry, type_name))))     {
      return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("field access needs a record type; ", 34) + type_name) + mlc::String(" is not one", 11)), field_source_span, diagnostic_codes::diagnostic_code_e016())};
    } else if (semantic_type_structure::type_is_array(object_type))     {
      return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("array value has no record fields for .", 38) + field_name), field_source_span, diagnostic_codes::diagnostic_code_e016())};
    } else if ((!semantic_type_structure::type_is_function(object_type)))     {
      return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("field access on value without record fields: ", 45) + semantic_type_structure::type_description(object_type)), field_source_span, diagnostic_codes::diagnostic_code_e016())};
    } else     {
      return mlc::Array<ast::Diagnostic>{};
    }
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}

} // namespace type_diagnostics
