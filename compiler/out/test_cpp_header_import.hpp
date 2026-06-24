#ifndef TEST_CPP_HEADER_IMPORT_HPP
#define TEST_CPP_HEADER_IMPORT_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "header_import.hpp"
#include "registry.hpp"
#include "ast.hpp"

namespace test_cpp_header_import {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_header_import_tests() noexcept;

} // namespace test_cpp_header_import

#endif // TEST_CPP_HEADER_IMPORT_HPP
