#include "codegen_harness.hpp"

#include "ast.hpp"
#include "load_item.hpp"
#include "module.hpp"
#include "program_to_semantic.hpp"

namespace codegen_harness {

using namespace ast;
using namespace load_item;
using namespace module;
using namespace program_to_semantic;
using namespace ast_tokens;

mlc::String gen_program(ast::Program program) noexcept;

mlc::String gen_program(ast::Program program) noexcept{
ast::Result<semantic_ir::SemanticLoadItem, mlc::Array<mlc::String>> semantic_result = program_to_semantic::program_to_semantic_load_item(program, mlc::String("test_main"));
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [errors] = err; return mlc::String("/* transform error */ ") + errors.join(mlc::String(", ")); },
  [&](const ast::Ok<semantic_ir::SemanticLoadItem>& ok) -> mlc::String { auto [semantic_load_item] = ok; return [&]() -> mlc::String { 
  mlc::Array<load_item::LoadItem> all_items = {};
  context::PrecomputedCtx precomputed_context = module::precompute(program, all_items);
  context::GenModuleOut module_parsed = module::gen_module(semantic_load_item, all_items, program, precomputed_context);
  return module_parsed.header + module_parsed.source;
 }(); }
}, semantic_result);
}

} // namespace codegen_harness
