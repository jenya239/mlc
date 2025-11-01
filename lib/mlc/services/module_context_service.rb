# frozen_string_literal: true

module MLC
  module Services
    # ModuleContextService - Module and namespace context management
    # Phase 17-A: Module context methods extracted from FunctionTransformer
    # Phase 17-B: Import alias helper methods extracted from FunctionTransformer
    # Phase 17-E: Import alias context scoping extracted from FunctionTransformer
    #
    # Responsibilities:
    # - Track current module name and namespace
    # - Track current import aliases
    # - Derive module namespace from module name
    # - Provide scoped access to module and import alias context
    # - Build module alias keys for functions
    #
    # Dependencies: None (pure state management)
    class ModuleContextService
      attr_reader :current_module_name, :current_module_namespace, :current_import_aliases

      def initialize
        @current_module_name = nil
        @current_module_namespace = nil
        @current_import_aliases = {}
      end

      # Execute block within a module context
      # Restores previous context after block completes
      #
      # @param module_name [String] The module name to set as current
      # @yield Block to execute within module context
      # @return Result of the block
      def with_current_module(module_name)
        previous_name = @current_module_name
        previous_namespace = @current_module_namespace

        @current_module_name = module_name
        @current_module_namespace = derive_module_namespace(module_name)

        yield
      ensure
        @current_module_name = previous_name
        @current_module_namespace = previous_namespace
      end

      # Execute block with import aliases context
      # Restores previous context after block completes
      #
      # @param aliases [Hash] Import alias mapping
      # @yield Block to execute with import aliases
      # @return Result of the block
      def with_import_aliases(aliases)
        previous = @current_import_aliases
        @current_import_aliases = aliases
        yield
      ensure
        @current_import_aliases = previous
      end

      # Derive namespace from module name
      # Converts module path to lowercase namespace format
      #
      # Examples:
      #   "Math" => "math"
      #   "MyApp/Utils" => "myapp::utils"
      #   "main" => nil
      #
      # @param module_name [String] The module name
      # @return [String, nil] The derived namespace, or nil for main module
      def derive_module_namespace(module_name)
        return nil if module_name.nil? || module_name.empty? || module_name == "main"

        module_name
          .gsub("/", "::")
          .split("::")
          .map(&:downcase)
          .join("::")
      end

      # Build module alias keys for a function
      # Combines function name with all possible module prefixes (name and alias)
      #
      # @param function_name [String] The function name
      # @param module_info [ModuleInfo] The module information
      # @param module_alias [String] The module alias (optional)
      # @return [Array<String>] List of fully qualified names
      #
      # Examples:
      #   build_module_alias_keys("foo", module_info("Math"), "M")
      #   => ["Math.foo", "M.foo"]
      def build_module_alias_keys(function_name, module_info, module_alias)
        return [] unless function_name

        prefixes = module_alias_prefixes(module_info&.name, module_alias)
        prefixes.map { |prefix| "#{prefix}.#{function_name}" }
      end

      # Get all module prefixes (name and alias)
      # Returns unique list of module name and alias
      #
      # @param module_name [String] The module name (optional)
      # @param module_alias [String] The module alias (optional)
      # @return [Array<String>] List of prefixes
      #
      # Examples:
      #   module_alias_prefixes("Math", "M") => ["Math", "M"]
      #   module_alias_prefixes("Math", nil) => ["Math"]
      #   module_alias_prefixes(nil, "M") => ["M"]
      def module_alias_prefixes(module_name, module_alias)
        prefixes = []
        prefixes << module_name if module_name && !module_name.empty?
        prefixes << module_alias if module_alias && !module_alias.empty?
        prefixes.uniq
      end
    end
  end
end
