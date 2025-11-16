# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Reducers
          # TypeReducer lowers AST::TypeDecl nodes into SemanticIR::TypeDecl
          # and gives the rule engine a chance to post-process declarations
          class TypeReducer
            def initialize(services:, rule_engine:)
              @services = services
              @rule_engine = rule_engine
        end

            def reduce(type_decl)
              result = nil
              @services.scope_context.with_current_node(type_decl) do
                result = @services.type_declaration_service.build(type_decl)
                result = apply_rules(result)
          end
              result
        end

            private

            def apply_rules(type_ir)
              return type_ir unless @rule_engine

              rule_result = @rule_engine.apply(
                :core_ir_type_decl,
                type_ir,
                context: { type_registry: @services.type_registry }
              )

              return type_ir unless rule_result
              rule_result.is_a?(MLC::SemanticIR::TypeDecl) ? rule_result : type_ir
        end
          end
        end
          end
        end
      end
    end