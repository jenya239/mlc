# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
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
            Snapshot = Struct.new(:types, :initializers, :moved_vars) do
              def [](key)
                types[key]
              end
            end

            def initialize
              @types = {}
              @initializers = {}
              @moved_vars = Set.new # Track moved variables for use-after-move detection
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
              @moved_vars.clear
            end

            # Создать snapshot текущего состояния (для scope management)
            # @return [Snapshot] копия текущего состояния
            def snapshot
              Snapshot.new(@types.dup, @initializers.dup, @moved_vars.dup)
            end

            # Восстановить состояние из snapshot
            # @param snapshot [Snapshot] snapshot состояния
            def restore(snapshot)
              raise ArgumentError, "Cannot restore from nil snapshot" if snapshot.nil?

              if snapshot.respond_to?(:types) && snapshot.respond_to?(:initializers)
                @types = snapshot.types.dup
                @initializers = snapshot.initializers.dup
                @moved_vars = snapshot.respond_to?(:moved_vars) && snapshot.moved_vars ? snapshot.moved_vars.dup : Set.new
              else
                @types = snapshot.dup
                @initializers = {}
                @moved_vars = Set.new
              end
            end

            def update_type(name, type)
              return unless name && type

              @types[name] = type
            end

            def initializer(name)
              @initializers[name]
            end

            # ===========================================
            # Move Semantics Tracking
            # ===========================================

            # Mark a variable as moved (ownership transferred)
            # @param name [String] variable name
            def mark_moved(name)
              @moved_vars.add(name)
            end

            # Check if a variable has been moved
            # @param name [String] variable name
            # @return [Boolean] true if variable was moved
            def moved?(name)
              @moved_vars.include?(name)
            end

            # Reset moved state (for testing or reassignment of mutable vars)
            # @param name [String] variable name
            def reset_moved(name)
              @moved_vars.delete(name)
            end

            # Types that are implicitly copied (no move semantics)
            COPY_TYPES = %w[i8 i16 i32 i64 u8 u16 u32 u64 f32 f64 bool char].freeze

            # Smart pointer wrappers that are reference-counted (clone, not move)
            RC_TYPES = %w[Shared Weak].freeze

            # Check if type is Copy (i.e. implicitly copied, no ownership transfer)
            # @param type [SemanticIR::Type] the type to check
            # @return [Boolean] true if type is Copy
            def self.copy_type?(type)
              return true if type.nil?
              return true if type.is_a?(MLC::SemanticIR::TypeVariable)
              return true if type.is_a?(MLC::SemanticIR::UnitType)
              return true if type.is_a?(MLC::SemanticIR::ErrorType)

              # Reference-counted smart pointers are not Copy, but they are Clone (no move)
              if type.is_a?(MLC::SemanticIR::GenericType)
                base_name = type.base_type&.name
                return true if RC_TYPES.include?(base_name)
                return false
              end

              name = type.name
              return true if name && COPY_TYPES.include?(name)
              return true if name && %w[void unit].include?(name)

              false
            end

            # Check if type requires move on variable binding (let b = a)
            # All non-Copy types are moved on binding.
            # @param type [SemanticIR::Type] the type to check
            # @return [Boolean] true if binding moves ownership
            def self.move_on_bind?(type)
              !copy_type?(type)
            end

            # Check if type requires explicit move on function argument passing.
            # Only Owned<T> requires explicit move when passed to a function.
            # Other non-Copy types (Array, String, Records) are passed by implicit copy
            # in C++ (the C++ compiler handles copy/move optimization).
            # @param type [SemanticIR::Type] the type to check
            # @return [Boolean] true if passing as argument moves ownership
            def self.move_on_pass?(type)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)

              type.base_type&.name == "Owned"
            end

            # Backwards compatibility: move_semantics? checks move_on_bind?
            def self.move_semantics?(type)
              move_on_bind?(type)
            end

            class << self
              alias has_move_semantics? move_semantics?
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
    end
  end
end
