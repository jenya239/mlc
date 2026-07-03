#ifndef PRESERVED_ANALYSES_HPP
#define PRESERVED_ANALYSES_HPP

#include "mlc.hpp"
#include <variant>
namespace preserved_analyses {

struct PreservedAnalyses {
  mlc::Array<mlc::String> analysis_names;
};
PreservedAnalyses preserved_analyses_empty() noexcept;
PreservedAnalyses preserved_analyses_mark(PreservedAnalyses preserved, mlc::String analysis_name) noexcept;
PreservedAnalyses preserved_analyses_invalidate_all(PreservedAnalyses _preserved) noexcept;
bool preserved_analyses_contains(PreservedAnalyses preserved, mlc::String analysis_name) noexcept;

} // namespace preserved_analyses

#endif // PRESERVED_ANALYSES_HPP
