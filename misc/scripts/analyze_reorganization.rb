#!/usr/bin/env ruby
# frozen_string_literal: true

require 'prism'
require 'pathname'
require 'json'

# Анализ структуры lib/mlc/ и планирование реорганизации
class ReorganizationAnalyzer
  def initialize(root_dir)
    @root_dir = Pathname.new(root_dir)
    @files = []
    @requires = {}
    @directory_mapping = build_directory_mapping
  end

  def analyze
    puts "=== Анализ текущей структуры lib/mlc/ ==="
    puts

    collect_files
    analyze_requires
    print_current_structure
    print_directory_mapping
    print_requires_summary
    generate_migration_plan
  end

  private

  def build_directory_mapping
    # Mapping: старый путь → новый путь
    {
      'ast' => 'source/ast',
      'parser' => 'source/parser',

      'semantic_ir' => 'representations/semantic/ir',
      'semantic_gen' => 'representations/semantic/gen',

      'backends' => 'backends',  # без изменений

      'core' => 'common/core',
      'type_system' => 'common/type_system',
      'diagnostics' => 'common/diagnostics',
      'analysis' => 'common/analysis',

      'compiler' => 'tools/compiler',
      'stdlib' => 'stdlib'  # .mlc файлы остаются как есть
    }
  end

  def collect_files
    Dir.glob(@root_dir.join('**', '*.rb')).each do |file|
      rel_path = Pathname.new(file).relative_path_from(@root_dir)
      @files << rel_path
    end
    puts "Найдено файлов: #{@files.size}"
  end

  def analyze_requires
    @files.each do |file|
      full_path = @root_dir.join(file)
      content = File.read(full_path)

      begin
        result = Prism.parse(content)
        requires = extract_requires(result.value)
        @requires[file] = requires if requires.any?
      rescue => e
        puts "Ошибка парсинга #{file}: #{e.message}"
      end
    end
  end

  def extract_requires(node, requires = [])
    case node
    when Prism::CallNode
      if node.name == :require_relative && node.arguments
        arg = node.arguments.arguments.first
        if arg.is_a?(Prism::StringNode)
          requires << arg.unescaped
        end
      end
    end

    node.compact_child_nodes.each do |child|
      extract_requires(child, requires)
    end

    requires
  end

  def print_current_structure
    puts "\n=== Текущая структура директорий ==="

    dirs = @files.map { |f| f.dirname.to_s }.uniq.sort
    dirs.each do |dir|
      count = @files.count { |f| f.dirname.to_s == dir }
      puts "  #{dir.ljust(50)} (#{count} файлов)"
    end
  end

  def print_directory_mapping
    puts "\n=== Планируемая реорганизация ==="
    puts
    puts "Структура:"
    puts "  lib/mlc/"
    puts "  ├── source/              # Исходный синтаксис"
    puts "  │   ├── ast/"
    puts "  │   └── parser/"
    puts "  ├── representations/"
    puts "  │   └── semantic/"
    puts "  │       ├── ir/          # semantic_ir"
    puts "  │       └── gen/         # semantic_gen"
    puts "  ├── backends/"
    puts "  │   └── cpp/"
    puts "  ├── common/              # Общие компоненты"
    puts "  │   ├── core/"
    puts "  │   ├── type_system/"
    puts "  │   ├── diagnostics/"
    puts "  │   └── analysis/"
    puts "  ├── tools/"
    puts "  │   └── compiler/        # stdlib support"
    puts "  └── stdlib/              # .mlc исходники (без изменений)"
    puts

    puts "Mapping директорий:"
    @directory_mapping.each do |old_path, new_path|
      next if old_path == new_path
      file_count = @files.count { |f| f.to_s.start_with?("#{old_path}/") }
      puts "  #{old_path.ljust(30)} → #{new_path.ljust(40)} (#{file_count} файлов)"
    end
  end

  def print_requires_summary
    puts "\n=== Анализ зависимостей ==="

    total_requires = @requires.values.flatten.size
    puts "Всего require_relative: #{total_requires}"
    puts

    # Топ файлов с наибольшим количеством require_relative
    puts "Файлы с наибольшим количеством зависимостей:"
    @requires.sort_by { |_, reqs| -reqs.size }.first(10).each do |file, reqs|
      puts "  #{file.to_s.ljust(60)} (#{reqs.size} requires)"
    end
  end

  def generate_migration_plan
    puts "\n=== План миграции ==="
    puts
    puts "Этапы:"
    puts
    puts "Phase 1: Создание новой структуры директорий"
    puts "  - Создать source/, representations/, common/, tools/"
    puts "  - Создать поддиректории"
    puts
    puts "Phase 2: Перемещение файлов (по группам)"
    puts "  2a. source/ (ast, parser)"
    puts "  2b. representations/semantic/ (semantic_ir, semantic_gen)"
    puts "  2c. common/ (core, type_system, diagnostics, analysis)"
    puts "  2d. tools/ (compiler)"
    puts
    puts "Phase 3: Обновление require_relative"
    puts "  - Использовать sed с точными паттернами"
    puts "  - Запускать тесты после каждого этапа"
    puts
    puts "Phase 4: Удаление старых пустых директорий"
    puts

    puts "Детальный порядок миграции (от независимых к зависимым):"
    ordered_dirs = calculate_migration_order
    ordered_dirs.each_with_index do |dir, i|
      new_path = @directory_mapping[dir]
      next if dir == new_path

      file_count = @files.count { |f| f.to_s.start_with?("#{dir}/") }
      deps = count_dependencies(dir)

      puts "  #{(i + 1).to_s.rjust(2)}. #{dir.ljust(30)} → #{new_path.ljust(40)} (#{file_count} файлов, #{deps} зависимостей)"
    end
  end

  def calculate_migration_order
    # Упрощённая сортировка: сначала те, от которых мало зависят
    dirs = @directory_mapping.keys.reject { |d| @directory_mapping[d] == d }

    dirs.sort_by do |dir|
      # Считаем сколько файлов зависят от файлов в этой директории
      dependency_count = 0
      @requires.each do |file, reqs|
        next if file.to_s.start_with?("#{dir}/")
        reqs.each do |req|
          dependency_count += 1 if resolves_to_dir?(file, req, dir)
        end
      end
      dependency_count
    end
  end

  def count_dependencies(dir)
    count = 0
    @requires.each do |file, reqs|
      next if file.to_s.start_with?("#{dir}/")
      reqs.each do |req|
        count += 1 if resolves_to_dir?(file, req, dir)
      end
    end
    count
  end

  def resolves_to_dir?(from_file, require_path, target_dir)
    # Упрощённая проверка
    resolved = resolve_require(from_file, require_path)
    return false unless resolved
    resolved.to_s.start_with?("#{target_dir}/")
  end

  def resolve_require(from_file, require_path)
    base_dir = from_file.dirname
    resolved = base_dir.join(require_path).cleanpath

    # Добавляем .rb если нужно
    if !resolved.to_s.end_with?('.rb')
      resolved = Pathname.new(resolved.to_s + '.rb')
    end

    return resolved if @files.include?(resolved)
    nil
  end
end

# Запуск анализа
root = File.expand_path('../../lib/mlc', __FILE__)
analyzer = ReorganizationAnalyzer.new(root)
analyzer.analyze
