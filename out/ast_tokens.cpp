#define main mlc_user_main
#include "ast_tokens.hpp"

namespace ast_tokens {

void LexOut_has_errors(LexOut self) noexcept{
return (/* unit */ > 0);
}
void Token_kind_value(Token self) noexcept{
return self;
}

} // namespace ast_tokens
