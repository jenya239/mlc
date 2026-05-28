#ifndef COMPILE_OPTIONS_HPP
#define COMPILE_OPTIONS_HPP

#include "mlc.hpp"
#include <variant>

namespace compile_options {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

struct CompileOptions {mlc::String entry_path;mlc::String out_directory;bool profile_enabled;bool check_only;};

mlc::String compile_usage_message() noexcept;

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

} // namespace compile_options

#endif // COMPILE_OPTIONS_HPP
