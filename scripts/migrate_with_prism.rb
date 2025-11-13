#!/usr/bin/env ruby
# frozen_string_literal: true

require 'prism'
require 'fileutils'
require 'pathname'

# Точная миграция с использованием Prism для парсинга require_relative
class PrismMigrator
  STEPS = [
    { from: 'parser', to: 'source/parser' },
    { from: 'semantic_ir', to: 'representations/semantic/nodes' },
    { from: 'analysis', to: 'common/analysis' },
    { from: 'ast', to: 'source/ast' },
    { from: 'diagnostics', to: 'common/diagnostics' },
    { from: 'compiler', to: 'tools/compiler' },
    { from: 'core', to: 'common/registry' },
    { from: 'type_system', to: 'common/typing' },
    { from: 'semantic_gen', to: 'representations/semantic/gen' }
  ]

  def initialize(root_dir, quick_check: true)
    @root_dir = Pathname.new(root_dir)
    @quick_check = quick_check
    @mapping = STEPS.each_with_object({}) { |step, h| h[step[:from]] = step[:to] }
  end

  def migrate
    puts "=== Миграция с Prism ==="
    puts "Root: #{@root_dir}"
    puts "Quick check: #{@quick_check}"
    puts

    create_directories

    STEPS.each_with_index do |step, i|
      puts "\n#{'=' * 80}"
      puts "Step #{i + 1}/#{STEPS.size}: #{step[:from]} → #{step[:to]}"

      execute_step(step)
    end

    puts "\n#{'=' * 80}"
    puts "✓ Миграция завершена"
  end

  private

  def create_directories
    puts "Создание директорий..."

    dirs = STEPS.map { |s| s[:to] }.uniq.flat_map do |path|
      parts = path.split('/')
      (0...parts.size).map { |i| parts[0..i].join('/') }
    end.uniq.sort

    dirs.each do |dir|
      full_path = @root_dir.join(dir)
      FileUtils.mkdir_p(full_path) unless full_path.exist?
    end

    puts "  ✓ Создано #{dirs.size} директорий"
  end

  def execute_step(step)
    from_path = @root_dir.join(step[:from])
    to_path = @root_dir.join(step[:to])

    unless from_path.exist?
      puts "  ⚠ Пропущено (не существует)"
      return
    end

    # 1. Перемещение
    puts "  1. Перемещение..."
    system("cd #{@root_dir.parent} && git mv mlc/#{step[:from]} mlc/#{step[:to]}")
    unless $?.success?
      puts "    ✗ Ошибка git mv"
      exit 1
    end
    puts "    ✓ Перемещено"

    # 2. Обновление require_relative
    puts "  2. Обновление require_relative..."
    updated = update_requires_with_prism(step)
    puts "    ✓ Обновлено файлов: #{updated}"

    # 3. Проверка
    if @quick_check
      puts "  3. Быстрая проверка..."
      if quick_syntax_check
        puts "    ✓ Синтаксис OK"
      else
        puts "    ✗ Ошибка синтаксиса"
        puts "Откатить? (Ctrl+C для отмены, Enter для отката)"
        $stdin.gets
        system("cd #{@root_dir.parent.parent} && git reset --hard HEAD")
        exit 1
      end
    end

    # 4. Коммит
    puts "  4. Коммит..."
    commit_message = "refactor: move #{step[:from]} to #{step[:to]}"
    system("cd #{@root_dir.parent.parent} && git add -A && git commit -m '#{commit_message}'")
    puts "    ✓ Закоммичено"
  end

  def update_requires_with_prism(step)
    updated_count = 0

    # Обновить все .rb файлы
    all_ruby_files = [
      *Dir.glob(@root_dir.join('**', '*.rb')),
      @root_dir.parent.join('mlc.rb'),
      *Dir.glob(@root_dir.parent.parent.join('test', '**', '*.rb'))
    ]

    all_ruby_files.each do |file_path|
      next unless File.exist?(file_path)

      content = File.read(file_path)
      new_content = update_file_requires(content, file_path, step)

      if new_content != content
        File.write(file_path, new_content)
        updated_count += 1
      end
    end

    updated_count
  end

  def update_file_requires(content, file_path, step)
    result = Prism.parse(content)
    replacements = []

    extract_requires(result.value) do |call_node, string_node|
      require_path = string_node.unescaped

      # Проверяем, содержит ли путь step[:from]
      if path_matches?(require_path, step[:from])
        new_path = replace_path_component(require_path, step[:from], step[:to])

        if new_path != require_path
          # Записываем замену: (start_offset, end_offset, new_string_literal)
          # Нужно заменить только содержимое строки, не кавычки
          start_offset = string_node.content_loc.start_offset
          end_offset = string_node.content_loc.end_offset
          replacements << [start_offset, end_offset, new_path]
        end
      end
    end

    # Применяем замены в обратном порядке (от конца к началу)
    replacements.reverse.each do |start_offset, end_offset, new_text|
      content[start_offset...end_offset] = new_text
    end

    content
  end

  def extract_requires(node, &block)
    if node.is_a?(Prism::CallNode) && node.name == :require_relative
      if node.arguments && node.arguments.arguments.first.is_a?(Prism::StringNode)
        yield(node, node.arguments.arguments.first)
      end
    end

    node.compact_child_nodes.each do |child|
      extract_requires(child, &block)
    end
  end

  def path_matches?(path, from)
    # Проверяем, есть ли from как компонент пути
    path_parts = path.split('/')
    from_parts = from.split('/')

    # Поиск подпоследовательности
    (0...(path_parts.size - from_parts.size + 1)).any? do |i|
      path_parts[i, from_parts.size] == from_parts
    end
  end

  def replace_path_component(path, from, to)
    path_parts = path.split('/')
    from_parts = from.split('/')
    to_parts = to.split('/')

    # Находим индекс, где начинается from
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

  def quick_syntax_check
    system("cd #{@root_dir.parent.parent} && ruby -c lib/mlc.rb > /dev/null 2>&1") &&
    system("cd #{@root_dir.parent.parent} && ruby -Ilib -rmlc -e '' > /dev/null 2>&1")
  end
end

# Запуск
root = File.expand_path('../../lib/mlc', __FILE__)
migrator = PrismMigrator.new(root, quick_check: true)
migrator.migrate
