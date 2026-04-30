# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # D3: Desugar a trait name used as a parameter type (e.g. x: Display) into an explicit
          # type parameter with a trait constraint, mirroring the self-hosted checker pass.
          module TraitParamExpandAst
            class << self
              def validate_trait_and_type_name_conflicts(program, type_checker)
                trait_names = {}
                nominal_names = {}
                program.declarations.each do |decl|
                  case decl
                  when MLC::Source::AST::TraitDecl
                    if nominal_names[decl.name]
                      type_checker.type_error(
                        %(the name "#{decl.name}" is declared as both a type and a trait),
                        node: decl
                      )
                    end
                    trait_names[decl.name] = true
                  when MLC::Source::AST::TypeDecl
                    if trait_names[decl.name]
                      type_checker.type_error(
                        %(the name "#{decl.name}" is declared as both a type and a trait),
                        node: decl
                      )
                    end
                    nominal_names[decl.name] = true
                  end
                end
              end

              def expand_function_declaration(func_decl, trait_registry:, type_decl_table:)
                explicit = {}
                Array(func_decl.type_params).each { |tp| explicit[tp.name] = true }

                appended_type_params = []
                counter = 0
                mutated = false

                new_params = func_decl.params.map do |param|
                  expanded_type, counter, mut = expand_type_tree(
                    param.type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  mutated ||= mut
                  MLC::Source::AST::Param.new(
                    name: param.name,
                    type: expanded_type,
                    mutable: param.mutable,
                    default: param.default,
                    origin: param.origin
                  )
                end

                expanded_ret, counter, mut_ret = expand_type_tree(
                  func_decl.ret_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                )
                mutated ||= mut_ret

                return func_decl if appended_type_params.empty? && !mutated

                combined_type_params = Array(func_decl.type_params) + appended_type_params

                MLC::Source::AST::FuncDecl.new(
                  name: func_decl.name,
                  params: new_params,
                  ret_type: expanded_ret,
                  body: func_decl.body,
                  type_params: combined_type_params,
                  where_clause: func_decl.where_clause,
                  exported: func_decl.exported,
                  external: func_decl.external,
                  is_async: func_decl.is_async,
                  origin: func_decl.origin
                )
              end

              private

              def expand_type_tree(type_node, explicit, trait_registry, type_decl_table, counter, appended_type_params)
                return [nil, counter, false] if type_node.nil?

                case type_node
                when MLC::Source::AST::PrimType
                  name = type_node.name
                  return [type_node, counter, false] if explicit[name]
                  if trait_registry&.get_trait(name) && !type_decl_table[name]
                    synthetic = "__trait_param_#{counter}"
                    appended_type_params << MLC::Source::AST::TypeParam.new(
                      name: synthetic,
                      constraint: name,
                      extra_trait_bounds: [],
                      origin: type_node.origin
                    )
                    return [
                      MLC::Source::AST::PrimType.new(name: synthetic, origin: type_node.origin),
                      counter + 1,
                      true
                    ]
                  end

                  [type_node, counter, false]

                when MLC::Source::AST::GenericType
                  new_base, counter, m0 = expand_type_tree(
                    type_node.base_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  new_args = []
                  mut = m0
                  type_node.type_params.each do |arg|
                    na, counter, m1 = expand_type_tree(arg, explicit, trait_registry, type_decl_table, counter, appended_type_params)
                    mut ||= m1
                    new_args << na
                  end
                  [
                    MLC::Source::AST::GenericType.new(base_type: new_base, type_params: new_args, origin: type_node.origin),
                    counter,
                    mut
                  ]

                when MLC::Source::AST::ArrayType
                  inner, counter, mut = expand_type_tree(
                    type_node.element_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  [
                    MLC::Source::AST::ArrayType.new(element_type: inner, origin: type_node.origin),
                    counter,
                    mut
                  ]

                when MLC::Source::AST::MapType
                  nk, counter, m0 = expand_type_tree(
                    type_node.key_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  nv, counter, m1 = expand_type_tree(
                    type_node.value_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  [
                    MLC::Source::AST::MapType.new(key_type: nk, value_type: nv, origin: type_node.origin),
                    counter,
                    m0 || m1
                  ]

                when MLC::Source::AST::FunctionType
                  new_pts = []
                  mut = false
                  type_node.param_types.each do |pt|
                    npt, counter, m1 = expand_type_tree(pt, explicit, trait_registry, type_decl_table, counter, appended_type_params)
                    mut ||= m1
                    new_pts << npt
                  end
                  nret, counter, m2 = expand_type_tree(
                    type_node.ret_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  mut ||= m2
                  [
                    MLC::Source::AST::FunctionType.new(param_types: new_pts, ret_type: nret, origin: type_node.origin),
                    counter,
                    mut
                  ]

                when MLC::Source::AST::TupleType
                  new_ts = []
                  mut = false
                  type_node.types.each do |t|
                    nt, counter, m1 = expand_type_tree(t, explicit, trait_registry, type_decl_table, counter, appended_type_params)
                    mut ||= m1
                    new_ts << nt
                  end
                  [MLC::Source::AST::TupleType.new(types: new_ts, origin: type_node.origin), counter, mut]

                when MLC::Source::AST::RefType
                  inner, counter, mut = expand_type_tree(
                    type_node.inner_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  [MLC::Source::AST::RefType.new(inner_type: inner, origin: type_node.origin), counter, mut]

                when MLC::Source::AST::MutRefType
                  inner, counter, mut = expand_type_tree(
                    type_node.inner_type, explicit, trait_registry, type_decl_table, counter, appended_type_params
                  )
                  [MLC::Source::AST::MutRefType.new(inner_type: inner, origin: type_node.origin), counter, mut]

                else
                  [type_node, counter, false]
                end
              end
            end
          end
        end
      end
    end
  end
end
