#include "ast_tokens.hpp"

namespace ast_tokens {

ast_tokens::TKind Token_kind_value(ast_tokens::Token self) noexcept;

ast_tokens::TKind Token_kind_value(ast_tokens::Token self) noexcept{return self.kind;}

} // namespace ast_tokens
