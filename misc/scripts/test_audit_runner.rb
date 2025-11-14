# frozen_string_literal: true

ENV["TEST_AUDIT_NO_AUTORUN"] ||= "1"

require "json"
require "fileutils"
require "prism"
require "minitest"
require "minitest/test"

ROOT_DIR = File.expand_path("..", __dir__)
TEST_ROOT = File.join(ROOT_DIR, "test")
DEFAULT_AUDIT_PATH = File.join(ROOT_DIR, "docs", "test_audit.jsonl")
$LOAD_PATH.unshift(TEST_ROOT) unless $LOAD_PATH.include?(TEST_ROOT)
$LOAD_PATH.unshift(File.join(ROOT_DIR, "lib")) unless $LOAD_PATH.include?(File.join(ROOT_DIR, "lib"))
$LOAD_PATH.unshift(TEST_ROOT) unless $LOAD_PATH.include?(TEST_ROOT)

module TestAudit
  module_function

  def metadata
    @metadata ||= {}
  end

  def test_files
    @test_files ||= begin
      files = Dir[File.join(TEST_ROOT, "**/*_test.rb")].sort

      if (glob = ENV["TEST_GLOB"])
        patterns = glob.split(":").map(&:strip).reject(&:empty?)
        files.select! do |path|
          patterns.any? { |pattern| File.fnmatch?(pattern, path, File::FNM_EXTGLOB) }
        end
      end

      if (regex = ENV["TEST_PATTERN"])
        pattern = Regexp.new(regex)
        files.select! { |path| pattern.match?(path) }
      end

      if (list = ENV["TEST_FILES"])
        files = list.split(",").map { |entry| File.expand_path(entry.strip, ROOT_DIR) }
      elsif (single = ENV["TEST_FILE"])
        files = [File.expand_path(single.strip, ROOT_DIR)]
      end

      files
    end
  end

  def output_path
    @output_path ||= begin
      path = ENV["TEST_AUDIT_OUTPUT"]
      path ? File.expand_path(path, ROOT_DIR) : DEFAULT_AUDIT_PATH
    end
  end

  def append_mode?
    ENV["TEST_AUDIT_APPEND"] == "1"
  end

  def collect_metadata
    test_files.each do |path|
      warn "[test_audit] Parsing #{path}" if ENV["TEST_AUDIT_DEBUG"] == "1"
      source = File.read(path)
      result = Prism.parse(source, filepath: path)
      next unless result.success?

      walk(result.value, nil, path) unless ENV["TEST_AUDIT_SKIP_WALK"] == "1"
    end
    warn "[test_audit] Metadata parsed" if ENV["TEST_AUDIT_DEBUG"] == "1"
  end

  def walk(node, current_scope, path)
    return unless node

    case node
    when Prism::ClassNode, Prism::ModuleNode
      name = constant_name(node.constant_path)
      traverse(node.body, name || current_scope, path)
    when Prism::DefNode
      register_test(node, current_scope, path)
      traverse(node.body, current_scope, path)
    else
      traverse(node, current_scope, path)
    end
  end

  def traverse(node, current_scope, path)
    return unless node.respond_to?(:child_nodes)

    node.child_nodes.each do |child|
      walk(child, current_scope, path) if child
    end
  end

  def constant_name(node)
    return unless node

    if node.is_a?(Prism::ConstantReadNode)
      return node.name.to_s
    end

    parts = []
    current = node
    seen = {}

    while current
      break if seen[current.object_id]
      seen[current.object_id] = true

      parts.unshift(current.name.to_s) if current.respond_to?(:name)

      parent = current.respond_to?(:parent) ? current.parent : nil
      break unless parent

      if parent.is_a?(Prism::ConstantReadNode)
        parts.unshift(parent.name.to_s)
        break
      end

      current = parent
    end

    parts.join("::")
  end

  def register_test(node, current_scope, path)
    method_name = node.name.to_s
    return unless method_name.start_with?("test_")

    key = [current_scope, method_name].compact.join("#")
    description = method_name.sub(/^test_/, "").tr("_", " ").strip
    dependencies = []
    collect_constants(node.body, dependencies)

    metadata[key] = {
      "file" => path,
      "line" => node.location.start_line,
      "description" => description.empty? ? "test #{method_name}" : description,
      "dependencies" => dependencies.uniq.sort
    }
  end

  def collect_constants(node, acc)
    return unless node&.respond_to?(:child_nodes)

    if (name = extract_constant_name(node))
      acc << name
    end

    node.child_nodes.each do |child|
      collect_constants(child, acc) if child
    end
  end

  def extract_constant_name(node)
    case node
    when Prism::ConstantReadNode
      node.name.to_s
    when Prism::ConstantPathNode
      constant_name(node)
    else
      nil
    end
  end

  class Recorder
    def initialize
      mode = TestAudit.append_mode? ? "a" : "w"
      FileUtils.mkdir_p(File.dirname(TestAudit.output_path))
      @io = File.open(TestAudit.output_path, mode)
    end

    def log(runnable, method_name, result)
      key = "#{runnable.name}##{method_name}"
      metadata = TestAudit.metadata[key] || {}
      status = determine_status(result)
      failure = result.failures.first
      warn "[test_audit] Recording #{key} status=#{status}" if ENV["TEST_AUDIT_DEBUG"] == "1"

      entry = {
        "test_case" => runnable.name,
        "name" => method_name,
        "status" => status,
        "assertions" => result.assertions,
        "time" => result.time,
        "file" => metadata["file"] || source_location(runnable, method_name, 0),
        "line" => metadata["line"] || source_location(runnable, method_name, 1),
        "description" => metadata["description"],
        "dependencies" => metadata["dependencies"],
        "message" => failure&.message&.lines&.first&.strip,
        "backtrace" => failure&.backtrace&.first
      }

      @io.puts(JSON.dump(entry))
    end

    def close
      @io.close
    end

    private

    def determine_status(result)
      return "skip" if result.skipped?
      return "error" if result.error?
      return "failure" if result.failures.any?

      "pass"
    end

    def source_location(runnable, method_name, index)
      method = runnable.instance_method(method_name)
      location = method&.source_location
      location ? location[index] : nil
    rescue NameError
      nil
    end
  end
end

TestAudit.collect_metadata

if ENV["TEST_AUDIT_ONLY_METADATA"] == "1"
  warn "[test_audit] Metadata collection finished"
  exit(0)
end

composite_reporter = Minitest::CompositeReporter.new
summary_reporter = Minitest::SummaryReporter.new($stdout)
recorder = TestAudit::Recorder.new
composite_reporter << summary_reporter

if ENV["TEST_AUDIT_DEBUG"] == "1"
  reporter_classes = composite_reporter.reporters.map { |r| r.class.name }
  warn "[test_audit] Reporters: #{reporter_classes.join(', ')}"
  warn "[test_audit] Selected #{TestAudit.test_files.size} files"
end

TestAudit.test_files.each do |file|
  warn "[test_audit] Requiring #{file}" if ENV["TEST_AUDIT_DEBUG"] == "1"
  require file
end

seed = srand % 0xFFFF
Minitest.seed = seed

test_methods = Minitest::Runnable.runnables.flat_map do |runnable|
  runnable.runnable_methods.map { |method| [runnable, method] }
end

composite_reporter.start

test_methods.each do |runnable, method|
  test = runnable.new(method)
  result = test.run
  recorder.log(runnable, method, result)
  composite_reporter.record(result)
end

composite_reporter.report
recorder.close

exit((summary_reporter.errors + summary_reporter.failures).zero? ? 0 : 1)
