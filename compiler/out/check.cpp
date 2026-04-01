#include "check.hpp"

#include "ast.hpp"
#include "names.hpp"
#include "registry.hpp"
#include "infer.hpp"
#include "check_context.hpp"

namespace check {

using namespace ast;
using namespace names;
using namespace registry;
using namespace infer;
using namespace check_context;
using namespace ast_tokens;

bool CheckOut_has_errors(check::CheckOut self) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

bool CheckOut_has_errors(check::CheckOut self) noexcept{return self.errors.size() > 0;}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
mlc::HashMap<mlc::String, bool> globals = names::collect_globals(full_program);
registry::TypeRegistry registry = registry::build_registry(full_program);
mlc::Array<ast::Diagnostic> all_diagnostics = {};
int declaration_index = 0;
while (declaration_index < entry.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::tuple<> { auto [_w0, type_parameters, _w1, parameters, _w2, body] = declfn; return [&]() -> std::tuple<> { 
  mlc::Array<mlc::String> locals = {};
  int type_parameter_index = 0;
  while (type_parameter_index < type_parameters.size()){
{
locals.push_back(type_parameters[type_parameter_index]);
globals.set(type_parameters[type_parameter_index], true);
type_parameter_index = type_parameter_index + 1;
}
}
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int parameter_index = 0;
  while (parameter_index < parameters.size()){
{
locals.push_back(ast::param_name(parameters[parameter_index]));
type_environment.set(ast::param_name(parameters[parameter_index]), registry::type_from_annotation(ast::param_typ(parameters[parameter_index])));
parameter_index = parameter_index + 1;
}
}
  all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body, locals, globals));
  check_context::CheckContext inference_context = check_context::check_context_new(type_environment, registry);
  infer_result::InferResult inference_result = infer::infer_expr(body, inference_context);
  all_diagnostics = ast::diagnostics_append(all_diagnostics, inference_result.errors);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [_w0, _w1, _w2] = decltype_; return std::make_tuple(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [_w0, _w1, _w2] = decltrait; return std::make_tuple(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [_w0, _w1, _w2] = declextend; return std::make_tuple(); },
  [&](const DeclImport& declimport) -> std::tuple<> { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) -> std::tuple<> { auto [_w0] = declexported; return std::make_tuple(); }
}, (*ast::decl_inner(entry.decls[declaration_index])));
declaration_index = declaration_index + 1;
}
}
return all_diagnostics.size() > 0 ? ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(ast::diagnostics_to_strings(all_diagnostics))) : ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Ok<check::CheckOut>(check::CheckOut{{}, registry}));
}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{return check_program_against_full(entry, full);}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept{return check_program_against_full(program, program);}

} // namespace check
