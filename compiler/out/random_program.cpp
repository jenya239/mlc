#include "random_program.hpp"

namespace random_program {

int fuzz_variant_count() noexcept;

int fuzz_mix(int seed) noexcept;

int fuzz_pick(int seed, int choice_count) noexcept;

int fuzz_program_kind(int seed) noexcept;

mlc::String fuzz_random_program(int seed) noexcept;

int fuzz_variant_count() noexcept{return 12;}

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

int fuzz_program_kind(int seed) noexcept{return fuzz_pick(seed, fuzz_variant_count());}

mlc::String fuzz_random_program(int seed) noexcept{
int kind = fuzz_program_kind(seed);
return kind == 0 ? mlc::String("fn fuzz_entry() -> i32 = 0") : kind == 1 ? mlc::String("fn fuzz_entry() -> i32 = 1 + 2") : kind == 2 ? mlc::String("fn fuzz_entry() -> string = \"s\"") : kind == 3 ? mlc::String("fn fuzz_entry() -> bool = true") : kind == 4 ? mlc::String("fn fuzz_entry() -> i32 = ") + mlc::to_string(fuzz_mix(seed + 1) % 100) : kind == 5 ? [&]() -> mlc::String { 
  mlc::String callee = mlc::String("helper_") + mlc::to_string(fuzz_mix(seed + 2) % 1000);
  return mlc::String("fn ") + callee + mlc::String("() -> i32 = 42\nfn fuzz_entry() -> i32 = ") + callee + mlc::String("()");
 }() : kind == 6 ? mlc::String("fn fuzz_entry() -> i32 = if true then 1 else 2") : kind == 7 ? mlc::String("fn fuzz_entry() -> i32 = do let x = 1; x end") : kind == 8 ? mlc::String("fn fuzz_entry() -> i32 = match ") + mlc::to_string(fuzz_mix(seed + 3) % 10) + mlc::String(" { 1 => 0, _ => 1 }") : kind == 9 ? [&]() -> mlc::String { 
  mlc::String type_name = mlc::String("FuzzStub_") + mlc::to_string(fuzz_mix(seed + 4) % 1000);
  return mlc::String("type ") + type_name + mlc::String(" = { tag: i32 }\nfn fuzz_entry() -> i32 = 0");
 }() : kind == 10 ? [&]() -> mlc::String { 
  mlc::String type_name = mlc::String("FuzzRecord_") + mlc::to_string(fuzz_mix(seed + 5) % 1000);
  mlc::String field_value = mlc::to_string(fuzz_mix(seed + 6) % 100);
  return mlc::String("type ") + type_name + mlc::String(" = { value: i32 }\nfn fuzz_entry() -> i32 = ") + type_name + mlc::String(" { value: ") + field_value + mlc::String(" }.value");
 }() : mlc::String("fn fuzz_entry() -> i32 = do\n  let outer = do\n    let inner = ") + mlc::to_string(fuzz_mix(seed + 7) % 50) + mlc::String("\n    inner\n  end\n  outer\nend");
}

} // namespace random_program
