# frozen_string_literal: true

module MLC
  class IRGen
    # PatternMatchingTransformer
    # Pattern matching AST → HighIR transformation
    # Phase 18-D: Extracted from IRGen main class
    #
    # Responsibilities:
    # - Transform match expressions to HighIR (expression form or statement form)
    # - Transform match arms with pattern binding
    # - Transform patterns (constructor, var, wildcard, literal, regex)
    # - Bind pattern variables to scrutinee types
    # - Determine if match can be optimized to statement form
    #
    # Dependencies (from IRGen):
    # - @var_type_registry: For binding pattern variables
    # - @rule_engine: For applying match expression rules
    # - @match_analyzer: For match analysis
    # - @type_unification_service: For getting constructor type info
    # - @expression_visitor: For transforming arm bodies (expressions) (Phase 25-B)
    # - transform_statement_block: For transforming arm bodies (statements)
    module PatternMatchingTransformer
      # Entry point: Transform match expression (chooses expression or statement form)
      def transform_match_expr(match_expr)
        scrutinee_ir = @expression_visitor.visit(match_expr.scrutinee)

        if match_stmt_applicable?(match_expr)
          return transform_match_expr_to_statement(match_expr, scrutinee_ir)
        end

        transform_match_expr_core(match_expr, scrutinee_ir)
      end

      # Transform match expression to HighIR::MatchExpr (expression form)
      # Uses rule engine to apply match expression rules
      def transform_match_expr_core(match_expr, scrutinee_ir = nil)
        scrutinee_ir ||= @expression_visitor.visit(match_expr.scrutinee)

        result = @rule_engine.apply(
          :core_ir_match_expr,
          match_expr,
          context: {
            scrutinee: scrutinee_ir,
            match_analyzer: @match_analyzer,
            transform_arm: method(:transform_match_arm)
          }
        )

        unless result.is_a?(HighIR::MatchExpr)
          raise "Match rule must produce HighIR::MatchExpr, got #{result.class}"
        end

        result
      end

      # Transform match expression to statement form (for side effects only)
      # Wraps MatchStmt in a BlockExpr with unit result
      def transform_match_expr_to_statement(match_expr, scrutinee_ir)
        arms = match_expr.arms.map do |arm|
          transform_match_arm_statement(scrutinee_ir.type, arm)
        end

        match_stmt = HighIR::Builder.match_stmt(scrutinee_ir, arms, origin: match_expr.origin)
        unit_literal = HighIR::Builder.unit_literal(origin: match_expr.origin)
        HighIR::Builder.block_expr([match_stmt], unit_literal, unit_literal.type, origin: match_expr.origin)
      end

      # Transform match arm for expression form
      # Transforms pattern, binds variables, transforms guard and body expression
      def transform_match_arm(scrutinee_type, arm)
        saved_var_types = @var_type_registry.snapshot
        pattern = transform_pattern(arm[:pattern])
        bind_pattern_variables(pattern, scrutinee_type)
        guard = arm[:guard] ? @expression_visitor.visit(arm[:guard]) : nil
        body = @expression_visitor.visit(arm[:body])
        {pattern: pattern, guard: guard, body: body}
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      # Transform match arm for statement form
      # Transforms pattern, binds variables, transforms guard and body statements
      def transform_match_arm_statement(scrutinee_type, arm)
        saved_var_types = @var_type_registry.snapshot
        pattern = transform_pattern(arm[:pattern])
        bind_pattern_variables(pattern, scrutinee_type)
        guard_ir = arm[:guard] ? @expression_visitor.visit(arm[:guard]) : nil
        body_ir = transform_statement_block(arm[:body])
        {pattern: pattern, guard: guard_ir, body: body_ir}
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      # Transform AST pattern to IR pattern hash
      # Converts pattern kinds: wildcard, literal, constructor, var, regex
      def transform_pattern(pattern)
        case pattern.kind
        when :wildcard
          {kind: :wildcard}
        when :literal
          {kind: :literal, value: pattern.data[:value]}
        when :constructor
          {kind: :constructor, name: pattern.data[:name], fields: pattern.data[:fields]}
        when :var
          {kind: :var, name: pattern.data[:name]}
        when :regex
          {
            kind: :regex,
            pattern: pattern.data[:pattern],
            flags: pattern.data[:flags],
            bindings: pattern.data[:bindings] || []
          }
        else
          raise "Unknown pattern kind: #{pattern.kind}"
        end
      end

      # Bind pattern variables to types in var_type_registry
      # Handles constructor field destructuring, var binding, and regex captures
      def bind_pattern_variables(pattern, scrutinee_type)
        case pattern[:kind]
        when :constructor
          info = @type_unification_service.constructor_info_for(pattern[:name], scrutinee_type)
          field_types = info ? info.param_types : []
          bindings = []

          Array(pattern[:fields]).each_with_index do |field_name, idx|
            next if field_name.nil? || field_name == "_"
            field_type = field_types[idx] || HighIR::Builder.primitive_type("auto")
            @var_type_registry.set(field_name, field_type)
            bindings << field_name
          end

          pattern[:bindings] = bindings unless bindings.empty?
        when :var
          name = pattern[:name]
          @var_type_registry.set(name, scrutinee_type) if name && name != "_"
        when :regex
          Array(pattern[:bindings]).each do |binding|
            next if binding.nil? || binding == "_"
            @var_type_registry.set(binding, HighIR::Builder.primitive_type("string"))
          end
        end
      end

      # Check if match expression can be optimized to statement form
      # Requires: all arms have unit bodies, no guards, safe patterns only
      def match_stmt_applicable?(match_expr)
        match_expr.arms.all? do |arm|
          unit_branch_ast?(arm[:body]) &&
            arm[:guard].nil? &&
            match_stmt_safe_pattern?(arm[:pattern])
        end
      end

      # Check if pattern is safe for statement form (no regex patterns)
      def match_stmt_safe_pattern?(pattern)
        kind = pattern.kind
        return false if kind == :regex
        %i[constructor wildcard var].include?(kind)
      end

      # Check if AST node is a unit branch (block with unit result)
      def unit_branch_ast?(node)
        node.is_a?(AST::BlockExpr) && node.result_expr.is_a?(AST::UnitLit)
      end
    end
  end
end
