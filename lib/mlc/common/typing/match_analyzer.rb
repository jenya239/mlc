# frozen_string_literal: true

module MLC
  module Common
    module Typing
      class MatchAnalyzer
      Analysis = Struct.new(:arms, :result_type)

      def initialize(ensure_compatible_type:, type_registry: nil, check_exhaustiveness: true)
        @ensure_compatible_type = ensure_compatible_type
        @type_registry = type_registry
        @check_exhaustiveness = check_exhaustiveness
      end

      def analyze(scrutinee_type:, arms:, transform_arm:)
        transformed_arms = arms.map do |arm|
          transform_arm.call(scrutinee_type, arm)
        end

        if transformed_arms.empty?
          raise ArgumentError, "match expression requires at least one arm"
        end

        first_body = transformed_arms.first[:body]
        result_type = first_body&.type
        raise ArgumentError, "match arm body must have a type" unless result_type

        transformed_arms.each_with_index do |arm, index|
          body = arm[:body]
          raise ArgumentError, "match arm body must be present" unless body

          body_type = body.type
          raise ArgumentError, "match arm body must have a type" unless body_type

          @ensure_compatible_type.call(body_type, result_type, "match arm #{index + 1}")
        end

        # Check exhaustiveness if enabled and type registry available
        if @check_exhaustiveness && @type_registry
          check_exhaustiveness(scrutinee_type, arms)
        end

        Analysis.new(transformed_arms, result_type)
      end

      private

      # Check if all constructors of a sum type are covered by patterns
      def check_exhaustiveness(scrutinee_type, arms)
        # Extract base type name from scrutinee_type
        type_name = extract_type_name(scrutinee_type)
        return unless type_name

        # Lookup type in registry
        type_info = @type_registry.lookup(type_name)
        return unless type_info&.sum?

        # Get all variants from sum type
        all_variants = type_info.variants
        return unless all_variants && !all_variants.empty?

        # Check if there's a wildcard pattern
        has_wildcard = arms.any? { |arm| arm[:pattern]&.kind == :wildcard }
        return if has_wildcard

        # Collect all covered constructors
        covered_constructors = Set.new
        arms.each do |arm|
          pattern = arm[:pattern]
          next unless pattern

          # Extract covered constructors from pattern (handles or-patterns recursively)
          extract_constructors(pattern, covered_constructors)
        end

        # Get all required constructors
        required_constructors = all_variants.map { |v| v[:name] }.to_set

        # Check if all constructors are covered
        missing = required_constructors - covered_constructors

        if missing.any?
          missing_list = missing.to_a.sort.join(", ")
          raise MLC::CompileError, "Non-exhaustive match: missing patterns for #{missing_list}"
        end
      end

      # Recursively extract all covered constructors from a pattern
      def extract_constructors(pattern, covered_set)
        case pattern.kind
        when :constructor
          covered_set << pattern.data[:name]
        when :or
          # Or-pattern: recursively extract from all alternatives
          alternatives = pattern.data[:alternatives] || []
          alternatives.each do |alt|
            extract_constructors(alt, covered_set)
          end
        end
      end

      # Extract type name from various type representations
      def extract_type_name(type)
        case type
        when String
          # "Option<T>" -> "Option"
          type.split("<").first&.split("::").last
        when MLC::SemanticIR::GenericType
          extract_type_name(type.base_type)
        else
          type.respond_to?(:name) ? type.name : nil
        end
      end
      end
    end
  end
end
