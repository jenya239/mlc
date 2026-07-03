#define main mlc_user_main
#include "cpp_ast.hpp"

namespace cpp_ast {

CppFnModifiers cpp_fn_modifiers_none() noexcept{
  return CppFnModifiers{false, false, false, false, false, false, false, false, false};
}
mlc::String cpp_capture_name(std::shared_ptr<CppCapture> capture) noexcept{
  return capture->name;
}
bool cpp_capture_by_reference(std::shared_ptr<CppCapture> capture) noexcept{
  return capture->by_reference;
}
mlc::String cpp_param_name(std::shared_ptr<CppParam> parameter) noexcept{
  return parameter->name;
}
std::shared_ptr<CppType> cpp_param_type(std::shared_ptr<CppParam> parameter) noexcept{
  return parameter->parameter_type;
}
mlc::String cpp_field_type(std::shared_ptr<CppField> field) noexcept{
  return field->type_value;
}
mlc::String cpp_field_name(std::shared_ptr<CppField> field) noexcept{
  return field->name;
}
mlc::String cpp_variant_arm_name(std::shared_ptr<CppVariantArm> arm) noexcept{
  return arm->name;
}
mlc::Array<std::shared_ptr<CppType>> cpp_variant_arm_types(std::shared_ptr<CppVariantArm> arm) noexcept{
  return arm->types;
}
mlc::Array<std::shared_ptr<CppDeclaration>> cpp_file_header(std::shared_ptr<CppFile> file) noexcept{
  return file->header;
}
mlc::Array<std::shared_ptr<CppDeclaration>> cpp_file_source(std::shared_ptr<CppFile> file) noexcept{
  return file->source;
}

} // namespace cpp_ast
