#!/usr/bin/env ruby
# frozen_string_literal: true

# Compile-lint for docs/LANGUAGE_REFERENCE.md fenced MLC examples.
# - ```mlc — body must compile (decls get `fn main() -> i32 = 0`;
#   bare stmts wrap in `fn main() -> i32 = do … end`).
# - ```mlc excerpt — compile the nearest preceding Source `*.mlc` path
#   (full fixture), not the truncated fence body.

require "fileutils"
require "tmpdir"

repo_root = File.expand_path("..", __dir__)
doc_path = ARGV[0] || File.join(repo_root, "docs/LANGUAGE_REFERENCE.md")
mlcc_path = ENV.fetch("MLCC", File.join(repo_root, "compiler/out/mlcc"))

unless File.executable?(mlcc_path)
  warn "lang_ref_lint: mlcc not executable: #{mlcc_path}"
  exit 2
end

unless File.file?(doc_path)
  warn "lang_ref_lint: missing doc: #{doc_path}"
  exit 2
end

text = File.read(doc_path)

def wrap_body(source)
  stripped = source.strip
  return "#{stripped}\n" if stripped.include?("fn main")

  top_level = stripped.lines.any? do |line|
    line.match?(/\A(?:type |fn |export |extern |extend )/)
  end
  return "#{stripped}\n\nfn main() -> i32 = 0\n" if top_level

  indented = stripped.lines.map { |line| "  #{line}" }.join
  "fn main() -> i32 = do\n#{indented}  0\nend\n"
end

def line_at(text, byte_index)
  text[0...byte_index].count("\n") + 1
end

def nearest_source_path(prefix)
  matches = prefix.scan(/\[`([^`]+?\.mlc)`\]/)
  return nil if matches.empty?

  matches.last[0]
end

def compile_mlc(mlcc_path, repo_root, source_path, work_dir)
  out_dir = File.join(work_dir, "out")
  FileUtils.mkdir_p(out_dir)
  absolute = File.expand_path(source_path, repo_root)
  command = [mlcc_path, "-o", out_dir, absolute]
  output = IO.popen(command, err: [:child, :out], &:read)
  status = $?.exitstatus
  [status, output]
end

failures = []
checked = 0
excerpts = 0

text.scan(/```(mlc(?: excerpt)?)\n(.*?)```/m) do |language, body|
  match = Regexp.last_match
  fence_line = line_at(text, match.begin(0))
  prefix = text[0...match.begin(0)]
  checked += 1

  Dir.mktmpdir("lang_ref_lint_") do |work_dir|
    if language == "mlc excerpt"
      excerpts += 1
      relative = nearest_source_path(prefix)
      if relative.nil?
        failures << "line #{fence_line}: mlc excerpt without Source `*.mlc` path"
        next
      end
      absolute = File.expand_path(relative, repo_root)
      unless File.file?(absolute)
        failures << "line #{fence_line}: Source missing: #{relative}"
        next
      end
      status, output = compile_mlc(mlcc_path, repo_root, relative, work_dir)
      if status != 0
        failures << "line #{fence_line}: Source compile failed (#{relative}):\n#{output.lines.last(12).join}"
      end
    else
      example_path = File.join(work_dir, "example.mlc")
      File.write(example_path, wrap_body(body))
      status, output = compile_mlc(mlcc_path, repo_root, example_path, work_dir)
      if status != 0
        failures << "line #{fence_line}: fence compile failed:\n#{output.lines.last(12).join}"
      end
    end
  end
end

if checked == 0
  warn "lang_ref_lint: no ```mlc fences found in #{doc_path}"
  exit 2
end

if failures.empty?
  puts "lang_ref_lint: ok (#{checked} fences, #{excerpts} excerpts→Source)"
  exit 0
end

warn "lang_ref_lint: #{failures.size} failure(s) of #{checked}:"
failures.each { |message| warn message }
exit 1
