#ifndef PRESERVED_ANALYSES_HPP
#define PRESERVED_ANALYSES_HPP

#include "mlc.hpp"
#include <variant>

namespace preserved_analyses {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct PreservedAnalyses {mlc::Array<mlc::String> analysis_names;};

preserved_analyses::PreservedAnalyses preserved_analyses_empty() noexcept;

preserved_analyses::PreservedAnalyses preserved_analyses_mark(preserved_analyses::PreservedAnalyses preserved, mlc::String analysis_name) noexcept;

preserved_analyses::PreservedAnalyses preserved_analyses_invalidate_all(preserved_analyses::PreservedAnalyses _preserved) noexcept;

bool preserved_analyses_contains(preserved_analyses::PreservedAnalyses preserved, mlc::String analysis_name) noexcept;

} // namespace preserved_analyses

#endif // PRESERVED_ANALYSES_HPP
