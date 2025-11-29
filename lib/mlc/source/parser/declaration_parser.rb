# frozen_string_literal: true

module MLC
  module Source
  module Parser
    # DeclarationParser
    # Declaration parsing - modules, imports, functions, type declarations
    # Auto-extracted from parser.rb during refactoring
    module DeclarationParser
    def parse_function(external: false, exported: false, is_async: false)
      consume(:FN)
      name_token = consume(:IDENTIFIER)
      name = name_token.value

      # Parse optional type parameters: fn identity<T>(x: T) -> T
      type_params = []
      if current.type == :OPERATOR && current.value == "<"
        consume(:OPERATOR)  # <
        type_params = parse_type_params
        expect_operator(">")
      end

      consume(:LPAREN)
      params = parse_params
      consume(:RPAREN)

      consume(:ARROW)
      ret_type = parse_type

      # Body is optional for extern functions
      body = nil
      if current.type == :EQUAL
        consume(:EQUAL)
        body = parse_expression
      end

      with_origin(name_token) do
        MLC::Source::AST::FuncDecl.new(
          name: name,
          params: params,
          ret_type: ret_type,
          body: body,
          type_params: type_params,
          external: external,
          exported: exported,
          is_async: is_async
        )
      end
    end

    def parse_import_decl
      import_token = consume(:IMPORT)

      # Four syntaxes:
      # 1. import { add, subtract } from "./math"  (ESM with file path)
      # 2. import * as Math from "./math"          (ESM wildcard with file path)
      # 3. import Math (backward compat - module name)
      # 4. import Math::{...} (backward compat - module name with selective)

      items = nil
      import_all = false
      alias_name = nil
      path = nil

      if current.type == :LBRACE
        # Syntax 1: import { add, subtract } from "./math"
        consume(:LBRACE)
        items = []
        loop do
          items << consume(:IDENTIFIER).value
          break if current.type != :COMMA
          consume(:COMMA)
        end
        consume(:RBRACE)
        consume(:FROM)
        path = parse_import_path  # Can be string or identifier
      elsif current.type == :OPERATOR && current.value == "*"
        # Syntax 2: import * as Math from "./math"
        consume(:OPERATOR)  # *
        consume(:AS)
        alias_name = consume(:IDENTIFIER).value
        consume(:FROM)
        path = parse_import_path  # Can be string or identifier
        import_all = true
      else
        # Syntax 3-4 (backward compat): import Math or import Math::{...}
        path = parse_module_path

        # Check for old-style selective imports: import Math::{sqrt, pow}
        if current.type == :COLON
          consume(:COLON)
          consume(:COLON)
          consume(:LBRACE)
          items = []
          loop do
            items << consume(:IDENTIFIER).value
            break if current.type != :COMMA
            consume(:COMMA)
          end
          consume(:RBRACE)
        end
      end

      with_origin(import_token) do
        MLC::Source::AST::ImportDecl.new(
          path: path,
          items: items,
          import_all: import_all,
          alias_name: alias_name
        )
      end
    end

    def parse_import_path
      # Can be string literal (ESM-style) or identifier (backward compat)
      if current.type == :STRING_LITERAL
        consume(:STRING_LITERAL).value
      else
        parse_module_path
      end
    end

    def parse_module_decl
      module_token = consume(:MODULE)
      path = parse_module_path
      with_origin(module_token) { MLC::Source::AST::ModuleDecl.new(name: path) }
    end

    def parse_module_path
      # Parse path like Math::Vector or just Math
      result = consume(:IDENTIFIER).value.dup

      loop do
        if current.type == :COLON && peek_ahead(1)&.type == :COLON && peek_ahead(2)&.type == :IDENTIFIER
          consume(:COLON)
          consume(:COLON)
          result << "::"
          result << consume(:IDENTIFIER).value
        elsif current.type == :OPERATOR && current.value == "/" && peek_ahead(1)&.type == :IDENTIFIER
          consume(:OPERATOR)  # /
          result << "/"
          result << consume(:IDENTIFIER).value
        else
          break
        end
      end

      result
    end

    def parse_params
      params = []

      while current.type != :RPAREN
        name_token = consume(:IDENTIFIER)
        name = name_token.value
        consume(:COLON)
        type = parse_type

        params << with_origin(name_token) { MLC::Source::AST::Param.new(name: name, type: type) }

        if current.type == :COMMA
          consume(:COMMA)
        else
          break
        end
      end

      params
    end

    def parse_program
      module_decl = nil
      imports = []
      declarations = []

      # Parse optional module declaration
      if current.type == :MODULE
        module_decl = parse_module_decl
        # Skip any remaining tokens on the module line (for malformed input like module app/geom)
        while !eof? && current.type != :FN && current.type != :TYPE && current.type != :IMPORT && current.type != :EXTERN && current.type != :EXPORT
          @pos += 1
        end
      end

      # Parse imports
      while current.type == :IMPORT
        imports << parse_import_decl
      end

      # Parse declarations
      while !eof?
        case current.type
        when :EXPORT
          # Parse exported declaration
          consume(:EXPORT)
          case current.type
          when :ASYNC
            # export async fn ...
            consume(:ASYNC)
            if current.type == :FN
              func = parse_function(exported: true, is_async: true)
              declarations << func
            else
              raise "Expected FN after export async, got #{current.type}"
            end
          when :EXTERN
            # export extern fn ...
            consume(:EXTERN)
            if current.type == :FN
              func = parse_function(external: true, exported: true)
              declarations << func
            else
              raise "Expected FN after export extern, got #{current.type}"
            end
          when :FN
            func = parse_function(exported: true)
            declarations << func
          when :TYPE
            declarations << parse_type_decl(exported: true)
          when :TRAIT
            declarations << parse_trait_decl(exported: true)
          when :EXTEND
            declarations << parse_extend_decl(exported: true)
          when :RECORD
            declarations << parse_record_decl(exported: true)
          else
            raise "Expected FN, TYPE, TRAIT, ASYNC, EXTEND, or RECORD after export, got #{current.type}"
          end
        when :ASYNC
          # Parse async function: async fn ...
          consume(:ASYNC)
          if current.type == :FN
            declarations << parse_function(is_async: true)
          else
            raise "Expected FN after async, got #{current.type}"
          end
        when :EXTERN
          # Parse external declaration
          consume(:EXTERN)
          case current.type
          when :FN
            declarations << parse_function(external: true)
          else
            raise "Expected FN after extern, got #{current.type}"
          end
        when :FN
          declarations << parse_function
        when :TYPE
          declarations << parse_type_decl
        when :TRAIT
          declarations << parse_trait_decl
        when :EXTEND
          declarations << parse_extend_decl
        when :RECORD
          declarations << parse_record_decl
        else
          break
        end
      end

      MLC::Source::AST::Program.new(
        module_decl: module_decl,
        imports: imports,
        declarations: declarations
      )
    end

    # Parse record literal fields with spread and shorthand support
    # Supports:
    #   { x: 1, y: 2 }           - regular fields
    #   { ...base, x: 1 }        - spread operator (JS-like)
    #   { x, y }                 - shorthand (equivalent to { x: x, y: y })
    # Returns: { fields: Hash, spreads: Array }
    def parse_record_fields
      fields = {}
      spreads = []
      position = 0

      while current.type != :RBRACE
        if current.type == :SPREAD
          # Spread operator: ...expr
          spread_token = consume(:SPREAD)
          spread_expr = parse_expression
          spreads << { expr: spread_expr, position: position }
        elsif current.type == :IDENTIFIER
          field_name_token = consume(:IDENTIFIER)
          field_name = field_name_token.value

          if current.type == :COLON
            # Regular field: name: value
            consume(:COLON)
            value = parse_expression
            fields[field_name] = value
          else
            # Shorthand: just name (equivalent to name: name)
            var_ref = with_origin(field_name_token) { MLC::Source::AST::VarRef.new(name: field_name) }
            fields[field_name] = var_ref
          end
        else
          raise "Unexpected token in record field: #{current}"
        end

        position += 1

        if current.type == :COMMA
          consume(:COMMA)
        else
          break
        end
      end

      { fields: fields, spreads: spreads }
    end

    def parse_type_decl(exported: false)
      consume(:TYPE)
      name_token = consume(:IDENTIFIER)
      name = name_token.value

      # Parse optional type parameters: type Option<T> = ...
      type_params = []
      if current.type == :OPERATOR && current.value == "<"
        consume(:OPERATOR)  # <
        type_params = parse_type_params
        expect_operator(">")
      end

      # For opaque types (export type Foo), EQUAL is optional
      # If there's no EQUAL, create an opaque type reference
      type = if current.type == :EQUAL
               consume(:EQUAL)

               case current.type
               when :LBRACE
                 parse_record_type
               when :ENUM
                 parse_enum_type
               when :OPERATOR
                 if current.value == "|"
                   parse_sum_type
                 else
                   parse_type
                 end
               when :IDENTIFIER
                 # Could be sum type (Variant1 | Variant2) or named type
                 # Look ahead to see if there's a | after the identifier
                 parse_type_or_sum
               else
                 parse_type
               end
             else
               # Opaque type - explicit type declaration without definition
               # Represented as pointer in C++, used for extern/stdlib types
               MLC::Source::AST::OpaqueType.new(name: name)
             end

      with_origin(name_token) { MLC::Source::AST::TypeDecl.new(name: name, type: type, type_params: type_params, exported: exported) }
    end

    # Parse record declaration - syntactic sugar for type Name = { fields }
    # record Name { field1: Type1, field2: Type2 }
    def parse_record_decl(exported: false)
      consume(:RECORD)
      name_token = consume(:IDENTIFIER)
      name = name_token.value

      # Parse optional type parameters: record Option<T> { ... }
      type_params = []
      if current.type == :OPERATOR && current.value == "<"
        consume(:OPERATOR)  # <
        type_params = parse_type_params
        expect_operator(">")
      end

      # Parse record body { field: Type, ... }
      type = parse_record_type

      with_origin(name_token) { MLC::Source::AST::TypeDecl.new(name: name, type: type, type_params: type_params, exported: exported) }
    end

    # Parse trait declaration
    # trait Name<T> { fn method(...) -> Type }
    def parse_trait_decl(exported: false)
      trait_token = consume(:TRAIT)
      name = consume(:IDENTIFIER).value

      # Parse optional type parameters
      type_params = []
      if current.type == :OPERATOR && current.value == "<"
        consume(:OPERATOR)  # <
        type_params = parse_type_params
        expect_operator(">")
      end

      consume(:LBRACE)
      methods = parse_trait_methods
      consume(:RBRACE)

      with_origin(trait_token) do
        MLC::Source::AST::TraitDecl.new(
          name: name,
          type_params: type_params,
          methods: methods,
          exported: exported
        )
      end
    end

    # Parse methods inside a trait
    def parse_trait_methods
      methods = []

      while current.type != :RBRACE
        methods << parse_trait_method
      end

      methods
    end

    # Parse a single trait method signature
    # fn name(...) -> Type  or  fn name(...) -> Type = default_body
    def parse_trait_method
      fn_token = consume(:FN)
      name = consume(:IDENTIFIER).value

      consume(:LPAREN)
      params = parse_params
      consume(:RPAREN)

      consume(:ARROW)
      ret_type = parse_type

      # Optional default implementation
      body = nil
      if current.type == :EQUAL
        consume(:EQUAL)
        body = parse_expression
      end

      # Determine if static: no params or first param is not 'self'
      # For now, all trait methods are considered static (associated functions)
      # Instance methods would use @ syntax which we can add later
      is_static = true

      with_origin(fn_token) do
        MLC::Source::AST::TraitMethod.new(
          name: name,
          params: params,
          ret_type: ret_type,
          body: body,
          is_static: is_static
        )
      end
    end

    # Parse extend declaration
    # extend Type : Trait<T> { fn ... }
    # extend Type { fn ... }  (add methods without trait)
    def parse_extend_decl(exported: false)
      extend_token = consume(:EXTEND)
      target_type = parse_type

      trait_name = nil
      trait_params = []

      # Check for : Trait
      if current.type == :COLON
        consume(:COLON)
        trait_name = consume(:IDENTIFIER).value

        # Parse optional trait type parameters
        if current.type == :OPERATOR && current.value == "<"
          consume(:OPERATOR)  # <
          trait_params = parse_type_list
          expect_operator(">")
        end
      end

      consume(:LBRACE)
      methods = parse_extend_methods
      consume(:RBRACE)

      with_origin(extend_token) do
        MLC::Source::AST::ExtendDecl.new(
          target_type: target_type,
          trait_name: trait_name,
          trait_params: trait_params,
          methods: methods,
          exported: exported
        )
      end
    end

    # Parse methods inside extend block
    # Supports: fn ..., extern fn ...
    def parse_extend_methods
      methods = []

      while current.type != :RBRACE
        external = false
        if current.type == :EXTERN
          consume(:EXTERN)
          external = true
        end
        methods << parse_function(external: external)
      end

      methods
    end

    # Parse comma-separated list of types (for trait params)
    def parse_type_list
      types = []
      types << parse_type

      while current.type == :COMMA
        consume(:COMMA)
        types << parse_type
      end

      types
    end

    end
  end
  end
end
