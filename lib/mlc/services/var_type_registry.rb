# frozen_string_literal: true

module MLC
  module Services
    # VarTypeRegistry - сервис для управления типами переменных во время трансформации
    #
    # Отвечает за:
    # - Хранение типов переменных (let bindings, function params, loop vars, pattern bindings)
    # - Scope management (snapshot/restore для вложенных scopes)
    # - Type lookup для variable references
    #
    # Использование:
    #   var_types = VarTypeRegistry.new
    #   var_types.set("x", i32_type)
    #   type = var_types.get("x")
    #
    #   # Scope management:
    #   snapshot = var_types.snapshot
    #   var_types.set("local", type)
    #   var_types.restore(snapshot)  # "local" больше не видна
    class VarTypeRegistry
      Snapshot = Struct.new(:types, :initializers) do
        def [](key)
          types[key]
        end
      end

      def initialize
        @types = {}
        @initializers = {}
      end

      # Установить тип переменной
      # @param name [String] имя переменной
      # @param type [SemanticIR::Type] тип переменной
      def set(name, type, initializer: nil)
        @types[name] = type
        @initializers[name] = initializer if initializer
      end

      # Получить тип переменной
      # @param name [String] имя переменной
      # @return [SemanticIR::Type, nil] тип или nil если не найдена
      def get(name)
        @types[name]
      end

      # Проверить наличие переменной
      # @param name [String] имя переменной
      # @return [Boolean] true если переменная существует
      def has?(name)
        @types.key?(name)
      end

      # Удалить переменную
      # @param name [String] имя переменной
      # @return [SemanticIR::Type, nil] удалённый тип
      def delete(name)
        @initializers.delete(name)
        @types.delete(name)
      end

      # Получить все имена переменных (для error messages)
      # @return [Array<String>] список имён переменных
      def keys
        @types.keys
      end

      # Очистить все переменные
      def clear
        @types.clear
      end

      # Создать snapshot текущего состояния (для scope management)
      # @return [Hash] копия текущего состояния
      def snapshot
        Snapshot.new(@types.dup, @initializers.dup)
      end

      # Восстановить состояние из snapshot
      # @param snapshot [Hash] snapshot состояния
      def restore(snapshot)
        raise ArgumentError, "Cannot restore from nil snapshot" if snapshot.nil?
        if snapshot.respond_to?(:types) && snapshot.respond_to?(:initializers)
          @types = snapshot.types.dup
          @initializers = snapshot.initializers.dup
        else
          @types = snapshot.dup
          @initializers = {}
        end
      end

      def update_type(name, type)
        return unless name && type
        @types[name] = type
      end

      def initializer(name)
        @initializers[name]
      end

      # Количество переменных
      # @return [Integer] количество переменных
      def size
        @types.size
      end

      # Проверка на пустоту
      # @return [Boolean] true если нет переменных
      def empty?
        @types.empty?
      end

      # Для отладки
      def inspect
        "#<VarTypeRegistry: #{@types.keys.join(', ')}>"
      end

      def to_s
        inspect
      end

    end
  end
end
