#define main mlc_user_main
#include "preserved_analyses.hpp"

namespace preserved_analyses {

PreservedAnalyses preserved_analyses_empty() noexcept{
  return PreservedAnalyses{{}};
}
PreservedAnalyses preserved_analyses_mark(PreservedAnalyses preserved, mlc::String analysis_name) noexcept{
  auto names = preserved.analysis_names;
  auto index = 0;
  while ((index < names.length()))   {
    if ((names[index] == analysis_name))     {
      return preserved;
    }
    (index = (index + 1));
  }
  names.push_back(analysis_name);
  return PreservedAnalyses{names};
}
PreservedAnalyses preserved_analyses_invalidate_all(PreservedAnalyses _preserved) noexcept{
  return preserved_analyses_empty();
}
bool preserved_analyses_contains(PreservedAnalyses preserved, mlc::String analysis_name) noexcept{
  auto index = 0;
  while ((index < preserved.analysis_names.length()))   {
    if ((preserved.analysis_names[index] == analysis_name))     {
      return true;
    }
    (index = (index + 1));
  }
  return false;
}

} // namespace preserved_analyses
