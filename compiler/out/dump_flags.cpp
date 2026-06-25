#include "dump_flags.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "ast_printer.hpp"
#include "semantic_ir_dump.hpp"

namespace dump_flags {

using namespace ast;
using namespace semantic_ir;
using namespace ast_printer;
using namespace semantic_ir_dump;
using namespace ast_tokens;

bool dump_label_character_is_safe(mlc::String character) noexcept;

bool dump_label_is_safe(mlc::String label) noexcept;

void emit_dump_ast(ast::Program program, mlc::String label) noexcept;

void emit_dump_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, mlc::String label) noexcept;

bool dump_label_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_") || character == mlc::String("/") || character == mlc::String(".") || character == mlc::String("-");}

bool dump_label_is_safe(mlc::String label) noexcept{
return label == mlc::String("") || label.length() > 256 ? false : [&]() -> bool { 
  bool safe = true;
  int index = 0;
  while (index < label.length() && safe){
{
if (!dump_label_character_is_safe(label.char_at(index))){
{
safe = false;
}
}
index = index + 1;
}
}
  return safe;
 }();
}

void emit_dump_ast(ast::Program program, mlc::String label) noexcept{
if (dump_label_is_safe(label)){
mlc::io::print(mlc::String("--- dump-ast: ") + label + mlc::String(" ---\n"));
mlc::io::print(ast_printer::print_mlc_program(program));
mlc::io::print(mlc::to_string(static_cast<char32_t>(10)));
}
}

void emit_dump_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, mlc::String label) noexcept{
if (dump_label_is_safe(label)){
mlc::io::print(mlc::String("--- dump-sem: ") + label + mlc::String(" ---\n"));
mlc::io::print(semantic_ir_dump::print_semantic_load_items(items));
mlc::io::print(mlc::to_string(static_cast<char32_t>(10)));
}
}

} // namespace dump_flags
