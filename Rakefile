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

# Только test/mlc/** — эталон для CI и README (~1100 runs); test_unit шире и может включать падающие suite вне mlc.
Rake::TestTask.new(:test_mlc) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/mlc/**/*_test.rb"]
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

# Self-hosted compiler: собирает compiler/tests/tests_main.mlc через Ruby и запускает run_tests (нужен g++).
desc "Тесты self-hosted компилятора (compiler/tests/build_tests.sh)"
task :test_compiler_mlc do
  sh "#{__dir__}/compiler/tests/build_tests.sh"
end

desc "Сборка mlcc (compiler/build.sh), затем triple-bootstrap (compiler/triple_bootstrap.sh); долго, нужны g++ и ~10+ мин"
task :triple_bootstrap do
  sh "#{__dir__}/compiler/build.sh"
  sh "#{__dir__}/compiler/triple_bootstrap.sh", "#{__dir__}/compiler/out/mlcc"
end

desc "test/mlc + тесты compiler/tests (рекомендуемый минимум для self-hosted). При MLC_TRIPLE_BOOTSTRAP=1 после этого выполняется triple_bootstrap"
task test_self_hosted_stack: %i[test_mlc test_compiler_mlc] do
  if ENV["MLC_TRIPLE_BOOTSTRAP"] == "1"
    Rake::Task["triple_bootstrap"].invoke
  end
end

# Optional RSpec task (only works if rspec gem is available)
begin
  require "rspec/core/rake_task"
  RSpec::Core::RakeTask.new(:spec)
rescue LoadError
  desc "Run RSpec examples (rspec gem not installed)"
  task :spec do
    abort "RSpec gem is not installed. Run `bundle install` to enable `rake spec`."
  end
end
