#ifndef MIR_BOOTSTRAP_REPORT_HPP
#define MIR_BOOTSTRAP_REPORT_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "mir_types.hpp"
#include "lower_program.hpp"
#include "mir_to_cpp.hpp"

namespace mir_bootstrap_report {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct MirBootstrapFunctionEntry {mlc::String function_name;int mir_block_count;bool cpp_ok;mlc::String cpp_signature;};

struct MirBootstrapReport {int mir_function_count;int simple_function_count;int cpp_ok_count;int cpp_skip_count;mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries;};

mir_bootstrap_report::MirBootstrapReport build_mir_bootstrap_report(mir_types::MirProgram program) noexcept;

mir_bootstrap_report::MirBootstrapReport build_mir_bootstrap_report_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mlc::String print_mir_bootstrap_report(mir_bootstrap_report::MirBootstrapReport report) noexcept;

bool mir_bootstrap_report_cpp_matches_mir(mir_bootstrap_report::MirBootstrapReport report) noexcept;

} // namespace mir_bootstrap_report

#endif // MIR_BOOTSTRAP_REPORT_HPP
