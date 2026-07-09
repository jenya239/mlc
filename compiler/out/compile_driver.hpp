#ifndef COMPILE_DRIVER_HPP
#define COMPILE_DRIVER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "pipeline.hpp"
#include "profile.hpp"
#include "dump_flags.hpp"
#include "path_normalize.hpp"
#include "program_merge.hpp"

namespace compile_driver {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands, bool verify_each_pass, bool dump_ast, bool dump_sem, bool dump_mir, bool mir_bootstrap_report, bool time_passes, bool run_interpreter, bool trace_vm) noexcept;

} // namespace compile_driver

#endif // COMPILE_DRIVER_HPP
