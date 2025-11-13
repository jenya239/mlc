#!/usr/bin/env ruby
# frozen_string_literal: true

require 'prism'
require 'fileutils'
require 'pathname'

# Пошаговая миграция - выполняет один шаг за раз
class StepMigrator
  def initialize(root_dir, from, to, dry_run: false, verbose: false, auto: false)
    @root_dir = Pathname.new(root_dir)
    @from = from
    @to = to
    @dry_run = dry_run
    @verbose = verbose
    @auto = auto
  end

  def migrate
    puts "=== Миграция: #{@from} → #{@to} ==="
    puts "Root: #{@root_dir}"
    puts "Dry run: #{@dry_run}"
    puts "Auto: #{@auto}" if @auto
    puts

    from_path = @root_dir.join(@from)
    unless from_path.exist?
      puts "✗ Директория #{@from} не существует"
      exit 1
    end

    # 1. Создать целевые директории
    create_target_dirs

    # 2. Показать план замен
    puts "\n2. Анализ require_relative для замены..."
    replacements = analyze_replacements

    if replacements.empty?
      puts "  ⚠ Не найдено require_relative для замены"
    else
      puts "  Найдено #{replacements.size} файлов с требуемыми заменами:"
      replacements.each do |file, changes|
        file_pathname = Pathname.new(file)
        puts "    #{file_pathname.relative_path_from(@root_dir.parent.parent)} (#{changes.size} замен)"
        if @verbose
          changes.each do |old_path, new_path, _|
            puts "      #{old_path} → #{new_path}"
          end
        end
      end
    end

    if @dry_run
      puts "\n[DRY RUN] Изменения не применены"
      return
    end

    # 3. Применить замены
    unless replacements.empty?
      unless @auto
        print "\nПрименить замены? (y/n): "
        return unless $stdin.gets.chomp.downcase == 'y'
      end

      puts "\n3. Применение замен..."
      apply_replacements(replacements)
      puts "  ✓ Замены применены"
    end

    # 4. Переместить директорию
    puts "\n4. Перемещение директории..."
    unless @auto
      print "Выполнить git mv #{@from} #{@to}? (y/n): "
      return unless $stdin.gets.chomp.downcase == 'y'
    end

    system("cd #{@root_dir.parent} && git mv mlc/#{@from} mlc/#{@to}")
    unless $?.success?
      puts "  ✗ Ошибка git mv"
      exit 1
    end
    puts "  ✓ Перемещено"

    # 5. Проверка
    puts "\n5. Проверка синтаксиса..."
    if check_syntax
      puts "  ✓ Синтаксис OK"
    else
      puts "  ✗ Ошибка синтаксиса"
      if @auto
        system("cd #{@root_dir.parent.parent} && git reset --hard HEAD")
        puts "  ✓ Автоматически откачено"
      else
        print "Откатить? (y/n): "
        if $stdin.gets.chomp.downcase == 'y'
          system("cd #{@root_dir.parent.parent} && git reset --hard HEAD")
          puts "  ✓ Откачено"
        end
      end
      exit 1
    end

    # 6. Коммит
    puts "\n6. Коммит..."
    unless @auto
      print "Создать коммит? (y/n): "
      return unless $stdin.gets.chomp.downcase == 'y'
    end

    commit_message = "refactor: move #{@from} to #{@to}"
    system("cd #{@root_dir.parent.parent} && git add -A && git commit -m '#{commit_message}'")
    puts "  ✓ Закоммичено"

    puts "\n✓ Миграция завершена успешно"
  end

  private

  def create_target_dirs
    puts "1. Создание целевых директорий..."

    parts = @to.split('/')
    (0...parts.size).each do |i|
      dir = parts[0..i].join('/')
      full_path = @root_dir.join(dir)

      unless full_path.exist?
        if @dry_run
          puts "  [DRY RUN] mkdir #{dir}"
        else
          FileUtils.mkdir_p(full_path)
          puts "  ✓ #{dir}"
        end
      end
    end
  end

  def analyze_replacements
    replacements = {}

    all_ruby_files = [
      *Dir.glob(@root_dir.join('**', '*.rb')),
      @root_dir.parent.join('mlc.rb'),
      *Dir.glob(@root_dir.parent.parent.join('test', '**', '*.rb'))
    ].select { |f| File.exist?(f) }

    all_ruby_files.each do |file_path|
      file_replacements = analyze_file(file_path)
      replacements[file_path] = file_replacements unless file_replacements.empty?
    end

    replacements
  end

  def analyze_file(file_path)
    # Skip if file is within the directory being moved
    file_pathname = Pathname.new(file_path).expand_path
    from_full_path = @root_dir.join(@from).expand_path

    # Check if file is inside the directory being moved
    if file_pathname.to_s.start_with?(from_full_path.to_s + '/')
      return []  # Skip - internal references don't need updating
    end

    content = File.read(file_path)
    result = Prism.parse(content)
    changes = []

    extract_requires(result.value) do |string_node|
      require_path = string_node.unescaped

      # Проверяем, содержит ли путь @from как компонент
      if contains_path_component?(require_path, @from)
        new_path = replace_path_component(require_path, @from, @to)

        if new_path != require_path
          changes << [require_path, new_path, string_node]
        end
      end
    end

    changes
  end

  def extract_requires(node, &block)
    if node.is_a?(Prism::CallNode) && node.name == :require_relative
      if node.arguments && node.arguments.arguments.first.is_a?(Prism::StringNode)
        yield(node.arguments.arguments.first)
      end
    end

    node.compact_child_nodes.each do |child|
      extract_requires(child, &block)
    end
  end

  def contains_path_component?(path, component)
    # Разбиваем пути на компоненты
    path_parts = path.split('/')
    component_parts = component.split('/')

    # Проверяем, есть ли component_parts как последовательность в path_parts
    (0...(path_parts.size - component_parts.size + 1)).any? do |i|
      path_parts[i, component_parts.size] == component_parts
    end
  end

  def replace_path_component(path, from, to)
    path_parts = path.split('/')
    from_parts = from.split('/')
    to_parts = to.split('/')

    # Находим первый индекс, где начинается from
    index = nil
    (0...(path_parts.size - from_parts.size + 1)).each do |i|
      if path_parts[i, from_parts.size] == from_parts
        index = i
        break
      end
    end

    return path unless index

    # Заменяем from_parts на to_parts
    new_parts = path_parts[0...index] + to_parts + path_parts[(index + from_parts.size)..-1]
    new_parts.join('/')
  end

  def apply_replacements(replacements)
    replacements.each do |file_path, changes|
      content = File.read(file_path)

      # Сортируем по позиции в обратном порядке (от конца к началу)
      changes.sort_by! { |_, _, node| -node.content_loc.start_offset }

      changes.each do |old_path, new_path, string_node|
        start_offset = string_node.content_loc.start_offset
        end_offset = string_node.content_loc.end_offset
        content[start_offset...end_offset] = new_path
      end

      File.write(file_path, content)
      file_pathname = Pathname.new(file_path)
      puts "    ✓ #{file_pathname.relative_path_from(@root_dir.parent.parent)}"
    end
  end

  def check_syntax
    # Только проверка синтаксиса, без загрузки
    # Загрузка может не работать в промежуточных состояниях
    system("cd #{@root_dir.parent.parent} && ruby -c lib/mlc.rb > /dev/null 2>&1")
  end
