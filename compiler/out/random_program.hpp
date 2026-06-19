#ifndef RANDOM_PROGRAM_HPP
#define RANDOM_PROGRAM_HPP

#include "mlc.hpp"
#include <variant>

namespace random_program {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

int fuzz_variant_count() noexcept;

int fuzz_mix(int seed) noexcept;

int fuzz_pick(int seed, int choice_count) noexcept;

int fuzz_program_kind(int seed) noexcept;

mlc::String fuzz_random_program(int seed) noexcept;

} // namespace random_program

#endif // RANDOM_PROGRAM_HPP
