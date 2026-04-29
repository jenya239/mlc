#define main mlc_user_main
#include "statement_context.hpp"

namespace statement_context {

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{
auto current_context = context;
auto index = 0;
while ((index < statements.length())) {
current_context = context::update_context_from_statement(statements[index], current_context);
index = (index + 1);
}
return current_context;
}

} // namespace statement_context
