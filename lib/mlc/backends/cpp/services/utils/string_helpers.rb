# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        module Utils
          # Pure functions for string manipulation in C++ code generation
          # Migrated from Backend::CodeGenHelpers
          module StringHelpers
            CPP_KEYWORDS = %w[
              alignas alignof and and_eq asm atomic_cancel atomic_commit atomic_noexcept auto bitand
              bitor bool break case catch char char8_t char16_t char32_t class compl concept const
              consteval constexpr constinit const_cast continue co_await co_return co_yield
              decltype default delete do double dynamic_cast else enum explicit export extern false
              float for friend goto if import inline int long module mutable namespace new noexcept
              not not_eq nullptr operator or or_eq private protected public register reinterpret_cast
              requires return short signed sizeof static static_assert static_cast struct switch
              template this thread_local throw true try typedef typeid typename union unsigned using
              virtual void volatile wchar_t while xor xor_eq
            ].freeze

            module_function

            # Check if name is a C++ keyword
            def cpp_keyword?(name)
              CPP_KEYWORDS.include?(name)
            end

            # Sanitize identifier for C++ (add _ suffix if keyword)
            def sanitize_identifier(name)
              return name unless name.is_a?(String)
              cpp_keyword?(name) ? "#{name}_" : name
            end

            # Escape string for C++ string literals
            def escape_cpp_string(value)
              value.each_char.map do |ch|
                case ch
                when "\\" then "\\\\"
                when "\"" then "\\\""
                when "\n" then "\\n"
                when "\r" then "\\r"
                when "\t" then "\\t"
                when "\0" then "\\0"
                else ch
                end
              end.join
            end

            # Build C++ string literal: "escaped content"
            def cpp_string_literal(value)
              "\"#{escape_cpp_string(value)}\""
            end
          end
        end
      end
    end
  end
end
