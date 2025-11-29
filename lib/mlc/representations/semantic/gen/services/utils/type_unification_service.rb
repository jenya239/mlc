# frozen_string_literal: true

# All classes autoloaded by Zeitwerk on-demand:
# - MLC::Registries::FunctionSignature (used at line 172)

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # TypeUnificationService - Type unification and substitution for generic type inference
          # Phase 18-B: Extracted during the legacy IR refactor
          #
          # Responsibilities:
          # - Unify type patterns with actual types (for generic type inference)
          # - Check type equivalence (structural equality)
          # - Apply type variable substitutions recursively
          # - Compute generic type substitutions from scrutinee types
          # - Resolve constructor info with generic substitutions
          #
          # Dependencies:
          # - type_checker: TypeChecker (for type_name helper)
          # - sum_type_constructors: Hash (for constructor lookup)
          class TypeUnificationService
            def initialize(type_checker:, sum_type_constructors:)
              @type_checker = type_checker
              @sum_type_constructors = sum_type_constructors
            end

            # Unify a type pattern with an actual type, collecting type variable substitutions
            # Returns true if unification succeeds, false otherwise
            # Mutates type_map to record discovered substitutions
            # Delegates to pure Unify module internally
            #
            # @param pattern [SemanticIR::Type] The pattern type (may contain type variables)
            # @param actual [SemanticIR::Type] The actual type to unify against
            # @param type_map [Hash] Mutable map from type variable names to concrete types
            # @param context [Hash] Context for error reporting (currently unused)
            # @return [Boolean] Whether unification succeeded
            def unify_type(pattern, actual, type_map, context:)
              result = MLC::Common::Typing::Unify.unify(pattern, actual, type_map)
              if result.success?
                type_map.merge!(result.substitutions)
                true
              else
                false
              end
            end

            # Check structural type equivalence (deep equality check)
            # Delegates to pure Equivalence module
            #
            # @param left [SemanticIR::Type] First type
            # @param right [SemanticIR::Type] Second type
            # @return [Boolean] Whether the types are equivalent
            def type_equivalent?(left, right)
              MLC::Common::Typing::Equivalence.equivalent?(left, right)
            end

            # Apply type variable substitutions recursively to a type
            # Delegates to pure Substitution module
            #
            # @param type [SemanticIR::Type] The type to apply substitutions to
            # @param substitutions [Hash<String, SemanticIR::Type>] Map from type variable names to concrete types
            # @return [SemanticIR::Type] The type with substitutions applied
            def apply_type_substitutions(type, substitutions)
              MLC::Common::Typing::Substitution.apply(type, substitutions)
            end

            # Compute generic type variable substitutions from scrutinee type
            # Delegates to pure Generics module
            #
            # @param info [FunctionSignature] The constructor function info with type parameters
            # @param scrutinee_type [SemanticIR::Type] The concrete type being matched on
            # @return [Hash<String, SemanticIR::Type>] Map from type parameter names to concrete types
            def generic_substitutions(info, scrutinee_type)
              MLC::Common::Typing::Generics.infer_substitutions(
                info.type_params,
                info.ret_type,
                scrutinee_type
              )
            end

            # Get constructor info with generic type substitutions applied
            # Returns specialized constructor info for a concrete scrutinee type
            #
            # @param name [String] The constructor name
            # @param scrutinee_type [SemanticIR::Type] The concrete type being matched on
            # @return [FunctionSignature, nil] Specialized constructor info, or nil if not found
            def constructor_info_for(name, scrutinee_type)
              info = @sum_type_constructors[name]

              # Handle stdlib Option<T> patterns (Some/None)
              # These are not registered as user-defined sum types, but map to std::optional<T>
              unless info
                stdlib_info = stdlib_option_constructor_info(name, scrutinee_type)
                return stdlib_info if stdlib_info
              end

              return unless info

              substitutions = generic_substitutions(info, scrutinee_type)
              return info if substitutions.empty?

              substituted_params = info.param_types.map do |type|
                apply_type_substitutions(type, substitutions)
              end
              substituted_ret_type = apply_type_substitutions(info.ret_type, substitutions)

              MLC::Registries::FunctionSignature.new(info.name, substituted_params, substituted_ret_type, info.type_params)
            end

            private

            # Handle stdlib Option<T> constructor patterns (Some/None)
            # @param name [String] The constructor name
            # @param scrutinee_type [SemanticIR::Type] The concrete type being matched on
            # @return [FunctionSignature, nil] Constructor info for stdlib Option, or nil
            def stdlib_option_constructor_info(name, scrutinee_type)
              return unless %w[Some None].include?(name)

              # Extract inner type from Option<T>
              inner_type = extract_option_inner_type(scrutinee_type)
              return unless inner_type

              case name
              when 'Some'
                # Some(value) has one parameter of type T
                MLC::Registries::FunctionSignature.new('Some', [inner_type], scrutinee_type, [])
              when 'None'
                # None has no parameters
                MLC::Registries::FunctionSignature.new('None', [], scrutinee_type, [])
              end
            end

            # Extract inner type T from Option<T> type
            # @param type [SemanticIR::Type] The scrutinee type
            # @return [SemanticIR::Type, nil] The inner type T, or nil if not Option<T>
            def extract_option_inner_type(type)
              return unless type.is_a?(SemanticIR::GenericType)

              base_name = @type_checker.type_name(type.base_type)
              return unless base_name == 'Option'

              # Option<T> has exactly one type argument
              return unless type.type_args.size == 1

              type.type_args.first
            end

          end
        end
      end
    end
  end
end
