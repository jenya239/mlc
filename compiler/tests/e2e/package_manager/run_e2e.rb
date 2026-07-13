#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_PACKAGE_MANAGER STEP=8 — local git fixture → fetch → mlcc compile+run.
# Usage: ruby compiler/tests/e2e/package_manager/run_e2e.rb [mlcc_binary]

require "fileutils"
require "json"
require "open3"
require "tmpdir"

require_relative "../../../../lib/mlc/common/index"

ROOT = File.expand_path("../../../..", __dir__)
MLCC = ARGV[0] || File.join(ROOT, "compiler/out/mlcc")
FETCH_SCRIPT = File.join(ROOT, "scripts/mlc_pkg_fetch.rb")
RUNTIME_INCLUDE = File.join(ROOT, "runtime/include")
RUNTIME_IO = File.join(ROOT, "runtime/src/io/io.cpp")
RUNTIME_STRING = File.join(ROOT, "runtime/src/core/string.cpp")

def run!(*command, chdir: nil)
  stdout, stderr, status =
    if chdir
      Open3.capture3(*command, chdir: chdir)
    else
      Open3.capture3(*command)
    end
  return stdout if status.success?

  raise "command failed: #{command.join(" ")}\n#{stderr.empty? ? stdout : stderr}"
end

def select_cxx
  script = File.join(ROOT, "compiler/scripts/select_cxx.sh")
  output = run!("bash", "-c", "source #{script.inspect} && printf '%s\\n' \"${CXX_CMD[@]}\"")
  output.lines.map(&:chomp).reject(&:empty?)
end

def build_package_remote(workspace)
  source = File.join(workspace, "package_source")
  remote = File.join(workspace, "demo_pkg.git")
  FileUtils.mkdir_p(source)
  File.write(
    File.join(source, "math.mlc"),
    <<~MLC
      export fn add(a: i32, b: i32) -> i32 = a + b
    MLC
  )
  run!("git", "init", chdir: source)
  run!("git", "config", "user.email", "e2e@example.com", chdir: source)
  run!("git", "config", "user.name", "E2E", chdir: source)
  run!("git", "add", "math.mlc", chdir: source)
  run!("git", "commit", "-m", "math", chdir: source)
  revision = run!("git", "rev-parse", "HEAD", chdir: source).strip
  run!("git", "clone", "--bare", source, remote)
  [remote, revision]
end

def write_app(project_root, remote, revision)
  FileUtils.mkdir_p(project_root)
  File.write(
    File.join(project_root, "mlc.json"),
    JSON.pretty_generate(
      "name" => "pkg_e2e_app",
      "version" => "0.0.1",
      "dependencies" => {
        "demo_pkg" => { "git" => remote, "rev" => revision }
      }
    )
  )
  File.write(
    File.join(project_root, "main.mlc"),
    <<~MLC
      import { add } from 'demo_pkg/math'

      fn main() -> i32 = do
        println(`${add(40, 2)}`)
        0
      end
    MLC
  )
end

abort("mlcc not executable: #{MLCC}") unless File.executable?(MLCC)

Dir.mktmpdir("mlc_pkg_e2e_") do |workspace|
  remote, revision = build_package_remote(workspace)
  project_root = File.join(workspace, "app")
  write_app(project_root, remote, revision)

  fetch_stdout = run!(RbConfig.ruby, FETCH_SCRIPT, project_root)
  unless fetch_stdout.include?("demo_pkg\tcloned\t#{revision}")
    abort("fetch unexpected:\n#{fetch_stdout}")
  end

  vendor_math = File.join(project_root, ".mlc_packages", "demo_pkg", "math.mlc")
  abort("missing vendor file: #{vendor_math}") unless File.file?(vendor_math)

  out_dir = File.join(workspace, "out")
  FileUtils.mkdir_p(out_dir)
  run!(MLCC, "-o", out_dir, "main.mlc", chdir: project_root)

  binary = File.join(workspace, "prog")
  cxx_command = select_cxx
  cpp_files = Dir[File.join(out_dir, "*.cpp")]
  run!(
    *cxx_command,
    "-std=c++20",
    "-I", out_dir,
    "-I", RUNTIME_INCLUDE,
    *cpp_files,
    RUNTIME_IO,
    RUNTIME_STRING,
    "-o", binary
  )

  actual = run!(binary).strip
  if actual == "42"
    puts "PASS package_manager_e2e (fetch+compile+run → 42)"
    exit 0
  end

  abort("FAIL expected 42, got #{actual.inspect}")
end
