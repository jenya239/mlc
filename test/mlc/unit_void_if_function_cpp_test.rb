# frozen_string_literal: true

require_relative "../test_helper"

# Regression: expression-bodied fn () = if … then … else () must not go through IfRule
# as a ternary (C++ forbids mixing void branch with push_back branch).
class UnitVoidIfFunctionCppTest < Minitest::Test
  # When a void function is spelled `fn f(...) -> () = if ...` (not only `do ... end`),
  # the C++ lowerer must emit an if-statement, not a ternary, or g++ rejects mixed
  # void / value branches. Same for the last expression of `do ... end` when it is
  # a unit if. Long else-if chains lower to nested ternary IIFEs: unify branch types
  # with a trailing `()`, and use a value-returning helper plus assignment (lexer
  # `errors = append_nonempty_scan_message_to_errors(...)`) instead of a void helper
  # mutating a by-value array parameter.
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

  def test_else_if_chain_with_unit_helper_needs_trailing_unit_per_arm
    mlc = <<~SRC
      fn accumulate(messages: [string], text: string) -> () = do
        if text != '' then messages.push(text) else () end
        ()
      end

      fn drive(flag: int, stash: [string]) -> () = do
        if flag == 0 then
          const scratch = 1
          accumulate(stash, "a")
          ()
        else if flag == 1 then
          const scratch = 2
          accumulate(stash, "b")
          ()
        else
          const scratch = 3
          accumulate(stash, "c")
          ()
        end
      end
    SRC
    cpp = MLC.to_cpp(mlc)
    refute_match(/return\s+accumulate\(/, cpp, "unit helper must not be sole return in ternary/IIFE branch")
    assert_match(/accumulate\(stash,[^\n]+\);\n\s+return\s+std::make_tuple\(\)/m, cpp)
  end

  def test_else_if_chain_assigns_array_returning_helper
    mlc = <<~SRC
      fn merge_nonempty(messages: [string], text: string) -> [string] = do
        if text != '' then
          let mutable_workspace = messages
          mutable_workspace.push(text)
          mutable_workspace
        else
          messages
        end
      end

      fn collect(marker: i32) -> [string] = do
        let mut messages: [string] = []
        if marker == 0 then
          messages = merge_nonempty(messages, "a")
        else if marker == 1 then
          messages = merge_nonempty(messages, "b")
        else
          ()
        end
        messages
      end
    SRC
    cpp = MLC.to_cpp(mlc)
    assert_match(/messages = merge_nonempty\(/m, cpp)
    refute_match(/return\s+merge_nonempty\(/m, cpp)
  end
end
