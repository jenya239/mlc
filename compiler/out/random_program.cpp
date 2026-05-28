#include "random_program.hpp"

namespace random_program {

int fuzz_mix(int seed) noexcept;

int fuzz_pick(int seed, int choice_count) noexcept;

mlc::String fuzz_random_program(int seed) noexcept;

int fuzz_mix(int seed) noexcept{
int value = seed * 1664525 + 1013904223;
if (value < 0){
{
value = -value;
}
}
return value;
}

int fuzz_pick(int seed, int choice_count) noexcept{return choice_count <= 0 ? 0 : fuzz_mix(seed) % choice_count;}

mlc::String fuzz_random_program(int seed) noexcept{
int kind = fuzz_pick(seed, 6);
return kind == 0 ? mlc::String("fn fuzz_entry() -> i32 = 0") : kind == 1 ? mlc::String("fn fuzz_entry() -> i32 = 1 + 2") : kind == 2 ? mlc::String("fn fuzz_entry() -> string = \"s\"") : kind == 3 ? mlc::String("fn fuzz_entry() -> bool = true") : kind == 4 ? mlc::String("fn fuzz_entry() -> i32 = ") + mlc::to_string(fuzz_mix(seed + 1) % 100) : [&]() -> mlc::String { 
  mlc::String callee = mlc::String("helper_") + mlc::to_string(fuzz_mix(seed + 2) % 1000);
  return mlc::String("fn ") + callee + mlc::String("() -> i32 = 42\nfn fuzz_entry() -> i32 = ") + callee + mlc::String("()");
 }();
}

} // namespace random_program
