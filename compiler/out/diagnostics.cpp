#include "diagnostics.hpp"

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"

namespace diagnostics {

using namespace ast;
using namespace lexer;
using namespace decls;
using namespace check;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> collect_diagnostics_in_source(mlc::String source_text, mlc::String source_file) noexcept;

mlc::Array<ast::Diagnostic> collect_diagnostics_in_source(mlc::String source_text, mlc::String source_file) noexcept{return check::program_diagnostics(decls::parse_program_with_source_path(lexer::tokenize(source_text).tokens, source_file));}

} // namespace diagnostics
