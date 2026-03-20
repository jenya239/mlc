# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::VariableDeclStmt to C++ variable declarations
          # Handles variable rebinding by generating assignment for subsequent let bindings
          class VariableDeclRule < StatementRule
            def applies?(node)
              context.checker.var_decl?(node)
            end

            def apply(node)
              # Special case: let x = result_expr? -> desugar to early-return pattern
              if node.value.is_a?(MLC::SemanticIR::TryExpr) && result_try_type?(node.value.operand.type)
                return lower_result_try_decl(node)
              end

              identifier = context.sanitize_identifier(node.name)
              init_expr = context.lower_expression(node.value)

              # Check if this is a rebinding (variable already declared)
              if context.variable_declared?(node.name)
                # Generate assignment instead of declaration
                return context.factory.expression_statement(
                  expression: context.factory.binary_expression(
                    left: context.factory.identifier(name: identifier),
                    operator: "=",
                    right: init_expr
                  )
                )
              end

              # First declaration - track it
              context.declare_variable(node.name)
              context.set_var_type(node.name, node.type)

              # Map type to C++
              type_str = context.map_type(node.type)
              use_auto = context.type_requires_auto?(node.type, type_str: type_str)

              # Build declarator with move semantics
              decl_type = use_auto ? "auto" : type_str
              init_source = init_expr.to_source
              init_source = "std::move(#{init_source})" if needs_move?(node)
              declarator = "#{identifier} = #{init_source}"

              # Use constexpr for const declarations
              prefix = node.constant ? "constexpr " : ""

              context.factory.variable_declaration(
                type: decl_type,
                declarators: [declarator],
                declarator_separators: [],
                type_suffix: " ",
                prefix_modifiers: prefix
              )
            end
            private

            def result_try_type?(type)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)

              base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil
              return false unless base_name

              type_info = context.type_registry&.lookup(base_name)
              type_info&.core_ir_type&.is_a?(MLC::SemanticIR::SumType)
            end

            # Generate early-return pattern for: let x: T = result_expr?
            # Expands to:
            #   auto __try_N = result_expr;
            #   if (std::holds_alternative<Err<E>>(__try_N)) return ResultType(__try_N);
            #   T x = std::get<Ok<T>>(__try_N)._0;
            def lower_result_try_decl(node)
              try_expr = node.value
              operand_type = try_expr.operand.type
              operand_expr = context.lower_expression(try_expr.operand)

              tmp = "__try_#{node.name}"

              # 1. auto __try_N = operand;
              tmp_decl = context.factory.variable_declaration(
                type: "auto",
                declarators: ["#{tmp} = #{operand_expr.to_source}"],
                declarator_separators: [],
                type_suffix: " "
              )

              # Compute qualified type names
              base_name = operand_type.base_type.name
              type_info = context.type_registry.lookup(base_name)
              ns = type_info.namespace
              type_args = operand_type.type_args
              t_cpp = context.map_type(type_args[0])
              e_cpp = context.map_type(type_args[1])
              ok_type = "#{ns}::Ok<#{t_cpp}, #{e_cpp}>"
              err_type = "#{ns}::Err<#{t_cpp}, #{e_cpp}>"
              result_type = context.map_type(operand_type)

              # 2. if (holds_alternative<Err>(__try_N)) return ResultType(__try_N);
              cond = "std::holds_alternative<#{err_type}>(#{tmp})"
              early_ret = "return #{result_type}(std::get<#{err_type}>(#{tmp}))"
              check_stmt = context.factory.raw_statement(
                code: "if (#{cond}) #{early_ret};"
              )

              # 3. T x = std::get<Ok<T>>(__try_N)._0;
              context.declare_variable(node.name)
              context.set_var_type(node.name, node.type)
              val_type = context.map_type(node.type)
              val_decl = context.factory.variable_declaration(
                type: val_type,
                declarators: ["#{context.sanitize_identifier(node.name)} = std::get<#{ok_type}>(#{tmp}).field0"],
                declarator_separators: [],
                type_suffix: " "
              )

              stmts = [tmp_decl, check_stmt, val_decl]
              CppAst::Nodes::Program.new(
                statements: stmts,
                statement_trailings: Array.new(stmts.size, "\n")
              )
            end

            # Check if the initializer should be wrapped in std::move()
            # Only when: RHS is a variable reference AND the type is a move type (non-Copy)
            def needs_move?(node)
              return false unless node.value.is_a?(MLC::SemanticIR::VarExpr)

              vtr = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry
              vtr.move_on_bind?(node.value.type)
            end
          end
        end
      end
    end
  end
end
