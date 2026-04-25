# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            # Destructuring: let (a, b), let { x, y }, let [h, ...t], let Ok(x)
            class DestructuringDeclRule < BaseRule
              def matches?(node, _context)
                node.is_a?(MLC::Source::AST::DestructuringDecl)
              end

              def produce(node, context)
                svc = services(context)
                expression_visitor = context.fetch(:expression_visitor)

                value_ir = expression_visitor.visit(node.value)

                case node.pattern.kind
                when :tuple
                  produce_tuple_destructuring(node, value_ir, svc, context)
                when :record
                  produce_record_destructuring(node, value_ir, svc, context)
                when :array
                  produce_array_destructuring(node, value_ir, svc, context)
                when :constructor
                  produce_constructor_destructuring(node, value_ir, svc, context)
                else
                  raise MLC::CompileError, "Unsupported destructuring pattern kind: #{node.pattern.kind}"
                end
              end

              private

              def tuple_element_binding_name(elem)
                raise MLC::CompileError, "Invalid tuple pattern" unless elem.is_a?(MLC::Source::AST::Pattern)

                case elem.kind
                when :wildcard
                  nil
                when :var
                  elem.data[:name]
                else
                  raise MLC::CompileError, "Tuple destructuring expects variables or _ in each position, got #{elem.kind}"
                end
              end

              def produce_tuple_destructuring(node, value_ir, svc, _context)
                elements = node.pattern.data[:elements]
                value_type = value_ir.type

                raise MLC::CompileError, "Cannot destructure non-tuple type: #{value_type.inspect}" unless value_type.is_a?(MLC::SemanticIR::TupleType)

                element_types = value_type.element_types
                if elements.size != element_types.size
                  raise MLC::CompileError, "Tuple destructuring size mismatch: pattern has #{elements.size} elements, value has #{element_types.size}"
                end

                bindings = []
                elements.each_with_index do |elem, idx|
                  name = tuple_element_binding_name(elem)
                  next if name.nil?

                  elem_type = element_types[idx]
                  accessor = svc.ir_builder.tuple_access_expr(
                    tuple: value_ir,
                    index: idx,
                    type: elem_type,
                    origin: node
                  )
                  svc.var_type_registry.set(name, elem_type, initializer: accessor, mutable: node.mutable)
                  bindings << { name: name, type: elem_type, accessor: accessor }
                end

                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node
                )
              end

              def produce_record_destructuring(node, value_ir, svc, _context)
                field_names = node.pattern.data[:bindings]
                rest_name = node.pattern.data[:rest]
                value_type = value_ir.type

                raise MLC::CompileError, "Cannot destructure non-record type: #{value_type.inspect}" unless value_type.is_a?(MLC::SemanticIR::RecordType)

                bindings = []
                field_names.each do |field_name|
                  field_info = value_type.fields.find { |f| f[:name] == field_name }
                  raise MLC::CompileError, "Record type #{value_type.name} has no field '#{field_name}'" unless field_info

                  field_type = field_info[:type]
                  accessor = svc.ir_builder.member(
                    object: value_ir,
                    member: field_name,
                    type: field_type,
                    origin: node
                  )
                  svc.var_type_registry.set(field_name, field_type, initializer: accessor, mutable: node.mutable)
                  bindings << { name: field_name, type: field_type, accessor: accessor }
                end

                if rest_name
                  rest_field_defs = value_type.fields.reject { |f| field_names.include?(f[:name]) }
                  if rest_field_defs.empty?
                    raise MLC::CompileError, "let { ...#{rest_name} } has no remaining fields in #{value_type.name}"
                  end

                  rest_type = MLC::SemanticIR::RecordType.new(
                    name: "#{value_type.name}_rest",
                    fields: rest_field_defs,
                    origin: node
                  )
                  field_entries = rest_field_defs.to_h do |f|
                    acc = svc.ir_builder.member(
                      object: value_ir,
                      member: f[:name],
                      type: f[:type],
                      origin: node
                    )
                    [f[:name], acc]
                  end
                  rest_expr = svc.ir_builder.record_expr(
                    type_name: value_type.name,
                    fields: field_entries,
                    type: rest_type,
                    origin: node
                  )
                  svc.var_type_registry.set(rest_name, rest_type, initializer: rest_expr, mutable: node.mutable)
                  bindings << { name: rest_name, type: rest_type, accessor: rest_expr }
                end

                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node
                )
              end

              def array_element_binding(elem)
                raise MLC::CompileError, "Invalid array pattern element" unless elem.is_a?(MLC::Source::AST::Pattern)

                case elem.kind
                when :wildcard
                  nil
                when :var
                  elem.data[:name]
                else
                  raise MLC::CompileError, "Array let pattern element must be a variable or _, got #{elem.kind}"
                end
              end

              def produce_array_destructuring(node, value_ir, svc, _context)
                data = node.pattern.data
                elements = data[:elements] || []
                rest = data[:rest]
                value_type = value_ir.type
                raise MLC::CompileError, "let [..] = requires an array value, got #{value_type.class}" unless value_type.is_a?(MLC::SemanticIR::ArrayType)

                el_t = value_type.element_type
                i32 = svc.ir_builder.prim_type(name: "i32", origin: node)
                bindings = []
                n = elements.size
                n.times do |i|
                  name = array_element_binding(elements[i])
                  next if name.nil?

                  idx_ir = svc.ir_builder.literal(value: i, type: i32, origin: node)
                  accessor = svc.ir_builder.index(
                    object: value_ir,
                    index: idx_ir,
                    type: el_t,
                    origin: node
                  )
                  svc.var_type_registry.set(name, el_t, initializer: accessor, mutable: node.mutable)
                  bindings << { name: name, type: el_t, accessor: accessor }
                end

                if rest
                  start_ir = svc.ir_builder.literal(value: n, type: i32, origin: node)
                  rest_acc = svc.ir_builder.slice(
                    object: value_ir,
                    start_index: start_ir,
                    end_index: nil,
                    type: value_type,
                    origin: node
                  )
                  svc.var_type_registry.set(rest, value_type, initializer: rest_acc, mutable: node.mutable)
                  bindings << { name: rest, type: value_type, accessor: rest_acc }
                end

                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node
                )
              end

              def produce_constructor_destructuring(node, value_ir, svc, _context)
                pat = node.pattern
                cname = pat.data[:name]
                raw_fields = pat.data[:fields] || []
                info = svc.type_unification_service.constructor_info_for(cname, value_ir.type)
                raise MLC::CompileError, "Unknown constructor `#{cname}` for this type" unless info

                param_types = info.param_types
                if raw_fields.size != param_types.size
                  raise MLC::CompileError, "Constructor `#{cname}` expects #{param_types.size} field(s), pattern has #{raw_fields.size}"
                end

                pattern_bindings = raw_fields.map do |f|
                  if f.is_a?(MLC::Source::AST::Pattern)
                    case f.kind
                    when :wildcard
                      "_"
                    when :var
                      f.data[:name]
                    else
                      raise MLC::CompileError, "let #{cname}(...) only supports simple bindings or _ in each position"
                    end
                  elsif f == "_"
                    "_"
                  else
                    f.to_s
                  end
                end

                bindings = []
                pattern_bindings.each_with_index do |name, i|
                  next if name == "_"

                  t = param_types[i]
                  svc.var_type_registry.set(name, t, initializer: nil, mutable: node.mutable)
                  bindings << { name: name, type: t, accessor: nil }
                end

                ctor_destructure = { variant_name: cname, bindings: pattern_bindings, value: value_ir, param_types: param_types }
                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node,
                  ctor_destructure: ctor_destructure
                )
              end
            end
          end
        end
      end
    end
  end
end
