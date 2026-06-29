#include "lower_program.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "mir_types.hpp"
#include "lower_fn.hpp"
#include "mir_passes.hpp"

namespace lower_program {

using namespace ast;
using namespace semantic_ir;
using namespace mir_types;
using namespace lower_fn;
using namespace mir_passes;
using namespace ast_tokens;

struct MirLowerAccum {mlc::Array<mir_types::MirFunction> functions;mlc::Array<mlc::String> errors;};

mlc::Array<mlc::String> mir_lower_append_errors(mlc::Array<mlc::String> accumulated, mlc::Array<mlc::String> errors) noexcept;

lower_program::MirLowerAccum mir_lower_append_function(lower_program::MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

lower_program::MirLowerAccum mir_lower_append_declaration(lower_program::MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

lower_program::MirLowerAccum mir_lower_declarations(lower_program::MirLowerAccum accum, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;

lower_program::MirLowerAccum mir_lower_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

ast::Result<mir_types::MirProgram, mlc::Array<mlc::String>> build_mir_program_from_semantic_items_checked(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mlc::Array<mlc::String> mir_lower_append_errors(mlc::Array<mlc::String> accumulated, mlc::Array<mlc::String> errors) noexcept{
mlc::Array<mlc::String> next = accumulated;
int index = 0;
while (index < errors.size()){
{
next.push_back(errors[index]);
index = index + 1;
}
}
return next;
}

lower_program::MirLowerAccum mir_lower_append_function(lower_program::MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mir_types::MirFunction>& ok) -> lower_program::MirLowerAccum { auto [function] = ok; return [&]() -> lower_program::MirLowerAccum { 
  mlc::Array<mir_types::MirFunction> next_functions = accum.functions;
  next_functions.push_back(mir_passes::run_mir_passes_on_function(function));
  return lower_program::MirLowerAccum{next_functions, accum.errors};
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> lower_program::MirLowerAccum { auto [errors] = err; return lower_program::MirLowerAccum{accum.functions, mir_lower_append_errors(accum.errors, errors)}; }
}, lower_fn::lower_semantic_function(declaration));}

lower_program::MirLowerAccum mir_lower_append_declaration(lower_program::MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> lower_program::MirLowerAccum { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(declaration))); auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7] = _v_semanticdeclarationfn; return mir_lower_append_function(accum, declaration); } return accum; }();}

lower_program::MirLowerAccum mir_lower_declarations(lower_program::MirLowerAccum accum, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
int index = 0;
lower_program::MirLowerAccum next = std::move(accum);
while (index < declarations.size()){
{
next = mir_lower_append_declaration(next, declarations[index]);
index = index + 1;
}
}
return next;
}

lower_program::MirLowerAccum mir_lower_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
int index = 0;
lower_program::MirLowerAccum accum = lower_program::MirLowerAccum{{}, {}};
while (index < items.size()){
{
accum = mir_lower_declarations(accum, items[index].decls);
index = index + 1;
}
}
return accum;
}

mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{return mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mir_lower_items(items).functions}}};}

ast::Result<mir_types::MirProgram, mlc::Array<mlc::String>> build_mir_program_from_semantic_items_checked(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
lower_program::MirLowerAccum accum = mir_lower_items(items);
return accum.errors.size() > 0 ? ast::Result<mir_types::MirProgram, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(accum.errors)) : ast::Result<mir_types::MirProgram, mlc::Array<mlc::String>>(ast::Ok<mir_types::MirProgram>(mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{accum.functions}}}));
}

} // namespace lower_program
