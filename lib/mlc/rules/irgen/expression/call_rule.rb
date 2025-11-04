# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # CallRule: Transform AST call expressions to HighIR call expressions
        # Contains FULL logic (no delegation to transformer)
        # Handles IO functions, member calls, module functions, lambda type inference
        class CallRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::Call)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            expression_visitor = context.fetch(:expression_visitor)
            type_checker = context.fetch(:type_checker)
            type_inference = context.fetch(:type_inference)
            scope_context = context.fetch(:scope_context)

            # Special case: IO functions have fixed return types
            if node.callee.is_a?(MLC::AST::VarRef) && transformer.class::IO_RETURN_TYPES.key?(node.callee.name)
              callee = expression_visitor.visit(node.callee)
              args = node.args.map { |arg| expression_visitor.visit(arg) }
              type = type_checker.io_return_type(node.callee.name)
              return MLC::HighIR::Builder.call(callee, args, type)
            end

            # General case: determine callee type
            callee_ast = node.callee
            object_ir = nil
            member_name = nil

            if callee_ast.is_a?(MLC::AST::MemberAccess)
              # Member access call: check if module function or instance method
              entry = transformer.send(:module_member_function_entry, callee_ast.object, callee_ast.member)
              if entry
                # Module function (e.g., Math.sqrt)
                canonical_name = entry.name
                callee = MLC::HighIR::Builder.var(canonical_name, type_inference.function_placeholder_type(canonical_name))
              else
                # Instance method call (e.g., arr.map)
                object_ir = expression_visitor.visit(callee_ast.object)
                member_name = callee_ast.member
                callee = MLC::HighIR::Builder.member(object_ir, member_name, type_inference.infer_member_type(object_ir.type, member_name))
              end
            elsif callee_ast.is_a?(MLC::AST::VarRef)
              # Variable reference (function name)
              var_type = type_inference.function_placeholder_type(callee_ast.name)
              callee = MLC::HighIR::Builder.var(callee_ast.name, var_type)
            else
              # Complex expression (e.g., lambda call)
              callee = expression_visitor.visit(callee_ast)
            end

            # Transform arguments with lambda type inference
            args = []
            node.args.each_with_index do |arg, index|
              # For map/filter/fold: infer lambda parameter types
              expected_params = type_inference.expected_lambda_param_types(object_ir, member_name, args, index)

              transformed_arg = if arg.is_a?(MLC::AST::Lambda)
                                  # Transform lambda with expected parameter types
                                  scope_context.with_lambda_param_types(expected_params) do
                                    expression_visitor.visit(arg)
                                  end
                                else
                                  expression_visitor.visit(arg)
                                end
              args << transformed_arg
            end

            # Infer call return type and build call expression
            type = type_inference.infer_call_type(callee, args)
            MLC::HighIR::Builder.call(callee, args, type)
          end
        end
      end
    end
  end
end
