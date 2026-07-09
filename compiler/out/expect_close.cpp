#define main mlc_user_main
#include "expect_close.hpp"

#include "predicates.hpp"

namespace expect_close {

using namespace predicates;

predicates::Parser parser_expect_rbrace(predicates::Parser state, mlc::String context) noexcept{
  if (predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)))   {
    return predicates::Parser_advance(state);
  } else   {
    return predicates::Parser_record_parse_error(state, (mlc::String("parse: expected } in ", 21) + context));
  }
}
predicates::Parser parser_expect_rparen(predicates::Parser state, mlc::String context) noexcept{
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(state)))   {
    return predicates::Parser_advance(state);
  } else   {
    return predicates::Parser_record_parse_error(state, (mlc::String("parse: expected ) in ", 21) + context));
  }
}
predicates::Parser parser_expect_rbracket(predicates::Parser state, mlc::String context) noexcept{
  if (predicates::TokenKind_is_rbracket(predicates::Parser_kind(state)))   {
    return predicates::Parser_advance(state);
  } else   {
    return predicates::Parser_record_parse_error(state, (mlc::String("parse: expected ] in ", 21) + context));
  }
}

} // namespace expect_close
