#include "test_core_ir.hpp"

#include "test_runner.hpp"
#include "core.hpp"
#include "verify_core.hpp"
#include "core_dump.hpp"

namespace test_core_ir {

using namespace test_runner;
using namespace core;
using namespace verify_core;
using namespace core_dump;

mlc::Array<test_runner::TestResult> core_ir_tests() noexcept;

mlc::Array<test_runner::TestResult> core_ir_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("core_node_id_is_valid accepts zero"), core::core_node_id_is_valid(core::core_node_id_new(0))));
results.push_back(test_runner::assert_true(mlc::String("core_node_id_is_valid rejects negative"), !core::core_node_id_is_valid(core::core_node_id_new(-1))));
results.push_back(test_runner::assert_true(mlc::String("core_function_name_is_safe accepts main"), core::core_function_name_is_safe(mlc::String("main"))));
results.push_back(test_runner::assert_true(mlc::String("core_function_name_is_safe rejects slash"), !core::core_function_name_is_safe(mlc::String("foo/bar"))));
results.push_back(test_runner::assert_true(mlc::String("core_block_label_is_safe rejects semicolon"), !core::core_block_label_is_safe(mlc::String("entry;"))));
results.push_back(test_runner::assert_eq_int(mlc::String("verify_core_module empty"), verify_core::verify_core_module(core::core_module_empty()).size(), 0));
core::CoreFunction valid_function = core::CoreFunction{mlc::String("main"), {}, mlc::Array<core::CoreBasicBlock>{core::CoreBasicBlock{core::core_node_id_new(0), mlc::String("entry"), {}, (core::CoreTerminatorUnreachable{})}}, core::core_node_id_new(0)};
results.push_back(test_runner::assert_eq_int(mlc::String("verify_core_module valid function"), verify_core::verify_core_module(core::CoreModule{mlc::Array<core::CoreFunction>{valid_function}}).size(), 0));
core::CoreFunction invalid_entry = core::CoreFunction{mlc::String("main"), {}, mlc::Array<core::CoreBasicBlock>{core::CoreBasicBlock{core::core_node_id_new(0), mlc::String("entry"), {}, (core::CoreTerminatorUnreachable{})}}, core::core_node_id_new(1)};
results.push_back(test_runner::assert_true(mlc::String("verify_core_module rejects bad return id"), verify_core::verify_core_module(core::CoreModule{mlc::Array<core::CoreFunction>{invalid_entry}}).size() > 0));
mlc::String summary = core_dump::print_core_module(core::core_module_empty());
results.push_back(test_runner::assert_true(mlc::String("print_core_module mentions functions"), summary.contains(mlc::String("functions="))));
return results;
}

} // namespace test_core_ir
