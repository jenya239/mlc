#define main mlc_user_main
#include "lexer.hpp"

#include "ast_tokens.hpp"

namespace lexer {

using namespace ast_tokens;

bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_lex_advance(LexState self) noexcept;
LexState LexState_lex_advance_by(LexState self, int advance_count) noexcept;
ast_tokens::Token LexState_token(LexState self, ast_tokens::TokenKind token_kind) noexcept;
void lexer_internal_panic(mlc::String message) noexcept{
  mlc::io::println((mlc::String("lexer panic: ", 13) + message));
  mlc::io::exit(1);
  std::make_tuple();
}
bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_lex_advance(LexState self) noexcept;
LexState LexState_lex_advance_by(LexState self, int advance_count) noexcept;
ast_tokens::Token LexState_token(LexState self, ast_tokens::TokenKind token_kind) noexcept;
bool LexState_eof(LexState self) noexcept{
return (self.position >= self.source.byte_size());
}
mlc::String LexState_current(LexState self) noexcept{
if (LexState_eof(self)) {
  return mlc::String("\0", 1);
} else {
  return self.source.byte_at(self.position);
}
}
mlc::String LexState_peek(LexState self, int offset) noexcept{
auto byte_index = (self.position + offset);
if ((byte_index < 0)) {
  lexer_internal_panic(mlc::String("lexer peek before start of source", 33));
  return mlc::String("\0", 1);
} else if ((byte_index >= self.source.byte_size())) {
  return mlc::String("\0", 1);
} else {
  return self.source.byte_at(byte_index);
}
}
LexState LexState_lex_advance(LexState self) noexcept{
if ((LexState_current(self) == mlc::String("\n", 1))) {
  return LexState{self.source, (self.position + 1), (self.line + 1), 1};
} else {
  return LexState{self.source, (self.position + 1), self.line, (self.column + 1)};
}
}
LexState LexState_lex_advance_by(LexState self, int advance_count) noexcept{
if ((advance_count <= 0)) {
  return self;
} else {
  return LexState_lex_advance_by(LexState_lex_advance(self), (advance_count - 1));
}
}
ast_tokens::Token LexState_token(LexState self, ast_tokens::TokenKind token_kind) noexcept{
return ast_tokens::Token{token_kind, self.line, self.column};
}
bool is_alpha(mlc::String character) noexcept{
  return ((((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1))) || ((character >= mlc::String("A", 1)) && (character <= mlc::String("Z", 1)))) || (character == mlc::String("_", 1)));
}
bool is_digit(mlc::String character) noexcept{
  return ((character >= mlc::String("0", 1)) && (character <= mlc::String("9", 1)));
}
bool is_alnum(mlc::String character) noexcept{
  return (is_alpha(character) || is_digit(character));
}
bool is_ws(mlc::String character) noexcept{
  return ((((character == mlc::String(" ", 1)) || (character == mlc::String("\t", 1))) || (character == mlc::String("\r", 1))) || (character == mlc::String("\n", 1)));
}
mlc::Array<mlc::String> mlc_reserved_keywords() noexcept{
  return mlc::Array<mlc::String>{mlc::String("fn", 2), mlc::String("type", 4), mlc::String("let", 3), mlc::String("mut", 3), mlc::String("const", 5), mlc::String("return", 6), mlc::String("break", 5), mlc::String("continue", 8), mlc::String("if", 2), mlc::String("then", 4), mlc::String("else", 4), mlc::String("unless", 6), mlc::String("while", 5), mlc::String("for", 3), mlc::String("in", 2), mlc::String("do", 2), mlc::String("end", 3), mlc::String("match", 5), mlc::String("with", 4), mlc::String("import", 6), mlc::String("from", 4), mlc::String("as", 2), mlc::String("extern", 6), mlc::String("extend", 6), mlc::String("where", 5), mlc::String("spawn", 5), mlc::String("move", 4), mlc::String("true", 4), mlc::String("false", 5)};
}
bool is_reserved_keyword(mlc::String word) noexcept{
  return mlc_reserved_keywords().any([=](mlc::String keyword) mutable { return (word == keyword); });
}
ast_tokens::TokenKind reserved_keyword_kind(mlc::String word) noexcept{
  if ((word == mlc::String("fn", 2)))   {
    return ast_tokens::KFn{};
  } else if ((word == mlc::String("type", 4)))   {
    return ast_tokens::KType{};
  } else if ((word == mlc::String("let", 3)))   {
    return ast_tokens::KLet{};
  } else if ((word == mlc::String("mut", 3)))   {
    return ast_tokens::KMut{};
  } else if ((word == mlc::String("const", 5)))   {
    return ast_tokens::KConst{};
  } else if ((word == mlc::String("return", 6)))   {
    return ast_tokens::KReturn{};
  } else if ((word == mlc::String("break", 5)))   {
    return ast_tokens::KBreak{};
  } else if ((word == mlc::String("continue", 8)))   {
    return ast_tokens::KContinue{};
  } else if ((word == mlc::String("if", 2)))   {
    return ast_tokens::KIf{};
  } else if ((word == mlc::String("then", 4)))   {
    return ast_tokens::KThen{};
  } else if ((word == mlc::String("else", 4)))   {
    return ast_tokens::KElse{};
  } else if ((word == mlc::String("unless", 6)))   {
    return ast_tokens::KUnless{};
  } else if ((word == mlc::String("while", 5)))   {
    return ast_tokens::KWhile{};
  } else if ((word == mlc::String("for", 3)))   {
    return ast_tokens::KFor{};
  } else if ((word == mlc::String("in", 2)))   {
    return ast_tokens::KIn{};
  } else if ((word == mlc::String("do", 2)))   {
    return ast_tokens::KDo{};
  } else if ((word == mlc::String("end", 3)))   {
    return ast_tokens::KEnd{};
  } else if ((word == mlc::String("match", 5)))   {
    return ast_tokens::KMatch{};
  } else if ((word == mlc::String("with", 4)))   {
    return ast_tokens::KWith{};
  } else if ((word == mlc::String("import", 6)))   {
    return ast_tokens::KImport{};
  } else if ((word == mlc::String("from", 4)))   {
    return ast_tokens::KFrom{};
  } else if ((word == mlc::String("as", 2)))   {
    return ast_tokens::KAs{};
  } else if ((word == mlc::String("extern", 6)))   {
    return ast_tokens::KExtern{};
  } else if ((word == mlc::String("extend", 6)))   {
    return ast_tokens::KExtend{};
  } else if ((word == mlc::String("where", 5)))   {
    return ast_tokens::KWhere{};
  } else if ((word == mlc::String("spawn", 5)))   {
    return ast_tokens::KSpawn{};
  } else if ((word == mlc::String("move", 4)))   {
    return ast_tokens::KMove{};
  } else if ((word == mlc::String("true", 4)))   {
    return ast_tokens::KTrue{};
  } else if ((word == mlc::String("false", 5)))   {
    return ast_tokens::KFalse{};
  } else   {
    return ast_tokens::Ident{word};
  }
}
ast_tokens::TokenKind keyword_kind(mlc::String word) noexcept{
  if (is_reserved_keyword(word))   {
    return reserved_keyword_kind(word);
  } else   {
    return ast_tokens::Ident{word};
  }
}
ScanResult scan_ident(LexState state) noexcept{
  auto start = state.position;
  auto input_text = state.source;
  auto token_line = state.line;
  auto token_column = state.column;
  auto current = state;
  while (((!LexState_eof(current)) && is_alnum(LexState_current(current))))   {
    (current = LexState_lex_advance(current));
  }
  auto word = input_text.byte_substring(start, (current.position - start));
  return ScanResult{current, ast_tokens::Token{keyword_kind(word), token_line, token_column}};
}
bool is_alpha_lower(mlc::String character) noexcept{
  return ((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1)));
}
SuffixScan try_scan_suffix(LexState state) noexcept{
  auto numeric_type_suffixes = mlc::Array<mlc::String>{mlc::String("i8", 2), mlc::String("i16", 3), mlc::String("i32", 3), mlc::String("i64", 3), mlc::String("u8", 2), mlc::String("u16", 3), mlc::String("u32", 3), mlc::String("u64", 3), mlc::String("usize", 5), mlc::String("f64", 3), mlc::String("f32", 3)};
  if (((!LexState_eof(state)) && is_alpha_lower(LexState_current(state))))   {
    auto start = state.position;
    auto orig_line = state.line;
    auto original_column = state.column;
    auto current_state = state;
    while (((!LexState_eof(current_state)) && (is_alpha_lower(LexState_current(current_state)) || is_digit(LexState_current(current_state)))))     {
      (current_state = LexState_lex_advance(current_state));
    }
    auto candidate = current_state.source.byte_substring(start, (current_state.position - start));
    if (numeric_type_suffixes.any([=](mlc::String suffix_under_scan) mutable { return (suffix_under_scan == candidate); }))     {
      return SuffixScan{candidate, current_state};
    } else     {
      auto back = LexState{current_state.source, start, orig_line, original_column};
      return SuffixScan{mlc::String("", 0), back};
    }
  } else   {
    return SuffixScan{mlc::String("", 0), state};
  }
}
ScanResult scan_int(LexState state) noexcept{
  auto token_line = state.line;
  auto token_column = state.column;
  auto int_start = state.position;
  auto current = state;
  auto value = 0;
  while (((!LexState_eof(current)) && is_digit(LexState_current(current))))   {
    (value = ((value * 10) + LexState_current(current).to_i()));
    (current = LexState_lex_advance(current));
  }
  if (((((!LexState_eof(current)) && (LexState_current(current) == mlc::String(".", 1))) && (!LexState_eof(LexState_lex_advance(current)))) && is_digit(LexState_current(LexState_lex_advance(current)))))   {
    (current = LexState_lex_advance(current));
    while (((!LexState_eof(current)) && is_digit(LexState_current(current))))     {
      (current = LexState_lex_advance(current));
    }
    auto raw_float = current.source.byte_substring(int_start, (current.position - int_start));
auto __tmp_0 = try_scan_suffix(current);
auto after = __tmp_0.after;

    return ScanResult{after, ast_tokens::Token{ast_tokens::LFloat{raw_float}, token_line, token_column}};
  } else   {
auto __tmp_0 = try_scan_suffix(current);
auto suffix = __tmp_0.suffix;
auto after = __tmp_0.after;

    auto token_kind = [&]() -> ast_tokens::TokenKind {
auto __match_subject = suffix;
if ((__match_subject == mlc::String("i8", 2))) {
  return ast_tokens::LInt{value};
} else if ((__match_subject == mlc::String("i16", 3))) {
  return ast_tokens::LInt{value};
} else if ((__match_subject == mlc::String("i32", 3))) {
  return ast_tokens::LInt{value};
} else if ((__match_subject == mlc::String("u16", 3))) {
  return ast_tokens::LInt{value};
} else if ((__match_subject == mlc::String("u32", 3))) {
  return ast_tokens::LInt{value};
} else if ((__match_subject == mlc::String("u64", 3))) {
  return ast_tokens::LInt{value};
} else if ((__match_subject == mlc::String("i64", 3))) {
  return ast_tokens::LI64{mlc::to_string(value)};
} else if ((__match_subject == mlc::String("u8", 2))) {
  return ast_tokens::LU8{mlc::to_string(value)};
} else if ((__match_subject == mlc::String("usize", 5))) {
  return ast_tokens::LUsize{mlc::to_string(value)};
} else {
  return ast_tokens::LInt{value};
}
}();
    return ScanResult{after, ast_tokens::Token{token_kind, token_line, token_column}};
  }
}
ScanStrResult scan_single_string(LexState state) noexcept{
  auto token_line = state.line;
  auto token_column = state.column;
  auto current = LexState_lex_advance(state);
  auto parts = mlc::Array<mlc::String>{};
  auto error = mlc::String("", 0);
  while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("'", 1))))   {
    if ((LexState_current(current) == mlc::String("\\", 1)))     {
      (current = LexState_lex_advance(current));
      if (LexState_eof(current))       {
        (error = mlc::String("unterminated escape in single-quoted string", 43));
      } else if ((LexState_current(current) == mlc::String("\\", 1)))       {
        parts.push_back(mlc::String("\\", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((LexState_current(current) == mlc::String("'", 1)))       {
        parts.push_back(mlc::String("'", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((LexState_current(current) == mlc::String("n", 1)))       {
        parts.push_back(mlc::String("\n", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((LexState_current(current) == mlc::String("t", 1)))       {
        parts.push_back(mlc::String("\t", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((LexState_current(current) == mlc::String("r", 1)))       {
        parts.push_back(mlc::String("\r", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((LexState_current(current) == mlc::String("0", 1)))       {
        parts.push_back(mlc::String("\0", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else       {
        parts.push_back(mlc::String("\\", 1));
        parts.push_back(LexState_current(current));
        (current = LexState_lex_advance(current));
      }
    } else     {
      parts.push_back(LexState_current(current));
      (current = LexState_lex_advance(current));
    }
  }
  if (LexState_eof(current))   {
    (error = mlc::String("unterminated single-quoted string", 33));
  } else   {
    (current = LexState_lex_advance(current));
  }
  auto joined = parts.join(mlc::String("", 0));
  auto token_kind = [&]() -> ast_tokens::TokenKind {
  if ((joined.length() == 1))   {
    return ast_tokens::LChar{joined};
  } else   {
    return ast_tokens::LStr{joined};
  }
}();
  return ScanStrResult{current, ast_tokens::Token{token_kind, token_line, token_column}, error};
}
mlc::String map_escape(mlc::String character) noexcept{
  if ((character == mlc::String("n", 1)))   {
    return mlc::String("\n", 1);
  } else if ((character == mlc::String("t", 1)))   {
    return mlc::String("\t", 1);
  } else if ((character == mlc::String("r", 1)))   {
    return mlc::String("\r", 1);
  } else if ((character == mlc::String("\"", 1)))   {
    return mlc::String("\"", 1);
  } else if ((character == mlc::String("\\", 1)))   {
    return mlc::String("\\", 1);
  } else if ((character == mlc::String("0", 1)))   {
    return mlc::String("\0", 1);
  } else if ((character == mlc::String("{", 1)))   {
    return mlc::String("{", 1);
  } else if ((character == mlc::String("}", 1)))   {
    return mlc::String("}", 1);
  } else   {
    return mlc::String("", 0);
  }
}
LexState lex_advance_past_unescaped_string_run(LexState state) noexcept{
  auto current = state;
  while ((((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\"", 1))) && (LexState_current(current) != mlc::String("\\", 1))))   {
    (current = LexState_lex_advance(current));
  }
  return current;
}
LexState lex_advance_past_template_literal_run(LexState state) noexcept{
  auto current = state;
  auto run_active = true;
  while (((run_active && (!LexState_eof(current))) && (LexState_current(current) != mlc::String("`", 1))))   {
    if ((LexState_current(current) == mlc::String("\\", 1)))     {
      (run_active = false);
    } else if (((LexState_current(current) == mlc::String("$", 1)) && (LexState_peek(current, 1) == mlc::String("{", 1))))     {
      (run_active = false);
    } else     {
      (current = LexState_lex_advance(current));
    }
  }
  return current;
}
ScanStrResult scan_string(LexState state) noexcept{
  auto token_line = state.line;
  auto token_column = state.column;
  auto current = LexState_lex_advance(state);
  auto parts = mlc::Array<mlc::String>{};
  auto error = mlc::String("", 0);
  while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\"", 1))))   {
    if ((LexState_current(current) == mlc::String("\\", 1)))     {
      (current = LexState_lex_advance(current));
      if (LexState_eof(current))       {
        (error = mlc::String("dangling escape at end of string", 32));
      } else       {
        auto mapped = map_escape(LexState_current(current));
        if ((mapped != mlc::String("", 0)))         {
          parts.push_back(mapped);
        } else         {
          parts.push_back(mlc::String("\\", 1));
          parts.push_back(LexState_current(current));
        }
        (current = LexState_lex_advance(current));
      }
    } else     {
      auto run_start = current.position;
      (current = lex_advance_past_unescaped_string_run(current));
      if ((current.position > run_start))       {
        parts.push_back(current.source.byte_substring(run_start, (current.position - run_start)));
      }
    }
  }
  if (LexState_eof(current))   {
    (error = mlc::String("unterminated string", 19));
  } else   {
    (current = LexState_lex_advance(current));
  }
  return ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr{parts.join(mlc::String("", 0))}, token_line, token_column}, error};
}
LexState scan_template_nested_string(LexState initial, mlc::Array<mlc::String> expr_chars) noexcept{
  auto current = LexState_lex_advance(initial);
  while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\"", 1))))   {
    if (((LexState_current(current) == mlc::String("\\", 1)) && (!LexState_eof(LexState_lex_advance(current)))))     {
      expr_chars.push_back(LexState_current(current));
      (current = LexState_lex_advance(current));
      expr_chars.push_back(LexState_current(current));
      (current = LexState_lex_advance(current));
    } else     {
      expr_chars.push_back(LexState_current(current));
      (current = LexState_lex_advance(current));
    }
  }
  if ((!LexState_eof(current)))   {
    expr_chars.push_back(LexState_current(current));
    (current = LexState_lex_advance(current));
  }
  return current;
}
ScanStrResult scan_template(LexState state) noexcept{
  auto token_line = state.line;
  auto token_column = state.column;
  auto current = LexState_lex_advance(state);
  auto parts = mlc::Array<mlc::String>{};
  auto current_lit = mlc::Array<mlc::String>{};
  auto error = mlc::String("", 0);
  while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("`", 1))))   {
    if (((LexState_current(current) == mlc::String("\\", 1)) && (!LexState_eof(LexState_lex_advance(current)))))     {
      auto next = LexState_current(LexState_lex_advance(current));
      (current = LexState_lex_advance(current));
      if ((next == mlc::String("`", 1)))       {
        current_lit.push_back(mlc::String("`", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((((next == mlc::String("$", 1)) && (!LexState_eof(LexState_lex_advance(current)))) && (LexState_current(LexState_lex_advance(current)) == mlc::String("{", 1))))       {
        current_lit.push_back(mlc::String("$", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
        current_lit.push_back(mlc::String("{", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((next == mlc::String("n", 1)))       {
        current_lit.push_back(mlc::String("\n", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((next == mlc::String("t", 1)))       {
        current_lit.push_back(mlc::String("\t", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((next == mlc::String("r", 1)))       {
        current_lit.push_back(mlc::String("\r", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else if ((next == mlc::String("\\", 1)))       {
        current_lit.push_back(mlc::String("\\", 1));
        std::make_tuple();
        (current = LexState_lex_advance(current));
      } else       {
        current_lit.push_back(mlc::String("\\", 1));
        current_lit.push_back(next);
        (current = LexState_lex_advance(current));
      }
    } else if ((((LexState_current(current) == mlc::String("$", 1)) && (!LexState_eof(LexState_lex_advance(current)))) && (LexState_current(LexState_lex_advance(current)) == mlc::String("{", 1))))     {
      parts.push_back(current_lit.join(mlc::String("", 0)));
      (current_lit = {});
      (current = LexState_lex_advance(LexState_lex_advance(current)));
      auto expr_chars = mlc::Array<mlc::String>{};
      auto depth = 1;
      while (((!LexState_eof(current)) && (depth > 0)))       {
        if ((LexState_current(current) == mlc::String("{", 1)))         {
          (depth = (depth + 1));
          std::make_tuple();
          expr_chars.push_back(LexState_current(current));
          std::make_tuple();
          (current = LexState_lex_advance(current));
        } else if ((LexState_current(current) == mlc::String("}", 1)))         {
          (depth = (depth - 1));
          if ((depth > 0))           {
            expr_chars.push_back(LexState_current(current));
          }
          (current = LexState_lex_advance(current));
        } else if ((LexState_current(current) == mlc::String("\"", 1)))         {
          expr_chars.push_back(LexState_current(current));
          (current = scan_template_nested_string(current, expr_chars));
        } else         {
          expr_chars.push_back(LexState_current(current));
          (current = LexState_lex_advance(current));
        }
      }
      if ((depth > 0))       {
        (error = mlc::String("unterminated interpolation in template literal", 46));
      }
      parts.push_back(expr_chars.join(mlc::String("", 0)));
    } else     {
      auto run_start = current.position;
      (current = lex_advance_past_template_literal_run(current));
      if ((current.position > run_start))       {
        current_lit.push_back(current.source.byte_substring(run_start, (current.position - run_start)));
      }
    }
  }
  if (LexState_eof(current))   {
    (error = mlc::String("unterminated template literal", 29));
  } else   {
    (current = LexState_lex_advance(current));
  }
  parts.push_back(current_lit.join(mlc::String("", 0)));
  return ScanStrResult{current, ast_tokens::Token{ast_tokens::LTemplate{parts}, token_line, token_column}, error};
}
SkipResult skip_whitespace(LexState state) noexcept{
  auto current = state;
  auto error = mlc::String("", 0);
  while ((!LexState_eof(current)))   {
    auto character = LexState_current(current);
    if (is_ws(character))     {
      (current = LexState_lex_advance(current));
    } else if (((character == mlc::String("/", 1)) && (LexState_peek(current, 1) == mlc::String("/", 1))))     {
      (current = LexState_lex_advance_by(current, 2));
      [&]() {
while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\n", 1)))) {
(current = LexState_lex_advance(current));
}
}();
    } else if (((character == mlc::String("/", 1)) && (LexState_peek(current, 1) == mlc::String("*", 1))))     {
      (current = LexState_lex_advance_by(current, 2));
      auto comment_closed = false;
      while ((!LexState_eof(current)))       {
        if (((LexState_current(current) == mlc::String("*", 1)) && (LexState_peek(current, 1) == mlc::String("/", 1))))         {
          (current = LexState_lex_advance_by(current, 2));
          (comment_closed = true);
          break;
        }
        (current = LexState_lex_advance(current));
      }
      if ((!comment_closed))       {
        (error = mlc::String("unterminated block comment", 26));
      }
    } else     {
      break;
    }
  }
  return SkipResult{current, error};
}
ScanResult scan_op_two_character(LexState state, mlc::String character, mlc::String next) noexcept{
  if (((character == mlc::String("-", 1)) && (next == mlc::String(">", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Arrow{})};
  } else if (((character == mlc::String("=", 1)) && (next == mlc::String(">", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::FatArrow{})};
  } else if (((character == mlc::String("=", 1)) && (next == mlc::String("=", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String("==", 2)})};
  } else if ((character == mlc::String("=", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Equal{})};
  } else if (((character == mlc::String("|", 1)) && (next == mlc::String(">", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Pipe{})};
  } else if (((character == mlc::String("|", 1)) && (next == mlc::String("|", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String("||", 2)})};
  } else if ((character == mlc::String("|", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Bar{})};
  } else if (((character == mlc::String("!", 1)) && (next == mlc::String("=", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String("!=", 2)})};
  } else if ((character == mlc::String("!", 1)))   {
    return scan_op_single(character, state);
  } else if (((character == mlc::String("<", 1)) && (next == mlc::String("=", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String("<=", 2)})};
  } else if (((character == mlc::String("<", 1)) && (next == mlc::String("<", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String("<<", 2)})};
  } else if ((character == mlc::String("<", 1)))   {
    return scan_op_single(character, state);
  } else if (((character == mlc::String(">", 1)) && (next == mlc::String("=", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String(">=", 2)})};
  } else if (((character == mlc::String(">", 1)) && (next == mlc::String(">", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String(">>", 2)})};
  } else if ((character == mlc::String(">", 1)))   {
    return scan_op_single(character, state);
  } else if (((character == mlc::String("&", 1)) && (next == mlc::String("&", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op{mlc::String("&&", 2)})};
  } else if ((character == mlc::String("&", 1)))   {
    return scan_op_single(character, state);
  } else   {
    return scan_op_single(character, state);
  }
}
ScanResult scan_op_single(mlc::String character, LexState state) noexcept{
  if ((character == mlc::String("=", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Equal{})};
  } else if ((character == mlc::String("|", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Bar{})};
  } else if ((character == mlc::String("?", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Question{})};
  } else if ((character == mlc::String(".", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Dot{})};
  } else if ((character == mlc::String("(", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::LParen{})};
  } else if ((character == mlc::String(")", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::RParen{})};
  } else if ((character == mlc::String("{", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::LBrace{})};
  } else if ((character == mlc::String("}", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::RBrace{})};
  } else if ((character == mlc::String("[", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::LBracket{})};
  } else if ((character == mlc::String("]", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::RBracket{})};
  } else if ((character == mlc::String(",", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Comma{})};
  } else if ((character == mlc::String(";", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Semicolon{})};
  } else if ((character == mlc::String(":", 1)))   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Colon{})};
  } else   {
    return ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Op{character})};
  }
}
ScanResult scan_op(LexState state) noexcept{
  auto character = LexState_current(state);
  auto next = LexState_peek(state, 1);
  if ((((character == mlc::String(".", 1)) && (next == mlc::String(".", 1))) && (LexState_peek(state, 2) == mlc::String(".", 1))))   {
    return ScanResult{LexState_lex_advance_by(state, 3), LexState_token(state, ast_tokens::Spread{})};
  } else   {
    return scan_op_two_character(state, character, next);
  }
}
void accumulate_nonempty_scan_message_into_errors(mlc::Array<mlc::String>& error_messages, mlc::String message_under_scan) noexcept{
  if ((message_under_scan != mlc::String("", 0)))   {
    error_messages.push_back(message_under_scan);
  }
  std::make_tuple();
}
LexState apply_skip_whitespace(LexState state, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_0 = skip_whitespace(state);
auto after = __tmp_0.after;
auto error = __tmp_0.error;

  accumulate_nonempty_scan_message_into_errors(error_messages, error);
  return after;
}
LexState push_ident_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept{
auto __tmp_0 = scan_ident(state);
auto after = __tmp_0.after;
auto token = __tmp_0.token;

  tokens.push_back(token);
  return after;
}
LexState push_int_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept{
auto __tmp_0 = scan_int(state);
auto after = __tmp_0.after;
auto token = __tmp_0.token;

  tokens.push_back(token);
  return after;
}
LexState push_string_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_0 = scan_string(state);
auto after = __tmp_0.after;
auto token = __tmp_0.token;
auto error = __tmp_0.error;

  tokens.push_back(token);
  accumulate_nonempty_scan_message_into_errors(error_messages, error);
  return after;
}
LexState push_single_string_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_0 = scan_single_string(state);
auto after = __tmp_0.after;
auto token = __tmp_0.token;
auto error = __tmp_0.error;

  tokens.push_back(token);
  accumulate_nonempty_scan_message_into_errors(error_messages, error);
  return after;
}
LexState push_template_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_0 = scan_template(state);
auto after = __tmp_0.after;
auto token = __tmp_0.token;
auto error = __tmp_0.error;

  tokens.push_back(token);
  accumulate_nonempty_scan_message_into_errors(error_messages, error);
  return after;
}
LexState push_operator_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept{
auto __tmp_0 = scan_op(state);
auto after = __tmp_0.after;
auto token = __tmp_0.token;

  tokens.push_back(token);
  return after;
}
ast_tokens::LexOut tokenize(mlc::String input_text) noexcept{
  auto lexer_state = LexState{input_text, 0, 1, 1};
  auto tokens = mlc::Array<ast_tokens::Token>{};
  auto errors = mlc::Array<mlc::String>{};
  while ((!LexState_eof(lexer_state)))   {
    auto position_before_token = lexer_state.position;
    (lexer_state = apply_skip_whitespace(lexer_state, errors));
    if (LexState_eof(lexer_state))     {
      break;
    }
    auto character = LexState_current(lexer_state);
    if (is_alpha(character))     {
      (lexer_state = push_ident_scan(lexer_state, tokens));
      std::make_tuple();
    } else if (is_digit(character))     {
      (lexer_state = push_int_scan(lexer_state, tokens));
      std::make_tuple();
    } else if ((character == mlc::String("\"", 1)))     {
      (lexer_state = push_string_scan(lexer_state, tokens, errors));
      std::make_tuple();
    } else if ((character == mlc::String("'", 1)))     {
      (lexer_state = push_single_string_scan(lexer_state, tokens, errors));
      std::make_tuple();
    } else if ((character == mlc::String("`", 1)))     {
      (lexer_state = push_template_scan(lexer_state, tokens, errors));
      std::make_tuple();
    } else     {
      (lexer_state = push_operator_scan(lexer_state, tokens));
      std::make_tuple();
    }
    if ((lexer_state.position == position_before_token))     {
      lexer_internal_panic((mlc::String("tokenize made no progress at byte ", 34) + mlc::to_string(lexer_state.position)));
    }
  }
  tokens.push_back(LexState_token(lexer_state, ast_tokens::Eof{}));
  return ast_tokens::LexOut{tokens, errors};
}

} // namespace lexer
