#define main mlc_user_main
#include "ast_tokens.hpp"

namespace ast_tokens {

bool LexOut_has_errors(LexOut self) noexcept;
bool LexOut_has_errors(LexOut self) noexcept{
return (self.errors.length() > 0);
}
TokenKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
TokenKind Token_kind_value(Token self) noexcept{
return self.kind;
}
int Token_line_number(Token self) noexcept{
return self.line;
}
int Token_column(Token self) noexcept{
return self.column;
}

} // namespace ast_tokens
