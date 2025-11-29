# frozen_string_literal: true

# Delegates to pure analysis module:
# - MLC::Backends::Cpp::Services::Utils::ComplexityAnalysis

module MLC
  module Backends
    module Cpp
      # BlockComplexityAnalyzer - анализ сложности block expressions
      # Определяет можно ли использовать простую стратегию или нужен IIFE
      # Delegates pure analysis functions to ComplexityAnalysis module
      class BlockComplexityAnalyzer
        attr_reader :block_expr, :statement_count, :has_control_flow, :has_early_return

        # Пороги для определения "простого" блока
        SIMPLE_STATEMENT_THRESHOLD = 3

        def initialize(block_expr)
          @block_expr = block_expr
          @statement_count = block_expr.statements.size
          @has_control_flow = Services::Utils::ComplexityAnalysis.block_has_control_flow?(block_expr)
          @has_early_return = false  # В MLC нет early return в блоках (пока)
        end

        # Простой блок: ≤3 statements, без control flow
        def simple?
          @statement_count <= SIMPLE_STATEMENT_THRESHOLD &&
            !@has_control_flow &&
            Services::Utils::ComplexityAnalysis.all_statements_simple?(@block_expr.statements)
        end

        # Сложный блок нуждается в IIFE
        def complex?
          !simple?
        end

        # Блок с единственным expression (можно inline)
        def trivial?
          @statement_count == 0
        end

        # Есть ли вложенные if/match/loop
        def has_nested_constructs?
          @has_control_flow
        end

        # Оценка стоимости генерации (для метрик)
        def complexity_score
          score = @statement_count
          score += 5 if @has_control_flow
          score += 3 if @has_early_return
          score += 2 if Services::Utils::ComplexityAnalysis.has_nested_blocks?(@block_expr.statements)
          score
        end
      end

      # Анализ if expressions
      # Delegates pure analysis functions to ComplexityAnalysis module
      class IfComplexityAnalyzer
        attr_reader :if_expr

        def initialize(if_expr)
          @if_expr = if_expr
        end

        # Можно ли использовать ternary (? :)
        def simple_ternary_candidate?
          Services::Utils::ComplexityAnalysis.simple_branch?(@if_expr.then_branch) &&
            Services::Utils::ComplexityAnalysis.simple_branch?(@if_expr.else_branch) &&
            !Services::Utils::ComplexityAnalysis.branches_have_nested_control_flow?(
              [@if_expr.then_branch, @if_expr.else_branch]
            )
        end

        # Типы веток одинаковые (не нужен variant)
        def same_types?
          return false unless @if_expr.then_branch && @if_expr.else_branch

          # Проверка через type system (если доступен)
          then_type = @if_expr.then_branch.type rescue nil
          else_type = @if_expr.else_branch.type rescue nil

          return true unless then_type && else_type
          then_type == else_type
        end
      end

      # Анализ match expressions
      # Delegates pure analysis functions to ComplexityAnalysis module
      class MatchComplexityAnalyzer
        attr_reader :match_expr

        def initialize(match_expr)
          @match_expr = match_expr
        end

        def arm_count
          @match_expr.arms.size
        end

        def has_regex?
          @match_expr.arms.any? do |arm|
            Services::Utils::ComplexityAnalysis.regex_pattern?(arm[:pattern])
          end
        end

        def pure_adt_match?
          !has_regex? && all_constructor_patterns?
        end

        def needs_named_visitor?(threshold = 5)
          arm_count > threshold && pure_adt_match?
        end

        private

        def all_constructor_patterns?
          @match_expr.arms.all? do |arm|
            Services::Utils::ComplexityAnalysis.constructor_pattern?(arm[:pattern])
          end
        end
      end
    end
  end
end
