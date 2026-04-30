# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        module Builders
          # Template/generic type handling for C++20
          module TemplateBuilder
            module_function

            # Build template signature for generics
            # Returns [params_string, params_suffix]
            # Example: ["typename T, typename U", "\nrequires Comparable<T>\n"]
            def build_template_signature(type_params)
              params = type_params.map { |tp| "typename #{tp.name}" }.join(", ")
              requires_clause = build_requires_clause(type_params)
              params_suffix = requires_clause.empty? ? "\n" : "\nrequires #{requires_clause}\n"
              [params, params_suffix]
            end

            # Build C++20 requires clause for type constraints
            # Example: "Comparable<T> && Hashable<U>"
            def build_requires_clause(type_params)
              clauses = []
              type_params.each do |tp|
                traits = tp.respond_to?(:trait_bounds) ? tp.trait_bounds : []
                traits = [tp.constraint].compact if traits.empty? && tp.constraint && !tp.constraint.empty?
                traits.each do |trait_name|
                  clauses << "#{trait_name}<#{tp.name}>"
                end
              end
              clauses.compact.join(" && ")
            end

            # Build lambda/function type using std::function
            # Example: "std::function<int(string, bool)>"
            def build_lambda_type(params, return_type)
              "std::function<#{return_type}(#{params.join(', ')})>"
            end
          end
        end
      end
    end
  end
end
