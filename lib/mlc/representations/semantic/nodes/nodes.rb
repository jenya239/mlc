# frozen_string_literal: true

module MLC
  module SemanticIR
    # Base node with origin tracking
    class Node
      attr_reader :origin
      
      def initialize(origin: nil)
        @origin = origin
      end
    end
    
    # Types
    class Type < Node
      attr_reader :kind, :name

      def initialize(kind:, name:, origin: nil)
        super(origin: origin)
        @kind = kind  # :prim/:record/:func/:unit
        @name = name
      end

      def primitive?
        @kind == :prim
      end

      def record?
        @kind == :record
      end

      def function?
        @kind == :func
      end

      def unit?
        @kind == :unit
      end

      def opaque?
        @kind == :opaque
      end

      def generic?
        @kind == :generic
      end

      def type_variable?
        @kind == :type_var
      end

      def array?
        @kind == :array
      end

      def tuple?
        @kind == :tuple
      end

      def symbol?
        @kind == :symbol
      end
    end
    
    # Record type with fields
    class RecordType < Type
      attr_reader :fields

      def initialize(name:, fields:, origin: nil)
        super(kind: :record, name: name, origin: origin)
        @fields = fields  # Array of {name: String, type: Type}
      end
    end

    # Sum type (variant/tagged union)
    class SumType < Type
      attr_reader :variants

      def initialize(name:, variants:, origin: nil)
        super(kind: :sum, name: name, origin: origin)
        @variants = variants  # Array of {name: String, fields: Array of {name:, type:}}
      end
    end

    # Function type
    class FunctionType < Type
      attr_reader :params, :ret_type

      def initialize(params:, ret_type:, origin: nil)
        super(kind: :func, name: "function", origin: origin)
        @params = params  # Array of {name: String, type: Type}
        @ret_type = ret_type
      end
    end

    # Unit type (void/empty type for side-effects)
    # Represents expressions that don't return a meaningful value
    # Example: if without else, statements, etc.
    class UnitType < Type
      def initialize(origin: nil)
        super(kind: :unit, name: "unit", origin: nil)
      end

      def unit?
        true
      end
    end

    # Error type - "poison" type for error recovery
    # Used when type checking encounters an error but wants to continue
    # Allows collecting multiple errors instead of stopping at first
    class ErrorType < Type
      attr_reader :error_message

      def initialize(error_message: nil, origin: nil)
        super(kind: :error, name: "<error>", origin: origin)
        @error_message = error_message
      end

      def error?
        true
      end

      # ErrorType is compatible with everything to prevent cascading errors
      def compatible_with?(_other)
        true
      end
    end

    # Opaque type - type without known structure in MLC
    # Represented as pointer in C++, can only come from extern/stdlib
    # Example: Window, DrawContext from Graphics module
    class OpaqueType < Type
      def initialize(name:, origin: nil)
        super(kind: :opaque, name: name, origin: origin)
      end

      def opaque?
        true
      end
    end

    # Reference type - &T (immutable borrow)
    # Only allowed in unsafe blocks, maps to const T& in C++
    class RefType < Type
      attr_reader :inner_type

      def initialize(inner_type:, origin: nil)
        super(kind: :ref, name: "ref", origin: origin)
        @inner_type = inner_type
      end

      def ref?
        true
      end
    end

    # Mutable reference type - &mut T
    # Only allowed in unsafe blocks, maps to T& in C++
    class MutRefType < Type
      attr_reader :inner_type

      def initialize(inner_type:, origin: nil)
        super(kind: :mut_ref, name: "mut_ref", origin: origin)
        @inner_type = inner_type
      end

      def mut_ref?
        true
      end
    end

    # Modules
    class Module < Node
      attr_reader :name, :items, :imports

      def initialize(name:, items:, imports: [], origin: nil)
        super(origin: origin)
        @name = name          # String or nil (for main/anonymous modules)
        @imports = imports    # Array of Import
        @items = items        # Array of declarations
      end
    end

    # Import declaration
    class Import < Node
      attr_reader :path, :items

      def initialize(path:, items: nil, origin: nil)
        super(origin: origin)
        @path = path    # String (e.g., "Math::Vector")
        @items = items  # nil (import all) or Array of String (selective import)
      end
    end

    # Alias for compatibility
    Program = Module
    
    # Function declaration
    class Func < Node
      attr_reader :name, :params, :ret_type, :body, :effects, :type_params, :external, :exported, :is_async

      def initialize(name:, params:, ret_type:, body: nil, effects: [], type_params: [], external: false, exported: false, is_async: false, origin: nil)
        super(origin: origin)
        @name = name
        @params = params  # Array of Param
        @ret_type = ret_type
        @body = body
        @effects = effects  # Array of :noexcept, :constexpr, etc.
        @type_params = type_params  # Array of TypeParam
        @external = external  # Boolean - is this an external (C++) function?
        @exported = exported  # Boolean - is this exported?
        @is_async = is_async  # Boolean - is this an async function (coroutine)?
      end
    end
    
    # Function parameter
    class Param < Node
      attr_reader :name, :type
      
      def initialize(name:, type:, origin: nil)
        super(origin: origin)
        @name = name
        @type = type
      end
    end
    
    # Expressions (normalized, no sugar)
    class Expr < Node
      attr_reader :kind, :data, :type
      
      def initialize(kind:, data:, type: nil, origin: nil)
        super(origin: origin)
        @kind = kind  # :lit/:var/:call/:binary/:let
        @data = data
        @type = type
      end
    end
    
    # Literal expression
    class LiteralExpr < Expr
      attr_reader :value

      def initialize(value:, type:, origin: nil)
        super(kind: :lit, data: value, type: type, origin: origin)
        @value = value
      end
    end

    # Unit literal - represents void/unit type ()
    class UnitLiteral < Expr
      def initialize(origin: nil)
        super(kind: :unit_lit, data: nil, type: UnitType.new, origin: origin)
      end
    end

    # Regex literal expression
    class RegexExpr < Expr
      attr_reader :pattern, :flags

      def initialize(pattern:, flags: "", type:, origin: nil)
        super(kind: :regex, data: {pattern: pattern, flags: flags}, type: type, origin: origin)
        @pattern = pattern
        @flags = flags
      end
    end

    # Variable reference
    class VarExpr < Expr
      attr_reader :name
      
      def initialize(name:, type:, origin: nil)
        super(kind: :var, data: name, type: type, origin: origin)
        @name = name
      end
    end
    
    # Binary operation
    class BinaryExpr < Expr
      attr_reader :op, :left, :right
      
      def initialize(op:, left:, right:, type:, origin: nil)
        super(kind: :binary, data: {op: op, left: left, right: right}, type: type, origin: origin)
        @op = op
        @left = left
        @right = right
      end
    end

    # Unary operation
    class UnaryExpr < Expr
      attr_reader :op, :operand

      def initialize(op:, operand:, type:, origin: nil)
        super(kind: :unary, data: {op: op, operand: operand}, type: type, origin: origin)
        @op = op
        @operand = operand
      end
    end
    
    # Function call
    class CallExpr < Expr
      attr_reader :callee, :args
      
      def initialize(callee:, args:, type:, origin: nil)
        super(kind: :call, data: {callee: callee, args: args}, type: type, origin: origin)
        @callee = callee
        @args = args
      end
    end
    
    # Member access
    class MemberExpr < Expr
      attr_reader :object, :member

      def initialize(object:, member:, type:, origin: nil)
        super(kind: :member, data: {object: object, member: member}, type: type, origin: origin)
        @object = object
        @member = member
      end
    end

    # Safe member access (optional chaining): obj?.member
    # When object is Option<T>, returns Option<U> where U is member's type
    class SafeMemberExpr < Expr
      attr_reader :object, :member

      def initialize(object:, member:, type:, origin: nil)
        super(kind: :safe_member, data: {object: object, member: member}, type: type, origin: origin)
        @object = object
        @member = member
      end
    end

    # Safe call (optional chaining): obj?.method(args)
    # When object is Option<T>, returns Option<U> where U is method's return type
    class SafeCallExpr < Expr
      attr_reader :object, :method_name, :args

      def initialize(object:, method_name:, args:, type:, origin: nil)
        super(kind: :safe_call, data: {object: object, method_name: method_name, args: args}, type: type, origin: origin)
        @object = object
        @method_name = method_name
        @args = args
      end
    end

    # Try expression (error propagation): expr?
    # If expr is None/Err, returns early from enclosing function
    # If expr is Some(v)/Ok(v), unwraps to v
    # Like Rust's ? operator
    class TryExpr < Expr
      attr_reader :operand

      def initialize(operand:, type:, origin: nil)
        super(kind: :try_expr, data: {operand: operand}, type: type, origin: origin)
        @operand = operand
      end
    end

    # Await expression: await expr
    # Suspends execution until the awaited future/promise completes
    # Can only be used inside async functions
    # Lowers to C++20 co_await
    class AwaitExpr < Expr
      attr_reader :operand

      def initialize(operand:, type:, origin: nil)
        super(kind: :await_expr, data: {operand: operand}, type: type, origin: origin)
        @operand = operand
      end
    end

    # Index access (array indexing)
    class IndexExpr < Expr
      attr_reader :object, :index

      def initialize(object:, index:, type:, origin: nil)
        super(kind: :index, data: {object: object, index: index}, type: type, origin: origin)
        @object = object  # Expr - array being indexed
        @index = index    # Expr - index expression
      end
    end

    # Slice access (array slicing): arr[start..end], arr[start..], arr[..end], arr[..]
    class SliceExpr < Expr
      attr_reader :object, :start_index, :end_index

      def initialize(object:, start_index: nil, end_index: nil, type:, origin: nil)
        super(kind: :slice, data: {object: object, start_index: start_index, end_index: end_index}, type: type, origin: origin)
        @object = object          # Expr - array being sliced
        @start_index = start_index # Expr or nil - start index (nil = from beginning)
        @end_index = end_index     # Expr or nil - end index (nil = to end, exclusive)
      end
    end

    # Range expression: 1..10, 1..<10
    class RangeExpr < Expr
      attr_reader :start_expr, :end_expr, :inclusive

      def initialize(start_expr:, end_expr:, inclusive: true, type:, origin: nil)
        super(kind: :range, data: {start: start_expr, end: end_expr, inclusive: inclusive}, type: type, origin: origin)
        @start_expr = start_expr  # Expr - start of range
        @end_expr = end_expr      # Expr - end of range
        @inclusive = inclusive    # true for .., false for ..<
      end
    end

    # Record literal
    class RecordExpr < Expr
      attr_reader :type_name, :fields

      def initialize(type_name:, fields:, type:, origin: nil)
        super(kind: :record, data: {type_name: type_name, fields: fields}, type: type, origin: origin)
        @type_name = type_name
        @fields = fields  # Hash of {field_name => value}
      end
    end

    # If expression
    class IfExpr < Expr
      attr_reader :condition, :then_branch, :else_branch

      def initialize(condition:, then_branch:, else_branch:, type:, origin: nil)
        super(kind: :if, data: {condition: condition, then_branch: then_branch, else_branch: else_branch}, type: type, origin: origin)
        @condition = condition
        @then_branch = then_branch
        @else_branch = else_branch
      end
    end

    # Match expression
    class MatchExpr < Expr
      attr_reader :scrutinee, :arms

      def initialize(scrutinee:, arms:, type:, origin: nil)
        super(kind: :match, data: {scrutinee: scrutinee, arms: arms}, type: type, origin: origin)
        @scrutinee = scrutinee  # Expression being matched
        @arms = arms  # Array of {pattern:, guard:, body:}
      end
    end

    # Statements
    class Stmt < Node
    end
    
    # Block of statements
    class Block < Stmt
      attr_reader :stmts
      alias_method :statements, :stmts

      def initialize(stmts:, origin: nil)
        super(origin: origin)
        @stmts = stmts
      end

      # Block (statement) doesn't have a result expression
      def result
        nil
      end
    end

    # Match statement (void-returning match expression)
    class MatchStmt < Stmt
      attr_reader :scrutinee, :arms

      def initialize(scrutinee:, arms:, origin: nil)
        super(origin: origin)
        @scrutinee = scrutinee
        @arms = arms
      end
    end
    
    # Return statement
    class Return < Stmt
      attr_reader :expr

      def initialize(expr: nil, origin: nil)
        super(origin: origin)
        @expr = expr
      end
    end

    # Expression used as statement
    class ExprStatement < Stmt
      attr_reader :expression

      def initialize(expression:, origin: nil)
        super(origin: origin)
        @expression = expression
      end
    end

    # Break statement
    class BreakStmt < Stmt
      def initialize(origin: nil)
        super(origin: origin)
      end
    end

    # Continue statement
    class ContinueStmt < Stmt
      def initialize(origin: nil)
        super(origin: origin)
      end
    end

    # Variable declaration statement
    class VariableDeclStmt < Stmt
      attr_reader :name, :type, :value, :mutable

      def initialize(name:, type:, value:, mutable:, origin: nil)
        super(origin: origin)
        @name = name
        @type = type
        @value = value
        @mutable = mutable
      end
    end

    # Destructuring declaration statement
    # Desugars to multiple VariableDeclStmt statements
    # bindings: array of {name:, type:, accessor:} hashes
    #   accessor is either TupleAccessExpr or MemberExpr for extracting values
    class DestructuringDeclStmt < Stmt
      attr_reader :bindings, :value, :mutable

      def initialize(bindings:, value:, mutable:, origin: nil)
        super(origin: origin)
        @bindings = bindings  # Array of {name:, type:, accessor:}
        @value = value
        @mutable = mutable
      end
    end

    # Assignment statement
    class AssignmentStmt < Stmt
      attr_reader :target, :value

      def initialize(target:, value:, origin: nil)
        super(origin: origin)
        @target = target
        @value = value
      end
    end
    
    # Type declaration
    class TypeDecl < Node
      attr_reader :name, :type, :type_params, :exported

      def initialize(name:, type:, type_params: [], exported: false, origin: nil)
        super(origin: origin)
        @name = name
        @type = type
        @type_params = type_params  # Array of TypeParam
        @exported = exported  # Boolean - is this exported?
      end
    end

    # Type parameter metadata for generics
    class TypeParam < Node
      attr_reader :name, :constraint

      def initialize(name:, constraint: nil, origin: nil)
        super(origin: origin)
        @name = name
        @constraint = constraint
      end
    end

    # Lambda expression (anonymous function)
    class LambdaExpr < Expr
      attr_reader :captures, :params, :body, :function_type

      def initialize(captures:, params:, body:, function_type:, origin: nil)
        super(kind: :lambda, data: {params: params, body: body}, type: function_type, origin: origin)
        @captures = captures      # Array of {name: String, type: Type, mode: :value/:ref}
        @params = params          # Array of Param (fully typed)
        @body = body              # Expr
        @function_type = function_type  # FunctionType
      end
    end

    # Block expression containing statements and final expression
    class BlockExpr < Expr
      attr_reader :statements, :result
      attr_accessor :desugared_expr

      def initialize(statements:, result:, type:, origin: nil)
        super(kind: :block, data: {}, type: type, origin: origin)
        @statements = statements
        @result = result
        @desugared_expr = nil
      end
    end

    # Unsafe block expression - allows reference types and raw pointers
    # unsafe { body }
    class UnsafeBlockExpr < Expr
      attr_reader :body

      def initialize(body:, type:, origin: nil)
        super(kind: :unsafe_block, data: {}, type: type, origin: origin)
        @body = body  # BlockExpr
      end
    end

    # For loop statement
    class ForStmt < Stmt
      attr_reader :var_name, :var_type, :iterable, :body

      def initialize(var_name:, var_type:, iterable:, body:, origin: nil)
        super(origin: origin)
        @var_name = var_name
        @var_type = var_type
        @iterable = iterable
        @body = body
      end
    end

    # If statement
    class IfStmt < Stmt
      attr_reader :condition, :then_body, :else_body

      def initialize(condition:, then_body:, else_body:, origin: nil)
        super(origin: origin)
        @condition = condition
        @then_body = then_body
        @else_body = else_body
      end
    end

    # While statement
    class WhileStmt < Stmt
      attr_reader :condition, :body

      def initialize(condition:, body:, origin: nil)
        super(origin: origin)
        @condition = condition
        @body = body
      end
    end

    # List comprehension desugars to loop + push
    class ListCompExpr < Expr
      attr_reader :element_type, :generators, :filters, :output_expr

      def initialize(element_type:, generators:, filters:, output_expr:, type:, origin: nil)
        super(kind: :list_comp, data: {}, type: type, origin: origin)
        @element_type = element_type
        @generators = generators
        @filters = filters
        @output_expr = output_expr
      end
    end

    # Array literal
    class ArrayLiteralExpr < Expr
      attr_reader :elements

      def initialize(elements:, type:, origin: nil)
        super(kind: :array_lit, data: elements, type: type, origin: origin)
        @elements = elements  # Array of Expr
      end
    end

    # Generic type - instantiated generic type with concrete type arguments
    # Example: Option<i32>, Result<String, Error>, Vec<T> (where T is bound)
    class GenericType < Type
      attr_reader :base_type, :type_args

      def initialize(base_type:, type_args:, origin: nil)
        super(kind: :generic, name: base_type.name, origin: origin)
        @base_type = base_type     # Type (the generic type itself, e.g., Option, Result)
        @type_args = type_args     # Array of Type (concrete types for substitution)
      end

      def generic?
        true
      end
    end

    # Type variable - reference to a type parameter
    # Example: T in fn foo<T>(x: T) -> T, the T in parameter/return type
    class TypeVariable < Type
      attr_reader :constraint

      def initialize(name:, constraint: nil, origin: nil)
        super(kind: :type_var, name: name, origin: origin)
        @constraint = constraint  # Optional Type for constraints (e.g., T: Display)
      end

      def type_variable?
        true
      end
    end

    # Array type
    class ArrayType < Type
      attr_reader :element_type

      def initialize(element_type:, origin: nil)
        super(kind: :array, name: "array", origin: origin)
        @element_type = element_type
      end
    end

    # Map type - Map<K, V> for key-value storage
    # Used for dynamic records and dictionaries
    class MapType < Type
      attr_reader :key_type, :value_type

      def initialize(key_type:, value_type:, origin: nil)
        super(kind: :map, name: "map", origin: origin)
        @key_type = key_type
        @value_type = value_type
      end

      def map?
        true
      end
    end

    # Tuple type - (T1, T2, ...)
    class TupleType < Type
      attr_reader :element_types

      def initialize(element_types:, origin: nil)
        super(kind: :tuple, name: "tuple", origin: origin)
        @element_types = element_types  # Array of Type
      end

      def tuple?
        true
      end
    end

    # Tuple literal expression
    class TupleExpr < Expr
      attr_reader :elements

      def initialize(elements:, type:, origin: nil)
        super(kind: :tuple, data: elements, type: type, origin: origin)
        @elements = elements  # Array of Expr
      end
    end

    # Tuple element access expression: tuple.0, tuple.1
    # Compile-time indexed access into tuple
    class TupleAccessExpr < Expr
      attr_reader :tuple, :index

      def initialize(tuple:, index:, type:, origin: nil)
        super(kind: :tuple_access, data: {tuple: tuple, index: index}, type: type, origin: origin)
        @tuple = tuple  # Expr - the tuple being accessed
        @index = index  # Integer - the positional index (0-based)
      end
    end

    # Symbol type - interned string for fast comparison
    class SymbolType < Type
      def initialize(origin: nil)
        super(kind: :symbol, name: "Symbol", origin: origin)
      end

      def symbol?
        true
      end
    end

    # Symbol literal expression
    class SymbolExpr < Expr
      attr_reader :name

      def initialize(name:, type:, origin: nil)
        super(kind: :symbol, data: name, type: type, origin: origin)
        @name = name  # String - symbol name without colon
      end
    end
  end
end
