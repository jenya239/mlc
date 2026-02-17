# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class CalculatorE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)
  CALC_SOURCE = File.expand_path("../../misc/examples/calculator.mlc", __dir__)

  def test_calculator_runs_and_computes
    stdout, stderr, status = Open3.capture3(CLI, CALC_SOURCE)
    
    # Should compile without errors
    refute_includes stderr, "error:", "Compilation failed: #{stderr}"
    
    # Should output the results
    lines = stdout.strip.split("\n")
    assert_equal 9, lines.size, "Expected 9 output lines"
    assert_equal "42", lines[0]
    assert_equal "30", lines[1]
    assert_equal "30", lines[2]
    assert_equal "70", lines[3]
    assert_equal "25", lines[4]
    assert_equal "300", lines[5]
    assert_equal "100", lines[6]
    assert_equal "500", lines[7]
    assert_equal "Total: 1097", lines[8]
    
    # Exit code should be sum % 256: 1097 % 256 = 73
    assert_equal 73, status.exitstatus
  end
end