end

# Парсинг аргументов
require 'optparse'

options = {
  dry_run: false,
  verbose: false,
  auto: false
}

parser = OptionParser.new do |opts|
  opts.banner = "Usage: migrate_step.rb FROM TO [options]"
  opts.on("--dry-run", "Показать план без применения") { options[:dry_run] = true }
  opts.on("-v", "--verbose", "Подробный вывод") { options[:verbose] = true }
  opts.on("--auto", "Автоматически подтверждать все действия") { options[:auto] = true }
  opts.on("-h", "--help", "Справка") do
    puts opts
    puts "\nПримеры:"
    puts "  ruby scripts/migrate_step.rb parser source/parser --dry-run"
    puts "  ruby scripts/migrate_step.rb parser source/parser --auto"
    puts "  ruby scripts/migrate_step.rb semantic_ir representations/semantic/nodes"
    exit
  end
end

parser.parse!

if ARGV.size != 2
  puts "Ошибка: требуется 2 аргумента: FROM TO"
  puts parser.help
  exit 1
end

from = ARGV[0]
to = ARGV[1]

root = File.expand_path('../../lib/mlc', __FILE__)
migrator = StepMigrator.new(root, from, to, dry_run: options[:dry_run], verbose: options[:verbose], auto: options[:auto])
migrator.migrate
