#include "test_verify_ir.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "verify_ast.hpp"
#include "verify_semantic_ir.hpp"
#include "registry.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"

namespace test_verify_ir {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace verify_ast;
using namespace verify_semantic_ir;
using namespace registry;
using namespace ast;
using namespace semantic_ir;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> verify_ir_tests() noexcept;

mlc::Array<test_runner::TestResult> verify_ir_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
mlc::String valid_source = mlc::String("fn answer() -> i32 = 42");
ast::Program valid_program = decls::parse_program(lexer::tokenize(valid_source).tokens);
results.push_back(test_runner::assert_eq_int(mlc::String("verify_ast_program valid source"), verify_ast::verify_ast_program(valid_program).size(), 0));
results.push_back(test_runner::assert_true(mlc::String("semantic_type_is_resolved rejects TUnknown"), !verify_semantic_ir::semantic_type_is_resolved(std::make_shared<registry::Type>((registry::TUnknown{})))));
std::shared_ptr<semantic_ir::SemanticExpression> unresolved_expression = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("value"), std::make_shared<registry::Type>((registry::TUnknown{})), ast::span_unknown()));
results.push_back(test_runner::assert_true(mlc::String("semantic expression with TUnknown is unresolved"), !verify_semantic_ir::semantic_type_is_resolved(semantic_ir::sexpr_type(unresolved_expression))));
return results;
}

} // namespace test_verify_ir
