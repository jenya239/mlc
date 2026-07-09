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

mlc::String mir_bootstrap_cpp_signature_from_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

int mir_bootstrap_block_count(mir_types::MirFunction function) noexcept;

mir_bootstrap_report::MirBootstrapFunctionEntry mir_bootstrap_entry_for_complex_function(mir_types::MirFunction function) noexcept;

mlc::String mir_bootstrap_cpp_signature_for_function(mir_types::MirFunction function) noexcept;

mir_bootstrap_report::MirBootstrapFunctionEntry mir_bootstrap_entry_for_simple_function(mir_types::MirFunction function) noexcept;

mir_bootstrap_report::MirBootstrapFunctionEntry mir_bootstrap_entry_for_function(mir_types::MirFunction function) noexcept;

mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> mir_bootstrap_append_function_entries(mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries, mlc::Array<mir_types::MirFunction> functions, int index) noexcept;

int mir_bootstrap_count_cpp_ok(mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries, int index, int count) noexcept;

int mir_bootstrap_count_simple_functions(mlc::Array<mir_types::MirFunction> functions, int index, int count) noexcept;

int mir_bootstrap_array_length_i32(mlc::Array<mir_types::MirFunction> items) noexcept;

mir_bootstrap_report::MirBootstrapReport build_mir_bootstrap_report(mir_types::MirProgram program) noexcept;

mir_bootstrap_report::MirBootstrapReport build_mir_bootstrap_report_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mlc::Array<mlc::String> mir_bootstrap_append_entry_lines(mlc::Array<mlc::String> lines, mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries, int index) noexcept;

mlc::String mir_bootstrap_join_lines(mlc::Array<mlc::String> lines) noexcept;

mlc::String print_mir_bootstrap_report(mir_bootstrap_report::MirBootstrapReport report) noexcept;

bool mir_bootstrap_report_cpp_matches_mir(mir_bootstrap_report::MirBootstrapReport report) noexcept;

mlc::String mir_bootstrap_cpp_signature_from_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppFnDef>((*declaration))) { auto _v_cppfndef = std::get<cpp_ast::CppFnDef>((*declaration)); auto [_w0, return_type, name, _w1, _w2, _w3] = _v_cppfndef; return return_type + mlc::String(" ") + name + mlc::String("()"); } return mlc::String(""); }();}

int mir_bootstrap_block_count(mir_types::MirFunction function) noexcept{
int block_count = 0;
int block_index = 0;
while (block_index < function.blocks.size()){
{
block_count = block_count + 1;
block_index = block_index + 1;
}
}
return block_count;
}

mir_bootstrap_report::MirBootstrapFunctionEntry mir_bootstrap_entry_for_complex_function(mir_types::MirFunction function) noexcept{return mir_bootstrap_report::MirBootstrapFunctionEntry{function.name, mir_bootstrap_block_count(function), false, mlc::String("")};}

mlc::String mir_bootstrap_cpp_signature_for_function(mir_types::MirFunction function) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<std::shared_ptr<cpp_ast::CppDeclaration>>& ok) -> mlc::String { auto [declaration] = ok; return mir_bootstrap_cpp_signature_from_declaration(declaration); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [_w0] = err; return mlc::String(""); }
}, mir_to_cpp::mir_function_to_cpp_declaration(function));}

mir_bootstrap_report::MirBootstrapFunctionEntry mir_bootstrap_entry_for_simple_function(mir_types::MirFunction function) noexcept{
mlc::String signature = mir_bootstrap_cpp_signature_for_function(function);
return mir_bootstrap_report::MirBootstrapFunctionEntry{function.name, mir_bootstrap_block_count(function), signature != mlc::String(""), signature};
}

mir_bootstrap_report::MirBootstrapFunctionEntry mir_bootstrap_entry_for_function(mir_types::MirFunction function) noexcept{return [&]() -> mir_bootstrap_report::MirBootstrapFunctionEntry { if (mir_to_cpp::mir_function_is_simple(function) == true) { return mir_bootstrap_entry_for_simple_function(function); } if (mir_to_cpp::mir_function_is_simple(function) == false) { return mir_bootstrap_entry_for_complex_function(function); } }();}

mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> mir_bootstrap_append_function_entries(mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries, mlc::Array<mir_types::MirFunction> functions, int index) noexcept{
if (index >= functions.size()){
{
return entries;
}
}
mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> next_entries = entries;
next_entries.push_back(mir_bootstrap_entry_for_function(functions[index]));
return mir_bootstrap_append_function_entries(next_entries, functions, index + 1);
}

