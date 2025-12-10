# frozen_string_literal: true

source "https://rubygems.org"

gem "minitest", "~> 5.0"
gem "rake", "~> 13.0"
gem "test-unit", "~> 3.6"
gem "zeitwerk", "~> 2.6"

group :development do
  gem "debug"
  gem "memory_profiler"
end

group :test do
  gem "rspec"
  # rspec-core is pulled in by rspec; kept explicit for clarity with existing tooling
  gem "rspec-core"
end

gem 'rubocop', '~> 1.62'
