# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # CaptureAnalyzer - analyzes lambda bodies to find free variables (captures)
          #
          # Free variables are variables that are:
          # - Referenced in the lambda body
          # - NOT lambda parameters
          # - Defined in enclosing scope (exist in var_type_registry)
          #
          # For smart pointers:
          # - Shared<T> => capture by value (reference counting handles it)
          # - Owned<T>  => error (cannot capture unique ownership) or capture by move
          # - Weak<T>   => capture by value
          # - Other     => capture by value (default)
          class CaptureAnalyzer
            def initialize(var_type_registry:)
              @var_type_registry = var_type_registry
            end

            # Analyze lambda AST node and return captures array
            # @param node [AST::Lambda] lambda node
            # @return [Array<Hash>] array of {name: String, type: Type, mode: Symbol}
            def analyze(node)
              param_names = extract_param_names(node.params)
              free_vars = collect_free_vars(node.body, param_names)

              free_vars.map do |name|
                type = @var_type_registry.get(name)
                next unless type  # Skip if variable not in scope

                {
                  name: name,
                  type: type,
                  mode: determine_capture_mode(type)
                }
              end.compact
            end

            private

            # Extract parameter names from lambda params
            def extract_param_names(params)
              Set.new(params.map do |param|
                param.respond_to?(:name) ? param.name : param.to_s
              end)
            end

            # Collect all variable references in expression, excluding bound vars
            # @param expr [AST::Expr] expression to analyze
            # @param bound_vars [Set<String>] variables bound in current scope (params, let bindings)
            # @return [Set<String>] free variable names
            def collect_free_vars(expr, bound_vars)
              free_vars = Set.new
              visit(expr, bound_vars, free_vars)
              free_vars
            end

            # Visit AST node and collect free variables
            def visit(node, bound_vars, free_vars)
              return unless node

              case node
              when MLC::Source::AST::VarRef
                # Variable reference - free if not bound
                free_vars.add(node.name) unless bound_vars.include?(node.name)

              when MLC::Source::AST::Let
                # Let binding adds new bound variable
                visit(node.value, bound_vars, free_vars)
                new_bound = bound_vars.dup
                new_bound.add(node.name)
                visit(node.body, new_bound, free_vars) if node.respond_to?(:body) && node.body

              when MLC::Source::AST::Lambda
                # Nested lambda - add its params to bound vars for its body
                nested_params = extract_param_names(node.params)
                nested_bound = bound_vars | nested_params
                visit(node.body, nested_bound, free_vars)

              when MLC::Source::AST::Block
                # Block with sequential statements
                current_bound = bound_vars.dup
                node.statements.each do |stmt|
                  visit(stmt, current_bound, free_vars)
                  # If it's a let, add to bound for subsequent statements
                  if stmt.is_a?(MLC::Source::AST::Let)
                    current_bound.add(stmt.name)
                  elsif stmt.is_a?(MLC::Source::AST::VariableDecl)
                    current_bound.add(stmt.name)
                  end
                end

              when MLC::Source::AST::BlockExpr
                # Block expression with statements and result
                current_bound = bound_vars.dup
                node.statements.each do |stmt|
                  visit(stmt, current_bound, free_vars)
                  if stmt.is_a?(MLC::Source::AST::Let)
                    current_bound.add(stmt.name)
                  elsif stmt.is_a?(MLC::Source::AST::VariableDecl)
                    current_bound.add(stmt.name)
                  end
                end
                visit(node.result_expr, current_bound, free_vars)

              when MLC::Source::AST::DoExpr
                # Do block with statements
                current_bound = bound_vars.dup
                if node.respond_to?(:statements) && node.statements
                  node.statements.each do |stmt|
                    visit(stmt, current_bound, free_vars)
                    if stmt.is_a?(MLC::Source::AST::Let)
                      current_bound.add(stmt.name)
                    elsif stmt.is_a?(MLC::Source::AST::VariableDecl)
                      current_bound.add(stmt.name)
                    end
                  end
                end
                visit(node.result, current_bound, free_vars) if node.respond_to?(:result) && node.result

              when MLC::Source::AST::BinaryOp
                visit(node.left, bound_vars, free_vars)
                visit(node.right, bound_vars, free_vars)

              when MLC::Source::AST::UnaryOp
                visit(node.operand, bound_vars, free_vars)

              when MLC::Source::AST::Call
                visit(node.callee, bound_vars, free_vars)
                node.args.each { |arg| visit(arg, bound_vars, free_vars) }

              when MLC::Source::AST::MemberAccess
                visit(node.object, bound_vars, free_vars)

              when MLC::Source::AST::TupleAccess
                visit(node.tuple, bound_vars, free_vars)

              when MLC::Source::AST::IndexAccess
                visit(node.object, bound_vars, free_vars)
                visit(node.index, bound_vars, free_vars)

              when MLC::Source::AST::SliceAccess
                visit(node.object, bound_vars, free_vars)
                visit(node.start_index, bound_vars, free_vars) if node.respond_to?(:start_index) && node.start_index
                visit(node.end_index, bound_vars, free_vars) if node.respond_to?(:end_index) && node.end_index

              when MLC::Source::AST::IfExpr, MLC::Source::AST::IfStmt
                visit(node.condition, bound_vars, free_vars)
                visit(node.then_branch, bound_vars, free_vars)
                visit(node.else_branch, bound_vars, free_vars) if node.else_branch

              when MLC::Source::AST::MatchExpr
                visit(node.scrutinee, bound_vars, free_vars)
                node.arms.each do |arm|
                  # arm is {pattern:, guard:, body:}
                  pattern = arm[:pattern]
                  guard = arm[:guard]
                  body = arm[:body]

                  # Pattern bindings introduce new variables
                  pattern_bindings = extract_pattern_bindings(pattern)
                  arm_bound = bound_vars | pattern_bindings
                  visit(guard, arm_bound, free_vars) if guard
                  visit(body, arm_bound, free_vars)
                end

              when MLC::Source::AST::ArrayLiteral
                node.elements.each { |elem| visit(elem, bound_vars, free_vars) }

              when MLC::Source::AST::TupleLit
                node.elements.each { |elem| visit(elem, bound_vars, free_vars) }

              when MLC::Source::AST::RecordLit
                node.fields.each { |field| visit(field[:value], bound_vars, free_vars) }

              when MLC::Source::AST::ForLoop
                visit(node.iterable, bound_vars, free_vars)
                new_bound = bound_vars.dup
                new_bound.add(node.var_name)
                visit(node.body, new_bound, free_vars)

              when MLC::Source::AST::WhileLoop, MLC::Source::AST::WhileStmt
                visit(node.condition, bound_vars, free_vars)
                visit(node.body, bound_vars, free_vars)

              when MLC::Source::AST::ListComprehension
                # Process generators and conditions
                current_bound = bound_vars.dup
                if node.respond_to?(:generators) && node.generators
                  node.generators.each do |gen|
                    visit(gen.iterable, current_bound, free_vars)
                    current_bound.add(gen.var_name)
                    if gen.respond_to?(:condition) && gen.condition
                      visit(gen.condition, current_bound, free_vars)
                    end
                  end
                end
                visit(node.body, current_bound, free_vars)

              when MLC::Source::AST::PipeOp
                visit(node.left, bound_vars, free_vars)
                visit(node.right, bound_vars, free_vars)

              when MLC::Source::AST::RangeExpr
                visit(node.start_expr, bound_vars, free_vars) if node.respond_to?(:start_expr) && node.start_expr
                visit(node.end_expr, bound_vars, free_vars) if node.respond_to?(:end_expr) && node.end_expr

              when MLC::Source::AST::Return
                visit(node.value, bound_vars, free_vars) if node.value

              when MLC::Source::AST::ExprStmt
                visit(node.expr, bound_vars, free_vars) if node.respond_to?(:expr)

              when MLC::Source::AST::VariableDecl
                visit(node.value, bound_vars, free_vars) if node.respond_to?(:value) && node.value

              when MLC::Source::AST::Assignment
                visit(node.target, bound_vars, free_vars) if node.respond_to?(:target)
                visit(node.value, bound_vars, free_vars)

              when MLC::Source::AST::StringInterpolation
                node.parts.each { |part| visit(part, bound_vars, free_vars) } if node.respond_to?(:parts)

              when MLC::Source::AST::UnsafeBlock
                visit(node.body, bound_vars, free_vars) if node.respond_to?(:body)

              # Literals don't contain variable references
              when MLC::Source::AST::IntLit,
                   MLC::Source::AST::FloatLit,
                   MLC::Source::AST::StringLit,
                   MLC::Source::AST::RegexLit,
                   MLC::Source::AST::UnitLit
                # No variables to capture

              else
                # For unknown node types, try to visit common child accessors
                visit_children(node, bound_vars, free_vars)
              end
            end

            # Fallback method to visit children of unknown node types
            def visit_children(node, bound_vars, free_vars)
              return unless node.respond_to?(:each)
              node.each { |child| visit(child, bound_vars, free_vars) }
            rescue
              # Ignore errors for nodes that don't support iteration
            end

            # Extract variable names bound by a pattern
            # Pattern is MLC::Source::AST::Pattern with :kind and :data
            def extract_pattern_bindings(pattern)
              bindings = Set.new
              collect_pattern_bindings(pattern, bindings)
              bindings
            end

            def collect_pattern_bindings(pattern, bindings)
              return unless pattern
              return unless pattern.is_a?(MLC::Source::AST::Pattern)

              case pattern.kind
              when :wildcard
                # _ doesn't bind anything
              when :var
                # Variable pattern binds the name
                name = pattern.data[:name] || pattern.data
                bindings.add(name.to_s) if name && name != '_'
              when :constructor
                # Constructor pattern may have nested patterns
                args = pattern.data[:args] || []
                args.each { |arg| collect_pattern_bindings(arg, bindings) }
                fields = pattern.data[:fields] || []
                fields.each do |field|
                  if field.is_a?(Hash)
                    collect_pattern_bindings(field[:pattern], bindings) if field[:pattern]
                  else
                    collect_pattern_bindings(field, bindings)
                  end
                end
              when :or
                # For or-patterns, both sides should bind the same variables
                # Take bindings from first pattern
                alternatives = pattern.data[:patterns] || pattern.data
                collect_pattern_bindings(alternatives.first, bindings) if alternatives.is_a?(Array) && alternatives.first
              when :tuple, :array
                # Tuple/array patterns have nested patterns
                elements = pattern.data[:elements] || pattern.data
                elements.each { |elem| collect_pattern_bindings(elem, bindings) } if elements.is_a?(Array)
              when :literal
                # Literal patterns don't bind variables
              end
            end

            # Determine capture mode based on type
            # @param type [SemanticIR::Type] the type of the variable
            # @return [Symbol] :value or :ref
            def determine_capture_mode(type)
              # For now, capture everything by value
              # Smart pointers like Shared<T> work correctly with value capture
              # because copying a shared_ptr increments the reference count
              #
              # Future: could add :ref for large structs, :move for Owned<T>
              :value
            end
          end
        end
      end
    end
  end
end
