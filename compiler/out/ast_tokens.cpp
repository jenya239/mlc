#include "ast_tokens.hpp"

namespace ast_tokens {

bool LexOut_has_errors(ast_tokens::LexOut self) noexcept;

ast_tokens::TKind Token_kind_value(ast_tokens::Token self) noexcept;

int Token_line_number(ast_tokens::Token self) noexcept;

int Token_column(ast_tokens::Token self) noexcept;

bool LexOut_has_errors(ast_tokens::LexOut self) noexcept{return self.errors.size() > 0;}

ast_tokens::TKind Token_kind_value(ast_tokens::Token self) noexcept{return self.kind;}

int Token_line_number(ast_tokens::Token self) noexcept{return self.line;}

int Token_column(ast_tokens::Token self) noexcept{return self.col;}

} // namespace ast_tokens
