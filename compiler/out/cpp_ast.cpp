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

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_file_header(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_file_source(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String cpp_capture_name(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept{return capture->name;}

bool cpp_capture_by_reference(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept{return capture->by_reference;}

mlc::String cpp_param_name(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept{return parameter->name;}

std::shared_ptr<cpp_ast::CppType> cpp_param_type(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept{return parameter->parameter_type;}

mlc::String cpp_field_type(std::shared_ptr<cpp_ast::CppField> field) noexcept{return field->typ;}

mlc::String cpp_field_name(std::shared_ptr<cpp_ast::CppField> field) noexcept{return field->name;}

mlc::String cpp_variant_arm_name(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept{return arm->name;}

mlc::Array<std::shared_ptr<cpp_ast::CppType>> cpp_variant_arm_types(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept{return arm->types;}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_file_header(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return file->header;}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_file_source(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return file->source;}

} // namespace cpp_ast
