#include "test_pass_manager.hpp"

#include "test_runner.hpp"
#include "pass_manager.hpp"
#include "preserved_analyses.hpp"

namespace test_pass_manager {

using namespace test_runner;
using namespace pass_manager;
using namespace preserved_analyses;

mlc::Array<test_runner::TestResult> pass_manager_tests() noexcept;

mlc::Array<test_runner::TestResult> pass_manager_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("pass_name_is_safe accepts checker"), pass_manager::pass_name_is_safe(mlc::String("checker"))));
results.push_back(test_runner::assert_true(mlc::String("pass_name_is_safe rejects empty"), !pass_manager::pass_name_is_safe(mlc::String(""))));
results.push_back(test_runner::assert_true(mlc::String("pass_name_is_safe rejects slash"), !pass_manager::pass_name_is_safe(mlc::String("check/er"))));
results.push_back(test_runner::assert_true(mlc::String("pass_name_is_safe rejects semicolon"), !pass_manager::pass_name_is_safe(mlc::String("check;er"))));
ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> registered_first = pass_manager::pass_manager_register(pass_manager::pass_manager_new(true), pass_manager::PassDescriptor{mlc::String("first"), {}, mlc::Array<mlc::String>{mlc::String("first")}, mlc::Array<mlc::String>{mlc::String("facts")}, false});
pass_manager::PassManager manager = std::visit(overloaded{
  [&](const ast::Ok<pass_manager::PassManager>& ok) -> pass_manager::PassManager { auto [value] = ok; return value; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> pass_manager::PassManager { auto [_w0] = err; return pass_manager::pass_manager_new(true); }
}, registered_first);
ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> duplicate = pass_manager::pass_manager_register(manager, pass_manager::PassDescriptor{mlc::String("first"), {}, {}, {}, false});
results.push_back(test_runner::assert_true(mlc::String("pass_manager_register rejects duplicate"), std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return true; },
  [&](const ast::Ok<pass_manager::PassManager>& ok) -> bool { auto [_w0] = ok; return false; }
}, duplicate)));
ast::Result<mlc::String, mlc::Array<mlc::String>> missing_keys = pass_manager::pass_manager_validate_descriptor(manager, pass_manager::PassDescriptor{mlc::String("needs_input"), mlc::Array<mlc::String>{mlc::String("modular_input")}, {}, {}, false}, {});
results.push_back(test_runner::assert_true(mlc::String("pass_manager_validate_descriptor rejects missing keys"), std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return true; },
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return false; }
}, missing_keys)));
ast::Result<mlc::String, mlc::Array<mlc::String>> ok_keys = pass_manager::pass_manager_validate_descriptor(manager, pass_manager::PassDescriptor{mlc::String("needs_input"), mlc::Array<mlc::String>{mlc::String("first")}, {}, {}, false}, mlc::Array<mlc::String>{mlc::String("first")});
results.push_back(test_runner::assert_true(mlc::String("pass_manager_validate_descriptor accepts produced keys"), std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return true; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return false; }
}, ok_keys)));
preserved_analyses::PreservedAnalyses preserved = preserved_analyses::preserved_analyses_empty();
preserved = preserved_analyses::preserved_analyses_mark(preserved, mlc::String("type_registry"));
results.push_back(test_runner::assert_true(mlc::String("preserved_analyses_mark"), preserved_analyses::preserved_analyses_contains(preserved, mlc::String("type_registry"))));
preserved = pass_manager::pass_manager_apply_preserved(preserved, pass_manager::PassDescriptor{mlc::String("transform"), {}, {}, {}, true});
results.push_back(test_runner::assert_true(mlc::String("pass_manager_apply_preserved invalidates"), !preserved_analyses::preserved_analyses_contains(preserved, mlc::String("type_registry"))));
std::visit(overloaded{
  [&](const ast::Ok<pass_manager::PassManager>& ok) -> void { auto [built] = ok; [&]() { 
  mlc::Array<mlc::String> names = pass_manager::pass_manager_pass_names(built);
  results.push_back(test_runner::assert_eq_int(mlc::String("build_compiler_pass_manager pass count"), names.size(), 4));
  results.push_back(test_runner::assert_eq_str(mlc::String("build_compiler_pass_manager first pass"), names[0], mlc::String("verify_ast")));
  return results.push_back(test_runner::assert_eq_str(mlc::String("build_compiler_pass_manager has codegen"), names[3], mlc::String("codegen")));
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> void { auto [_w0] = err; results.push_back(test_runner::assert_true(mlc::String("build_compiler_pass_manager failed"), false)); }
}, pass_manager::build_compiler_pass_manager());
return results;
}

} // namespace test_pass_manager
