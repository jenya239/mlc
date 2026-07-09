#ifndef EXPECT_CLOSE_HPP
#define EXPECT_CLOSE_HPP

#include "mlc.hpp"
#include <variant>
#include "predicates.hpp"
namespace expect_close {

predicates::Parser parser_expect_rbrace(predicates::Parser state, mlc::String context) noexcept;
predicates::Parser parser_expect_rparen(predicates::Parser state, mlc::String context) noexcept;
predicates::Parser parser_expect_rbracket(predicates::Parser state, mlc::String context) noexcept;

} // namespace expect_close

#endif // EXPECT_CLOSE_HPP
