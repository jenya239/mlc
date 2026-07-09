#ifndef MAIN_HPP
#define MAIN_HPP

#include "mlc.hpp"
#include <variant>

#include "cli.hpp"

namespace mlc_main {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

int main(int argc, char** argv) noexcept;

} // namespace mlc_main

#endif // MAIN_HPP
