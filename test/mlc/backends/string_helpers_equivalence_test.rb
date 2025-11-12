# frozen_string_literal: true

require_relative "../../test_helper"
require_relative "../../../lib/mlc/backend/codegen/helpers"
require_relative "../../../lib/mlc/backends/cpp/services/utils/string_helpers"

module MLC
  module Backends
    module Cpp
      module Services
        module Utils
          # Test equivalence between legacy CodeGenHelpers and new StringHelpers
          class StringHelpersEquivalenceTest < Minitest::Test
            # Legacy module alias for brevity
            Legacy = ::MLC::Backend::CodeGenHelpers
            New = StringHelpers

            def test_cpp_keyword_equivalence
              cpp_keywords = %w[class int void return if else for while]
              non_keywords = %w[foo bar main result value]

              cpp_keywords.each do |word|
                assert_equal Legacy.cpp_keyword?(word), New.cpp_keyword?(word),
                  "cpp_keyword?(#{word.inspect}) should match"
              end

              non_keywords.each do |word|
                assert_equal Legacy.cpp_keyword?(word), New.cpp_keyword?(word),
                  "cpp_keyword?(#{word.inspect}) should match"
              end
            end

            def test_sanitize_identifier_equivalence
              test_cases = [
                "class",     # C++ keyword
                "int",       # C++ keyword
                "return",    # C++ keyword
                "foo",       # normal identifier
                "myFunc",    # normal identifier
                "result_",   # already sanitized
                nil,         # edge case
                123,         # non-string
              ]

              test_cases.each do |name|
                assert_equal Legacy.sanitize_identifier(name), New.sanitize_identifier(name),
                  "sanitize_identifier(#{name.inspect}) should match"
              end
            end

            def test_escape_cpp_string_equivalence
              test_cases = [
                "hello",                    # simple string
                "hello\nworld",             # newline
                "hello\tworld",             # tab
                "hello\rworld",             # carriage return
                "hello\\world",             # backslash
                "hello\"world",             # quote
                "hello\0world",             # null
                "mix: \n\t\r\\\"",          # mixed escapes
                "",                         # empty string
                "no_special_chars",         # no escapes needed
              ]

              test_cases.each do |str|
                assert_equal Legacy.escape_cpp_string(str), New.escape_cpp_string(str),
                  "escape_cpp_string(#{str.inspect}) should match"
              end
            end

            def test_cpp_string_literal_equivalence
              # NOTE: Legacy cpp_string_literal returns CppAst::Nodes::StringLiteral.new
              # New cpp_string_literal returns a string "escaped_value"
              # This is INTENTIONAL - new architecture builds AST via rules, not helpers
              # We only test the string value part here

              test_cases = [
                "hello",
                "hello\nworld",
                "hello\tworld",
                "hello\\world",
                "hello\"world",
                "",
              ]

              test_cases.each do |str|
                legacy_result = Legacy.cpp_string_literal(str)
                new_result = New.cpp_string_literal(str)

                # Legacy returns CppAst node with .value attribute
                # New returns string directly
                assert legacy_result.is_a?(CppAst::Nodes::StringLiteral),
                  "Legacy should return CppAst::Nodes::StringLiteral"
                assert new_result.is_a?(String),
                  "New should return String"

                # Compare the actual string values
                assert_equal legacy_result.value, new_result,
                  "cpp_string_literal(#{str.inspect}) string value should match"
              end
            end

            def test_cpp_keywords_constant_equivalence
              # Verify CPP_KEYWORDS constant is identical in both modules
              assert_equal Legacy::CPP_KEYWORDS, New::CPP_KEYWORDS,
                "CPP_KEYWORDS constants should be identical"
            end
          end
        end
      end
    end
  end
end