int mir_bootstrap_count_cpp_ok(mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries, int index, int count) noexcept{
if (index >= entries.size()){
{
return count;
}
}
mir_bootstrap_report::MirBootstrapFunctionEntry entry = entries[index];
int next_count = entry.cpp_ok ? count + 1 : count;
return mir_bootstrap_count_cpp_ok(entries, index + 1, next_count);
}

int mir_bootstrap_count_simple_functions(mlc::Array<mir_types::MirFunction> functions, int index, int count) noexcept{
if (index >= functions.size()){
{
return count;
}
}
int next_count = mir_to_cpp::mir_function_is_simple(functions[index]) ? count + 1 : count;
return mir_bootstrap_count_simple_functions(functions, index + 1, next_count);
}

int mir_bootstrap_array_length_i32(mlc::Array<mir_types::MirFunction> items) noexcept{
int count = 0;
int index = 0;
while (index < items.size()){
{
count = count + 1;
index = index + 1;
}
}
return count;
}

mir_bootstrap_report::MirBootstrapReport build_mir_bootstrap_report(mir_types::MirProgram program) noexcept{
mlc::Array<mir_types::MirFunction> functions = program.modules[0].functions;
mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries = mir_bootstrap_append_function_entries({}, functions, 0);
int cpp_ok_count = mir_bootstrap_count_cpp_ok(entries, 0, 0);
return mir_bootstrap_report::MirBootstrapReport{mir_bootstrap_array_length_i32(functions), mir_bootstrap_count_simple_functions(functions, 0, 0), cpp_ok_count, mir_bootstrap_array_length_i32(functions) - cpp_ok_count, entries};
}

mir_bootstrap_report::MirBootstrapReport build_mir_bootstrap_report_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{return build_mir_bootstrap_report(lower_program::build_mir_program_from_semantic_items(items));}

mlc::Array<mlc::String> mir_bootstrap_append_entry_lines(mlc::Array<mlc::String> lines, mlc::Array<mir_bootstrap_report::MirBootstrapFunctionEntry> entries, int index) noexcept{return index >= entries.size() ? lines : [&]() -> mlc::Array<mlc::String> { 
  mir_bootstrap_report::MirBootstrapFunctionEntry entry = entries[index];
  mlc::Array<mlc::String> next_lines = lines;
  if (entry.cpp_ok){
{
next_lines.push_back(mlc::String("  fn ") + entry.function_name + mlc::String(" blocks=") + mlc::to_string(entry.mir_block_count) + mlc::String(" cpp_ok signature=") + entry.cpp_signature);
}
} else {
{
next_lines.push_back(mlc::String("  fn ") + entry.function_name + mlc::String(" blocks=") + mlc::to_string(entry.mir_block_count) + mlc::String(" cpp_skip"));
}
}
  return mir_bootstrap_append_entry_lines(next_lines, entries, index + 1);
 }();}

mlc::String mir_bootstrap_join_lines(mlc::Array<mlc::String> lines) noexcept{
if (lines.size() == 0){
{
return mlc::String("");
}
}
mlc::String result = lines[0];
int index = 1;
while (index < lines.size()){
{
result = result + mlc::String("\n") + lines[index];
index = index + 1;
}
}
return result;
}

mlc::String print_mir_bootstrap_report(mir_bootstrap_report::MirBootstrapReport report) noexcept{
mlc::Array<mlc::String> lines = mlc::Array<mlc::String>{mlc::String("mir_functions=") + mlc::to_string(report.mir_function_count) + mlc::String(" simple=") + mlc::to_string(report.simple_function_count) + mlc::String(" cpp_ok=") + mlc::to_string(report.cpp_ok_count) + mlc::String(" cpp_skip=") + mlc::to_string(report.cpp_skip_count)};
lines = mir_bootstrap_append_entry_lines(lines, report.entries, 0);
return mir_bootstrap_join_lines(lines);
}

bool mir_bootstrap_report_cpp_matches_mir(mir_bootstrap_report::MirBootstrapReport report) noexcept{
int index = 0;
while (index < report.entries.size()){
{
mir_bootstrap_report::MirBootstrapFunctionEntry entry = report.entries[index];
if (entry.cpp_ok && !entry.cpp_signature.contains(entry.function_name)){
{
return false;
}
}
index = index + 1;
}
}
return true;
}

} // namespace mir_bootstrap_report
