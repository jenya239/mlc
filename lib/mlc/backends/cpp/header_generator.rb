# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      # Generates C++ header (.hpp) and implementation (.cpp) files from SemanticIR modules
      class HeaderGenerator
        def initialize(lowering)
          @lowering = lowering
        end

        # Generate header and implementation files
        # Returns: { header: String, implementation: String }
        def generate(module_node, all_modules: nil)
          header_guard = generate_header_guard(module_node.name)
          dependencies = collect_required_import_paths(module_node)

          # Separate: header = exported only (step 23), impl = all non-external
          header_items = []
          impl_items = []
          impl_types = []  # non-exported types for .cpp

          module_node.items.each do |item|
            case item
            when SemanticIR::TypeDecl
              if item.exported
                header_items << item
              else
                impl_types << item
              end
            when SemanticIR::Func
              header_items << item if item.exported
              next if item.external
              impl_items << item
            end
          end

          # Generate header file
          header = generate_header(
            module_node: module_node,
            module_name: module_node.name,
            guard: header_guard,
            imports: module_node.imports,
            items: header_items,
            dependencies: dependencies,
            all_modules: all_modules
          )

          implementation = generate_implementation(
            module_node: module_node,
            module_name: module_node.name,
            imports: module_node.imports,
            items: impl_items,
            type_items: impl_types,
            required_imports: dependencies[:implementation]
          )

          { header: header, implementation: implementation }
        end

        private

        def generate_header_guard(module_name)
          # Convert Math::Vector -> MATH_VECTOR_HPP
          "#{module_name.upcase.gsub('::', '_').gsub('/', '_')}_HPP"
        end

        def generate_header(module_node:, module_name:, guard:, imports:, items:, dependencies:, all_modules: nil)
          lines = []

          # Header guard
          lines << "#ifndef #{guard}"
          lines << "#define #{guard}"
          lines << ""

          # Standard includes (mlc.hpp provides String, Array, etc.)
          lines << "#include \"mlc.hpp\""
          lines << "#include <variant>"
          lines << ""

          # Module imports -> #include statements
          header_imports = filtered_imports(imports, dependencies[:header])
          header_imports.each do |import|
            header_name = module_path_to_header(import.path)
            lines << "#include \"#{header_name}\""
          end
          lines << "" unless header_imports.empty?

          forward_decls = build_forward_declarations(module_name, items, dependencies)
          unless forward_decls.empty?
            lines.concat(forward_decls)
            lines << ""
          end

          cyclic = all_modules ? cyclic_sum_types_global(all_modules) : cyclic_sum_types(module_node)
          @lowering.cyclic_sum_types = cyclic

          # Namespace for module
          namespace = module_name_to_namespace(module_name)
          unless namespace.empty?
            lines << "namespace #{namespace} {"
            lines << ""
          end

          unless cyclic.empty?
            cyclic.each { |name| lines << "struct #{name};" }
            lines << ""
          end

          # Generate type definitions and functions
          items.each do |item|
            case item
            when SemanticIR::TypeDecl
              # Full type definition goes in header
              cpp_ast = @lowering.lower(item)
              lines << cpp_ast.to_source
              lines << ""
            when SemanticIR::Func
              lines << generate_function_declaration(item)
              lines << ""
            end
          end

          # Non-exported extend helpers that other modules call must appear in the
          # header (e.g. infer_result::InferResult_with_type). Restrict to names
          # generated for InferResult so we do not declare LexState_* with a type
          # that exists only in the .cpp.
          module_node&.items&.each do |item|
            next unless item.is_a?(SemanticIR::Func)
            next if item.external
            next if item.exported
            next unless item.name.match?(/\AInferResult_/)

            lines << generate_function_declaration(item)
            lines << ""
          end

          # Close namespace
          unless namespace.empty?
            lines << "} // namespace #{namespace}"
            lines << ""
          end

          # Close header guard
          lines << "#endif // #{guard}"
          lines << ""

          lines.join("\n")
        end

        def generate_implementation(module_node: nil, module_name:, imports:, items:, type_items: [], required_imports: nil)
          lines = []

          # Set user_functions so local calls are not qualified with wrong namespace
          if module_node
            func_names = module_node.items.grep(SemanticIR::Func).map(&:name).to_set
            @lowering.container.user_functions = func_names
          end

          # Include own header
          header_name = module_path_to_header(module_name)
          lines << "#include \"#{header_name}\""
          lines << ""

          # Additional includes for implementation
          impl_imports = filtered_imports(imports, required_imports)
          impl_imports.each do |import|
            header_name = module_path_to_header(import.path)
            lines << "#include \"#{header_name}\""
          end
          lines << "" unless impl_imports.empty?

          # Namespace for module
          namespace = module_name_to_namespace(module_name)
          unless namespace.empty?
            lines << "namespace #{namespace} {"
            lines << ""
          end

          # Using directives for imported modules (e.g. ast_tokens for TKind variants)
          impl_imports = filtered_imports(imports, required_imports)
          impl_imports.each do |import|
            imp_ns = module_name_to_namespace(File.basename(import.path, ".mlc"))
            lines << "using namespace #{imp_ns};" if imp_ns && !imp_ns.empty?
          end
          # ast re-exports ast_tokens; add it when we have ast
          if impl_imports.any? { |i| File.basename(i.path, ".mlc") == "ast" }
            lines << "using namespace ast_tokens;"
          end
          lines << "" if impl_imports.any?

          # Trait vtable structs for this module (before type defs)
          generate_trait_structs_for_module(module_node).each do |code|
            lines << code
            lines << ""
          end

          # Non-exported type definitions in implementation
          type_items.each do |item|
            cpp_ast = @lowering.lower(item)
            lines << cpp_ast.to_source
            lines << ""
          end

          # Forward declarations for internal functions (used before definition)
          items.each do |item|
            next unless item.is_a?(SemanticIR::Func)
            next if item.type_params.any?
            next if item.body.nil?

            lines << generate_function_declaration(item)
            lines << ""
          end

          # Trait adapter functions (after function forward declarations)
          generate_trait_adapters_for_module(module_node).each do |code|
            lines << code
            lines << ""
          end

          # Generate function implementations
          items.each do |item|
            case item
            when SemanticIR::Func
              func_impl = generate_function_implementation(item)
              lines << func_impl
              lines << ""
            end
          end

          # Close namespace
          unless namespace.empty?
            lines << "} // namespace #{namespace}"
            lines << ""
          end

          wrap_qualified_variants_in_ternary(lines.join("\n"))
        end

        # Wrap qualified variant refs in LexState_token(state, X) and similar - only
        # when used as direct function arg: , ast_tokens::Colon) -> , (ast_tokens::Colon{}))
        TOKEN_NULLARY = %w[Arrow FatArrow Pipe Bar Equal Question Spread Dot LParen RParen
          LBrace RBrace LBracket RBracket Comma Semicolon Colon Eof].freeze

        def wrap_qualified_variants_in_ternary(cpp)
          TOKEN_NULLARY.each do |v|
            cpp = cpp.gsub(/(, )(ast_tokens::#{v})(\))/, '\1(ast_tokens::' + v + '{})\3')
            cpp = cpp.gsub(/(, )(ast_tokens::#{v})(,)/, '\1(ast_tokens::' + v + '{})\3')
          end
          cpp
        end

        def filtered_imports(imports, required_imports)
          user_imports = imports.reject { |imp| stdlib_import?(imp.path) }
          return user_imports if required_imports.nil? || required_imports.empty?

          user_imports.select { |import| include_import?(import, required_imports) }
        end

        def include_import?(import, required_imports)
          path = import.path
          return false if stdlib_import?(path)
          return true if path.start_with?("./", "../", "/")

          required_imports.include?(path)
        end

        def stdlib_import?(path)
          return true if path.start_with?("std/")
          stdlib_aliases = %w[Option Result Array String Conv IO File Json Math Graphics]
          stdlib_aliases.include?(path)
        end

        def generate_function_declaration(func)
          ret_type = @lowering.send(:map_type, func.ret_type)
          name = @lowering.send(:sanitize_identifier, func.name)
          params = if @lowering.respond_to?(:build_function_parameters_cpp)
                     @lowering.build_function_parameters_cpp(func, include_default_values: true)
                   else
                     func.params.map do |param|
                       type = @lowering.send(:map_type, param.type)
                       "#{type} #{@lowering.send(:sanitize_identifier, param.name)}"
                     end
                   end.join(", ")

          effects = Array(func.effects)
          prefix = ""
          suffix = effects.include?(:noexcept) ? " noexcept" : ""

          lines = []
          type_params = func.type_params || []

          lines.concat(build_template_lines(type_params)) unless type_params.empty?

          lines << "#{prefix}#{ret_type} #{name}(#{params})#{suffix};"
          lines.join("\n")
        end

        def generate_function_implementation(func)
          effects = Array(func.effects)
          if effects.include?(:constexpr)
            new_effects = effects - [:constexpr]
            f = SemanticIR::Func.new(
              name: func.name, params: func.params, ret_type: func.ret_type, body: func.body,
              effects: new_effects, type_params: func.type_params, external: func.external,
              exported: func.exported, is_async: func.is_async
            )
            @lowering.lower(f).to_source
          else
            @lowering.lower(func).to_source
          end
        end

        def module_path_to_header(path)
          # Handle three cases:
          # 1. File path: "./foo" -> "foo.hpp" (step 18: same-dir include)
          # 2. File path: "../lib/bar" -> "bar.hpp" (basename for flat out_dir)
          # 3. Module name: "Math::Vector" -> "math/vector.hpp"

          if path.start_with?("./", "../", "/")
            base = File.basename(path, ".hpp")
            base.end_with?(".hpp") ? base : "#{base}.hpp"
          elsif path.include?("::")
            # Module path: Math::Vector -> math/vector.hpp
            "#{path.split('::').map(&:downcase).join('/')}.hpp"
          else
            # Simple name: Math -> math.hpp
            "#{path.downcase}.hpp"
          end
        end

        TRAIT_AST_TYPE_MAP = {
          "unit" => "void", "void" => "void", "string" => "mlc::String", "str" => "mlc::String",
          "bool" => "bool", "i32" => "int", "i64" => "int64_t", "i8" => "int8_t",
          "u32" => "uint32_t", "u64" => "uint64_t", "f32" => "float", "f64" => "double",
          "int" => "int"
        }.freeze

        def ast_type_to_cpp(ast_type)
          return "void" unless ast_type
          name = ast_type.respond_to?(:name) ? ast_type.name.to_s : ast_type.to_s
          TRAIT_AST_TYPE_MAP[name] || name
        end

        def generate_trait_structs_for_module(module_node)
          return [] unless module_node
          tr = @lowering.container.trait_registry
          return [] unless tr
          trait_names_in_module = collect_trait_names_in_module(module_node)
          return [] if trait_names_in_module.empty?
          tr.all_traits.select { |t| trait_names_in_module.include?(t.name) }.map do |trait_info|
            fields = trait_info.trait_methods.reject { |m| m[:name] == "self" }.map do |m|
              params = Array(m[:params]).reject { |p| p.respond_to?(:name) ? p.name == "self" : p[:name] == "self" }
              param_types = params.map do |p|
                ast_type_to_cpp(p.respond_to?(:type) ? p.type : p[:type])
              end.join(", ")
              ret_type = ast_type_to_cpp(m[:ret_type])
              "  std::function<#{ret_type}(#{param_types})> #{m[:name]};"
            end.join("\n")
            "struct #{trait_info.name} {\n#{fields}\n};"
          end
        end

        def generate_trait_adapters_for_module(module_node)
          return [] unless module_node
          tr = @lowering.container.trait_registry
          return [] unless tr
          type_registry = @lowering.container.type_registry
          adapters = []
          tr.all_traits.each do |trait_info|
            trait_name = trait_info.name
            tr.implementations_for_trait(trait_name).each do |impl|
              type_name = impl.type_name
              cpp_type = type_registry&.cpp_name(type_name) || type_name
              field_inits = trait_info.trait_methods.reject { |m| m[:name] == "self" }.map do |m|
                method_name = m[:name]
                params = Array(m[:params]).reject { |p| p.respond_to?(:name) ? p.name == "self" : p[:name] == "self" }
                param_decls = params.each_with_index.map do |p, i|
                  t = p.respond_to?(:type) ? p.type : p[:type]
                  "#{ast_type_to_cpp(t)} _p#{i}"
                end.join(", ")
                param_names = params.each_with_index.map { |_, i| "_p#{i}" }.join(", ")
                sep = param_names.empty? ? "" : ", "
                ret_type = ast_type_to_cpp(m[:ret_type])
                stmt = ret_type == "void" ? "" : "return "
                "  .#{method_name} = [self](#{param_decls}) noexcept { #{stmt}#{type_name}_#{method_name}(self#{sep}#{param_names}); }"
              end.join(",\n")
              adapters << "inline #{trait_name} #{type_name}_as_#{trait_name}(#{cpp_type} self) noexcept {\n  return #{trait_name}{\n#{field_inits}\n  };\n}"
            end
          end
          adapters
        end

        def collect_trait_names_in_module(module_node)
          # We consider all traits registered in the trait_registry as "in module"
          # (since traits are registered globally from source files)
          tr = @lowering.container.trait_registry
          return Set.new unless tr
          Set.new(tr.all_traits.map(&:name))
        end

        RESERVED_NAMESPACES = %w[main stdin stdout stderr errno].freeze

        def module_name_to_namespace(name)
          base = name.gsub("/", "::").split("::").map(&:downcase).join("::")
          RESERVED_NAMESPACES.include?(base) ? "mlc_#{base}" : base
        end

        def build_template_lines(type_params)
          template_params = type_params.map { |tp| "typename #{tp.name}" }.join(", ")
          requires_clause = build_requires_clause(type_params)
          lines = ["template<#{template_params}>"]
          lines << "requires #{requires_clause}" unless requires_clause.empty?
          lines
        end

        def build_requires_clause(type_params)
          clauses = []
          type_params.each do |tp|
            traits = tp.respond_to?(:trait_bounds) ? tp.trait_bounds : []
            traits = [tp.constraint].compact if traits.empty? && tp.constraint && !tp.constraint.empty?
            traits.each do |trait_name|
              clauses << "#{trait_name}<#{tp.name}>"
            end
          end
          clauses.compact.join(" && ")
        end

        def collect_required_import_paths(module_node)
          registry = @lowering.respond_to?(:function_registry) ? @lowering.function_registry : nil
          empty_result = { header: Set.new, implementation: Set.new, forward: Set.new }
          return empty_result unless registry

          @include_modules = Set.new
          @forward_modules = Set.new
          current_module = module_node.name

          module_node.items.each do |item|
            case item
            when MLC::SemanticIR::Func
              next unless item.body

              collect_modules_from_type(item.ret_type, current_module, require_include: true)
              item.params.each { |param| collect_modules_from_type(param.type, current_module, require_include: true) }
              collect_modules_from_expr(item.body, registry, current_module)
            when MLC::SemanticIR::TypeDecl
              type_info = @lowering.type_registry&.lookup(item.name)
              type_info&.referenced_type_names&.each do |type_name|
                module_name = @lowering.type_registry&.module_name_for(type_name)
                mark_module_dependency(module_name, current_module, require_include: true)
              end
            end
          end

          {
            header: @include_modules.dup,
            implementation: (@include_modules | @forward_modules),
            forward: @forward_modules.dup
          }
        ensure
          @include_modules = nil
          @forward_modules = nil
        end

        def cyclic_sum_types(module_node)
          type_registry = @lowering.type_registry
          return Set.new unless type_registry

          sum_decls = module_node.items.grep(SemanticIR::TypeDecl).select do |td|
            td.type.is_a?(SemanticIR::SumType) && td.type_params.empty?
          end
          return Set.new if sum_decls.empty?

          module_type_names = sum_decls.map(&:name).to_set
          refs = {}
          sum_decls.each do |td|
            info = type_registry.lookup(td.name)
            next unless info

            refs[td.name] = (info.referenced_type_names || []).select { |n| module_type_names.include?(n) }.to_set
          end

          cyclic = Set.new
          refs.each_key do |a|
            refs[a].each do |b|
              cyclic << a << b if refs[b]&.include?(a)
            end
          end
          cyclic
        end

        # Union of cyclic sum types across all modules (for cross-module match on wrapper types)
        def cyclic_sum_types_global(module_nodes)
          type_registry = @lowering.type_registry
          return Set.new unless type_registry

          module_nodes.flat_map { |mod| cyclic_sum_types(mod).to_a }.to_set
        end

        def build_forward_declarations(current_module, items, dependencies)
          type_registry = @lowering.type_registry
          return [] unless type_registry

          include_modules = dependencies[:header] || Set.new
          forward_modules = dependencies[:forward] || Set.new
          forward_lines = []
          seen = Set.new

          items.each do |item|
            next unless item.is_a?(MLC::SemanticIR::TypeDecl)

            info = type_registry.lookup(item.name)
            next unless info

            info.referenced_type_names.each do |type_name|
              next if seen.include?(type_name)

              referenced_module = type_registry.module_name_for(type_name)
              next unless referenced_module
              next if referenced_module == current_module
              next if include_modules.include?(referenced_module)
              next unless forward_modules.include?(referenced_module)

              decl = forward_declaration_for(type_registry.lookup(type_name))
              next unless decl

              forward_lines << decl
              seen << type_name
            end
          end

          forward_lines
        end

        def forward_declaration_for(type_info)
          return nil unless type_info

          case type_info.kind
          when :record, :sum
            "struct #{type_info.name};"
          end
        end

        def collect_modules_from_expr(expr, registry, current_module)
          return unless expr

          case expr
          when MLC::SemanticIR::CallExpr
            if expr.callee.is_a?(MLC::SemanticIR::VarExpr)
              entry = registry.fetch_entry(expr.callee.name)
              mark_module_dependency(entry&.module_name, current_module, require_include: true)
            else
              collect_modules_from_expr(expr.callee, registry, current_module)
            end
            expr.args.each { |arg| collect_modules_from_expr(arg, registry, current_module) }
          when MLC::SemanticIR::BinaryExpr
            collect_modules_from_expr(expr.left, registry, current_module)
            collect_modules_from_expr(expr.right, registry, current_module)
          when MLC::SemanticIR::UnaryExpr
            collect_modules_from_expr(expr.operand, registry, current_module)
          when MLC::SemanticIR::MemberExpr
            collect_modules_from_expr(expr.object, registry, current_module)
          when MLC::SemanticIR::IndexExpr
            collect_modules_from_expr(expr.object, registry, current_module)
            collect_modules_from_expr(expr.index, registry, current_module)
          when MLC::SemanticIR::RecordExpr
            expr.fields.each_value { |value| collect_modules_from_expr(value, registry, current_module) }
            collect_modules_from_type(expr.type, current_module, require_include: true)
          when MLC::SemanticIR::IfExpr
            collect_modules_from_expr(expr.condition, registry, current_module)
            collect_modules_from_expr(expr.then_branch, registry, current_module)
            collect_modules_from_expr(expr.else_branch, registry, current_module) if expr.else_branch
          when MLC::SemanticIR::MatchExpr
            collect_modules_from_expr(expr.scrutinee, registry, current_module)
            expr.arms.each do |arm|
              collect_modules_from_expr(arm[:guard], registry, current_module) if arm[:guard]
              collect_modules_from_expr(arm[:body], registry, current_module)
            end
          when MLC::SemanticIR::LambdaExpr
            collect_modules_from_expr(expr.body, registry, current_module)
          when MLC::SemanticIR::BlockExpr
            expr.statements.each { |stmt| collect_modules_from_statement(stmt, registry, current_module) }
            collect_modules_from_expr(expr.result, registry, current_module) if expr.result
          when MLC::SemanticIR::ArrayLiteralExpr
            expr.elements.each { |element| collect_modules_from_expr(element, registry, current_module) }
            collect_modules_from_type(expr.type, current_module)
          when MLC::SemanticIR::ListCompExpr
            Array(expr.generators).each do |gen|
              collect_modules_from_expr(gen[:iterable], registry, current_module) if gen.respond_to?(:[]) && gen[:iterable]
              collect_modules_from_expr(gen[:body], registry, current_module) if gen.respond_to?(:[]) && gen[:body]
            end
            Array(expr.filters).each { |filter| collect_modules_from_expr(filter, registry, current_module) }
            collect_modules_from_expr(expr.output_expr, registry, current_module)
          when MLC::SemanticIR::Block
            expr.stmts.each { |stmt| collect_modules_from_statement(stmt, registry, current_module) }
          when MLC::SemanticIR::VarExpr, MLC::SemanticIR::LiteralExpr, MLC::SemanticIR::UnitLiteral, MLC::SemanticIR::RegexExpr
            # no-op
          else
            collect_modules_from_type(expr.type, current_module) if expr.respond_to?(:type)
          end
        end

        def collect_modules_from_statement(stmt, registry, current_module)
          return unless stmt

          case stmt
          when MLC::SemanticIR::ExprStatement
            collect_modules_from_expr(stmt.expression, registry, current_module)
          when MLC::SemanticIR::Return
            collect_modules_from_expr(stmt.expr, registry, current_module)
          when MLC::SemanticIR::VariableDeclStmt
            collect_modules_from_expr(stmt.value, registry, current_module)
            collect_modules_from_type(stmt.type, current_module, require_include: true)
          when MLC::SemanticIR::AssignmentStmt
            collect_modules_from_expr(stmt.value, registry, current_module)
          when MLC::SemanticIR::IfStmt
            collect_modules_from_expr(stmt.condition, registry, current_module)
            collect_modules_from_statement(stmt.then_body, registry, current_module)
            collect_modules_from_statement(stmt.else_body, registry, current_module)
          when MLC::SemanticIR::ForStmt
            collect_modules_from_expr(stmt.iterable, registry, current_module)
            collect_modules_from_statement(stmt.body, registry, current_module)
          when MLC::SemanticIR::WhileStmt
            collect_modules_from_expr(stmt.condition, registry, current_module)
            collect_modules_from_statement(stmt.body, registry, current_module)
          when MLC::SemanticIR::MatchStmt
            collect_modules_from_expr(stmt.scrutinee, registry, current_module)
            stmt.arms.each do |arm|
              collect_modules_from_expr(arm[:guard], registry, current_module) if arm[:guard]
              collect_modules_from_expr(arm[:body], registry, current_module)
            end
          when MLC::SemanticIR::Block
            stmt.stmts.each { |inner| collect_modules_from_statement(inner, registry, current_module) }
          end
        end

        def collect_modules_from_type(type, current_module, require_include: false)
          return unless type

          case type
          when MLC::SemanticIR::GenericType
            collect_modules_from_type(type.base_type, current_module, require_include: require_include)
            type.type_args.each { |arg| collect_modules_from_type(arg, current_module, require_include: require_include) }
          when MLC::SemanticIR::ArrayType
            collect_modules_from_type(type.element_type, current_module, require_include: require_include)
          when MLC::SemanticIR::FunctionType
            type.params.each { |param| collect_modules_from_type(param[:type], current_module, require_include: true) }
            collect_modules_from_type(type.ret_type, current_module, require_include: true)
          when MLC::SemanticIR::RecordType, MLC::SemanticIR::SumType
            module_name = module_name_for_type(type)
            mark_module_dependency(module_name, current_module, require_include: true)
          when MLC::SemanticIR::Type
            module_name = module_name_for_type(type)
            mark_module_dependency(module_name, current_module, require_include: require_include || requires_definition?(type))
          end
        end

        def mark_module_dependency(module_name, current_module, require_include: false)
          return unless module_name
          return if module_name.empty?
          return if current_module && module_name == current_module

          if require_include
            @include_modules << module_name
            @forward_modules.delete(module_name)
          elsif !@include_modules.include?(module_name)
            @forward_modules << module_name
          end
        end

        def requires_definition?(type)
          type_info = lookup_type_info(type)
          return false unless type_info

          type_info.record? || type_info.sum?
        end

        def lookup_type_info(type)
          type_registry = @lowering.type_registry
          return nil unless type_registry

          type_name = type.respond_to?(:name) ? type.name : nil
          return nil unless type_name

          type_registry.lookup(type_name)
        end

        def module_name_for_type(type)
          lookup_type_info(type)&.module_name
        end
      end
    end
  end
end
