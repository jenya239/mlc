# frozen_string_literal: true

module MLC
  module Services
    # PurityAnalyzer - сервис для анализа чистоты (purity) выражений и констант
    #
    # Отвечает за:
    # - Определение является ли выражение "pure" (без побочных эффектов)
    # - Определение constexpr-совместимости для C++20
    # - Проверка non-literal типов (string, collections)
    #
    # Используется в:
    # - EffectAnalyzer для определения constexpr
    # - Генерации C++ кода с правильными effect markers
    #
    # Использование:
    #   analyzer = PurityAnalyzer.new
    #   if analyzer.is_pure_expression(expr)
    #     # expression can be constexpr
    #   end
    class PurityAnalyzer
      # Проверяет является ли выражение чистым (pure)
      # @param expr [HighIR::*] HighIR выражение
      # @return [Boolean] true если выражение чистое
      def is_pure_expression(expr)
        case expr
        when MLC::HighIR::LiteralExpr, MLC::HighIR::VarExpr
          true
        when MLC::HighIR::BinaryExpr
          is_pure_expression(expr.left) && is_pure_expression(expr.right)
        when MLC::HighIR::UnaryExpr
          is_pure_expression(expr.operand)
        when MLC::HighIR::CallExpr
          is_pure_call?(expr)
        when MLC::HighIR::MemberExpr
          is_pure_expression(expr.object)
        when MLC::HighIR::RecordExpr
          expr.fields.values.all? { |field| is_pure_expression(field) }
        when MLC::HighIR::BlockExpr
          pure_block_expr?(expr)
        else
          false
        end
      end

      # Проверяет является ли вызов функции чистым
      # @param call_expr [HighIR::CallExpr] вызов функции
      # @return [Boolean] true если вызов чистый
      def is_pure_call?(call_expr)
        # Check if function name indicates non-pure operation
        if call_expr.callee.is_a?(MLC::HighIR::VarExpr)
          func_name = call_expr.callee.name
          # IO functions are not constexpr-compatible
          return false if func_name =~ /^(println|print|read|write|open|close)/
          # Stdlib functions that return non-literal types
          return false if func_name =~ /^(to_string|format|String)/
        end

        # Check if return type is non-literal (String, collections, etc.)
        return false if non_literal_type?(call_expr.type)

        # Recursively check arguments
        call_expr.args.all? { |arg| is_pure_expression(arg) }
      end

      # Проверяет является ли тип non-literal (не может быть constexpr в C++20)
      # @param type [HighIR::Type] тип для проверки
      # @return [Boolean] true если тип non-literal
      def non_literal_type?(type)
        return false if type.nil?
        return false unless type.respond_to?(:name)

        # String and collection types are not literal types in C++20
        type.name == "string" ||
          type.name == "String" ||
          type.name =~ /^(Array|Vec|HashMap|HashSet)$/
      end

      # Проверяет является ли блок чистым
      # @param block_expr [HighIR::BlockExpr] блок выражений
      # @return [Boolean] true если блок чистый
      def pure_block_expr?(block_expr)
        statements_pure = block_expr.statements.all? { |stmt| pure_statement?(stmt) }
        result_pure = block_expr.result.nil? || is_pure_expression(block_expr.result)
        statements_pure && result_pure
      end

      # Проверяет является ли statement чистым
      # @param stmt [HighIR::*Stmt] statement для проверки
      # @return [Boolean] true если statement чистый
      def pure_statement?(stmt)
        case stmt
        when MLC::HighIR::VariableDeclStmt
          !stmt.mutable && is_pure_expression(stmt.value)
        when MLC::HighIR::ExprStatement
          is_pure_expression(stmt.expression)
        when MLC::HighIR::Block
          stmt.stmts.all? { |inner| pure_statement?(inner) }
        when MLC::HighIR::MatchStmt
          is_pure_expression(stmt.scrutinee) &&
            stmt.arms.all? do |arm|
              (arm[:guard].nil? || is_pure_expression(arm[:guard])) &&
                pure_block_expr?(arm[:body])
            end
        else
          false
        end
      end
    end
  end
end
