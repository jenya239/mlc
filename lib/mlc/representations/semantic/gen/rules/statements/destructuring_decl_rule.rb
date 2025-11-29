# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            # Rule for handling destructuring declarations:
            #   let (a, b) = expr  -- tuple destructuring
            #   let { x, y } = expr -- record destructuring
            class DestructuringDeclRule < BaseRule
              def matches?(node, _context)
                node.is_a?(MLC::Source::AST::DestructuringDecl)
              end

              def produce(node, context)
                svc = services(context)
                expression_visitor = context.fetch(:expression_visitor)

                # Evaluate the RHS expression
                value_ir = expression_visitor.visit(node.value)

                case node.pattern.kind
                when :tuple
                  produce_tuple_destructuring(node, value_ir, svc, context)
                when :record
                  produce_record_destructuring(node, value_ir, svc, context)
                else
                  raise "Unsupported destructuring pattern kind: #{node.pattern.kind}"
                end
              end

              private

              # Produce statements for tuple destructuring: let (a, b) = expr
              # Desugars to:
              #   let __tmp = expr
              #   let a = __tmp.0
              #   let b = __tmp.1
              def produce_tuple_destructuring(node, value_ir, svc, context)
                elements = node.pattern.data[:elements]
                value_type = value_ir.type

                unless value_type.is_a?(MLC::SemanticIR::TupleType)
                  raise "Cannot destructure non-tuple type: #{value_type.inspect}"
                end

                element_types = value_type.element_types

                if elements.size != element_types.size
                  raise "Tuple destructuring size mismatch: pattern has #{elements.size} elements, value has #{element_types.size}"
                end

                bindings = []
                elements.each_with_index do |elem, idx|
                  next if elem.kind == :wildcard

                  name = elem.data[:name]
                  elem_type = element_types[idx]

                  # Create accessor: TupleAccessExpr for extracting element
                  accessor = svc.ir_builder.tuple_access_expr(
                    tuple: value_ir,
                    index: idx,
                    type: elem_type,
                    origin: node
                  )

                  # Register variable in scope
                  svc.var_type_registry.set(name, elem_type, initializer: accessor)

                  bindings << { name: name, type: elem_type, accessor: accessor }
                end

                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node
                )
              end

              # Produce statements for record destructuring: let { x, y } = expr
              # Desugars to:
              #   let __tmp = expr
              #   let x = __tmp.x
              #   let y = __tmp.y
              def produce_record_destructuring(node, value_ir, svc, _context)
                field_names = node.pattern.data[:bindings]
                value_type = value_ir.type

                unless value_type.is_a?(MLC::SemanticIR::RecordType)
                  raise "Cannot destructure non-record type: #{value_type.inspect}"
                end

                bindings = []
                field_names.each do |field_name|
                  field_info = value_type.fields.find { |f| f[:name] == field_name }
                  unless field_info
                    raise "Record type #{value_type.name} has no field '#{field_name}'"
                  end

                  field_type = field_info[:type]

                  # Create accessor: MemberExpr for extracting field
                  accessor = svc.ir_builder.member(
                    object: value_ir,
                    member: field_name,
                    type: field_type,
                    origin: node
                  )

                  # Register variable in scope
                  svc.var_type_registry.set(field_name, field_type, initializer: accessor)

                  bindings << { name: field_name, type: field_type, accessor: accessor }
                end

                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node
                )
              end
            end
          end
        end
      end
    end
  end
end
