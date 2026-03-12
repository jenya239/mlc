# frozen_string_literal: true

module MLC
  module Common
    module ModularCompilation
      # Resolves import paths and builds module dependency graph.
      # Step 1-6 of modular compilation plan.
      class ModuleGraph
        class CircularImportError < StandardError
          attr_reader :cycle

          def initialize(cycle)
            @cycle = cycle
            super("Circular import: #{cycle.join(' -> ')}")
          end
        end

        def initialize(root_dir: Dir.pwd)
          @root_dir = File.expand_path(root_dir)
          @nodes = {} # path => Node
          @edges = {} # path => [imported paths]
        end

        # Resolve import path relative to from_file.
        # @param import_path [String] e.g. "./foo", "../lib/bar"
        # @param from_file [String] path to file containing the import
        # @return [String] absolute path to .mlc file, or nil if not found
        def self.resolve_import_path(import_path, from_file:)
          dir = File.dirname(File.expand_path(from_file))
          base = if import_path.start_with?("./", "../")
                   File.expand_path(import_path, dir)
                 else
                   File.join(dir, import_path)
                 end
          path = base.end_with?(".mlc") ? base : "#{base}.mlc"
          File.exist?(path) ? path : nil
        end

        # Add file to graph (parse and extract imports).
        # @param path [String] absolute path to .mlc file
        # @return [Array<String>] imported paths (absolute)
        def add_file(path)
          abs = File.expand_path(path)
          return @edges[abs] if @edges.key?(abs)

          source = File.read(abs)
          imports = extract_imports(source, from_file: abs)
          @edges[abs] = imports
          imports.each { |imp| add_file(imp) unless @edges.key?(imp) }
          imports
        end

        # Topological order (dependencies first).
        # @return [Array<String>] paths in compile order
        # @raise [CircularImportError] if cycle detected
        def dependency_order
          visited = {}
          stack = {}
          path_stack = []
          result = []

          @edges.keys.each do |path|
            visit(path, visited, stack, path_stack, result) unless visited[path]
          end

          result
        end

        # @return [Hash] path => [imported paths]
        def graph
          @edges.dup
        end

        private

        def extract_imports(source, from_file:)
          imports = []
          import_re = /^\s*import\s+(?:\{[^}]*\}|\*\s+as\s+\w+)\s+from\s+"(\.\.?\/[^"]+)"/
          source.each_line do |line|
            m = line.match(import_re)
            next unless m

            resolved = self.class.resolve_import_path(m[1], from_file: from_file)
            imports << resolved if resolved && !imports.include?(resolved)
          end
          imports
        end

        def visit(path, visited, stack, path_stack, result)
          return if visited[path]

          if stack[path]
            cycle_start = path_stack.index(path)
            cycle = path_stack[cycle_start..] + [path]
            raise CircularImportError.new(cycle)
          end

          stack[path] = true
          path_stack << path
          (@edges[path] || []).each do |dep|
            visit(dep, visited, stack, path_stack, result)
          end
          path_stack.pop
          stack.delete(path)
          visited[path] = true
          result << path
        end

      end
    end
  end
end
