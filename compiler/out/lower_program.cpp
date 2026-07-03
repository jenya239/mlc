#define main mlc_user_main
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

mlc::Array<mlc::String> mir_lower_append_errors(mlc::Array<mlc::String> accumulated, mlc::Array<mlc::String> errors) noexcept{
  auto next = accumulated;
  auto index = 0;
  while ((index < errors.length()))   {
    next.push_back(errors[index]);
    (index = (index + 1));
  }
  return next;
}
MirLowerAccum mir_lower_append_function(MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return std::visit(overloaded{[&](const ast::Ok<mir_types::MirFunction>& ok) -> MirLowerAccum { auto [function] = ok; return [&]() {
auto next_functions = accum.functions;
next_functions.push_back(mir_passes::run_mir_passes_on_function(function));
return MirLowerAccum{next_functions, accum.errors};
}(); },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> MirLowerAccum { auto [errors] = err; return MirLowerAccum{accum.functions, mir_lower_append_errors(accum.errors, errors)}; }
}, lower_fn::lower_semantic_function(declaration));
}
MirLowerAccum mir_lower_append_declaration(MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> MirLowerAccum {
auto __match_subject = semantic_ir::sdecl_inner(declaration);
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, __5, __6, __7] = semanticDeclarationFn; return mir_lower_append_function(accum, declaration);
}
return accum;
std::abort();
}();
}
MirLowerAccum mir_lower_declarations(MirLowerAccum accum, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
  auto index = 0;
  auto next = accum;
  while ((index < declarations.length()))   {
    (next = mir_lower_append_declaration(next, declarations[index]));
    (index = (index + 1));
  }
  return next;
}
MirLowerAccum mir_lower_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
  auto index = 0;
  auto accum = MirLowerAccum{{}, {}};
  while ((index < items.length()))   {
    (accum = mir_lower_declarations(accum, items[index].decls));
    (index = (index + 1));
  }
  return accum;
}
mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
  return mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mir_lower_items(items).functions}}};
}
ast::Result<mir_types::MirProgram, mlc::Array<mlc::String>> build_mir_program_from_semantic_items_checked(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
  auto accum = mir_lower_items(items);
  if ((accum.errors.length() > 0))   {
    return ast::Err<mlc::Array<mlc::String>>{accum.errors};
  } else   {
    return ast::Ok<mir_types::MirProgram>{mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{accum.functions}}}};
  }
}

} // namespace lower_program
