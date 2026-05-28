#ifndef RANDOM_PROGRAM_HPP
#define RANDOM_PROGRAM_HPP

#include "mlc.hpp"
#include <variant>

namespace random_program {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

int fuzz_mix(int seed) noexcept;

int fuzz_pick(int seed, int choice_count) noexcept;

mlc::String fuzz_random_program(int seed) noexcept;

} // namespace random_program

#endif // RANDOM_PROGRAM_HPP
