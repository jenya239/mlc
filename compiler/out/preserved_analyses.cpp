#include "preserved_analyses.hpp"

namespace preserved_analyses {

preserved_analyses::PreservedAnalyses preserved_analyses_empty() noexcept;

preserved_analyses::PreservedAnalyses preserved_analyses_mark(preserved_analyses::PreservedAnalyses preserved, mlc::String analysis_name) noexcept;

preserved_analyses::PreservedAnalyses preserved_analyses_invalidate_all(preserved_analyses::PreservedAnalyses _preserved) noexcept;

bool preserved_analyses_contains(preserved_analyses::PreservedAnalyses preserved, mlc::String analysis_name) noexcept;

preserved_analyses::PreservedAnalyses preserved_analyses_empty() noexcept{return preserved_analyses::PreservedAnalyses{{}};}

preserved_analyses::PreservedAnalyses preserved_analyses_mark(preserved_analyses::PreservedAnalyses preserved, mlc::String analysis_name) noexcept{
mlc::Array<mlc::String> names = preserved.analysis_names;
int index = 0;
while (index < names.size()){
{
if (names[index] == analysis_name){
{
return preserved;
}
}
index = index + 1;
}
}
names.push_back(analysis_name);
return preserved_analyses::PreservedAnalyses{names};
}

preserved_analyses::PreservedAnalyses preserved_analyses_invalidate_all(preserved_analyses::PreservedAnalyses _preserved) noexcept{return preserved_analyses_empty();}

bool preserved_analyses_contains(preserved_analyses::PreservedAnalyses preserved, mlc::String analysis_name) noexcept{
int index = 0;
while (index < preserved.analysis_names.size()){
{
if (preserved.analysis_names[index] == analysis_name){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

} // namespace preserved_analyses
