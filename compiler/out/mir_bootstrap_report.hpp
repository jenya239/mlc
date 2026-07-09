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

struct MirBootstrapFunctionEntry {
  mlc::String function_name;
  int mir_block_count;
  bool cpp_ok;
  mlc::String cpp_signature;
};
struct MirBootstrapReport {
  int mir_function_count;
  int simple_function_count;
  int cpp_ok_count;
  int cpp_skip_count;
  mlc::Array<MirBootstrapFunctionEntry> entries;
};
mlc::String mir_bootstrap_cpp_signature_from_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;
int mir_bootstrap_block_count(mir_types::MirFunction function) noexcept;
MirBootstrapFunctionEntry mir_bootstrap_entry_for_complex_function(mir_types::MirFunction function) noexcept;
mlc::String mir_bootstrap_cpp_signature_for_function(mir_types::MirFunction function) noexcept;
MirBootstrapFunctionEntry mir_bootstrap_entry_for_simple_function(mir_types::MirFunction function) noexcept;
MirBootstrapFunctionEntry mir_bootstrap_entry_for_function(mir_types::MirFunction function) noexcept;
mlc::Array<MirBootstrapFunctionEntry> mir_bootstrap_append_function_entries(mlc::Array<MirBootstrapFunctionEntry> entries, mlc::Array<mir_types::MirFunction> functions, int index) noexcept;
int mir_bootstrap_count_cpp_ok(mlc::Array<MirBootstrapFunctionEntry> entries, int index, int count) noexcept;
int mir_bootstrap_count_simple_functions(mlc::Array<mir_types::MirFunction> functions, int index, int count) noexcept;
int mir_bootstrap_array_length_i32(mlc::Array<mir_types::MirFunction> items) noexcept;
MirBootstrapReport build_mir_bootstrap_report(mir_types::MirProgram program) noexcept;
MirBootstrapReport build_mir_bootstrap_report_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;
mlc::Array<mlc::String> mir_bootstrap_append_entry_lines(mlc::Array<mlc::String> lines, mlc::Array<MirBootstrapFunctionEntry> entries, int index) noexcept;
mlc::String mir_bootstrap_join_lines(mlc::Array<mlc::String> lines) noexcept;
mlc::String print_mir_bootstrap_report(MirBootstrapReport report) noexcept;
bool mir_bootstrap_report_cpp_matches_mir(MirBootstrapReport report) noexcept;

} // namespace mir_bootstrap_report

#endif // MIR_BOOTSTRAP_REPORT_HPP
