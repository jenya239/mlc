#ifndef TEST_LSP_SERVER_HPP
#define TEST_LSP_SERVER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lsp_protocol.hpp"
#include "symbols.hpp"
#include "hover.hpp"
#include "diagnostics.hpp"
#include "server.hpp"
#include "lsp_cli.hpp"

namespace test_lsp_server {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> lsp_server_tests() noexcept;

} // namespace test_lsp_server

#endif // TEST_LSP_SERVER_HPP
