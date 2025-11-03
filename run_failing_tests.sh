#!/bin/bash
# Quick test runner for currently failing tests
# Runs only the tests that are known to fail

echo "=== Running Known Failing Tests ==="
echo ""

# Control flow tests (return statements)
echo "1. Testing return statements..."
ruby -Ilib:test test/mlc/control_flow_statement_test.rb \
  -n "/test_return/"

# List comprehension tests
echo ""
echo "2. Testing list comprehensions..."
ruby -Ilib:test test/mlc/list_comprehension_test.rb

# Lambda tests
echo ""
echo "3. Testing lambda expressions..."
ruby -Ilib:test test/mlc/lambda_test.rb 2>&1 | grep -E "(runs|assertions|failures|errors|Error:|Failure:)" | head -20

# Sum type tests (Option/Result)
echo ""
echo "4. Testing stdlib Option type..."
ruby -Ilib:test test/mlc/stdlib_option_test.rb 2>&1 | grep -E "(runs|assertions|failures|errors|Error:)" | head -10

echo ""
echo "5. Testing stdlib Result type..."
ruby -Ilib:test test/mlc/stdlib_result_test.rb 2>&1 | grep -E "(runs|assertions|failures|errors|Error:)" | head -10

# Type inference tests
echo ""
echo "6. Testing type inference with generics..."
ruby -Ilib:test test/mlc/type_inference_generics_test.rb 2>&1 | grep -E "(runs|assertions|failures|errors)" | head -5

echo ""
echo "7. Testing type inference architecture..."
ruby -Ilib:test test/mlc/type_inference_architecture_test.rb 2>&1 | grep -E "(runs|assertions|failures|errors)" | head -5

# Roundtrip tests (record type issues)
echo ""
echo "8. Testing roundtrip (record types)..."
ruby -Ilib:test test/mlc/roundtrip_test.rb 2>&1 | grep -E "(runs|assertions|failures|errors|record)" | head -10

echo ""
echo "=== Summary ==="
echo "Run 'rake test 2>&1 | grep -E \"^[0-9]+ runs\"' for full results"
