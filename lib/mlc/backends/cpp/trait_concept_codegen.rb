# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module TraitConceptCodegen
        module_function

        def concept_trait?(trait_info)
          !(trait_info.associated_types&.any?) && Array(trait_info.type_params).map(&:to_s).empty?
        end

        def ast_type_to_cpp(ast_type)
          TraitCppTypes.ast_type_to_cpp(ast_type)
        end

        def trait_dispatch_name(trait_name, method_name)
          "#{trait_name}_#{method_name}"
        end

        def self_receiver?(method_entry)
          first = Array(method_entry[:params]).first
          return false unless first

          type_syntax = first.respond_to?(:type) ? first.type : first[:type]
          type_syntax.is_a?(MLC::Source::AST::Type) && type_syntax.name == "Self"
        end

        def parameter_proto_list(parameters)
          Array(parameters).map do |parameter|
            name = parameter.respond_to?(:name) ? parameter.name : parameter[:name]
            type_syntax = parameter.respond_to?(:type) ? parameter.type : parameter[:type]
            "#{ast_type_to_cpp(type_syntax)} #{name}"
          end.join(", ")
        end

        def parameter_call_names(parameters)
          Array(parameters).map { |parameter| parameter.respond_to?(:name) ? parameter.name : parameter[:name] }.join(", ")
        end

        def concept_requires_expression(trait_name, method_entry, return_cpp)
          callee = trait_dispatch_name(trait_name, method_entry[:name])
          non_self = Array(method_entry[:params]).drop(1)
          declval_arguments = non_self.map do |parameter|
            type_syntax = parameter.respond_to?(:type) ? parameter.type : parameter[:type]
            "std::declval<#{ast_type_to_cpp(type_syntax)}>()"
          end.join(", ")
          if declval_arguments.empty?
            "{ #{callee}(self) } -> std::convertible_to<#{return_cpp}>"
          else
            "{ #{callee}(self, #{declval_arguments}) } -> std::convertible_to<#{return_cpp}>"
          end
        end

        def generate_trait_concept_header_lines(trait_info)
          return [] unless concept_trait?(trait_info)

          trait_name = trait_info.name
          methods = trait_info.trait_methods
                              .reject { |entry| entry[:name] == "self" }
                              .select { |entry| self_receiver?(entry) }
          lines = []

          methods.each do |method_entry|
            return_syntax = method_entry[:ret_type]
            return_cpp = return_syntax ? ast_type_to_cpp(return_syntax) : "void"
            dispatch_name = trait_dispatch_name(trait_name, method_entry[:name])
            lines << "template<typename Self>"
            lines << "#{return_cpp} #{dispatch_name}(const Self& self) noexcept;"
          end

          requires_parts = methods.map do |method_entry|
            return_syntax = method_entry[:ret_type]
            return_cpp = return_syntax ? ast_type_to_cpp(return_syntax) : "void"
            concept_requires_expression(trait_name, method_entry, return_cpp)
          end
          unless requires_parts.empty?
            lines << "template<typename Self>"
            lines << "concept #{trait_name} = requires(const Self& self) { #{requires_parts.join('; ')}; };"
          end
          lines
        end

        def inline_forwarding_call(return_cpp, name, parameters_proto, callee, call_arguments)
          "inline #{return_cpp} #{name}(#{parameters_proto}) noexcept { return #{callee}(#{call_arguments}); }"
        end

        def generate_extend_trait_suffix_header_lines(trait_info, implementation, type_registry)
          return [] unless concept_trait?(trait_info)
          return [] unless implementation.trait_name

          type_name = implementation.type_name
          cpp_type = type_registry&.cpp_name(type_name) || type_name
          trait_name = trait_info.name
          lines = []

          trait_info.trait_methods.reject { |entry| entry[:name] == "self" }.each do |trait_method_entry|
            method_name = trait_method_entry[:name]
            implementation_method = implementation.impl_methods[method_name]
            next unless implementation_method

            mangled_name = "#{type_name}_#{method_name}"
            parameters_proto = parameter_proto_list(implementation_method.params)
            call_arguments = parameter_call_names(implementation_method.params)
            return_cpp = ast_type_to_cpp(implementation_method.ret_type)

            if implementation_method.external
              lines << "#{return_cpp} #{mangled_name}(#{parameters_proto}) noexcept;"
            end

            lines << inline_forwarding_call(return_cpp, method_name, parameters_proto, mangled_name, call_arguments)

            next unless self_receiver?(trait_method_entry)

            dispatch_name = trait_dispatch_name(trait_name, method_name)
            lines << inline_forwarding_call(return_cpp, dispatch_name, parameters_proto, mangled_name, call_arguments)
          end

          lines << "static_assert(#{trait_name}<#{cpp_type}>, \"#{type_name} does not implement #{trait_name}\");"
          lines
        end

        def generate_extern_extend_implementation_lines(trait_info, implementation)
          return [] unless concept_trait?(trait_info)
          return [] unless implementation.trait_name

          type_name = implementation.type_name
          lines = []

          implementation.impl_methods.each do |method_name, implementation_method|
            next unless implementation_method.external

            mangled_name = "#{type_name}_#{method_name}"
            parameters_proto = parameter_proto_list(implementation_method.params)
            return_cpp = ast_type_to_cpp(implementation_method.ret_type)
            first_parameter = Array(implementation_method.params).first
            first_name = first_parameter.respond_to?(:name) ? first_parameter.name : first_parameter[:name]

            body = if type_name == "i32" && method_name == "to_string" && first_name
                     "return mlc::to_string(#{first_name});"
                   end
            next unless body

            lines << "#{return_cpp} #{mangled_name}(#{parameters_proto}) noexcept { #{body} }"
          end
          lines
        end
      end
    end
  end
end
