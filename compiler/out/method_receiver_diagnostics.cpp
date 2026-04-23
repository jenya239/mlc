#include "method_receiver_diagnostics.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace method_receiver_diagnostics {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> infer_method_receiver_push(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_set(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_length_or_size(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_to_integer_string_only(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_string_only_methods(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_join(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_has(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_get_remove_keys_values(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_push(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("push") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method push expects an array receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_set(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("set") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) && !semantic_type_structure::receiver_type_is_map(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method set expects an array or Map receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_length_or_size(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("length") || method_name == mlc::String("size") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_string(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("methods length and size apply to string or array; receiver is ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_to_integer_string_only(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("to_i") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_string(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method to_i expects a string receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_string_only_methods(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("char_at") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_string(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a string receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_join(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("join") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method join expects an array receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_has(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("has") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::receiver_type_is_map(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method has expects a Map receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_get_remove_keys_values(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("get") || method_name == mlc::String("remove") || method_name == mlc::String("keys") || method_name == mlc::String("values") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::receiver_type_is_map(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a Map receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept{
mlc::Array<ast::Diagnostic> from_push = infer_method_receiver_push(method_name, receiver_type, method_span);
return from_push.size() > 0 ? from_push : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_set = infer_method_receiver_set(method_name, receiver_type, method_span);
  return from_set.size() > 0 ? from_set : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_length = infer_method_receiver_length_or_size(method_name, receiver_type, method_span);
  return from_length.size() > 0 ? from_length : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_to_i = infer_method_receiver_to_integer_string_only(method_name, receiver_type, method_span);
  return from_to_i.size() > 0 ? from_to_i : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_string_ops = infer_method_receiver_string_only_methods(method_name, receiver_type, method_span);
  return from_string_ops.size() > 0 ? from_string_ops : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_join = infer_method_receiver_join(method_name, receiver_type, method_span);
  return from_join.size() > 0 ? from_join : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_map_access = infer_method_receiver_get_remove_keys_values(method_name, receiver_type, method_span);
  return from_map_access.size() > 0 ? from_map_access : infer_method_receiver_has(method_name, receiver_type, method_span);
 }();
 }();
 }();
 }();
 }();
 }();
}

} // namespace method_receiver_diagnostics
