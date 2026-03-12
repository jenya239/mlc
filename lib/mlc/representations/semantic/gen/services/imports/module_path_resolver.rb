# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # ModulePathResolver - Phase 24-D
          # Resolves import paths to .mlcmeta file paths
          #
          # Responsibilities:
          # - Handle relative paths (./module, ../module)
          # - Handle absolute module names (Module)
          # - Resolve paths relative to source file directory
          #
          # Usage:
          #   resolver = ModulePathResolver.new(source_file_dir: "/path/to/src")
          #   metadata_path = resolver.resolve("./math")
          #   # => "/path/to/src/math.mlcmeta"
          class ModulePathResolver
            attr_writer :source_file_dir

            def initialize(source_file_dir: nil)
              @source_file_dir = source_file_dir || Dir.pwd
            end

            # Resolve import path to module name (from filename).
            # @param import_path [String] e.g. "./bar", "../lib/foo"
            # @return [String, nil] e.g. "bar", "foo", or nil if source not found
            def resolve_module_name(import_path)
              path = resolve_source(import_path)
              path ? File.basename(path, ".mlc") : nil
            end

            # Resolve import path to .mlc source file path (returns nil if not found)
            def resolve_source(import_path)
              base = if import_path.start_with?("./", "../")
                       File.expand_path(import_path, @source_file_dir)
                     else
                       File.join(@source_file_dir, import_path)
                     end
              path = "#{base}.mlc"
              File.exist?(path) ? path : nil
            end

            def resolve(import_path)
              # Handle relative paths
              if import_path.start_with?("./", "../")
                abs_path = File.expand_path(import_path, @source_file_dir)
                metadata_path = "#{abs_path}.mlcmeta"
                return metadata_path if File.exist?(metadata_path)

                # Try lowercase fallback
                lowercase_path = "#{abs_path.downcase}.mlcmeta"
                return lowercase_path if File.exist?(lowercase_path)

                return metadata_path # Return original for error handling
              end

              # Handle absolute module names (search in current directory)
              # Try exact case first
              exact_path = File.join(@source_file_dir, "#{import_path}.mlcmeta")
              return exact_path if File.exist?(exact_path)

              # Try lowercase
              lower_path = File.join(@source_file_dir, "#{import_path.downcase}.mlcmeta")
              return lower_path if File.exist?(lower_path)

              # Default to exact case
              exact_path
            end
          end
        end
      end
    end
  end
end
