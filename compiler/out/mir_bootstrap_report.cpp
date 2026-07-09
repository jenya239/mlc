#define main mlc_user_main
#include "mir_bootstrap_report.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "mir_types.hpp"
#include "lower_program.hpp"
#include "mir_to_cpp.hpp"

namespace mir_bootstrap_report {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace mir_types;
using namespace lower_program;
using namespace mir_to_cpp;

mlc::String mir_bootstrap_cpp_signature_from_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = declaration;
if (std::holds_alternative<cpp_ast::CppFnDef>((*__match_subject))) {
const cpp_ast::CppFnDef& cppFnDef = std::get<cpp_ast::CppFnDef>((*__match_subject));
auto [__0, return_type, name, __3, __4, __5] = cppFnDef; return ((((mlc::String("", 0) + mlc::to_string(return_type)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String("()", 2));
}
return mlc::String("", 0);
std::abort();
}();
}
int mir_bootstrap_block_count(mir_types::MirFunction function) noexcept{
  auto block_count = 0;
  auto block_index = 0;
  while ((block_index < function.blocks.length()))   {
    (block_count = (block_count + 1));
    (block_index = (block_index + 1));
  }
  return block_count;
}
MirBootstrapFunctionEntry mir_bootstrap_entry_for_complex_function(mir_types::MirFunction function) noexcept{
  return MirBootstrapFunctionEntry{function.name, mir_bootstrap_block_count(function), false, mlc::String("", 0)};
}
mlc::String mir_bootstrap_cpp_signature_for_function(mir_types::MirFunction function) noexcept{
  return std::visit(overloaded{[&](const ast::Ok<std::shared_ptr<cpp_ast::CppDeclaration>>& ok) -> mlc::String { auto [declaration] = ok; return mir_bootstrap_cpp_signature_from_declaration(declaration); },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [__0] = err; return mlc::String("", 0); }
}, mir_to_cpp::mir_function_to_cpp_declaration(function));
}
MirBootstrapFunctionEntry mir_bootstrap_entry_for_simple_function(mir_types::MirFunction function) noexcept{
  auto signature = mir_bootstrap_cpp_signature_for_function(function);
  return MirBootstrapFunctionEntry{function.name, mir_bootstrap_block_count(function), (signature != mlc::String("", 0)), signature};
}
MirBootstrapFunctionEntry mir_bootstrap_entry_for_function(mir_types::MirFunction function) noexcept{
  return [&]() -> MirBootstrapFunctionEntry {
auto __match_subject = mir_to_cpp::mir_function_is_simple(function);
if ((__match_subject == true)) {
return mir_bootstrap_entry_for_simple_function(function);
}
if ((__match_subject == false)) {
return mir_bootstrap_entry_for_complex_function(function);
}
std::abort();
}();
}
mlc::Array<MirBootstrapFunctionEntry> mir_bootstrap_append_function_entries(mlc::Array<MirBootstrapFunctionEntry> entries, mlc::Array<mir_types::MirFunction> functions, int index) noexcept{
  if ((index >= functions.length()))   {
    return entries;
  }
  auto next_entries = entries;
  next_entries.push_back(mir_bootstrap_entry_for_function(functions[index]));
  return mir_bootstrap_append_function_entries(next_entries, functions, (index + 1));
}
int mir_bootstrap_count_cpp_ok(mlc::Array<MirBootstrapFunctionEntry> entries, int index, int count) noexcept{
  if ((index >= entries.length()))   {
    return count;
  }
  auto entry = entries[index];
  auto next_count = (entry.cpp_ok ? ((count + 1)) : (count));
  return mir_bootstrap_count_cpp_ok(entries, (index + 1), next_count);
}
int mir_bootstrap_count_simple_functions(mlc::Array<mir_types::MirFunction> functions, int index, int count) noexcept{
  if ((index >= functions.length()))   {
    return count;
  }
  auto next_count = (mir_to_cpp::mir_function_is_simple(functions[index]) ? ((count + 1)) : (count));
  return mir_bootstrap_count_simple_functions(functions, (index + 1), next_count);
}
int mir_bootstrap_array_length_i32(mlc::Array<mir_types::MirFunction> items) noexcept{
  auto count = 0;
  auto index = 0;
  while ((index < items.length()))   {
    (count = (count + 1));
    (index = (index + 1));
  }
  return count;
}
MirBootstrapReport build_mir_bootstrap_report(mir_types::MirProgram program) noexcept{
  auto functions = program.modules[0].functions;
  auto entries = mir_bootstrap_append_function_entries(mlc::Array<MirBootstrapFunctionEntry>{}, functions, 0);
  auto cpp_ok_count = mir_bootstrap_count_cpp_ok(entries, 0, 0);
  return MirBootstrapReport{mir_bootstrap_array_length_i32(functions), mir_bootstrap_count_simple_functions(functions, 0, 0), cpp_ok_count, (mir_bootstrap_array_length_i32(functions) - cpp_ok_count), entries};
}
MirBootstrapReport build_mir_bootstrap_report_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
  return build_mir_bootstrap_report(lower_program::build_mir_program_from_semantic_items(items));
}
mlc::Array<mlc::String> mir_bootstrap_append_entry_lines(mlc::Array<mlc::String> lines, mlc::Array<MirBootstrapFunctionEntry> entries, int index) noexcept{
  if ((index >= entries.length()))   {
    return lines;
  } else   {
    auto entry = entries[index];
    auto next_lines = lines;
    if (entry.cpp_ok)     {
      next_lines.push_back(((((((mlc::String("  fn ", 5) + mlc::to_string(entry.function_name)) + mlc::String(" blocks=", 8)) + mlc::to_string(entry.mir_block_count)) + mlc::String(" cpp_ok signature=", 18)) + mlc::to_string(entry.cpp_signature)) + mlc::String("", 0)));
    } else     {
      next_lines.push_back(((((mlc::String("  fn ", 5) + mlc::to_string(entry.function_name)) + mlc::String(" blocks=", 8)) + mlc::to_string(entry.mir_block_count)) + mlc::String(" cpp_skip", 9)));
    }
    return mir_bootstrap_append_entry_lines(next_lines, entries, (index + 1));
  }
}
mlc::String mir_bootstrap_join_lines(mlc::Array<mlc::String> lines) noexcept{
  if ((lines.length() == 0))   {
    return mlc::String("", 0);
  }
  auto result = lines[0];
  auto index = 1;
  while ((index < lines.length()))   {
    (result = ((result + mlc::String("\n", 1)) + lines[index]));
    (index = (index + 1));
  }
  return result;
}
mlc::String print_mir_bootstrap_report(MirBootstrapReport report) noexcept{
  auto lines = mlc::Array<mlc::String>{((((((((mlc::String("mir_functions=", 14) + mlc::to_string(report.mir_function_count)) + mlc::String(" simple=", 8)) + mlc::to_string(report.simple_function_count)) + mlc::String(" cpp_ok=", 8)) + mlc::to_string(report.cpp_ok_count)) + mlc::String(" cpp_skip=", 10)) + mlc::to_string(report.cpp_skip_count)) + mlc::String("", 0))};
  (lines = mir_bootstrap_append_entry_lines(lines, report.entries, 0));
  return mir_bootstrap_join_lines(lines);
}
bool mir_bootstrap_report_cpp_matches_mir(MirBootstrapReport report) noexcept{
  auto index = 0;
  while ((index < report.entries.length()))   {
    auto entry = report.entries[index];
    if ((entry.cpp_ok && (!entry.cpp_signature.contains(entry.function_name))))     {
      return false;
    }
    (index = (index + 1));
  }
  return true;
}

} // namespace mir_bootstrap_report
