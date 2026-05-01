#define main mlc_user_main
#include "ast_tokens.hpp"

namespace ast_tokens {

bool LexOut_has_errors(LexOut self) noexcept{
return (self.errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return self.kind;
}
int Token_line_number(Token self) noexcept{
return self.line;
}
int Token_column(Token self) noexcept{
return self.col;
}

} // namespace ast_tokens
