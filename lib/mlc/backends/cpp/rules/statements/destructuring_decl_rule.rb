# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # let (a, b), let {x,y}, let [a,...t], let Ok(x) → C++
          class DestructuringDeclRule < StatementRule
            def applies?(node)
              node.is_a?(MLC::SemanticIR::DestructuringDeclStmt)
            end

            def apply(node)
              value_src = context.lower_expression(node.value).to_source
              bindings = node.bindings
              if node.ctor_destructure
                return [generate_ctor_let_block(node, value_src)]
              end
              if tuple_destructuring?(bindings)
                return generate_tuple_destructuring(bindings, context.lower_expression(node.value), node)
              end

              # Index/slice/record member/rest: one temp, then each binding
              generate_mixed_destructuring(node, value_src, bindings)
            end

            private

            def match_helper
              @match_helper ||= MLC::Backends::Cpp::Rules::Expressions::MatchRule.new(context)
            end

            def generate_ctor_let_block(node, value_src)
              st = node.value.type
              meta = node.ctor_destructure
              vname = meta[:variant_name]
              pbind = meta[:bindings]
              else_src = context.lower_expression(meta[:else_body]).to_source
              m = match_helper
              if m.send(:option_type?, st) && vname == "Some" && pbind.any? { |b| b != "_" }
                s = context.sanitize_identifier(pbind.find { |b| b != "_" })
                return context.factory.raw_statement(
                  code: "{\nauto __s = #{value_src};\nif (!__s.has_value()) { #{else_src}; }\n" \
                        "auto #{s} = *__s;\n}\n"
                )
              end
              if m.send(:option_type?, st) && vname == "None"
                return context.factory.raw_statement(
                  code: "{\nauto __s = #{value_src};\nif (__s.has_value()) { #{else_src}; }\n}\n"
                )
              end

              qcase = m.send(:qualified_case_name, vname, st)
              scr = "__s"
              vs = m.send(:variant_src_for, scr, st)
              bdecls, = m.send(:build_binding_extractions, vname, pbind, vs, st)
              inner = bdecls.join(" ")
              code = "{\nauto #{scr} = #{value_src};\n" \
                     "if (!std::holds_alternative<#{qcase}>(#{vs})) { #{else_src}; }\n" \
                     "#{inner}\n}\n"
              context.factory.raw_statement(code: code)
            end

            def tuple_destructuring?(bindings)
              return false if bindings.empty?

              bindings.first[:accessor].is_a?(MLC::SemanticIR::TupleAccessExpr)
            end

            def generate_tuple_destructuring(bindings, value_expr, _node)
              binding_names = bindings.map { |b| context.sanitize_identifier(b[:name]) }
              bindings.each { |b| context.declare_variable(b[:name]) }
              binding_list = "[#{binding_names.join(', ')}]"
              declarator = "#{binding_list} = #{value_expr.to_source}"
              context.factory.variable_declaration(
                type: "auto",
                declarators: [declarator],
                declarator_separators: [],
                type_suffix: " ",
                prefix_modifiers: ""
              )
            end

            def generate_mixed_destructuring(_node, value_src, bindings)
              value_ir = _node.value
              temp = context.generate_temp_name
              temp_decl = context.factory.variable_declaration(
                type: "auto",
                declarators: ["#{temp} = #{value_src}"],
                declarator_separators: [],
                type_suffix: " ",
                prefix_modifiers: ""
              )
              stmts = [temp_decl]
              bindings.each do |b|
                context.declare_variable(b[:name])
                rhs = accessor_cxx(b[:accessor], value_ir, temp, _node)
                ident = context.sanitize_identifier(b[:name])
                stmts << context.factory.variable_declaration(
                  type: "auto",
                  declarators: ["#{ident} = #{rhs}"],
                  declarator_separators: [],
                  type_suffix: " ",
                  prefix_modifiers: ""
                )
              end
              stmts
            end

            def accessor_cxx(acc, value_ir, temp, origin_node)
              case acc
              when MLC::SemanticIR::MemberExpr
                if acc.object.equal?(value_ir)
                  "#{temp}.#{acc.member}"
                else
                  context.lower_expression(acc).to_source
                end
              when MLC::SemanticIR::IndexExpr
                if acc.object.equal?(value_ir)
                  idx = context.lower_expression(acc.index).to_source
                  "#{temp}[#{idx}]"
                else
                  context.lower_expression(acc).to_source
                end
              when MLC::SemanticIR::SliceExpr
                el_t = context.map_type(acc.type.element_type)
                if acc.object.equal?(value_ir) && acc.end_index.nil? && acc.start_index
                  s = context.lower_expression(acc.start_index).to_source
                  "mlc::Array<#{el_t}>(#{temp}.begin() + #{s}, #{temp}.end())"
                elsif acc.object.equal?(value_ir)
                  context.lower_expression(acc).to_source
                else
                  context.lower_expression(acc).to_source
                end
              when MLC::SemanticIR::RecordExpr
                record_init_from_temp(acc, value_ir, temp, origin_node)
              else
                context.lower_expression(acc).to_source
              end
            end

            def record_init_from_temp(rec, value_ir, temp, _origin_node)
              args = rec.fields.map do |fname, mem|
                accessor_cxx(mem, value_ir, temp, _origin_node)
              end
              type_str = context.map_type(rec.type)
              "#{type_str}{#{args.join(', ')}}"
            end
          end
        end
      end
    end
  end
end
