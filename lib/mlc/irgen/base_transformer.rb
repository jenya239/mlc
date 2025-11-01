# frozen_string_literal: true

module MLC
  class IRGen
      # BaseTransformer
      # Shared utilities for transformation
      # Auto-extracted from to_core.rb during refactoring
      module BaseTransformer
      def builtin_function_info(name)
        case name
        when "sqrt"
          f32 = HighIR::Builder.primitive_type("f32")
          FunctionInfo.new("sqrt", [f32], f32)
        else
          if IO_RETURN_TYPES.key?(name)
            FunctionInfo.new(name, [], @type_checker_service.io_return_type(name))
          else
            nil
          end
        end
      end

      def function_placeholder_type(name)
        if (info = lookup_function_info(name))
          @type_inference_service.function_type_from_info(info)
        else
          HighIR::Builder.function_type([], HighIR::Builder.primitive_type("auto"))
        end
      end







      def lookup_function_info(name)
        @function_registry.fetch(name) || @sum_type_constructors[name] || builtin_function_info(name)
      end







      def with_current_node(node)
        previous = @current_node
        @current_node = node if node
        yield
      ensure
        @current_node = previous
      end
    end
  end
end
