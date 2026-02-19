# frozen_string_literal: true

module MLC
  module Source
    module Parser
      # ExpressionParser
      # Expression parsing - literals, operators, lambdas, control flow
      # Auto-extracted from parser.rb during refactoring
      module ExpressionParser
        MAX_COMPLEX_NESTING = 128

        def looks_like_lambda?
          # Save position
          saved_pos = @pos

          return false unless current.type == :LPAREN

          @pos += 1 # Skip (

          # Skip params
          @pos += 1 while !eof? && current.type != :RPAREN && current.type != :FAT_ARROW

          # Check if we find =>
          found_arrow = false
          if current.type == :RPAREN
            @pos += 1
            found_arrow = current.type == :FAT_ARROW
          end

          # Restore position
          @pos = saved_pos
          found_arrow
        end

        def looks_like_match_arms?
          # Lookahead to determine if { starts match arms or record literal
          # Match arms start with patterns: /, _, uppercase IDENTIFIER, lowercase identifier, etc.
          # Record literals start with: lowercase_identifier :
          return false unless current.type == :LBRACE

          next_token = peek
          return false unless next_token

          # If next token is /, _, or RBRACE, it's match arms
          return true if next_token.type == :REGEX
          return true if next_token.type == :UNDERSCORE
          return true if next_token.type == :RBRACE

          # If next token is IDENTIFIER followed by :, it's a record field
          # If next token is IDENTIFIER followed by =>, it's a match arm
          if next_token.type == :IDENTIFIER
            token_after_next = @tokens[@pos + 2] if @pos + 2 < @tokens.length
            return false if token_after_next && token_after_next.type == :COLON
            return true if token_after_next && token_after_next.type == :FAT_ARROW

            # Otherwise, assume it's a match arm if identifier is uppercase (constructor pattern)
            # or lowercase (variable binding pattern)
            return true
          end

          false
        end

        def parse_addition
          left = parse_multiplication

          while current.type == :OPERATOR && %w[+ -].include?(current.value)
            token = consume(:OPERATOR)
            op = token.value
            right = parse_multiplication
            node = MLC::Source::AST::BinaryOp.new(op: op, left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_args
          args = []

          while current.type != :RPAREN
            args << parse_expression

            break unless current.type == :COMMA

            consume(:COMMA)

          end

          args
        end

        def parse_array_literal_or_comprehension
          lbracket_token = consume(:LBRACKET)

          # Empty array
          if current.type == :RBRACKET
            consume(:RBRACKET)
            return with_origin(lbracket_token) { MLC::Source::AST::ArrayLiteral.new(elements: []) }
          end

          # Parse first expression
          first_expr = parse_if_expression

          if current.type == :FOR
            # It's a comprehension: [expr for var in iterable]
            generators = []
            filters = []

            while current.type == :FOR
              for_token = consume(:FOR)
              var_token = consume(:IDENTIFIER)
              var_name = var_token.value
              consume(:IN)
              # Use parse_logical_or to avoid consuming IF as postfix (IF is filter keyword here)
              iterable = parse_logical_or

              generators << with_origin(for_token) do
                MLC::Source::AST::Generator.new(
                  var_name: var_name,
                  iterable: iterable
                )
              end

              # Check for filter
              next unless current.type == :IF

              consume(:IF)
              # Use parse_logical_or for filter condition (avoid postfix if)
              filters << parse_logical_or
            end

            consume(:RBRACKET)

            with_origin(lbracket_token) do
              MLC::Source::AST::ListComprehension.new(
                output_expr: first_expr,
                generators: generators,
                filters: filters
              )
            end
          else
            # Regular array literal
            elements = [first_expr]

            while current.type == :COMMA
              consume(:COMMA)
              break if current.type == :RBRACKET # Trailing comma

              elements << parse_if_expression
            end

            consume(:RBRACKET)

            with_origin(lbracket_token) { MLC::Source::AST::ArrayLiteral.new(elements: elements) }
          end
        end

        def parse_comparison
          left = parse_range

          while current.type == :OPERATOR && %w[< > <= >=].include?(current.value)
            token = consume(:OPERATOR)
            op = token.value
            right = parse_range
            node = MLC::Source::AST::BinaryOp.new(op: op, left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        # Range expressions: a..b (inclusive) or a...b (exclusive)
        # Lower precedence than shift, higher than comparison
        def parse_range
          left = parse_shift

          # Check for range operator .. or spread/exclusive range ...
          if current.type == :RANGE
            range_token = consume(:RANGE)
            right = parse_shift
            attach_origin(MLC::Source::AST::RangeExpr.new(start: left, end_expr: right, inclusive: true), range_token)
          elsif current.type == :SPREAD
            spread_token = consume(:SPREAD)
            right = parse_shift
            # ... is exclusive range (does not include end)
            attach_origin(MLC::Source::AST::RangeExpr.new(start: left, end_expr: right, inclusive: false), spread_token)
          else
            left
          end
        end

        # Shift operators: a << b, a >> b (between addition and comparison)
        def parse_shift
          left = parse_addition

          while current.type == :OPERATOR && %w[<< >>].include?(current.value)
            token = consume(:OPERATOR)
            op = token.value
            right = parse_addition
            node = MLC::Source::AST::BinaryOp.new(op: op, left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_do_expression
          do_token = consume(:DO)
          items = parse_block_items_until_end
          build_block_expr(items, do_token)
        end

        def parse_do_statement
          # Parse a single statement/expression in a do block
          case current.type
          when :LET
            parse_variable_decl_statement
          when :CONST
            parse_const_decl_statement
          when :IF, :UNLESS
            parse_if_expression_in_block
          when :MATCH
            parse_match_expression
          when :DO
            parse_do_expression
          when :BREAK
            token = consume(:BREAK)
            with_origin(token) { MLC::Source::AST::Break.new }
          when :CONTINUE
            token = consume(:CONTINUE)
            with_origin(token) { MLC::Source::AST::Continue.new }
          when :IDENTIFIER
            # Check for assignment: x = ...
            if peek && peek.type == :EQUAL
              target_token = consume(:IDENTIFIER)
              target_name = target_token.value
              consume(:EQUAL)
              value = parse_expression
              target = attach_origin(MLC::Source::AST::VarRef.new(name: target_name), target_token)
              with_origin(target_token) { MLC::Source::AST::Assignment.new(target: target, value: value) }
            else
              # Otherwise it's an expression (inside block, don't steal END)
              parse_if_expression_in_block
            end
          else
            # Parse any other expression (inside block, don't steal END)
            parse_if_expression_in_block
          end
        end

        def parse_equality
          left = parse_pipe

          while current.type == :OPERATOR && %w[== !=].include?(current.value)
            token = consume(:OPERATOR)
            op = token.value
            right = parse_pipe
            node = MLC::Source::AST::BinaryOp.new(op: op, left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_complex_expression(inside_block: false)
          @complex_expr_depth = (@complex_expr_depth || 0) + 1
          raise parse_error("Expression nesting too deep") if @complex_expr_depth > MAX_COMPLEX_NESTING

          parse_complex_expression_inner(inside_block: inside_block)
        ensure
          @complex_expr_depth = [(@complex_expr_depth || 1) - 1, 0].max
        end

        def parse_complex_expression_inner(inside_block:)
          case current.type
          when :MATCH
            parse_match_expression
          when :DO
            parse_do_expression
          else
            inside_block ? parse_let_expression_in_block : parse_let_expression
          end
        end

        def parse_expression
          parse_complex_expression
        end

        # Parse expression inside a block context (don't consume END for if)
        def parse_expression_in_block
          parse_complex_expression(inside_block: true)
        end

        def parse_let_expression_in_block
          parse_let_expression_common(inside_block: true)
        end

        def parse_for_loop
          for_token = consume(:FOR)
          var_token = consume(:IDENTIFIER)
          var_name = var_token.value
          consume(:IN)
          iterable = parse_if_expression
          consume(:DO)

          items = parse_block_items_until_end
          body = build_block_expr(items, for_token)

          with_origin(for_token) do
            MLC::Source::AST::ForLoop.new(
              var_name: var_name,
              iterable: iterable,
              body: body
            )
          end
        end

        def parse_if_branch_expression(then_line: nil)
          if current.type == :LBRACE
            block = parse_block_expression
            wrap_block_like_expr(block)
          elsif current.type == :DO
            wrap_block_like_expr(parse_do_expression)
          elsif then_line && current.line > then_line
            # Body starts on a new line after 'then' — multiline if, parse until end
            items = []
            until current.type == :END || current.type == :ELSE || eof?
              items << parse_do_statement
              consume(:SEMICOLON) if current.type == :SEMICOLON
            end
            # Only consume 'end' if it terminates this if (not 'else' branch)
            consume(:END) if current.type == :END
            build_block_expr(items, current)
          else
            # Single statement/expression on same line as 'then'
            item = parse_do_statement
            wrap_block_like_expr(item)
          end
        end

        def wrap_block_like_expr(node)
          case node
          when MLC::Source::AST::BlockExpr
            node
          when MLC::Source::AST::Block
            origin = node.origin
            MLC::Source::AST::BlockExpr.new(
              statements: node.stmts,
              result_expr: MLC::Source::AST::UnitLit.new(origin: origin),
              origin: origin
            )
          when MLC::Source::AST::Stmt
            origin = node.origin
            MLC::Source::AST::BlockExpr.new(
              statements: [node],
              result_expr: MLC::Source::AST::UnitLit.new(origin: origin),
              origin: origin
            )
          else
            origin = node.respond_to?(:origin) ? node.origin : nil
            MLC::Source::AST::BlockExpr.new(
              statements: [],
              result_expr: node,
              origin: origin
            )
          end
        end

        def parse_if_expression
          if [:IF, :UNLESS].include?(current.type)
            parse_if_or_unless_expression(inside_block: false)
          else
            # Parse primary expression, then check for postfix if/unless
            expr = parse_logical_or
            parse_postfix_conditional(expr)
          end
        end

        # Parse if expression when inside a block (do...end, while, etc.)
        # Does not consume trailing END to avoid stealing parent's END
        def parse_if_expression_in_block
          if [:IF, :UNLESS].include?(current.type)
            parse_if_or_unless_expression(inside_block: true)
          else
            expr = parse_logical_or
            parse_postfix_conditional(expr)
          end
        end

        def parse_block_items_until_end
          items = []

          until current.type == :END
            raise "Unexpected EOF in block, expected 'end'" if eof?

            items << parse_do_statement
            consume(:SEMICOLON) if current.type == :SEMICOLON
          end

          consume(:END)
          items
        end

        def build_block_expr(items, origin_token)
          statements = []
          result_expr = nil

          if items.empty?
            result_expr = MLC::Source::AST::UnitLit.new
          else
            *stmt_items, last_item = items

            stmt_items.each do |item|
              statements << if item.is_a?(MLC::Source::AST::Stmt)
                              item
                            else
                              MLC::Source::AST::ExprStmt.new(expr: item)
                            end
            end

            if last_item.is_a?(MLC::Source::AST::Stmt) && !last_item.is_a?(MLC::Source::AST::VariableDecl)
              statements << last_item
              result_expr = MLC::Source::AST::UnitLit.new
            elsif last_item.is_a?(MLC::Source::AST::VariableDecl)
              statements << last_item
              result_expr = MLC::Source::AST::UnitLit.new
            else
              result_expr = last_item
            end
          end

          with_origin(origin_token) { MLC::Source::AST::BlockExpr.new(statements: statements, result_expr: result_expr) }
        end

        # Parse if/unless with then/else/end
        # Compatible with both old syntax (no end) and new syntax (with end)
        def parse_if_or_unless_expression(inside_block: false)
          is_unless = current.type == :UNLESS
          if_token = consume(current.type) # IF or UNLESS
          condition = parse_logical_or

          # Optional then keyword
          then_token = current if current.type == :THEN
          consume(:THEN) if current.type == :THEN
          then_line = then_token&.line || if_token.line

          # Parse then branch
          then_branch = parse_if_branch_expression(then_line: then_line)

          # Parse else branch
          else_branch = nil
          if current.type == :ELSE
            else_token = consume(:ELSE)
            else_line = else_token.line
            # Check for else-if chain
            else_branch = if [:IF, :UNLESS].include?(current.type)
                            wrap_block_like_expr(parse_if_or_unless_expression(inside_block: inside_block))
                          else
                            parse_if_branch_expression(then_line: else_line)
                          end
          end

          # Consume trailing end when not inside a block
          # (multiline then-branch without else already consumed its own end)
          consume(:END) if current.type == :END && !inside_block

          # For unless, invert condition: unless x = if !x
          final_condition = if is_unless
                              MLC::Source::AST::UnaryOp.new(op: "!", operand: condition)
                            else
                              condition
                            end

          with_origin(if_token) { MLC::Source::AST::IfExpr.new(condition: final_condition, then_branch: then_branch, else_branch: else_branch) }
        end

        # Parse postfix if/unless: expr if condition, expr unless condition
        # Only applies when if/unless is on the same line as the expression
        def parse_postfix_conditional(expr)
          expr_line = expression_line(expr)
          return expr unless expr_line
          return expr unless [:IF, :UNLESS].include?(current.type)
          return expr unless current.line == expr_line

          token = consume(current.type)
          condition = parse_logical_or
          condition = MLC::Source::AST::UnaryOp.new(op: "!", operand: condition) if token.type == :UNLESS

          then_branch = wrap_block_like_expr(expr)
          else_branch = MLC::Source::AST::BlockExpr.new(statements: [], result_expr: MLC::Source::AST::UnitLit.new)

          with_origin(token) do
            MLC::Source::AST::IfExpr.new(
              condition: condition,
              then_branch: then_branch,
              else_branch: else_branch
            )
          end
        end

        # Parse string interpolation: "Hello, {name}!"
        # Token value is array of {type: :text/:expr, value: String}
        def parse_string_interpolation(token)
          raw_parts = token.value

          # Parse expression parts by creating sub-parsers
          parsed_parts = raw_parts.map do |part|
            if part[:type] == :text
              { type: :text, value: part[:value] }
            else
              # Parse the expression string
              expr_source = part[:value]
              sub_lexer = MLC::Source::Parser::Lexer.new(expr_source)
              sub_tokens = sub_lexer.tokenize
              sub_parser = MLC::Source::Parser::Parser.new_from_tokens(sub_tokens)
              expr = sub_parser.parse_single_expression
              { type: :expr, value: expr }
            end
          end

          attach_origin(MLC::Source::AST::StringInterpolation.new(parts: parsed_parts), token)
        end

        def parse_lambda
          if current.type == :IDENTIFIER && peek && peek.type == :FAT_ARROW
            # Single parameter: x => expr
            param_token = consume(:IDENTIFIER)
            param_name = param_token.value
            consume(:FAT_ARROW)
            body = parse_if_expression

            param = with_origin(param_token) { MLC::Source::AST::LambdaParam.new(name: param_name) }
            with_origin(param_token) { MLC::Source::AST::Lambda.new(params: [param], body: body) }

          elsif current.type == :LPAREN
            # Multiple parameters: (x, y) => expr or (x: i32, y: i32) => expr
            lparen_token = consume(:LPAREN)
            params = parse_lambda_params
            consume(:RPAREN)
            consume(:FAT_ARROW)
            body = parse_lambda_body

            with_origin(lparen_token) { MLC::Source::AST::Lambda.new(params: params, body: body) }
          else
            expect!(false, "Expected lambda expression")
          end
        end

        def parse_lambda_body
          if current.type == :LBRACE
            # Legacy block body: { stmts }
            consume(:LBRACE)
            body = parse_expression
            consume(:RBRACE)
            body
          elsif current.type == :DO
            # New block body: do ... end
            parse_do_expression
          else
            # Single expression
            parse_if_expression
          end
        end

        def parse_lambda_params
          params = []

          while current.type != :RPAREN
            name_token = consume(:IDENTIFIER)
            name = name_token.value
            param_type = nil

            if current.type == :COLON
              consume(:COLON)
              param_type = parse_type
            end

            params << with_origin(name_token) { MLC::Source::AST::LambdaParam.new(name: name, type: param_type) }

            break unless current.type == :COMMA

            consume(:COMMA)
          end

          params
        end

        def parse_let_expression
          parse_let_expression_common(inside_block: false)
        end

        def parse_let_expression_common(inside_block:)
          parse_if_expr = inside_block ? :parse_if_expression_in_block : :parse_if_expression
          parse_expr = inside_block ? :parse_expression_in_block : :parse_expression

          return send(parse_if_expr) unless current.type == :LET

          consume(:LET)
          let_info = parse_let_header
          mutable = let_info[:mutable]
          name_token = let_info[:name_token]
          type_annotation = let_info[:type_annotation]
          consume(:EQUAL)
          value = parse_let_value(parse_if_expr)

          unless current.type == :SEMICOLON
            body = send(parse_expr)
            return with_origin(name_token) do
              MLC::Source::AST::Let.new(
                name: name_token.value,
                value: value,
                body: body,
                mutable: mutable,
                type: type_annotation
              )
            end
          end

          consume(:SEMICOLON)
          statements = [with_origin(name_token) do
            MLC::Source::AST::VariableDecl.new(
              name: name_token.value,
              value: value,
              mutable: mutable,
              type: type_annotation
            )
          end]
          block = parse_statement_sequence(statements)
          ensure_block_has_result(block, require_value: false)
          block
        end

        def parse_let_statement
          # Parse: let x = value or let mut x = value or let x: Type = value
          let_token = consume(:LET)
          let_info = parse_let_header
          mutable = let_info[:mutable]
          name_token = let_info[:name_token]
          type_annotation = let_info[:type_annotation]
          value = parse_let_value(:parse_if_expression)

          # In do-blocks, let is a statement that doesn't need 'in'
          # It returns a special LetStmt expression
          with_origin(let_token) do
            MLC::Source::AST::Let.new(
              name: name_token.value,
              value: value,
              body: nil,
              mutable: mutable,
              type: type_annotation
            )
          end
        end

        def parse_let_header
          mutable = false
          
          if current.type == :MUT
            consume(:MUT)
            mutable = true
          end

          name_token = consume(:IDENTIFIER)

          type_annotation = nil
          if current.type == :COLON
            consume(:COLON)
            type_annotation = parse_type
          end

          {
            mutable: mutable,
            name_token: name_token,
            type_annotation: type_annotation
          }
        end

        def parse_let_value(parse_if_expr)
          return parse_do_expression if current.type == :DO

          send(parse_if_expr)
        end

        def parse_logical_and
          left = parse_bitwise_or

          while current.type == :OPERATOR && current.value == "&&"
            token = consume(:OPERATOR)
            right = parse_bitwise_or
            node = MLC::Source::AST::BinaryOp.new(op: "&&", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        # Bitwise OR: a | b (lower precedence than XOR)
        def parse_bitwise_or
          left = parse_bitwise_xor

          while current.type == :OPERATOR && current.value == "|"
            token = consume(:OPERATOR)
            right = parse_bitwise_xor
            node = MLC::Source::AST::BinaryOp.new(op: "|", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        # Bitwise XOR: a ^ b (lower precedence than AND)
        def parse_bitwise_xor
          left = parse_bitwise_and

          while current.type == :OPERATOR && current.value == "^"
            token = consume(:OPERATOR)
            right = parse_bitwise_and
            node = MLC::Source::AST::BinaryOp.new(op: "^", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        # Bitwise AND: a & b (lower precedence than equality)
        def parse_bitwise_and
          left = parse_equality

          while current.type == :OPERATOR && current.value == "&"
            token = consume(:OPERATOR)
            right = parse_equality
            node = MLC::Source::AST::BinaryOp.new(op: "&", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_logical_or
          left = parse_logical_and

          while current.type == :OPERATOR && current.value == "||"
            token = consume(:OPERATOR)
            right = parse_logical_and
            node = MLC::Source::AST::BinaryOp.new(op: "||", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_match_expression
          match_token = consume(:MATCH)
          scrutinee = parse_match_scrutinee

          arms = []

          # Check for brace-style or pipe-style match
          if current.type == :LBRACE
            # Brace style: match expr { pattern => body, ... }
            consume(:LBRACE)

            while current.type != :RBRACE
              # Parse pattern (with or-pattern support)
              pattern = parse_or_pattern

              # Parse guard (optional: "if condition")
              guard = parse_optional_match_guard

              # Expect =>
              expect_token!(:FAT_ARROW, current)

              consume(:FAT_ARROW)

              # Parse body
              body = parse_match_arm_body_node

              arms << { pattern: pattern, guard: guard, body: body }

              # Expect comma or closing brace
              if current.type == :COMMA
                consume(:COMMA)
              elsif current.type != :RBRACE
                expect!(current.type == :COMMA || current.type == :RBRACE, "Expected , or } in match expression")
              end
            end

            consume(:RBRACE)
          else
            parse_pipe_style_match_arms(arms)
          end

          with_origin(match_token) { MLC::Source::AST::MatchExpr.new(scrutinee: scrutinee, arms: arms) }
        end

        def parse_pipe_style_match_arms(arms)
          if current.type == :OPERATOR && current.value == "|"
            # Pipe style: match expr | pattern => body | ... end
            while current.type == :OPERATOR && current.value == "|"
              consume(:OPERATOR)
              pattern = parse_pattern
              pattern = build_or_pattern_if_needed(pattern)
              guard = parse_optional_match_guard
              consume_fat_arrow!
              body = parse_match_arm_body_node
              arms << { pattern: pattern, guard: guard, body: body }
            end
          elsif pattern_start?(current)
            # End-terminated style: match expr\n  pattern => body\nend
            until current.type == :END || eof?
              pattern = parse_pattern
              pattern = build_or_pattern_if_needed(pattern)
              guard = parse_optional_match_guard
              consume_fat_arrow!
              body = parse_match_arm_body_node
              arms << { pattern: pattern, guard: guard, body: body }
            end
          end
          consume(:END) if current.type == :END
        end

        def build_or_pattern_if_needed(pattern)
          alternatives = gather_or_pattern_alternatives(pattern)
          return pattern if alternatives.length <= 1

          origin = pattern.respond_to?(:origin) ? pattern.origin : nil
          MLC::Source::AST::Pattern.new(
            kind: :or,
            data: { alternatives: alternatives },
            origin: origin
          )
        end

        def gather_or_pattern_alternatives(first_pattern)
          alternatives = [first_pattern]
          while current.type == :OPERATOR && current.value == "|" && pattern_start?(peek)
            consume(:OPERATOR) # consume |
            alternatives << parse_pattern
          end
          alternatives
        end

        def parse_optional_match_guard
          return unless current.type == :IF

          consume(:IF)
          parse_equality
        end

        def consume_fat_arrow!
          return consume(:FAT_ARROW) if current.type == :FAT_ARROW

          return consume(:OPERATOR) if current.type == :OPERATOR && current.value == "=>"

          expect_token!(:FAT_ARROW, current)
        end

        def parse_match_arm_body_node
          if current.type == :DO
            wrap_block_like_expr(parse_do_expression)
          elsif [:IF, :UNLESS].include?(current.type)
            # inside_block: false so the if consumes its own 'end',
            # preventing the match arm parser from stopping prematurely
            wrap_block_like_expr(parse_if_or_unless_expression(inside_block: false))
          elsif current.type == :MATCH
            wrap_block_like_expr(parse_match_expression)
          else
            wrap_block_like_expr(parse_match_arm_body)
          end
        end

        # Parse match arm body - expression that stops at | or end
        def parse_match_arm_body
          # Parse single expression, but be careful not to consume | as operator
          parse_match_arm_expression
        end

        def parse_match_arm_expression
          # Parse expression but stop before | (which starts next arm)
          # This is a restricted version of parse_if_expression
          left = parse_logical_or_no_pipe
          parse_postfix_conditional(left)
        end

        def parse_logical_or_no_pipe
          # Like parse_logical_or but doesn't consume | for bitwise or
          # This is used in match arms where | separates arms, not bitwise or
          left = parse_logical_and_no_pipe

          while current.type == :OPERATOR && current.value == "||"
            token = consume(:OPERATOR)
            right = parse_logical_and_no_pipe
            node = MLC::Source::AST::BinaryOp.new(op: "||", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        # Logical AND that doesn't consume | for match arm bodies
        def parse_logical_and_no_pipe
          left = parse_bitwise_xor # Skip parse_bitwise_or - don't consume |

          while current.type == :OPERATOR && current.value == "&&"
            token = consume(:OPERATOR)
            right = parse_bitwise_xor
            node = MLC::Source::AST::BinaryOp.new(op: "&&", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_match_scrutinee
          # Parse scrutinee for match expression - special handling to avoid
          # consuming { as record literal when it's actually the start of match arms
          # We parse at comparison level to get most operators but stop before
          # consuming record literals
          parse_comparison

          # Don't parse record literal here - that's handled by match arms
          # Just return the identifier/expression
        end

        def parse_multiplication
          left = parse_unary

          while current.type == :OPERATOR && %w[* / %].include?(current.value)
            token = consume(:OPERATOR)
            op = token.value
            right = parse_unary
            node = MLC::Source::AST::BinaryOp.new(op: op, left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_pipe
          left = parse_comparison

          while current.type == :PIPE || (current.type == :OPERATOR && current.value == "|>")
            token = consume(current.type) # Consume PIPE or OPERATOR
            right = parse_comparison
            node = MLC::Source::AST::BinaryOp.new(op: "|>", left: left, right: right)
            left = attach_origin(node, token)
          end

          left
        end

        def parse_postfix
          expr = parse_primary
          expr_line = last_token&.line

          loop do
            next_expr, next_line = next_postfix_expression(expr, expr_line)
            break unless next_expr

            expr = next_expr
            expr_line = next_line
          end

          expr
        end

        def same_line?(line)
          line && current.line == line
        end

        def expression_line(expr)
          return expr.origin.line if expr.respond_to?(:origin) && expr.origin

          last_token&.line
        end

        def next_postfix_expression(expr, expr_line)
          case current.type
          when :SAFE_NAV
            handle_safe_nav_postfix(expr)
          when :OPERATOR
            current.value == "." ? handle_dot_postfix(expr) : nil
          when :LBRACKET
            handle_bracket_postfix(expr)
          when :LPAREN
            same_line?(expr_line) ? handle_call_postfix(expr) : nil
          when :QUESTION
            same_line?(expr_line) ? handle_try_postfix(expr) : nil
          end
        end

        def handle_safe_nav_postfix(expr)
          wrap_postfix(handle_safe_navigation(expr))
        end

        def handle_dot_postfix(expr)
          wrap_postfix(handle_dot_or_member(expr))
        end

        def handle_bracket_postfix(expr)
          lbracket_token = consume(:LBRACKET)
          wrap_postfix(parse_bracket_access(expr, lbracket_token))
        end

        def handle_call_postfix(expr)
          wrap_postfix(handle_call(expr))
        end

        def handle_try_postfix(expr)
          question_token = consume(:QUESTION)
          wrap_postfix(attach_origin(MLC::Source::AST::TryExpr.new(operand: expr), question_token))
        end

        def wrap_postfix(expr)
          [expr, last_token&.line]
        end

        def handle_safe_navigation(expr)
          safe_nav_token = consume(:SAFE_NAV)
          member_token = consume(:IDENTIFIER)
          member = member_token.value
          member_line = member_token.line

          if current.type == :LPAREN && current.line == member_line
            consume(:LPAREN)
            args = parse_args
            consume(:RPAREN)
            attach_origin(MLC::Source::AST::SafeCall.new(object: expr, method_name: member, args: args), safe_nav_token)
          else
            attach_origin(MLC::Source::AST::SafeMemberAccess.new(object: expr, member: member), safe_nav_token)
          end
        end

        def handle_dot_or_member(expr)
          consume(:OPERATOR) # consume '.'

          if current.type == :INT_LITERAL
            index_token = consume(:INT_LITERAL)
            index = index_token.value.to_i
            attach_origin(MLC::Source::AST::TupleAccess.new(tuple: expr, index: index), index_token)
          elsif current.type == :FLOAT_LITERAL
            float_token = consume(:FLOAT_LITERAL)
            float_str = float_token.value.to_s
            parts = float_str.split('.')
            raise parse_error("Invalid tuple access syntax") unless parts.size == 2 && parts[0] =~ /^\d+$/ && parts[1] =~ /^\d+$/

            first_index = parts[0].to_i
            second_index = parts[1].to_i
            inner_access = attach_origin(MLC::Source::AST::TupleAccess.new(tuple: expr, index: first_index), float_token)
            attach_origin(MLC::Source::AST::TupleAccess.new(tuple: inner_access, index: second_index), float_token)
          else
            member_token = consume(:IDENTIFIER)
            member = member_token.value
            member_line = member_token.line

            if current.type == :LPAREN && current.line == member_line
              consume(:LPAREN)
              args = parse_args
              consume(:RPAREN)
              member_access = member_access_node(expr, member, member_token)
              attach_origin(MLC::Source::AST::Call.new(callee: member_access, args: args), member_token)
            else
              member_access_node(expr, member, member_token)
            end
          end
        end

        def handle_call(expr)
          lparen_token = consume(:LPAREN)
          args = parse_args
          consume(:RPAREN)
          attach_origin(MLC::Source::AST::Call.new(callee: expr, args: args), lparen_token)
        end

        def member_access_node(object, member, token)
          attach_origin(
            MLC::Source::AST::MemberAccess.new(object: object, member: member, origin: origin_from(token)),
            token
          )
        end

        def parse_bracket_access(expr, lbracket_token)
          return parse_open_start_slice(expr, lbracket_token) if current.type == :RANGE

          # Parse first expression - use parse_shift to not consume RANGE operator
          first_expr = parse_shift
          return parse_slice_after_start(expr, lbracket_token, first_expr) if current.type == :RANGE

          consume(:RBRACKET)
          attach_origin(MLC::Source::AST::IndexAccess.new(object: expr, index: first_expr), lbracket_token)
        end

        def parse_open_start_slice(expr, lbracket_token)
          consume(:RANGE)
          if current.type == :RBRACKET
            consume(:RBRACKET)
            attach_origin(MLC::Source::AST::SliceAccess.new(object: expr), lbracket_token)
          else
            end_index = parse_shift
            consume(:RBRACKET)
            attach_origin(MLC::Source::AST::SliceAccess.new(object: expr, end_index: end_index), lbracket_token)
          end
        end

        def parse_slice_after_start(expr, lbracket_token, start_expr)
          consume(:RANGE)
          if current.type == :RBRACKET
            consume(:RBRACKET)
            attach_origin(MLC::Source::AST::SliceAccess.new(object: expr, start_index: start_expr), lbracket_token)
          else
            end_index = parse_shift
            consume(:RBRACKET)
            attach_origin(MLC::Source::AST::SliceAccess.new(object: expr, start_index: start_expr, end_index: end_index), lbracket_token)
          end
        end

        def parse_primary
          case current.type
          when :FOR
            parse_for_loop
          when :WHILE
            parse_while_loop
          when :INT_LITERAL
            token = consume(:INT_LITERAL)
            value = token.value
            attach_origin(MLC::Source::AST::IntLit.new(value: value), token)
          when :CHAR_LITERAL
            # Character literal 'a' -> integer value (e.g., 97)
            token = consume(:CHAR_LITERAL)
            value = token.value # Already converted to integer in lexer
            attach_origin(MLC::Source::AST::IntLit.new(value: value), token)
          when :FLOAT_LITERAL
            token = consume(:FLOAT_LITERAL)
            value = token.value
            attach_origin(MLC::Source::AST::FloatLit.new(value: value), token)
          when :STRING_LITERAL
            token = consume(:STRING_LITERAL)
            value = token.value
            attach_origin(MLC::Source::AST::StringLit.new(value: value), token)
          when :STRING_INTERP
            token = consume(:STRING_INTERP)
            parse_string_interpolation(token)
          when :REGEX
            token = consume(:REGEX)
            regex_data = token.value
            attach_origin(MLC::Source::AST::RegexLit.new(pattern: regex_data[:pattern], flags: regex_data[:flags]), token)
          when :IDENTIFIER
            # Check for lambda: x => expr
            if peek && peek.type == :FAT_ARROW
              parse_lambda
            else
              name_token = consume(:IDENTIFIER)
              name = name_token.value

              if current.type == :LPAREN
                # Function call
                lparen_token = consume(:LPAREN)
                args = parse_args
                consume(:RPAREN)
                callee = attach_origin(MLC::Source::AST::VarRef.new(name: name), name_token)
                attach_origin(MLC::Source::AST::Call.new(callee: callee, args: args), lparen_token)
              elsif current.type == :LBRACE && !looks_like_match_arms?
                # Record literal (but not match arms)
                lbrace_token = consume(:LBRACE)
                result = parse_record_fields
                consume(:RBRACE)
                attach_origin(MLC::Source::AST::RecordLit.new(type_name: name, fields: result[:fields], spreads: result[:spreads]),
                              lbrace_token)
              else
                # Variable reference
                attach_origin(MLC::Source::AST::VarRef.new(name: name), name_token)
              end
            end
          when :LPAREN
            # Could be lambda, tuple literal, or grouped expression
            # Lookahead to determine
            if looks_like_lambda?
              parse_lambda
            else
              parse_tuple_or_grouped
            end
          when :SYMBOL
            # Symbol literal: :foo
            token = consume(:SYMBOL)
            attach_origin(MLC::Source::AST::SymbolLit.new(name: token.value), token)
          when :LBRACE
            # Anonymous record literal
            lbrace_token = consume(:LBRACE)
            result = parse_record_fields
            consume(:RBRACE)
            attach_origin(MLC::Source::AST::RecordLit.new(type_name: "record", fields: result[:fields], spreads: result[:spreads]),
                          lbrace_token)
          when :LBRACKET
            parse_array_literal_or_comprehension
          when :STRING_ARRAY, :STRING_ARRAY_INTERP
            # %w[] or %W[] - array of strings
            parse_percent_string_array
          when :SYMBOL_ARRAY, :SYMBOL_ARRAY_INTERP
            # %i[] or %I[] - array of symbols
            parse_percent_symbol_array
          when :UNSAFE
            parse_unsafe_block
          when :SELF_ACCESS
            # @field desugars to self.field
            token = consume(:SELF_ACCESS)
            field_name = token.value
            self_ref = attach_origin(MLC::Source::AST::VarRef.new(name: "self"), token)
            member_access_node(self_ref, field_name, token)
          else
            raise "Unexpected token: #{current}"
          end
        end

        # Parse unsafe block: unsafe ... end or unsafe { body } (legacy)
        def parse_unsafe_block
          unsafe_token = consume(:UNSAFE)

          # Support both old brace syntax and new end syntax
          if current.type == :LBRACE
            # Legacy: unsafe { body }
            consume(:LBRACE)
            body = parse_unsafe_body_until(:RBRACE)
            consume(:RBRACE)
          else
            # New: unsafe ... end
            body = parse_unsafe_body_until(:END)
            consume(:END)
          end

          attach_origin(MLC::Source::AST::UnsafeBlock.new(body: body), unsafe_token)
        end

        # Parse unsafe body until terminator token
        def parse_unsafe_body_until(terminator)
          statements = []
          result_expr = nil
          all_items = []

          while current.type != terminator && !eof?
            item = parse_do_statement
            all_items << item
            consume(:SEMICOLON) if current.type == :SEMICOLON
          end

          # Process: last item is result, others are statements
          if all_items.empty?
            result_expr = MLC::Source::AST::UnitLit.new
          else
            *stmt_items, last_item = all_items
            statements = stmt_items.map do |item|
              item.is_a?(MLC::Source::AST::Stmt) ? item : MLC::Source::AST::ExprStmt.new(expr: item)
            end

            if last_item.is_a?(MLC::Source::AST::Stmt)
              statements << last_item
              result_expr = MLC::Source::AST::UnitLit.new
            else
              result_expr = last_item
            end
          end

          MLC::Source::AST::BlockExpr.new(statements: statements, result_expr: result_expr)
        end

        def parse_unary
          # Check for await keyword (like a unary prefix operator)
          if current.type == :AWAIT
            token = consume(:AWAIT)
            operand = parse_unary # Right-associative
            attach_origin(MLC::Source::AST::AwaitExpr.new(operand: operand), token)
          # Check for unary operators: !, -, +, ~ (bitwise NOT)
          elsif current.type == :OPERATOR && %w[! - + ~].include?(current.value)
            token = consume(:OPERATOR)
            op = token.value
            operand = parse_unary # Right-associative
            attach_origin(MLC::Source::AST::UnaryOp.new(op: op, operand: operand), token)
          else
            parse_postfix
          end
        end

        # Parse tuple literal or grouped expression
        # (x) -> grouped expression (returns x)
        # (x,) -> single-element tuple
        # (x, y) -> two-element tuple
        # (x, y, z) -> three-element tuple
        def parse_tuple_or_grouped
          lparen_token = consume(:LPAREN)

          # Empty tuple ()
          if current.type == :RPAREN
            consume(:RPAREN)
            return with_origin(lparen_token) { MLC::Source::AST::TupleLit.new(elements: []) }
          end

          # Parse first expression
          first_expr = parse_expression

          # Check what follows
          if current.type == :COMMA
            # It's a tuple
            elements = [first_expr]
            consume(:COMMA)

            # Check for trailing comma (single element tuple)
            unless current.type == :RPAREN
              # More elements
              elements << parse_expression
              while current.type == :COMMA
                consume(:COMMA)
                break if current.type == :RPAREN # Trailing comma

                elements << parse_expression
              end
            end

            consume(:RPAREN)
            with_origin(lparen_token) { MLC::Source::AST::TupleLit.new(elements: elements) }
          else
            # It's a grouped expression (x)
            consume(:RPAREN)
            first_expr
          end
        end

        def parse_while_loop
          while_token = consume(:WHILE)
          condition = parse_if_expression
          consume(:DO)

          # After 'do', parse the block directly as BlockExpr
          # This eliminates the need for 'while x < 3 do do ... end'
          # Now: 'while x < 3 do ... end' works directly
          statements = []
          result_expr = nil
          all_items = []

          until current.type == :END
            raise "Unexpected EOF in while block, expected 'end'" if eof?

            # Parse one statement/expression
            expr = parse_do_statement
            all_items << expr

            # Skip optional semicolon between statements
            consume(:SEMICOLON) if current.type == :SEMICOLON
          end

          consume(:END)

          # Process items: last expression becomes result, others become statements
          # Same logic as parse_do_expression
          if all_items.empty?
            # Empty block returns void
            result_expr = MLC::Source::AST::UnitLit.new
          else
            # Last item is the result expression
            *stmt_items, last_item = all_items

            # Convert all non-last items to statements
            stmt_items.each do |item|
              statements << if item.is_a?(MLC::Source::AST::Stmt)
                              # Already a statement (VariableDecl, Assignment)
                              item
                            else
                              # Wrap expression as statement
                              MLC::Source::AST::ExprStmt.new(expr: item)
                            end
            end

            # Last item is the result
            if last_item.is_a?(MLC::Source::AST::Stmt) && !last_item.is_a?(MLC::Source::AST::VariableDecl)
              # If last is a non-VariableDecl statement, add it and use void result
              statements << last_item
              result_expr = MLC::Source::AST::UnitLit.new
            elsif last_item.is_a?(MLC::Source::AST::VariableDecl)
              # VariableDecl is statement, not expression - add it and use void result
              statements << last_item
              result_expr = MLC::Source::AST::UnitLit.new
            else
              # Last is expression - use as result
              result_expr = last_item
            end
          end

          body = with_origin(while_token) { MLC::Source::AST::BlockExpr.new(statements: statements, result_expr: result_expr) }

          with_origin(while_token) do
            MLC::Source::AST::WhileLoop.new(
              condition: condition,
              body: body
            )
          end
        end

        # Parse %w[] or %W[] - array of strings
        # Token value is already an array of strings from the lexer
        def parse_percent_string_array
          token = consume(current.type) # Consume STRING_ARRAY or STRING_ARRAY_INTERP
          words = token.value # Array of strings

          # Convert each word to a StringLit and wrap in ArrayLiteral
          elements = words.map do |word|
            attach_origin(MLC::Source::AST::StringLit.new(value: word), token)
          end

          attach_origin(MLC::Source::AST::ArrayLiteral.new(elements: elements), token)
        end

        # Parse %i[] or %I[] - array of symbols
        # Token value is already an array of strings (symbol names) from the lexer
        def parse_percent_symbol_array
          token = consume(current.type) # Consume SYMBOL_ARRAY or SYMBOL_ARRAY_INTERP
          words = token.value # Array of symbol names

          # Convert each word to a SymbolLit and wrap in ArrayLiteral
          elements = words.map do |word|
            attach_origin(MLC::Source::AST::SymbolLit.new(name: word), token)
          end

          attach_origin(MLC::Source::AST::ArrayLiteral.new(elements: elements), token)
        end
      end
    end
  end
end
