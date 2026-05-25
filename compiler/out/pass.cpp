#include "pass.hpp"

#include "ast.hpp"

namespace pass {

using namespace ast;
using namespace ast_tokens;

template<typename Input, Output>
struct CompilerPass {
  std::function<Result<Output, mlc::Array<Diagnostic>>(Input)> run;
};

} // namespace pass
