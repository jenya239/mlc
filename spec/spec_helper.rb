# frozen_string_literal: true

RSpec.configure do |config|
  config.order = :random
  config.formatter = :documentation
  config.color = true
  config.full_backtrace = true
  config.example_status_persistence_file_path = ".rspec_status"

  config.expect_with :rspec do |c|
    c.syntax = :expect
  end

  config.disable_monkey_patching!
end
