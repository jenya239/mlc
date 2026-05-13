# frozen_string_literal: true

require_relative "../test_helper"

# Regression: expression-bodied fn () = if … then … else () must not go through IfRule
# as a ternary (C++ forbids mixing void branch with push_back branch).
class UnitVoidIfFunctionCppTest < Minitest::Test
  # When a void function is spelled `fn f(...) -> () = if ...` (not only `do ... end`),
  # the C++ lowerer must emit an if-statement, not a ternary, or g++ rejects mixed
  # void / value branches. Same for the last expression of `do ... end` when it is
  # a unit if. (Inside long else-if chains lowered to nested ternaries, calling a
  # void helper is still unsafe; see compiler/lexer tokenize string branches.)
  def test_arrow_body_unit_if_uses_cpp_if_statement
    mlc = <<~SRC
      fn conditional_append(messages: [string], text: string) -> () =
        if text.length() > 0 then messages.push(text) else () end
    SRC
    cpp = MLC.to_cpp(mlc)
    assert_includes cpp, "if (text.length() > 0)"
    assert_includes cpp, "messages.push_back(text)"
    refute_match(/\breturn\s+[^;\n]+\?[^\n]+\:/m, cpp, "unexpected ternary used as sole return")
  end

  def test_do_block_trailing_unit_if_uses_cpp_if_statement
    mlc = <<~SRC
      fn via_do(messages: [string], text: string) -> () = do
        if text.length() > 0 then messages.push(text) else () end
      end
    SRC
    cpp = MLC.to_cpp(mlc)
    assert_includes cpp, "if (text.length() > 0)"
    assert_includes cpp, "messages.push_back(text)"
  end
end
