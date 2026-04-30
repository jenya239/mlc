# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # D4: merge `where T: Trait + …` into type parameter trait bounds before codegen / trait-param expand.
          module WhereClauseMerge
            class << self
              def validate!(func_decl, type_checker:)
                where_clause = func_decl.where_clause
                return unless where_clause

                allowed = func_decl.type_params.map(&:name).to_set
                Array(where_clause.bounds).each do |bound|
                  next if allowed.include?(bound.type_param)

                  type_checker.type_error(
                    %(where clause names unknown type parameter "#{bound.type_param}"),
                    node: bound
                  )
                end
              end

              def merge_where_into_func_decl(func_decl)
                where_clause = func_decl.where_clause
                return func_decl unless where_clause

                extras_by_name = Hash.new { |hash, key| hash[key] = [] }
                Array(where_clause.bounds).each do |bound|
                  extras_by_name[bound.type_param].concat(bound.traits)
                end

                new_type_params = func_decl.type_params.map do |type_parameter|
                  additionals = extras_by_name[type_parameter.name]
                  next type_parameter if additionals.empty?

                  MLC::Source::AST::TypeParam.new(
                    name: type_parameter.name,
                    constraint: type_parameter.constraint,
                    extra_trait_bounds: Array(type_parameter.extra_trait_bounds) + additionals,
                    origin: type_parameter.origin
                  )
                end

                MLC::Source::AST::FuncDecl.new(
                  name: func_decl.name,
                  params: func_decl.params,
                  ret_type: func_decl.ret_type,
                  body: func_decl.body,
                  type_params: new_type_params,
                  where_clause: nil,
                  exported: func_decl.exported,
                  external: func_decl.external,
                  is_async: func_decl.is_async,
                  origin: func_decl.origin
                )
              end
            end
          end
        end
      end
    end
  end
end
