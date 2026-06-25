#include "test_cpp_header_import.hpp"

#include "test_runner.hpp"
#include "header_import.hpp"
#include "registry.hpp"
#include "ast.hpp"

namespace test_cpp_header_import {

using namespace test_runner;
using namespace header_import;
using namespace registry;
using namespace ast;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> cpp_header_import_tests() noexcept;

mlc::Array<test_runner::TestResult> cpp_header_import_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("is_cpp_header_path .h"), header_import::is_cpp_header_path(mlc::String("foo.h"))));
results.push_back(test_runner::assert_true(mlc::String("is_cpp_header_path .hpp"), header_import::is_cpp_header_path(mlc::String("bar.hpp"))));
results.push_back(test_runner::assert_true(mlc::String("is_cpp_header_path rejects .mlc"), !header_import::is_cpp_header_path(mlc::String("mod.mlc"))));
mlc::String header_source = mlc::String("int add(int left, int right);\nstruct Point { int x; int y; };");
header_import::HeaderImportResult parsed = header_import::parse_cpp_header_source(header_source);
results.push_back(test_runner::assert_eq_int(mlc::String("header import decl count"), parsed.declarations.size(), 2));
registry::TypeRegistry registry = header_import::registry_from_cpp_header_source(header_source);
results.push_back(test_runner::assert_true(mlc::String("registry has add from header"), registry::TypeRegistry_has_fn(registry, mlc::String("add"))));
results.push_back(test_runner::assert_true(mlc::String("registry has Point from header"), registry.record_index.record_field_names_ordered.has(mlc::String("Point"))));
mlc::String fixture_path = mlc::String("compiler/tests/fixtures/cpp_parser/sample_header.h");
header_import::HeaderImportResult loaded = header_import::load_cpp_header_decls(fixture_path);
results.push_back(test_runner::assert_eq_int(mlc::String("load_cpp_header_decls fixture"), loaded.declarations.size(), 2));
results.push_back(test_runner::assert_true(mlc::String("fixture registry has add"), registry::TypeRegistry_has_fn(registry::build_registry(ast::Program{loaded.declarations}), mlc::String("add"))));
mlc::String full_fixture_path = mlc::String("compiler/tests/fixtures/header_import/header_import.h");
mlc::String full_header_source = mlc::file::read_to_string(full_fixture_path);
header_import::HeaderImportResult full_parsed = header_import::parse_cpp_header_source(full_header_source);
results.push_back(test_runner::assert_eq_int(mlc::String("full parser header import decl count"), full_parsed.declarations.size(), 4));
registry::TypeRegistry full_registry = header_import::registry_from_cpp_header_source(full_header_source);
results.push_back(test_runner::assert_true(mlc::String("full registry has identity"), registry::TypeRegistry_has_fn(full_registry, mlc::String("identity"))));
results.push_back(test_runner::assert_true(mlc::String("full registry has c_entry"), registry::TypeRegistry_has_fn(full_registry, mlc::String("c_entry"))));
results.push_back(test_runner::assert_true(mlc::String("full registry has size_t alias"), registry::TypeRegistry_has_type_alias(full_registry, mlc::String("size_t"))));
results.push_back(test_runner::assert_true(mlc::String("full registry has Widget"), full_registry.record_index.record_field_names_ordered.has(mlc::String("Widget"))));
header_import::HeaderImportResult full_loaded = header_import::load_cpp_header_decls(full_fixture_path);
results.push_back(test_runner::assert_eq_int(mlc::String("load_cpp_header_decls full fixture"), full_loaded.declarations.size(), 4));
return results;
}

} // namespace test_cpp_header_import
