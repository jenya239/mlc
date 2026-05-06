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

                if value_type.is_a?(MLC::SemanticIR::TupleType)
                  return produce_tuple_tuple_destructuring(node, value_ir, svc, elements, value_type)
                end

                ordered_record_fields = ordered_record_fields_for_positional_tuple_pattern(value_type, svc)
                if ordered_record_fields
                  return produce_tuple_as_ordered_record_fields(node, value_ir, svc, elements, ordered_record_fields)
                end

                raise MLC::CompileError, "Cannot destructure non-tuple type as (...) tuple pattern: #{value_type.inspect}"
              end

              def produce_tuple_tuple_destructuring(node, value_ir, svc, elements, tuple_type)
                element_types = tuple_type.element_types
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

              def produce_tuple_as_ordered_record_fields(node, value_ir, svc, elements, ordered_record_fields)
                if elements.size != ordered_record_fields.size
                  raise MLC::CompileError,
                        "Tuple destructuring size mismatch: pattern has #{elements.size} elements, record type has #{ordered_record_fields.size} fields"
                end

                bindings = []
                elements.each_with_index do |elem, idx|
                  name = tuple_element_binding_name(elem)
                  next if name.nil?

                  field_entry = ordered_record_fields[idx]
                  field_name = field_entry[:name]
                  elem_type = field_entry[:type]
                  accessor = svc.ir_builder.member(
                    object: value_ir,
                    member: field_name,
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

              def type_parameter_declaration_names(decl)
                return [] unless decl

                Array(decl.type_params).map do |type_parameter_entry|
                  if type_parameter_entry.respond_to?(:name)
                    type_parameter_entry.name
                  else
                    type_parameter_entry.to_s
                  end
                end
              end

              def ordered_record_fields_for_positional_tuple_pattern(value_type, svc)
                case value_type
                when MLC::SemanticIR::RecordType
                  value_type.fields.map do |field_entry|
                    {
                      name: (field_entry[:name] || field_entry["name"]).to_s,
                      type: field_entry[:type] || field_entry["type"]
                    }
                  end
                when MLC::SemanticIR::GenericType
                  base_record_type = value_type.base_type
                  return nil unless base_record_type.is_a?(MLC::SemanticIR::RecordType)

                  type_declaration = svc.type_decl_table[base_record_type.name]
                  substitution_names = type_parameter_declaration_names(type_declaration)
                  substitution_map = {}
                  substitution_names.each_with_index do |parameter_name, index|
                    type_argument = value_type.type_args[index]
                    substitution_map[parameter_name] = type_argument if type_argument
                  end

                  base_record_type.fields.map do |field_entry|
                    raw_field_type = field_entry[:type] || field_entry["type"]
                    resolved_field_type = if substitution_map.empty?
                                              raw_field_type
                                            else
                                              svc.type_inference_service.substitute_type(raw_field_type, substitution_map)
                                            end
                    {
                      name: (field_entry[:name] || field_entry["name"]).to_s,
                      type: resolved_field_type
                    }
                  end
                else
                  nil
                end
              end

              def produce_record_destructuring(node, value_ir, svc, _context)
                field_names = node.pattern.data[:bindings]
                rest_name = node.pattern.data[:rest]
                value_type = value_ir.type

                record_for_lookup = record_type_for_record_pattern(value_type)
                raise MLC::CompileError, "Cannot destructure non-record type: #{value_type.inspect}" unless record_for_lookup

                substitution_map = record_substitution_map_for_destructure(value_type, record_for_lookup)

                bindings = []
                field_names.each do |field_name|
                  field_info = record_for_lookup.fields.find { |field_definition| field_definition[:name] == field_name }
                  raise MLC::CompileError, "Record type #{record_for_lookup.name} has no field '#{field_name}'" unless field_info

                  field_type = resolve_record_field_type_for_destructure(field_info[:type], substitution_map, svc)
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
                  rest_field_defs = record_for_lookup.fields.reject { |field_definition| field_names.include?(field_definition[:name]) }
                  if rest_field_defs.empty?
                    raise MLC::CompileError, "let { ...#{rest_name} } has no remaining fields in #{record_for_lookup.name}"
                  end

                  rest_type_fields = rest_field_defs.map do |field_definition|
                    {
                      name: field_definition[:name],
                      type: resolve_record_field_type_for_destructure(field_definition[:type], substitution_map, svc)
                    }
                  end

                  rest_type = MLC::SemanticIR::RecordType.new(
                    name: "#{record_for_lookup.name}_rest",
                    fields: rest_type_fields,
                    origin: node
                  )
                  field_entries = rest_type_fields.to_h do |field_definition|
                    access_expression = svc.ir_builder.member(
                      object: value_ir,
                      member: field_definition[:name],
                      type: field_definition[:type],
                      origin: node
                    )
                    [field_definition[:name], access_expression]
                  end
                  rest_expr = svc.ir_builder.record_expr(
                    type_name: record_for_lookup.name,
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

              def record_type_for_record_pattern(value_type)
                case value_type
                when MLC::SemanticIR::RecordType
                  value_type
                when MLC::SemanticIR::GenericType
                  base_type = value_type.base_type
                  base_type if base_type.is_a?(MLC::SemanticIR::RecordType)
                end
              end

              def record_substitution_map_for_destructure(value_type, record_for_lookup)
                return {} unless value_type.is_a?(MLC::SemanticIR::GenericType)

                type_declaration = svc.type_decl_table[record_for_lookup.name]
                substitution_names = type_parameter_declaration_names(type_declaration)
                substitution_map = {}
                substitution_names.each_with_index do |parameter_name, index|
                  type_argument = value_type.type_args[index]
                  substitution_map[parameter_name] = type_argument if type_argument
                end
                substitution_map
              end

              def resolve_record_field_type_for_destructure(field_type, substitution_map, svc)
                return field_type if substitution_map.nil? || substitution_map.empty?

                svc.type_inference_service.substitute_type(field_type, substitution_map)
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

              def produce_constructor_destructuring(node, value_ir, svc, context)
                pat = node.pattern
                cname = pat.data[:name]
                raw_fields = pat.data[:fields] || []
                info = svc.type_unification_service.constructor_info_for(cname, value_ir.type)
                raise MLC::CompileError, "Unknown constructor `#{cname}` for this type" unless info

                raise MLC::CompileError, "refutable pattern in let requires else branch" if node.else_body.nil?

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

                expression_visitor = context.fetch(:expression_visitor)
                else_body_ir = expression_visitor.visit(node.else_body)

                ctor_destructure = { variant_name: cname, bindings: pattern_bindings, value: value_ir, param_types: param_types, else_body: else_body_ir }
                MLC::SemanticIR::DestructuringDeclStmt.new(
                  bindings: bindings,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node,
                  ctor_destructure: ctor_destructure,
                  else_body: else_body_ir
                )
              end
            end
          end
        end
      end
    end
  end
end
