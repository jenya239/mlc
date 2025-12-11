# frozen_string_literal: true

begin
  require "simplecov"
  require "simplecov-cobertura"

  SimpleCov.formatters = [
    SimpleCov::Formatter::HTMLFormatter,
    SimpleCov::Formatter::CoberturaFormatter
  ]
  SimpleCov.start do
    enable_coverage :branch
    add_filter "/spec/"
    add_filter "/test/"
  end
rescue LoadError
  warn "SimpleCov not available; coverage report will not be generated"
end

RSpec.configure do |config|
  config.order = :random
  config.expect_with :rspec do |c|
    c.syntax = :expect
  end
  config.disable_monkey_patching!
end

