# frozen_string_literal: true

module MLC
  module Services
    # GenericCallResolverService - сервис для разрешения generic function calls
    # Заменяет прямое использование @generic_call_resolver.instantiate из правил
    #
    # Использование:
    #   resolver = context[:generic_call_resolver]
    #   inst = resolver.instantiate(function_info, args, name: func_name)
    #   inst.ret_type      # конкретный тип возврата
    #   inst.param_types   # конкретные типы параметров
    class GenericCallResolverService
      def initialize(base_transformer)
        @transformer = base_transformer
      end

      # Разрешить generic function call с конкретными аргументами
      #
      # @param info [FunctionInfo] информация о generic функции
      # @param args [Array] аргументы вызова
      # @param name [String] имя функции для error messages
      # @return [TypeConstraintSolver::Instantiation] результат с конкретными типами
      def instantiate(info, args, name:)
        # Делегируем к TypeConstraintSolver через @generic_call_resolver
        @transformer.generic_call_resolver.instantiate(info, args, name: name)
      end

      # Вспомогательный метод для проверки, является ли функция generic
      #
      # @param info [FunctionInfo] информация о функции
      # @return [Boolean] true если функция имеет type parameters
      def generic_function?(info)
        info.type_params && !info.type_params.empty?
      end

      # Вспомогательный метод для извлечения type map из instantiation
      #
      # @param instantiation [TypeConstraintSolver::Instantiation] результат resolve
      # @return [Hash] mapping type parameter names to concrete types
      def type_map(instantiation)
        instantiation.type_map
      end

      # Получить конкретные типы параметров после instantiation
      #
      # @param instantiation [TypeConstraintSolver::Instantiation] результат resolve
      # @return [Array] конкретные типы параметров
      def param_types(instantiation)
        instantiation.param_types
      end

      # Получить конкретный return type после instantiation
      #
      # @param instantiation [TypeConstraintSolver::Instantiation] результат resolve
      # @return [Type] конкретный return type
      def return_type(instantiation)
        instantiation.ret_type
      end
    end
  end
end
