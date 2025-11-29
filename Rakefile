# frozen_string_literal: true

require "rake/testtask"

# Full test suite (~20 min)
Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/**/*_test.rb"]
  t.verbose = true
end

# Fast tests - excludes integration/e2e tests (~2-3 min)
Rake::TestTask.new(:test_fast) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/**/*_test.rb"].exclude("test/integration/*_e2e_test.rb")
  t.verbose = true
end

# Unit tests only - excludes all integration tests (~1-2 min)
Rake::TestTask.new(:test_unit) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/**/*_test.rb"].exclude("test/integration/**/*_test.rb")
  t.verbose = true
end

# Integration tests only
Rake::TestTask.new(:test_integration) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/integration/**/*_test.rb"]
  t.verbose = true
end

# E2E tests only (compile and run binaries)
Rake::TestTask.new(:test_e2e) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/integration/*_e2e_test.rb"]
  t.verbose = true
end

task default: :test

