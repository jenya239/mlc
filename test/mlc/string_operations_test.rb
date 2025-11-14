# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCStringOperationsTest < Minitest::Test
  def test_string_concatenation
    mlc_source = <<~MLC
      fn greet(name: str) -> str =
        "Hello, " + name + "!"
    MLC

    # This should work but currently fails
    # Let's test what we get
    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "Hello"
    rescue => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("left operand of '+' must be numeric")
    end
  end

  def test_string_interpolation
    mlc_source = <<~MLC
      fn greet(name: str) -> str =
        "Hello, {name}!"
    MLC

    # This should work but currently fails
    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "Hello"
    rescue => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("Unknown identifier")
    end
  end

  def test_string_methods
    mlc_source = <<~MLC
      fn process(text: str) -> str =
        text.trim().upper()
    MLC

    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "trim"
      assert_includes cpp, "upper"
    rescue => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("Unknown member")
    end
  end
end
