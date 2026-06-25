#include "test_mir_types.hpp"

#include "test_runner.hpp"
#include "mir_types.hpp"
#include "verify_mir.hpp"
#include "mir_dump.hpp"
#include "registry.hpp"

namespace test_mir_types {

using namespace test_runner;
using namespace mir_types;
using namespace verify_mir;
using namespace mir_dump;
using namespace registry;

mlc::Array<test_runner::TestResult> mir_types_tests() noexcept;

mlc::Array<test_runner::TestResult> mir_types_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
auto unit_type = std::make_shared<registry::Type>((registry::TUnit{}));
results.push_back(test_runner::assert_true(mlc::String("mir_id_is_valid accepts zero"), mir_types::mir_id_is_valid(0)));
results.push_back(test_runner::assert_true(mlc::String("mir_id_is_valid rejects negative"), !mir_types::mir_id_is_valid(-1)));
results.push_back(test_runner::assert_true(mlc::String("mir_function_name_is_safe accepts main"), mir_types::mir_function_name_is_safe(mlc::String("main"))));
results.push_back(test_runner::assert_true(mlc::String("mir_function_name_is_safe rejects slash"), !mir_types::mir_function_name_is_safe(mlc::String("foo/bar"))));
mir_types::MirBlock entry = mir_types::MirBlock{mir_types::mir_block_id(0), mlc::String("entry"), {}, mir_types::MirReturn(mir_types::MirOperandConstInt(0))};
mir_types::MirFunction valid_fn = mir_types::MirFunction{mlc::String("main"), {}, {}, mlc::Array<mir_types::MirBlock>{entry}, unit_type};
mir_types::MirProgram program = mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mlc::Array<mir_types::MirFunction>{valid_fn}}}};
results.push_back(test_runner::assert_eq_int(mlc::String("verify_mir_program valid"), verify_mir::verify_mir_program(program).size(), 0));
mir_types::MirBlock bad_jump = mir_types::MirBlock{mir_types::mir_block_id(0), mlc::String("entry"), {}, mir_types::MirJump(mir_types::mir_block_id(99))};
mir_types::MirFunction bad_fn = mir_types::MirFunction{mlc::String("main"), {}, {}, mlc::Array<mir_types::MirBlock>{bad_jump}, unit_type};
results.push_back(test_runner::assert_true(mlc::String("verify_mir rejects bad jump target"), verify_mir::verify_mir_program(mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mlc::Array<mir_types::MirFunction>{bad_fn}}}}).size() > 0));
mlc::String summary = mir_dump::print_mir_program(mir_types::mir_program_empty());
results.push_back(test_runner::assert_true(mlc::String("print_mir_program mentions modules"), summary.contains(mlc::String("modules="))));
return results;
}

} // namespace test_mir_types
