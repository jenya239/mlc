# frozen_string_literal: true

module MLC
  module Source
  module Parser
    # ExpressionParser
    # Expression parsing - literals, operators, lambdas, control flow
    # Auto-extracted from parser.rb during refactoring
    module ExpressionParser
    def looks_like_lambda?
      # Save position
      saved_pos = @pos

      return false unless current.type == :LPAREN

      @pos += 1  # Skip (

      # Skip params
      while !eof? && current.type != :RPAREN && current.type != :FAT_ARROW
        @pos += 1
      end

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

        if current.type == :COMMA
          consume(:COMMA)
        else
          break
        end
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
          if current.type == :IF
            consume(:IF)
            # Use parse_logical_or for filter condition (avoid postfix if)
            filters << parse_logical_or
          end
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
          break if current.type == :RBRACKET  # Trailing comma
          elements << parse_if_expression
        end

        consume(:RBRACKET)

        with_origin(lbracket_token) { MLC::Source::AST::ArrayLiteral.new(elements: elements) }
      end
    end

    def parse_comparison
      left = parse_addition

      while current.type == :OPERATOR && %w[< > <= >=].include?(current.value)
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
      statements = []
      result_expr = nil
      all_items = []

      # Parse expressions until we hit END
      until current.type == :END
        if eof?
          raise "Unexpected EOF in do block, expected 'end'"
        end

        # Parse one expression
        expr = parse_do_statement
        all_items << expr

        # Skip optional semicolon between statements
        if current.type == :SEMICOLON
          consume(:SEMICOLON)
        end
      end

      consume(:END)

      # Process items: last expression becomes result, others become statements
      if all_items.empty?
        # Empty block returns unit type
        result_expr = MLC::Source::AST::UnitLit.new
      else
        # Last item is the result expression
        *stmt_items, last_item = all_items

        # Convert all non-last items to statements
        stmt_items.each do |item|
          if item.is_a?(MLC::Source::AST::Stmt)
            # Already a statement (VariableDecl, Assignment)
            statements << item
          else
            # Wrap expression as statement
            statements << MLC::Source::AST::ExprStmt.new(expr: item)
          end
        end

        # Last item is the result
        if last_item.is_a?(MLC::Source::AST::Stmt) && !last_item.is_a?(MLC::Source::AST::VariableDecl)
          # If last is a non-VariableDecl statement, add it and use unit result
          statements << last_item
          result_expr = MLC::Source::AST::UnitLit.new
        elsif last_item.is_a?(MLC::Source::AST::VariableDecl)
          # VariableDecl is statement, not expression - add it and use unit result
          statements << last_item
          result_expr = MLC::Source::AST::UnitLit.new
        else
          # Last is expression - use as result
          result_expr = last_item
        end
      end

      with_origin(do_token) { MLC::Source::AST::BlockExpr.new(statements: statements, result_expr: result_expr) }
    end

    def parse_do_statement
      # Parse a single statement/expression in a do block
      case current.type
      when :LET
        parse_variable_decl_statement
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

    def parse_expression
      if current.type == :MATCH
        parse_match_expression
      elsif current.type == :DO
        parse_do_expression
      else
        parse_let_expression
      end
    end

    # Parse expression inside a block context (don't consume END for if)
    def parse_expression_in_block
      if current.type == :MATCH
        parse_match_expression
      elsif current.type == :DO
        parse_do_expression
      else
        parse_let_expression_in_block
      end
    end

    # Parse let expression inside a block context
    def parse_let_expression_in_block
      return parse_if_expression_in_block unless current.type == :LET

      consume(:LET)
      mutable = false
      if current.type == :MUT
        consume(:MUT)
        mutable = true
      end

      name_token = consume(:IDENTIFIER)
      name = name_token.value

      type_annotation = nil
      if current.type == :COLON
        consume(:COLON)
        type_annotation = parse_type
      end

      consume(:EQUAL)
      value = parse_if_expression_in_block

      unless current.type == :SEMICOLON
        body = parse_expression_in_block
        return with_origin(name_token) { MLC::Source::AST::Let.new(name: name, value: value, body: body, mutable: mutable, type: type_annotation) }
      end

      consume(:SEMICOLON)
      statements = [with_origin(name_token) { MLC::Source::AST::VariableDecl.new(name: name, value: value, mutable: mutable, type: type_annotation) }]
      block = parse_statement_sequence(statements)
      ensure_block_has_result(block, require_value: false)
      block
    end

    def parse_for_loop
      for_token = consume(:FOR)
      var_token = consume(:IDENTIFIER)
      var_name = var_token.value
      consume(:IN)
      iterable = parse_if_expression
      consume(:DO)

      # After 'do', parse the block directly as BlockExpr
      # Same unified approach as while loop
      statements = []
      result_expr = nil
      all_items = []

      until current.type == :END
        if eof?
          raise "Unexpected EOF in for loop, expected 'end'"
        end

        # Parse one statement/expression
        expr = parse_do_statement
        all_items << expr

        # Skip optional semicolon between statements
        if current.type == :SEMICOLON
          consume(:SEMICOLON)
        end
      end

      consume(:END)

      # Process items: last expression becomes result, others become statements
      if all_items.empty?
        # Empty block returns void
        result_expr = MLC::Source::AST::UnitLit.new
      else
        # Last item is the result expression
        *stmt_items, last_item = all_items

        # Convert all non-last items to statements
        stmt_items.each do |item|
          if item.is_a?(MLC::Source::AST::Stmt)
            # Already a statement (VariableDecl, Assignment)
            statements << item
          else
            # Wrap expression as statement
            statements << MLC::Source::AST::ExprStmt.new(expr: item)
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

      body = with_origin(for_token) { MLC::Source::AST::BlockExpr.new(statements: statements, result_expr: result_expr) }

      with_origin(for_token) do
        MLC::Source::AST::ForLoop.new(
          var_name: var_name,
          iterable: iterable,
          body: body
        )
      end
    end

    def parse_if_branch_expression
      if current.type == :LBRACE
        block = parse_block_expression
        wrap_block_like_expr(block)
      elsif current.type == :DO
        wrap_block_like_expr(parse_do_expression)
      else
        # Allow single statement/expression without 'do'
        # This handles: if x then y = 1
        # as well as:    if x then 42
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
      if current.type == :IF || current.type == :UNLESS
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
      if current.type == :IF || current.type == :UNLESS
        parse_if_or_unless_expression(inside_block: true)
      else
        expr = parse_logical_or
        parse_postfix_conditional(expr)
      end
    end

    # Parse if/unless with then/else/end
    # Compatible with both old syntax (no end) and new syntax (with end)
    def parse_if_or_unless_expression(inside_block: false)
      is_unless = current.type == :UNLESS
      if_token = consume(current.type) # IF or UNLESS
      condition = parse_logical_or

      # Optional then keyword
      consume(:THEN) if current.type == :THEN

      # Parse then branch as single expression/statement (backward compatible)
      then_branch = parse_if_branch_expression

      # Parse else branch
      else_branch = nil
      if current.type == :ELSE
        consume(:ELSE)
        # Check for else-if chain
        if current.type == :IF || current.type == :UNLESS
          else_branch = wrap_block_like_expr(parse_if_or_unless_expression(inside_block: inside_block))
        else
          else_branch = parse_if_branch_expression
        end
      end

      # Consume optional end (new syntax) - but NOT if we're inside a block
      # to avoid stealing the parent block's END
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
      # Get expression's line (from origin or last token)
      expr_line = expr.respond_to?(:origin) && expr.origin ? expr.origin.line : nil
      expr_line ||= last_token&.line

      # Postfix only if if/unless is on the same line as expression
      if current.type == :IF && expr_line && current.line == expr_line
        if_token = consume(:IF)
        condition = parse_logical_or
        then_branch = wrap_block_like_expr(expr)
        else_branch = MLC::Source::AST::BlockExpr.new(statements: [], result_expr: MLC::Source::AST::UnitLit.new)
        with_origin(if_token) { MLC::Source::AST::IfExpr.new(condition: condition, then_branch: then_branch, else_branch: else_branch) }
      elsif current.type == :UNLESS && expr_line && current.line == expr_line
        unless_token = consume(:UNLESS)
        condition = parse_logical_or
        # unless = if !condition
        inverted = MLC::Source::AST::UnaryOp.new(op: "!", operand: condition)
        then_branch = wrap_block_like_expr(expr)
        else_branch = MLC::Source::AST::BlockExpr.new(statements: [], result_expr: MLC::Source::AST::UnitLit.new)
        with_origin(unless_token) { MLC::Source::AST::IfExpr.new(condition: inverted, then_branch: then_branch, else_branch: else_branch) }
      else
        expr
      end
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
        raise "Expected lambda expression"
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
      return parse_if_expression unless current.type == :LET

      consume(:LET)
      mutable = false
      if current.type == :MUT
        consume(:MUT)
        mutable = true
      end

      name_token = consume(:IDENTIFIER)
      name = name_token.value

      # Optional type annotation: let x: Type = value
      type_annotation = nil
      if current.type == :COLON
        consume(:COLON)
        type_annotation = parse_type
      end

      consume(:EQUAL)
      value = parse_if_expression

      unless current.type == :SEMICOLON
        body = parse_expression
        return with_origin(name_token) { MLC::Source::AST::Let.new(name: name, value: value, body: body, mutable: mutable, type: type_annotation) }
      end

      consume(:SEMICOLON)
      statements = [with_origin(name_token) { MLC::Source::AST::VariableDecl.new(name: name, value: value, mutable: mutable, type: type_annotation) }]
      block = parse_statement_sequence(statements)
      ensure_block_has_result(block, require_value: false)
      block
    end

    def parse_let_statement
      # Parse: let x = value or let mut x = value or let x: Type = value
      let_token = consume(:LET)
      mutable = false
      if current.type == :MUT
        consume(:MUT)
        mutable = true
      end

      name_token = consume(:IDENTIFIER)
      name = name_token.value

      # Optional type annotation: let x: Type = value
      type_annotation = nil
      if current.type == :COLON
        consume(:COLON)
        type_annotation = parse_type
      end

      consume(:EQUAL)

      # Value can be any expression including do-blocks
      value = if current.type == :DO
        parse_do_expression
      else
        parse_if_expression
      end

      # In do-blocks, let is a statement that doesn't need 'in'
      # It returns a special LetStmt expression
      with_origin(let_token) { MLC::Source::AST::Let.new(name: name, value: value, body: nil, mutable: mutable, type: type_annotation) }
    end

    def parse_logical_and
      left = parse_equality

      while current.type == :OPERATOR && current.value == "&&"
        token = consume(:OPERATOR)
        right = parse_equality
        node = MLC::Source::AST::BinaryOp.new(op: "&&", left: left, right: right)
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
          # Parse pattern
          pattern = parse_pattern

          # Parse guard (optional: "if condition")
          guard = nil
          if current.type == :IF
            consume(:IF)
            guard = parse_equality
          end

          # Expect =>
          if current.type == :FAT_ARROW
            consume(:FAT_ARROW)
          else
            raise "Expected => in match arm"
          end

          # Parse body
          body = if current.type == :DO
                    wrap_block_like_expr(parse_do_expression)
                  else
                    wrap_block_like_expr(parse_match_arm_body)
                  end

          arms << {pattern: pattern, guard: guard, body: body}

          # Expect comma or closing brace
          if current.type == :COMMA
            consume(:COMMA)
          elsif current.type != :RBRACE
            raise "Expected , or } in match expression"
          end
        end

        consume(:RBRACE)
      else
        # Pipe style: match expr | pattern => body | ... end
        while current.type == :OPERATOR && current.value == "|"
          consume(:OPERATOR)  # consume |

          # Parse pattern
          pattern = parse_pattern

          # Parse guard (optional: "if condition")
          guard = nil
          if current.type == :IF
            consume(:IF)
            guard = parse_equality
          end

          # Expect =>
          if current.type == :FAT_ARROW
            consume(:FAT_ARROW)
          elsif current.type == :OPERATOR && current.value == "=>"
            # Fallback for old lexer compatibility
            consume(:OPERATOR)
          else
            raise "Expected => in match arm"
          end

          # Parse body - stop at next | or end
          body = if current.type == :DO
                    wrap_block_like_expr(parse_do_expression)
                  else
                    wrap_block_like_expr(parse_match_arm_body)
                  end

          arms << {pattern: pattern, guard: guard, body: body}
        end

        # Consume end for pipe-style match
        consume(:END) if current.type == :END
      end

      with_origin(match_token) { MLC::Source::AST::MatchExpr.new(scrutinee: scrutinee, arms: arms) }
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
      # Like parse_logical_or but doesn't consume | for pipe operator
      left = parse_logical_and

      while current.type == :OPERATOR && current.value == "||"
        token = consume(:OPERATOR)
        right = parse_logical_and
        node = MLC::Source::AST::BinaryOp.new(op: "||", left: left, right: right)
        left = attach_origin(node, token)
      end

      left
    end

    def parse_match_scrutinee
      # Parse scrutinee for match expression - special handling to avoid
      # consuming { as record literal when it's actually the start of match arms
      # We parse at comparison level to get most operators but stop before
      # consuming record literals
      left = parse_comparison

      # Don't parse record literal here - that's handled by match arms
      # Just return the identifier/expression
      left
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
        token = consume(current.type)  # Consume PIPE or OPERATOR
        right = parse_comparison
        node = MLC::Source::AST::BinaryOp.new(op: "|>", left: left, right: right)
        left = attach_origin(node, token)
      end

      left
    end

    def parse_postfix
      expr = parse_primary
      expr_line = last_token&.line

      # Handle member access, method calls, and array indexing
      loop do
        case current.type
        when :OPERATOR
          if current.value == "."
            consume(:OPERATOR) # consume '.'
            member_token = consume(:IDENTIFIER)
            member = member_token.value
            member_line = member_token.line

            # Check if it's a method call: obj.method()
            # Only treat LPAREN as method call if it's on the same line as the member name
            if current.type == :LPAREN && current.line == member_line
              consume(:LPAREN)
              args = parse_args
              consume(:RPAREN)
              # Create a call with member access as callee
              member_access = attach_origin(MLC::Source::AST::MemberAccess.new(object: expr, member: member), member_token)
              expr = attach_origin(MLC::Source::AST::Call.new(callee: member_access, args: args), member_token)
            else
              # Just member access: obj.field
              expr = attach_origin(MLC::Source::AST::MemberAccess.new(object: expr, member: member), member_token)
            end
            expr_line = last_token&.line
          else
            break
          end
        when :LBRACKET
          # Array indexing: expr[index]
          lbracket_token = consume(:LBRACKET)
          index = parse_expression
          consume(:RBRACKET)
          expr = attach_origin(MLC::Source::AST::IndexAccess.new(object: expr, index: index), lbracket_token)
          expr_line = last_token&.line
        when :LPAREN
          paren_line = current.line
          break unless expr_line && paren_line == expr_line
          lparen_token = consume(:LPAREN)
          args = parse_args
          consume(:RPAREN)
          expr = attach_origin(MLC::Source::AST::Call.new(callee: expr, args: args), lparen_token)
          expr_line = last_token&.line
        else
          break
        end
      end

      expr
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
      when :FLOAT_LITERAL
        token = consume(:FLOAT_LITERAL)
        value = token.value
        attach_origin(MLC::Source::AST::FloatLit.new(value: value), token)
      when :STRING_LITERAL
        token = consume(:STRING_LITERAL)
        value = token.value
        attach_origin(MLC::Source::AST::StringLit.new(value: value), token)
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
            fields = parse_record_fields
            consume(:RBRACE)
            attach_origin(MLC::Source::AST::RecordLit.new(type_name: name, fields: fields), lbrace_token)
          else
            # Variable reference
            attach_origin(MLC::Source::AST::VarRef.new(name: name), name_token)
          end
        end
      when :LPAREN
        # Could be lambda or grouped expression
        # Lookahead to determine
        if looks_like_lambda?
          parse_lambda
        else
          consume(:LPAREN)
          expr = parse_expression
          consume(:RPAREN)
          expr
        end
      when :LBRACE
        # Anonymous record literal
        lbrace_token = consume(:LBRACE)
        fields = parse_record_fields
        consume(:RBRACE)
        attach_origin(MLC::Source::AST::RecordLit.new(type_name: "record", fields: fields), lbrace_token)
      when :LBRACKET
        parse_array_literal_or_comprehension
      when :UNSAFE
        parse_unsafe_block
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
      # Check for unary operators: !, -, +
      if current.type == :OPERATOR && %w[! - +].include?(current.value)
        token = consume(:OPERATOR)
        op = token.value
        operand = parse_unary  # Right-associative
        attach_origin(MLC::Source::AST::UnaryOp.new(op: op, operand: operand), token)
      else
        parse_postfix
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
        if eof?
          raise "Unexpected EOF in while block, expected 'end'"
        end

        # Parse one statement/expression
        expr = parse_do_statement
        all_items << expr

        # Skip optional semicolon between statements
        if current.type == :SEMICOLON
          consume(:SEMICOLON)
        end
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
          if item.is_a?(MLC::Source::AST::Stmt)
            # Already a statement (VariableDecl, Assignment)
            statements << item
          else
            # Wrap expression as statement
            statements << MLC::Source::AST::ExprStmt.new(expr: item)
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

    end
  end
  end
end
