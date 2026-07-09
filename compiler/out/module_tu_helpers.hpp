#ifndef MODULE_TU_HELPERS_HPP
#define MODULE_TU_HELPERS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_naming.hpp"
#include "cpp_ast.hpp"
namespace module_tu_helpers {

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> push_parsed_include_line(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String line) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_cpp_declarations_from_include_text(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String include_text) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_include_guard_ifndef_cpp_declaration(mlc::String guard_macro) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_include_guard_define_cpp_declaration(mlc::String guard_macro) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_include_guard_endif_cpp_declaration(mlc::String guard_macro) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_namespace_open_cpp_declaration(mlc::String namespace_identifier) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_namespace_close_cpp_declaration(mlc::String namespace_identifier) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_define_main_as_user_main_cpp_declaration() noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_quoted_include_cpp_declaration(mlc::String header_filename) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_blank_line_cpp_declaration() noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_using_namespace_cpp_declaration(mlc::String namespace_identifier) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_using_namespace_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::Array<mlc::String> import_paths) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_entry_host_main_cpp_declaration(mlc::String qualified_namespace) noexcept;

} // namespace module_tu_helpers

#endif // MODULE_TU_HELPERS_HPP
