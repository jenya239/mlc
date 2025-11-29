# frozen_string_literal: true

module MLC
  module Source
  module Parser
    # TypeParser
    # Type parsing - primitives, sum types, record types, generics
    # Auto-extracted from parser.rb during refactoring
    module TypeParser
    def parse_enum_type
      enum_token = consume(:ENUM)
      consume(:LBRACE)
      variants = []

      while current.type != :RBRACE
        variant_name = consume(:IDENTIFIER).value
        variants << variant_name

        if current.type == :COMMA
          consume(:COMMA)
        else
          break
        end
      end

      consume(:RBRACE)
      with_origin(enum_token) { MLC::Source::AST::EnumType.new(name: "enum", variants: variants) }
    end

    def parse_record_type
      lbrace_token = consume(:LBRACE)
      fields = []

      while current.type != :RBRACE
        field_name = consume(:IDENTIFIER).value
        consume(:COLON)
        field_type = parse_type

        fields << {name: field_name, type: field_type}

        if current.type == :COMMA
          consume(:COMMA)
        else
          break
        end
      end

      consume(:RBRACE)
      with_origin(lbrace_token) { MLC::Source::AST::RecordType.new(name: "record", fields: fields) }
    end

    def parse_sum_type
      variants = []
      sum_origin_token = nil

      # Parse first variant (may or may not have leading |)
      if current.type == :OPERATOR && current.value == "|"
        consume(:OPERATOR)
      end

      loop do
        name_token = consume(:IDENTIFIER)
        sum_origin_token ||= name_token
        variant_name = name_token.value
        variant_fields = []

        # Check if variant has fields
        if current.type == :LPAREN
          # Tuple-like variant: Circle(f32, f32)
          consume(:LPAREN)
          field_index = 0
          while current.type != :RPAREN
            field_type = parse_type
            # Generate field name for tuple-like variants
            variant_fields << {name: "field#{field_index}", type: field_type}
            field_index += 1

            if current.type == :COMMA
              consume(:COMMA)
            else
              break
            end
          end
          consume(:RPAREN)
        elsif current.type == :LBRACE
          # Named fields variant: Ok { value: i32 }
          consume(:LBRACE)
          while current.type != :RBRACE
            field_name = consume(:IDENTIFIER).value
            consume(:COLON)
            field_type = parse_type
            variant_fields << {name: field_name, type: field_type}

            if current.type == :COMMA
              consume(:COMMA)
            else
              break
            end
          end
          consume(:RBRACE)
        end

        variants << {name: variant_name, fields: variant_fields}

        # Check for next variant
        break unless current.type == :OPERATOR && current.value == "|"
        consume(:OPERATOR)  # consume |
      end

      with_origin(sum_origin_token) { MLC::Source::AST::SumType.new(name: "sum", variants: variants) }
    end

    def parse_type
      # Check for reference types: ref T or ref mut T
      if current.type == :REF
        return parse_reference_type
      end

      base_token = nil
      base_type = case current.type
                  when :LPAREN
                    # Parse tuple type: (T1, T2, ...)
                    # Supports trailing comma for single-element tuples: (i32,)
                    base_token = consume(:LPAREN)
                    types = []
                    unless current.type == :RPAREN
                      loop do
                        types << parse_type
                        break unless current.type == :COMMA
                        consume(:COMMA)
                        # Handle trailing comma: (i32,) - break if next token is RPAREN
                        break if current.type == :RPAREN
                      end
                    end
                    consume(:RPAREN)
                    with_origin(base_token) { MLC::Source::AST::TupleType.new(types: types) }
                  when :LBRACKET
                    # Parse prefix array type: [Type]
                    base_token = consume(:LBRACKET)
                    element_type = parse_type
                    consume(:RBRACKET)
                    return with_origin(base_token) { MLC::Source::AST::ArrayType.new(element_type: element_type) }
                  when :I8
                    base_token = consume(:I8)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "i8") }
                  when :I16
                    base_token = consume(:I16)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "i16") }
                  when :I32
                    base_token = consume(:I32)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "i32") }
                  when :I64
                    base_token = consume(:I64)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "i64") }
                  when :U8
                    base_token = consume(:U8)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "u8") }
                  when :U16
                    base_token = consume(:U16)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "u16") }
                  when :U32
                    base_token = consume(:U32)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "u32") }
                  when :U64
                    base_token = consume(:U64)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "u64") }
                  when :F32
                    base_token = consume(:F32)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "f32") }
                  when :F64
                    base_token = consume(:F64)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "f64") }
                  when :BOOL
                    base_token = consume(:BOOL)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "bool") }
                  when :VOID
                    base_token = consume(:VOID)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "void") }
                  when :STR
                    base_token = consume(:STR)
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: "str") }
                  when :IDENTIFIER
                    base_token = consume(:IDENTIFIER)
                    name = base_token.value
                    with_origin(base_token) { MLC::Source::AST::PrimType.new(name: name) }
                  when :LBRACE
                    base_type = parse_record_type
                    base_token = @last_token
                    base_type
                  when :FN
                    # Parse function type: fn(T, U) -> V
                    base_token = consume(:FN)
                    consume(:LPAREN)

                    # Parse parameter types
                    param_types = []
                    unless current.type == :RPAREN
                      loop do
                        param_types << parse_type
                        break unless current.type == :COMMA
                        consume(:COMMA)
                      end
                    end

                    consume(:RPAREN)
                    consume(:ARROW)
                    ret_type = parse_type

                    with_origin(base_token) do
                      MLC::Source::AST::FunctionType.new(
                        param_types: param_types,
                        ret_type: ret_type
                      )
                    end
                  else
                    raise "Unexpected token: #{current}"
                  end

      # Check for generic type parameters <T1, T2, ...>
      if current.type == :OPERATOR && current.value == "<"
        consume_operator("<")
        type_params = []

        loop do
          type_params << parse_type
          break unless current.type == :COMMA
          consume(:COMMA)
        end

        consume_operator(">")

        base_type = with_origin(base_token) do
          MLC::Source::AST::GenericType.new(
            base_type: base_type,
            type_params: type_params
          )
        end
      end

      # Check for array type suffix []
      if current.type == :LBRACKET
        lbracket_token = consume(:LBRACKET)
        consume(:RBRACKET)
        with_origin(lbracket_token) { MLC::Source::AST::ArrayType.new(element_type: base_type) }
      else
        base_type
      end
    end

    def parse_type_or_sum
      # Try parsing as sum type first
      start_pos = @pos
      first_variant_name = consume(:IDENTIFIER).value

      # Check if this is a sum type variant
      # Patterns: Variant(...) or Variant {...} or Variant | ...
      is_sum_type = current.type == :LPAREN ||
                    (current.type == :LBRACE && peek_for_sum_type?) ||
                    (current.type == :OPERATOR && current.value == "|")

      if is_sum_type
        # Reset and parse as sum type
        @pos = start_pos
        return parse_sum_type
      end

      # Otherwise, it's just a type reference
      @pos = start_pos
      parse_type
    end

    def parse_type_params
      # Parse comma-separated list of type parameters: T, E, R or T: Constraint
      params = []
      loop do
        name_token = consume(:IDENTIFIER)
        name = name_token.value
        constraint = nil

        if current.type == :COLON
          consume(:COLON)

          if current.type == :IDENTIFIER
            constraint = consume(:IDENTIFIER).value
          else
            raise "Expected constraint identifier, got #{current.type}(#{current.value})"
          end
        end

        params << with_origin(name_token) { MLC::Source::AST::TypeParam.new(name: name, constraint: constraint) }

        break unless current.type == :COMMA
        consume(:COMMA)
      end
      params
    end

    def peek_for_sum_type?
      # Look ahead to see if this is a sum type variant with named fields
      # Check if after { there's eventually a | (indicating more variants)
      saved_pos = @pos
      depth = 0

      while @pos < @tokens.size
        case current.type
        when :LBRACE
          depth += 1
        when :RBRACE
          depth -= 1
          if depth == 0
            @pos += 1
            # After closing brace, check for | indicating another variant
            result = current.type == :OPERATOR && current.value == "|"
            @pos = saved_pos
            return result
          end
        end
        @pos += 1
      end

      @pos = saved_pos
      false
    end

    # Parse reference type: ref T or ref mut T
    def parse_reference_type
      ref_token = consume(:REF)

      # Check for mutable reference: ref mut T
      if current.type == :MUT
        consume(:MUT)
        inner_type = parse_type
        with_origin(ref_token) { MLC::Source::AST::MutRefType.new(inner_type: inner_type) }
      else
        # Immutable reference: ref T
        inner_type = parse_type
        with_origin(ref_token) { MLC::Source::AST::RefType.new(inner_type: inner_type) }
      end
    end

    end
  end
  end
end
