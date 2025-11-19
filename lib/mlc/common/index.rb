# frozen_string_literal: true

# Entry point for MLC compiler library
# Provides module-level API for compilation pipeline with Zeitwerk autoloading

require "zeitwerk"

# Define root module before Zeitwerk setup
module MLC
end

# Setup Zeitwerk loader for MLC
loader = Zeitwerk::Loader.new
loader.tag = "mlc"

# Push lib/mlc as the root namespace directory
loader.push_dir(File.expand_path("../..", __FILE__), namespace: MLC)

# Configure inflections for acronyms (keep them uppercase)
loader.inflector.inflect(
  "ast" => "AST",          # lib/mlc/source/ast → MLC::Source::AST
  "cpp" => "Cpp"           # lib/mlc/backends/cpp → MLC::Backends::Cpp
)

# Ignore files that don't follow Zeitwerk conventions
loader.ignore("#{__dir__}/../representations/semantic/nodes")  # Manual loading needed
loader.ignore("#{__dir__}/../representations/semantic/semantic_gen.rb")

# Setup the loader
loader.setup

# Manually require files that don't follow Zeitwerk conventions
# errors.rb and compiler_api.rb are in root MLC module (not MLC::Common)
# They must be required manually since they define core exception classes and module methods
require_relative "../errors"
require_relative "compiler_api"

# Zeitwerk-ignored files must be loaded explicitly
require_relative "../source/ast/nodes"
require_relative "../representations/semantic/nodes/nodes"
require_relative "../representations/semantic/nodes/builder"
require_relative "../representations/semantic/semantic_gen"
