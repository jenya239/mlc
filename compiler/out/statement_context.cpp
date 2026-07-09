#define main mlc_user_main
#include "statement_context.hpp"

#include "semantic_ir.hpp"
#include "context.hpp"

namespace statement_context {

using namespace semantic_ir;
using namespace context;

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{
  auto current_context = context;
  auto index = 0;
  while ((index < statements.length()))   {
    context::mutate_context_from_statement(statements[index], current_context);
    (index = (index + 1));
  }
  return current_context;
}

} // namespace statement_context
