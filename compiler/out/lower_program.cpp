#include "lower_program.hpp"

#include "semantic_ir.hpp"
#include "mir_types.hpp"
#include "lower_fn.hpp"
#include "mir_passes.hpp"

namespace lower_program {

using namespace semantic_ir;
using namespace mir_types;
using namespace lower_fn;
using namespace mir_passes;

mlc::Array<mir_types::MirFunction> mir_lower_append_function(mlc::Array<mir_types::MirFunction> functions, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::Array<mir_types::MirFunction> mir_lower_append_declaration(mlc::Array<mir_types::MirFunction> functions, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::Array<mir_types::MirFunction> mir_lower_declarations(mlc::Array<mir_types::MirFunction> functions, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;

mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mlc::Array<mir_types::MirFunction> mir_lower_append_function(mlc::Array<mir_types::MirFunction> functions, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mir_types::MirFunction>& ok) -> mlc::Array<mir_types::MirFunction> { auto [function] = ok; return [&]() -> mlc::Array<mir_types::MirFunction> { 
  mlc::Array<mir_types::MirFunction> next_functions = functions;
  next_functions.push_back(mir_passes::run_mir_passes_on_function(function));
  return next_functions;
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::Array<mir_types::MirFunction> { auto [_w0] = err; return functions; }
}, lower_fn::lower_semantic_function(declaration));}

mlc::Array<mir_types::MirFunction> mir_lower_append_declaration(mlc::Array<mir_types::MirFunction> functions, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> mlc::Array<mir_types::MirFunction> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(declaration))); auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7] = _v_semanticdeclarationfn; return mir_lower_append_function(functions, declaration); } return functions; }();}

mlc::Array<mir_types::MirFunction> mir_lower_declarations(mlc::Array<mir_types::MirFunction> functions, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
int index = 0;
mlc::Array<mir_types::MirFunction> accumulated = functions;
while (index < declarations.size()){
{
accumulated = mir_lower_append_declaration(accumulated, declarations[index]);
index = index + 1;
}
}
return accumulated;
}

mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
mlc::Array<mir_types::MirFunction> functions = {};
int index = 0;
while (index < items.size()){
{
functions = mir_lower_declarations(functions, items[index].decls);
index = index + 1;
}
}
return mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{functions}}};
}

} // namespace lower_program
