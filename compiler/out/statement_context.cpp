#include "statement_context.hpp"

#include "semantic_ir.hpp"
#include "context.hpp"

namespace statement_context {

using namespace semantic_ir;
using namespace context;

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{
context::CodegenContext current_context = std::move(context);
int index = 0;
while (index < statements.size()){
{
context::mutate_context_from_statement(statements[index], current_context);
index = index + 1;
}
}
return current_context;
}

} // namespace statement_context
