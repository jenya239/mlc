#!/usr/bin/env ruby
# frozen_string_literal: true

require 'fileutils'
require 'pathname'

# Автоматическая миграция структуры lib/mlc/
class StructureMigrator
  MIGRATION_STEPS = [
    {
      name: 'parser',
      from: 'parser',
      to: 'source/parser',
      description: 'Parser files (наименее зависимые)'
    },
    {
      name: 'semantic_ir',
      from: 'semantic_ir',
      to: 'representations/semantic/nodes',
      description: 'Semantic IR nodes (переименовано в nodes)'
    },
    {
      name: 'analysis',
      from: 'analysis',
      to: 'common/analysis',
      description: 'Analysis passes'
    },
    {
      name: 'ast',
      from: 'ast',
      to: 'source/ast',
      description: 'AST nodes'
    },
    {
      name: 'diagnostics',
      from: 'diagnostics',
      to: 'common/diagnostics',
      description: 'Diagnostics (event bus, logger, formatter, errors)'
    },
    {
      name: 'compiler',
      from: 'compiler',
      to: 'tools/compiler',
      description: 'Compiler support (stdlib scanner, resolver, registry, metadata loader)'
    },
    {
      name: 'core',
      from: 'core',
      to: 'common/registry',
      description: 'Registries (FunctionRegistry, TypeRegistry, FunctionSignature)'
    },
    {
      name: 'type_system',
      from: 'type_system',
      to: 'common/typing',
      description: 'Type system (constraint solver, match analyzer, generic resolver, effect analyzer)'
    },
    {
      name: 'semantic_gen',
      from: 'semantic_gen',
      to: 'representations/semantic/gen',
      description: 'Semantic generation (transformation pipeline)'
    }
  ]

  def initialize(root_dir, dry_run: false, run_tests: true, quick_check: false)
    @root_dir = Pathname.new(root_dir)
    @dry_run = dry_run
    @run_tests = run_tests
    @quick_check = quick_check
    @completed_steps = []
  end

  def migrate
    puts "=== Миграция структуры lib/mlc/ ==="
    puts "Root: #{@root_dir}"
    puts "Dry run: #{@dry_run}"
    puts "Run tests: #{@run_tests}"
    puts "Quick check: #{@quick_check}" if @quick_check
    puts

    if @dry_run
      puts "DRY RUN MODE - изменения НЕ будут применены"
      puts
    end

    create_directory_structure

    MIGRATION_STEPS.each_with_index do |step, i|
      puts "\n#{'=' * 80}"
      puts "Step #{i + 1}/#{MIGRATION_STEPS.size}: #{step[:name]}"
      puts "  #{step[:from]} → #{step[:to]}"
      puts "  #{step[:description]}"
      puts

      execute_step(step)
      @completed_steps << step[:name]
    end

    puts "\n#{'=' * 80}"
    puts "Миграция завершена!"
    puts "Выполнено шагов: #{@completed_steps.size}/#{MIGRATION_STEPS.size}"
  end

  private

  def create_directory_structure
    puts "Phase 1: Создание новой структуры директорий"

    dirs = [
      'source',
      'source/ast',
      'source/parser',
      'representations',
      'representations/semantic',
      'representations/semantic/nodes',
      'representations/semantic/gen',
      'common',
      'common/registry',
      'common/typing',
      'common/diagnostics',
      'common/analysis',
      'tools',
      'tools/compiler'
    ]

    dirs.each do |dir|
      full_path = @root_dir.join(dir)
      if @dry_run
        puts "  [DRY RUN] mkdir -p #{dir}"
      else
        FileUtils.mkdir_p(full_path) unless full_path.exist?
        puts "  ✓ #{dir}"
      end
    end

    puts
  end

  def execute_step(step)
    from_path = @root_dir.join(step[:from])
    to_path = @root_dir.join(step[:to])

    unless from_path.exist?
      puts "  ⚠ Директория #{step[:from]} не существует, пропускаем"
      return
    end

    # 1. Переместить файлы
    puts "  1. Перемещение файлов..."
    if @dry_run
      puts "    [DRY RUN] git mv #{step[:from]} #{step[:to]}"
    else
      # Используем git mv для сохранения истории
      system("cd #{@root_dir.parent} && git mv mlc/#{step[:from]} mlc/#{step[:to]}")
      unless $?.success?
        puts "    ✗ Ошибка при перемещении"
        exit 1
      end
      puts "    ✓ Перемещено"
    end

    # 2. Обновить require_relative
    puts "  2. Обновление require_relative..."
    update_requires(step)

    # 3. Запустить тесты
    if @run_tests && !@dry_run
      puts "  3. Запуск тестов..."
      if run_tests_command
        puts "    ✓ Тесты прошли"
      else
        puts "    ✗ Тесты упали"
        puts
        puts "Откатить изменения? (y/n)"
        response = $stdin.gets.chomp
        if response.downcase == 'y'
          rollback_step(step)
        end
        exit 1
      end
    end

    # 4. Коммит
    unless @dry_run
      commit_step(step)
    end
  end

  def update_requires(step)
    # Найти все .rb файлы в lib/mlc и обновить require_relative

    # Паттерны для замены
    patterns = generate_require_patterns(step)

    if @dry_run
      puts "    [DRY RUN] Паттерны замены:"
      patterns.each do |pattern|
        puts "      #{pattern[:from]} → #{pattern[:to]}"
      end
      return
    end

    ruby_files = Dir.glob(@root_dir.join('**', '*.rb'))
    updated_count = 0

    ruby_files.each do |file|
      content = File.read(file)
      original_content = content.dup

      patterns.each do |pattern|
        content.gsub!(pattern[:from], pattern[:to])
      end

      if content != original_content
        File.write(file, content)
        updated_count += 1
      end
    end

    # Также обновить test/ и lib/mlc.rb
    [
      @root_dir.parent.join('mlc.rb'),
      *Dir.glob(@root_dir.parent.parent.join('test', '**', '*.rb'))
    ].each do |file|
      next unless File.exist?(file)

      content = File.read(file)
      original_content = content.dup

      patterns.each do |pattern|
        content.gsub!(pattern[:from], pattern[:to])
      end

      if content != original_content
        File.write(file, content)
        updated_count += 1
      end
    end

    puts "    ✓ Обновлено файлов: #{updated_count}"
  end

  def generate_require_patterns(step)
    from = step[:from]
    to = step[:to]

    patterns = []

    # Для require_relative с одинарными кавычками
    patterns << {
      from: /require_relative\s+'([^']*\/)#{Regexp.escape(from)}(\/[^']*)?'/,
      to: "require_relative '\\1#{to}\\2'"
    }

    # Для require_relative с двойными кавычками
    patterns << {
      from: /require_relative\s+"([^"]*\/)#{Regexp.escape(from)}(\/[^"]*)?"/,
      to: 'require_relative "\1' + to + '\2"'
    }

    # Для абсолютных путей из lib/mlc.rb
    patterns << {
      from: /require_relative\s+"mlc\/#{Regexp.escape(from)}(\/[^"]*)?"/,
      to: 'require_relative "mlc/' + to + '\1"'
    }

    # Для относительных путей без префикса ../
    patterns << {
      from: /require_relative\s+'#{Regexp.escape(from)}(\/[^']*)?'/,
      to: "require_relative '#{to}\\1'"
    }

    patterns << {
      from: /require_relative\s+"#{Regexp.escape(from)}(\/[^"]*)?"/,
      to: 'require_relative "' + to + '\1"'
    }

    patterns
  end

  def run_tests_command
    if @quick_check
      puts "    Быстрая проверка: ruby -c lib/mlc.rb"
      system("cd #{@root_dir.parent.parent} && ruby -c lib/mlc.rb > /dev/null 2>&1")
      return false unless $?.success?

      puts "    Проверка загрузки: ruby -Ilib -rmlc -e 'puts \"OK\"'"
      system("cd #{@root_dir.parent.parent} && ruby -Ilib -rmlc -e 'puts \"OK\"' > /dev/null 2>&1")
      $?.success?
    else
      puts "    Запуск: rake test"
      system("cd #{@root_dir.parent.parent} && rake test > /dev/null 2>&1")
      $?.success?
    end
  end

  def commit_step(step)
    puts "  4. Коммит изменений..."
    commit_message = "refactor: move #{step[:from]} to #{step[:to]}\n\n#{step[:description]}"

    system("cd #{@root_dir.parent.parent} && git add -A")
    system("cd #{@root_dir.parent.parent} && git commit -m '#{commit_message}'")

    if $?.success?
      puts "    ✓ Коммит создан"
    else
      puts "    ⚠ Ошибка при создании коммита"
    end
  end

  def rollback_step(step)
    puts "  Откат изменений..."
    system("cd #{@root_dir.parent.parent} && git reset --hard HEAD")
    puts "    ✓ Изменения отменены"
  end
end

# Парсинг аргументов командной строки
require 'optparse'

options = {
  dry_run: false,
  run_tests: true,
  quick_check: false
}

OptionParser.new do |opts|
  opts.banner = "Usage: migrate_structure.rb [options]"

  opts.on("--dry-run", "Показать план без применения изменений") do
    options[:dry_run] = true
  end

  opts.on("--no-tests", "Не запускать тесты после каждого шага") do
    options[:run_tests] = false
  end

  opts.on("--quick-check", "Быстрая проверка (только синтаксис и загрузка) вместо полных тестов") do
    options[:quick_check] = true
  end

  opts.on("-h", "--help", "Показать справку") do
    puts opts
    exit
  end
end.parse!

# Запуск миграции
root = File.expand_path('../../lib/mlc', __FILE__)
migrator = StructureMigrator.new(
  root,
  dry_run: options[:dry_run],
  run_tests: options[:run_tests],
  quick_check: options[:quick_check]
)
migrator.migrate
