#include "expect_close.hpp"

#include "predicates.hpp"

namespace expect_close {

using namespace predicates;

predicates::Parser parser_expect_rbrace(predicates::Parser state, mlc::String context) noexcept;

predicates::Parser parser_expect_rparen(predicates::Parser state, mlc::String context) noexcept;

predicates::Parser parser_expect_rbracket(predicates::Parser state, mlc::String context) noexcept;

predicates::Parser parser_expect_rbrace(predicates::Parser state, mlc::String context) noexcept{return predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : predicates::Parser_record_parse_error(state, mlc::String("parse: expected } in ") + context);}

predicates::Parser parser_expect_rparen(predicates::Parser state, mlc::String context) noexcept{return predicates::TokenKind_is_rparen(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : predicates::Parser_record_parse_error(state, mlc::String("parse: expected ) in ") + context);}

predicates::Parser parser_expect_rbracket(predicates::Parser state, mlc::String context) noexcept{return predicates::TokenKind_is_rbracket(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : predicates::Parser_record_parse_error(state, mlc::String("parse: expected ] in ") + context);}

} // namespace expect_close
