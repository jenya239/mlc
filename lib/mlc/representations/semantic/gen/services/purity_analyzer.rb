# frozen_string_literal: true

# Delegates to pure analysis module:
# - MLC::Representations::Semantic::Gen::Services::Utils::PurityAnalysis

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # PurityAnalyzer - сервис для анализа чистоты (purity) выражений и констант
          #
          # Отвечает за:
          # - Определение является ли выражение "pure" (без побочных эффектов)
          # - Определение constexpr-совместимости для C++20
          # - Проверка non-literal типов (string, collections)
          #
          # Delegates pure analysis functions to Utils::PurityAnalysis module
          class PurityAnalyzer
            # Проверяет является ли выражение чистым (pure)
            def is_pure_expression(expr)
              Utils::PurityAnalysis.pure_expression?(expr)
            end

            # Проверяет является ли вызов функции чистым
            def is_pure_call?(call_expr)
              Utils::PurityAnalysis.pure_call?(call_expr)
            end

            # Проверяет является ли тип non-literal (не может быть constexpr в C++20)
            def non_literal_type?(type)
              Utils::PurityAnalysis.non_literal_type?(type)
            end

            # Проверяет является ли блок чистым
            def pure_block_expr?(block_expr)
              Utils::PurityAnalysis.pure_block?(block_expr)
            end

            # Проверяет является ли statement чистым
            def pure_statement?(stmt)
              Utils::PurityAnalysis.pure_statement?(stmt)
            end
          end
        end
      end
    end
  end
end
