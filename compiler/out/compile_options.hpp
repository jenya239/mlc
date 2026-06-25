#ifndef COMPILE_OPTIONS_HPP
#define COMPILE_OPTIONS_HPP

#include "mlc.hpp"
#include <variant>

namespace compile_options {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CompileOptions {mlc::String entry_path;mlc::String out_directory;bool profile_enabled;bool check_only;bool emit_compile_commands;bool verify_each_pass;bool dump_ast;bool dump_sem;bool time_passes;};

mlc::String compile_usage_message() noexcept;

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

} // namespace compile_options

#endif // COMPILE_OPTIONS_HPP
