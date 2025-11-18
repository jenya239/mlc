# frozen_string_literal: true

# All classes autoloaded by Zeitwerk on-demand:
# - MLC::Common::Stdlib::Scanner (line 23)

module MLC
  module Common
    module Stdlib
    # Resolves stdlib module names to file paths
    # Now uses Scanner for automatic discovery
    class Resolver
    # DEPRECATED: Legacy constant for backward compatibility
    # Use Scanner for automatic module discovery
    STDLIB_MODULES = {
      'Math' => 'math.mlc',
      'IO' => 'io.mlc',
      'String' => 'string.mlc',
      'Conv' => 'conv.mlc',
      'Graphics' => 'graphics.mlc'
    }.freeze

    def initialize(stdlib_dir = nil, scanner: nil)
      @stdlib_dir = stdlib_dir || __dir__
      @scanner = scanner || MLC::Common::Stdlib::Scanner.new(@stdlib_dir)
    end

    # Check if a module name is a stdlib module
    def stdlib_module?(name)
      @scanner.module_exists?(name)
    end

    # Resolve a module name to a file path
    # Returns nil if not a stdlib module
    def resolve(name)
      @scanner.module_file_path(name)
    end

    # Get all available stdlib module names
    def available_modules
      @scanner.available_modules
    end

    # Get the scanner instance (for accessing function/type metadata)
    def scanner
      @scanner
    end
    end
    end
  end
end
