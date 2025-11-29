# frozen_string_literal: true

module MLC
  module Source
  module Parser
    # StatementParser
    # Statement parsing - variable declarations, assignments, blocks
    # Auto-extracted from parser.rb during refactoring
    module StatementParser
    def ensure_block_has_result(block, require_value: true)
      return unless require_value
      return if block.stmts.last.is_a?(MLC::Source::AST::ExprStmt)

      raise "Block must end with an expression"
    end

    def parse_assignment_statement
      target_token = consume(:IDENTIFIER)
      target_name = target_token.value
      consume(:EQUAL)
      value = parse_expression
      consume(:SEMICOLON) if current.type == :SEMICOLON

      target = attach_origin(MLC::Source::AST::VarRef.new(name: target_name), target_token)
      with_origin(target_token) { MLC::Source::AST::Assignment.new(target: target, value: value) }
    end

    def parse_block_expression
      lbrace_token = consume(:LBRACE)
      statements = []

      until current.type == :RBRACE
        statements << parse_statement
      end

      consume(:RBRACE)
      with_origin(lbrace_token) { MLC::Source::AST::Block.new(stmts: statements) }
    end

    def parse_return_statement
      return_token = consume(:RETURN)
      expr = nil
      unless current.type == :SEMICOLON || current.type == :RBRACE || current.type == :EOF
        expr = parse_expression
      end
      consume(:SEMICOLON) if current.type == :SEMICOLON

      with_origin(return_token) { MLC::Source::AST::Return.new(expr: expr) }
    end

    def parse_statement
      case current.type
      when :LET
        parse_variable_decl_statement
      when :RETURN
        parse_return_statement
      when :BREAK
        break_token = consume(:BREAK)
        consume(:SEMICOLON) if current.type == :SEMICOLON
        attach_origin(MLC::Source::AST::Break.new, break_token)
      when :CONTINUE
        continue_token = consume(:CONTINUE)
        consume(:SEMICOLON) if current.type == :SEMICOLON
        attach_origin(MLC::Source::AST::Continue.new, continue_token)
      when :IDENTIFIER
        if peek && peek.type == :EQUAL
          parse_assignment_statement
        else
          expr = parse_expression
          consume(:SEMICOLON) if current.type == :SEMICOLON
          attach_origin(MLC::Source::AST::ExprStmt.new(expr: expr), expr.origin)
        end
      when :LBRACE
        parse_block_expression
      else
        expr = parse_expression
        consume(:SEMICOLON) if current.type == :SEMICOLON
        attach_origin(MLC::Source::AST::ExprStmt.new(expr: expr), expr.origin)
      end
    end

    def parse_statement_sequence(statements)
      loop do
        break if current.type == :EOF || current.type == :RBRACE

        stmt = parse_statement
        statements << stmt

        if current.type == :SEMICOLON
          consume(:SEMICOLON)
          next
        end
      end

      block = MLC::Source::AST::Block.new(stmts: statements)
      first_origin = statements.first&.origin
      attach_origin(block, first_origin)
    end

    def parse_variable_decl_statement
      let_token = consume(:LET)
      mutable = false
      if current.type == :MUT
        consume(:MUT)
        mutable = true
      end

      # Check for destructuring patterns: let (a, b) = ... or let { x, y } = ...
      if current.type == :LPAREN
        return parse_tuple_destructuring(let_token, mutable)
      elsif current.type == :LBRACE
        return parse_record_destructuring(let_token, mutable)
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
      # Use parse_expression_in_block to avoid stealing END from parent block
      value = parse_expression_in_block
      consume(:SEMICOLON) if current.type == :SEMICOLON

      with_origin(name_token) { MLC::Source::AST::VariableDecl.new(name: name, value: value, mutable: mutable, type: type_annotation) }
    end

    # Parse tuple destructuring: let (a, b) = expr
    def parse_tuple_destructuring(let_token, mutable)
      pattern = parse_pattern  # Uses PatternParser which handles (a, b) as tuple pattern
      consume(:EQUAL)
      value = parse_expression_in_block
      consume(:SEMICOLON) if current.type == :SEMICOLON

      with_origin(let_token) { MLC::Source::AST::DestructuringDecl.new(pattern: pattern, value: value, mutable: mutable) }
    end

    # Parse record destructuring: let { x, y } = expr
    # Supports rest pattern: let { x, ...rest } = expr
    def parse_record_destructuring(let_token, mutable)
      lbrace_token = consume(:LBRACE)
      bindings = []
      rest_binding = nil

      while current.type != :RBRACE
        if current.type == :SPREAD
          # Rest pattern: ...rest
          consume(:SPREAD)
          rest_binding = consume(:IDENTIFIER).value
          break  # Rest must be last
        else
          bindings << consume(:IDENTIFIER).value
        end

        break unless current.type == :COMMA
        consume(:COMMA)
      end

      consume(:RBRACE)

      pattern = with_origin(lbrace_token) do
        MLC::Source::AST::Pattern.new(
          kind: :record,
          data: { bindings: bindings, rest: rest_binding }
        )
      end

      consume(:EQUAL)
      value = parse_expression_in_block
      consume(:SEMICOLON) if current.type == :SEMICOLON

      with_origin(let_token) { MLC::Source::AST::DestructuringDecl.new(pattern: pattern, value: value, mutable: mutable) }
    end

    end
  end
  end
end
