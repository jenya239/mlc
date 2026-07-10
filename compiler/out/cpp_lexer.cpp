#define main mlc_user_main
#include "cpp_lexer.hpp"

#include "lexer.hpp"
#include "cpp_tokens.hpp"
#include "cpp_tokens.hpp"

namespace cpp_lexer {

using namespace lexer;
using namespace cpp_tokens;
using namespace cpp_tokens;

bool cpp_is_horizontal_whitespace(mlc::String character) noexcept{
  return ((character == mlc::String(" ", 1)) || (character == mlc::String("\t", 1)));
}
mlc::String cpp_append_range(mlc::String source_text, int start, int end_position, mlc::String accumulated) noexcept{
  if ((end_position > start))   {
    return (accumulated + source_text.substring(start, mlc::arith::checked_sub(end_position, start)));
  } else   {
    return accumulated;
  }
}
bool cpp_lexer_is_pragma_after_hash(lexer::LexState state) noexcept{
  auto source_text = state.source;
  auto scan = lexer::LexState_lex_advance(state);
  while (((!lexer::LexState_eof(scan)) && cpp_is_horizontal_whitespace(lexer::LexState_current(scan))))   {
    (scan = lexer::LexState_lex_advance(scan));
  }
  auto word_start = scan.position;
  while (((!lexer::LexState_eof(scan)) && lexer::is_alnum(lexer::LexState_current(scan))))   {
    (scan = lexer::LexState_lex_advance(scan));
  }
  auto word = source_text.substring(word_start, mlc::arith::checked_sub(scan.position, word_start));
  return (word == mlc::String("pragma", 6));
}
bool cpp_lexer_pragma_line_starts(lexer::LexState state) noexcept{
  if ((lexer::LexState_current(state) != mlc::String("#", 1)))   {
    return false;
  } else   {
    return cpp_lexer_is_pragma_after_hash(state);
  }
}
CppTriviaScan cpp_scan_line_through_newline(lexer::LexState state, mlc::String trivia) noexcept{
  auto source_text = state.source;
  auto current = state;
  auto start = current.position;
  while (((!lexer::LexState_eof(current)) && (lexer::LexState_current(current) != mlc::String("\n", 1))))   {
    (current = lexer::LexState_lex_advance(current));
  }
  if ((!lexer::LexState_eof(current)))   {
    (current = lexer::LexState_lex_advance(current));
  }
  return CppTriviaScan{current, cpp_append_range(source_text, start, current.position, trivia), mlc::String("", 0)};
}
CppTriviaScan cpp_scan_line_comment(lexer::LexState state, mlc::String trivia) noexcept{
  auto source_text = state.source;
  auto start = state.position;
  auto current = lexer::LexState_lex_advance_by(state, 2);
  while (((!lexer::LexState_eof(current)) && (lexer::LexState_current(current) != mlc::String("\n", 1))))   {
    (current = lexer::LexState_lex_advance(current));
  }
  if ((!lexer::LexState_eof(current)))   {
    (current = lexer::LexState_lex_advance(current));
  }
  return CppTriviaScan{current, cpp_append_range(source_text, start, current.position, trivia), mlc::String("", 0)};
}
CppTriviaScan cpp_scan_block_comment(lexer::LexState state, mlc::String trivia) noexcept{
  auto source_text = state.source;
  auto start = state.position;
  auto current = lexer::LexState_lex_advance_by(state, 2);
  auto error = mlc::String("", 0);
  auto closed = false;
  while ((!lexer::LexState_eof(current)))   {
    if (((lexer::LexState_current(current) == mlc::String("*", 1)) && (lexer::LexState_peek(current, 1) == mlc::String("/", 1))))     {
      (current = lexer::LexState_lex_advance_by(current, 2));
      (closed = true);
      break;
    }
    (current = lexer::LexState_lex_advance(current));
  }
  if ((!closed))   {
    (error = mlc::String("unterminated block comment", 26));
  }
  return CppTriviaScan{current, cpp_append_range(source_text, start, current.position, trivia), error};
}
CppTriviaScan cpp_collect_leading_trivia(lexer::LexState state) noexcept{
  auto source_text = state.source;
  auto current = state;
  auto trivia = mlc::String("", 0);
  auto error = mlc::String("", 0);
  while ((!lexer::LexState_eof(current)))   {
    auto character = lexer::LexState_current(current);
    if ((character == mlc::String("\n", 1)))     {
      (trivia = (trivia + mlc::String("\n", 1)));
      (current = lexer::LexState_lex_advance(current));
    } else if (cpp_is_horizontal_whitespace(character))     {
      auto start = current.position;
      while (((!lexer::LexState_eof(current)) && cpp_is_horizontal_whitespace(lexer::LexState_current(current))))       {
        (current = lexer::LexState_lex_advance(current));
      }
      (trivia = cpp_append_range(source_text, start, current.position, trivia));
    } else if (((character == mlc::String("/", 1)) && (lexer::LexState_peek(current, 1) == mlc::String("/", 1))))     {
      auto scan = cpp_scan_line_comment(current, trivia);
      (current = scan.state);
      (trivia = scan.trivia);
      if ((scan.error != mlc::String("", 0)))       {
        (error = scan.error);
      }
    } else if (((character == mlc::String("/", 1)) && (lexer::LexState_peek(current, 1) == mlc::String("*", 1))))     {
      auto scan = cpp_scan_block_comment(current, trivia);
      (current = scan.state);
      (trivia = scan.trivia);
      if ((scan.error != mlc::String("", 0)))       {
        (error = scan.error);
      }
    } else if (((character == mlc::String("#", 1)) && cpp_lexer_pragma_line_starts(current)))     {
      auto scan = cpp_scan_line_through_newline(current, trivia);
      (current = scan.state);
      (trivia = scan.trivia);
    } else     {
      break;
    }
  }
  return CppTriviaScan{current, trivia, error};
}
CppTriviaScan cpp_collect_trailing_trivia(lexer::LexState state) noexcept{
  auto current = state;
  auto trivia = mlc::String("", 0);
  while (((!lexer::LexState_eof(current)) && cpp_is_horizontal_whitespace(lexer::LexState_current(current))))   {
    (trivia = (trivia + lexer::LexState_current(current)));
    (current = lexer::LexState_lex_advance(current));
  }
  return CppTriviaScan{current, trivia, mlc::String("", 0)};
}
CppScanIdR cpp_scan_ident(lexer::LexState state) noexcept{
  auto start = state.position;
  auto input_text = state.source;
  auto line = state.line;
  auto token_column = state.column;
  auto current = state;
  while (((!lexer::LexState_eof(current)) && lexer::is_alnum(lexer::LexState_current(current))))   {
    (current = lexer::LexState_lex_advance(current));
  }
  auto word = input_text.substring(start, mlc::arith::checked_sub(current.position, start));
  return CppScanIdR{current, cpp_tokens::cpp_token_make(cpp_tokens::cpp_keyword_kind(word), line, token_column)};
}
bool cpp_is_hex_digit(mlc::String character) noexcept{
  return ((((((((((((lexer::is_digit(character) || (character == mlc::String("a", 1))) || (character == mlc::String("A", 1))) || (character == mlc::String("b", 1))) || (character == mlc::String("B", 1))) || (character == mlc::String("c", 1))) || (character == mlc::String("C", 1))) || (character == mlc::String("d", 1))) || (character == mlc::String("D", 1))) || (character == mlc::String("e", 1))) || (character == mlc::String("E", 1))) || (character == mlc::String("f", 1))) || (character == mlc::String("F", 1)));
}
bool cpp_is_number_suffix(mlc::String character) noexcept{
  return ((((((character == mlc::String("u", 1)) || (character == mlc::String("U", 1))) || (character == mlc::String("l", 1))) || (character == mlc::String("L", 1))) || (character == mlc::String("f", 1))) || (character == mlc::String("F", 1)));
}
lexer::LexState cpp_scan_exponent(lexer::LexState state) noexcept{
  auto current = state;
  if (((lexer::LexState_current(current) == mlc::String("e", 1)) || (lexer::LexState_current(current) == mlc::String("E", 1))))   {
    (current = lexer::LexState_lex_advance(current));
    if (((lexer::LexState_current(current) == mlc::String("+", 1)) || (lexer::LexState_current(current) == mlc::String("-", 1))))     {
      (current = lexer::LexState_lex_advance(current));
    }
    [&]() {
while (((!lexer::LexState_eof(current)) && lexer::is_digit(lexer::LexState_current(current)))) {
(current = lexer::LexState_lex_advance(current));
}
}();
  }
  return current;
}
lexer::LexState cpp_scan_number_suffixes(lexer::LexState state) noexcept{
  auto current = state;
  while (((!lexer::LexState_eof(current)) && cpp_is_number_suffix(lexer::LexState_current(current))))   {
    (current = lexer::LexState_lex_advance(current));
  }
  return current;
}
int cpp_parse_decimal_integer(mlc::String source_text, int start, int end_position) noexcept{
  auto value = 0;
  auto index = start;
  while ((index < end_position))   {
    (value = mlc::arith::checked_add(mlc::arith::checked_mul(value, 10), source_text.char_at(index).to_i()));
    (index = mlc::arith::checked_add(index, 1));
  }
  return value;
}
CppScanNumberR cpp_scan_number(lexer::LexState state) noexcept{
  auto source_text = state.source;
  auto start = state.position;
  auto line = state.line;
  auto token_column = state.column;
  auto current = state;
  if (((lexer::LexState_current(current) == mlc::String(".", 1)) && lexer::is_digit(lexer::LexState_peek(current, 1))))   {
    (current = lexer::LexState_lex_advance(current));
    while (((!lexer::LexState_eof(current)) && lexer::is_digit(lexer::LexState_current(current))))     {
      (current = lexer::LexState_lex_advance(current));
    }
    (current = cpp_scan_exponent(current));
    (current = cpp_scan_number_suffixes(current));
    auto lexeme = source_text.substring(start, mlc::arith::checked_sub(current.position, start));
    return CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLFloat{lexeme}, line, token_column)};
  } else   {
    while (((!lexer::LexState_eof(current)) && lexer::is_digit(lexer::LexState_current(current))))     {
      (current = lexer::LexState_lex_advance(current));
    }
    if ((((source_text.char_at(start) == mlc::String("0", 1)) && (!lexer::LexState_eof(current))) && ((lexer::LexState_current(current) == mlc::String("x", 1)) || (lexer::LexState_current(current) == mlc::String("X", 1)))))     {
      (current = lexer::LexState_lex_advance(current));
      while (((!lexer::LexState_eof(current)) && cpp_is_hex_digit(lexer::LexState_current(current))))       {
        (current = lexer::LexState_lex_advance(current));
      }
      (current = cpp_scan_number_suffixes(current));
      auto hex_lexeme = source_text.substring(start, mlc::arith::checked_sub(current.position, start));
      return CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLHex{hex_lexeme}, line, token_column)};
    } else if (((!lexer::LexState_eof(current)) && (lexer::LexState_current(current) == mlc::String(".", 1))))     {
      (current = lexer::LexState_lex_advance(current));
      while (((!lexer::LexState_eof(current)) && lexer::is_digit(lexer::LexState_current(current))))       {
        (current = lexer::LexState_lex_advance(current));
      }
      (current = cpp_scan_exponent(current));
      (current = cpp_scan_number_suffixes(current));
      auto float_lexeme = source_text.substring(start, mlc::arith::checked_sub(current.position, start));
      return CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLFloat{float_lexeme}, line, token_column)};
    } else if (((!lexer::LexState_eof(current)) && ((lexer::LexState_current(current) == mlc::String("e", 1)) || (lexer::LexState_current(current) == mlc::String("E", 1)))))     {
      (current = cpp_scan_exponent(current));
      (current = cpp_scan_number_suffixes(current));
      auto exponent_lexeme = source_text.substring(start, mlc::arith::checked_sub(current.position, start));
      return CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLFloat{exponent_lexeme}, line, token_column)};
    } else     {
      (current = cpp_scan_number_suffixes(current));
      auto integer_value = cpp_parse_decimal_integer(source_text, start, current.position);
      return CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLInt{integer_value}, line, token_column)};
    }
  }
}
CppScanCharR cpp_scan_char(lexer::LexState state) noexcept{
  auto line = state.line;
  auto token_column = state.column;
  auto current = lexer::LexState_lex_advance(state);
  auto character_value = mlc::String("", 0);
  auto error = mlc::String("", 0);
  if (lexer::LexState_eof(current))   {
    (error = mlc::String("unterminated character literal", 30));
  } else if ((lexer::LexState_current(current) == mlc::String("'", 1)))   {
    (error = mlc::String("empty character literal", 23));
    (current = lexer::LexState_lex_advance(current));
  } else if ((lexer::LexState_current(current) == mlc::String("\\", 1)))   {
    (current = lexer::LexState_lex_advance(current));
    if (lexer::LexState_eof(current))     {
      (error = mlc::String("unterminated character literal", 30));
    } else     {
      auto mapped = lexer::map_escape(lexer::LexState_current(current));
      (character_value = ((mapped != mlc::String("", 0)) ? (mapped) : (lexer::LexState_current(current))));
      (current = lexer::LexState_lex_advance(current));
    }
  } else   {
    (character_value = lexer::LexState_current(current));
    (current = lexer::LexState_lex_advance(current));
  }
  if ((((error == mlc::String("", 0)) && (!lexer::LexState_eof(current))) && (lexer::LexState_current(current) == mlc::String("'", 1))))   {
    (current = lexer::LexState_lex_advance(current));
  } else if ((error == mlc::String("", 0)))   {
    (error = mlc::String("unterminated character literal", 30));
  }
  return CppScanCharR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLChar{character_value}, line, token_column), error};
}
CppScanIntR cpp_scan_int(lexer::LexState state) noexcept{
  auto line = state.line;
  auto token_column = state.column;
  auto current = state;
  auto value = 0;
  while (((!lexer::LexState_eof(current)) && lexer::is_digit(lexer::LexState_current(current))))   {
    (value = mlc::arith::checked_add(mlc::arith::checked_mul(value, 10), lexer::LexState_current(current).to_i()));
    (current = lexer::LexState_lex_advance(current));
  }
  return CppScanIntR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLInt{value}, line, token_column)};
}
CppScanStrR cpp_scan_string(lexer::LexState state) noexcept{
  auto line = state.line;
  auto token_column = state.column;
  auto current = lexer::LexState_lex_advance(state);
  auto parts = mlc::Array<mlc::String>{};
  auto error = mlc::String("", 0);
  while (((!lexer::LexState_eof(current)) && (lexer::LexState_current(current) != mlc::String("\"", 1))))   {
    if ((lexer::LexState_current(current) == mlc::String("\\", 1)))     {
      (current = lexer::LexState_lex_advance(current));
      auto mapped = lexer::map_escape(lexer::LexState_current(current));
      if ((mapped != mlc::String("", 0)))       {
        parts.push_back(mapped);
      } else       {
        parts.push_back(mlc::String("\\", 1));
        parts.push_back(lexer::LexState_current(current));
      }
      (current = lexer::LexState_lex_advance(current));
    } else     {
      parts.push_back(lexer::LexState_current(current));
      (current = lexer::LexState_lex_advance(current));
    }
  }
  if (lexer::LexState_eof(current))   {
    (error = mlc::String("unterminated string", 19));
  }
  if ((!lexer::LexState_eof(current)))   {
    (current = lexer::LexState_lex_advance(current));
  }
  return CppScanStrR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLStr{parts.join(mlc::String("", 0))}, line, token_column), error};
}
CppScanOpR cpp_scan_op_single(lexer::LexState state, mlc::String character, int line, int column) noexcept{
  if ((character == mlc::String("(", 1)))   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CLParen{}, line, column)};
  } else if ((character == mlc::String(")", 1)))   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CRParen{}, line, column)};
  } else if ((character == mlc::String("{", 1)))   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CLBrace{}, line, column)};
  } else if ((character == mlc::String("}", 1)))   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CRBrace{}, line, column)};
  } else if ((character == mlc::String("[", 1)))   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CLBracket{}, line, column)};
  } else if ((character == mlc::String("]", 1)))   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CRBracket{}, line, column)};
  } else   {
    return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::COp{character}, line, column)};
  }
}
CppScanOpR cpp_scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next_char, int line, int column) noexcept{
  if ((character == mlc::String("-", 1)))   {
    if ((next_char == mlc::String(">", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CArrow{}, line, column)};
    } else if ((next_char == mlc::String("-", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CDec{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else if ((character == mlc::String(":", 1)))   {
    if ((next_char == mlc::String(":", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CScope{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else if ((character == mlc::String("=", 1)))   {
    if ((next_char == mlc::String("=", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CEq{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else if ((character == mlc::String("!", 1)))   {
    if ((next_char == mlc::String("=", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CNe{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else if ((character == mlc::String("<", 1)))   {
    if ((next_char == mlc::String("=", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CLe{}, line, column)};
    } else if ((next_char == mlc::String("<", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CLShift{}, line, column)};
    } else     {
      return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CLAngle{}, line, column)};
    }
  } else if ((character == mlc::String(">", 1)))   {
    if ((next_char == mlc::String("=", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CGe{}, line, column)};
    } else if ((next_char == mlc::String(">", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CRShift{}, line, column)};
    } else     {
      return CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::CRAngle{}, line, column)};
    }
  } else if ((character == mlc::String("&", 1)))   {
    if ((next_char == mlc::String("&", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CAnd{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else if ((character == mlc::String("|", 1)))   {
    if ((next_char == mlc::String("|", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::COr{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else if ((character == mlc::String("+", 1)))   {
    if ((next_char == mlc::String("+", 1)))     {
      return CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make(cpp_tokens::CInc{}, line, column)};
    } else     {
      return cpp_scan_op_single(state, character, line, column);
    }
  } else   {
    return cpp_scan_op_single(state, character, line, column);
  }
}
CppScanOpR cpp_scan_op(lexer::LexState state) noexcept{
  auto line = state.line;
  auto token_column = state.column;
  auto character = lexer::LexState_current(state);
  auto next_char = lexer::LexState_peek(state, 1);
  return cpp_scan_op_two_character(state, character, next_char, line, token_column);
}
lexer::LexState cpp_push_token_with_trivia(mlc::Array<cpp_tokens::CppToken>& tokens, cpp_tokens::CppToken token, mlc::String leading_trivia, lexer::LexState lexer_state) noexcept{
  auto trailing_scan = cpp_collect_trailing_trivia(lexer_state);
  tokens.push_back(cpp_tokens::cpp_token_with_trivia(token, leading_trivia, trailing_scan.trivia));
  return trailing_scan.state;
}
lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
  auto scan_result = cpp_scan_ident(state);
  return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}
lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
  auto scan_result = cpp_scan_int(state);
  return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}
lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept{
  auto scan_result = cpp_scan_string(state);
  if ((scan_result.error != mlc::String("", 0)))   {
    errors.push_back(scan_result.error);
  }
  return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}
lexer::LexState push_number_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
  auto scan_result = cpp_scan_number(state);
  return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}
lexer::LexState push_char_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept{
  auto scan_result = cpp_scan_char(state);
  if ((scan_result.error != mlc::String("", 0)))   {
    errors.push_back(scan_result.error);
  }
  return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}
lexer::LexState push_hash_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
  auto token = cpp_tokens::cpp_token_make(cpp_tokens::CHash{}, state.line, state.column);
  return cpp_push_token_with_trivia(tokens, token, leading_trivia, lexer::LexState_lex_advance(state));
}
lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
  auto scan_result = cpp_scan_op(state);
  return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}
CppLexOut cpp_tokenize(mlc::String input_text) noexcept{
  auto state = lexer::LexState{input_text, 0, 1, 1};
  auto tokens = mlc::Array<cpp_tokens::CppToken>{};
  auto errors = mlc::Array<mlc::String>{};
  auto lexer_state = state;
  auto pending_leading = mlc::String("", 0);
  while ((!lexer::LexState_eof(lexer_state)))   {
    auto leading_scan = cpp_collect_leading_trivia(lexer_state);
    (lexer_state = leading_scan.state);
    (pending_leading = (pending_leading + leading_scan.trivia));
    if ((leading_scan.error != mlc::String("", 0)))     {
      errors.push_back(leading_scan.error);
    }
    if (lexer::LexState_eof(lexer_state))     {
      break;
    }
    auto character = lexer::LexState_current(lexer_state);
    auto leading_trivia = pending_leading;
    (pending_leading = mlc::String("", 0));
    if (lexer::is_alpha(character))     {
      (lexer_state = push_ident_scan(lexer_state, tokens, leading_trivia));
    } else if (lexer::is_digit(character))     {
      (lexer_state = push_number_scan(lexer_state, tokens, leading_trivia));
    } else if (((character == mlc::String(".", 1)) && lexer::is_digit(lexer::LexState_peek(lexer_state, 1))))     {
      (lexer_state = push_number_scan(lexer_state, tokens, leading_trivia));
    } else if ((character == mlc::String("\"", 1)))     {
      (lexer_state = push_string_scan(lexer_state, tokens, errors, leading_trivia));
    } else if ((character == mlc::String("'", 1)))     {
      (lexer_state = push_char_scan(lexer_state, tokens, errors, leading_trivia));
    } else if ((character == mlc::String("#", 1)))     {
      (lexer_state = push_hash_scan(lexer_state, tokens, leading_trivia));
    } else     {
      (lexer_state = push_operator_scan(lexer_state, tokens, leading_trivia));
    }
  }
  tokens.push_back(cpp_tokens::cpp_token_with_trivia(cpp_tokens::cpp_token_make(cpp_tokens::CEof{}, lexer_state.line, lexer_state.column), pending_leading, mlc::String("", 0)));
  return CppLexOut{tokens, errors};
}
bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept{
  auto first_tokens = cpp_tokenize(source_text).tokens;
  auto rebuilt_source = cpp_tokens::cpp_tokens_to_source(first_tokens);
  auto second_tokens = cpp_tokenize(rebuilt_source).tokens;
  return cpp_tokens::cpp_token_kinds_equal(first_tokens, second_tokens);
}
bool cpp_lex_roundtrip_preserves_source(mlc::String source_text) noexcept{
  return (cpp_tokens::cpp_tokens_to_source(cpp_tokenize(source_text).tokens) == source_text);
}

} // namespace cpp_lexer
