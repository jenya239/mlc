#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_TEXT_GLYPH_CACHE_SCALING STEP=3 — wall-time gate for GL text demos.
# Soft-skips when glfw3/freetype2/harfbuzz/font missing.
# Usage: ruby scripts/run_text_gl_perf_corpus.rb [mlcc_binary]

require "fileutils"
require "open3"

ROOT = File.expand_path("..", __dir__)
MLCC = ARGV[0] || File.join(ROOT, "compiler/out/mlcc")
BUILD_BIN = File.join(ROOT, "compiler/build_bin.sh")
WORK = File.join(ROOT, ".tmp/text_gl_perf_corpus")
FONT = ENV.fetch("TEXT_GL_PERF_FONT", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")

# Thresholds: measured 2026-07-13 on host (ide 0.97s, dashboard 0.29s) with headroom.
CORPUS = [
  {
    name: "text_ide_panels_demo",
    entry: File.join(ROOT, "misc/examples/text_ide_panels_demo.mlc"),
    env_name: "MLC_TEXT_IDE_BENCH",
    max_wall_seconds: 3.0
  },
  {
    name: "text_dashboard_demo",
    entry: File.join(ROOT, "misc/examples/text_dashboard_demo.mlc"),
    env_name: "MLC_TEXT_DASHBOARD_BENCH",
    max_wall_seconds: 1.0
  }
].freeze

def run_capture(*command, env: {})
  Open3.capture3(env, *command)
end

def pkg_exists?(name)
  system("pkg-config", "--exists", name, out: File::NULL, err: File::NULL)
end

def soft_skip(reason)
  warn "[text gl perf corpus] SKIP: #{reason}"
  exit 0
end

soft_skip("mlcc missing at #{MLCC}") unless File.executable?(MLCC)
soft_skip("pkg-config/glfw3 missing") unless pkg_exists?("glfw3")
soft_skip("freetype2 missing") unless pkg_exists?("freetype2")
soft_skip("harfbuzz missing") unless pkg_exists?("harfbuzz")
soft_skip("font missing #{FONT}") unless File.file?(FONT)

ENV["TMPDIR"] ||= File.join(ROOT, "tmp")
ENV["MLCC_OBJ_CLEAN"] ||= "1"
ENV["MLCC_PCH"] ||= "0"
ENV["MLC_GLFW_VISIBLE"] ||= "0"

FileUtils.rm_rf(WORK)
FileUtils.mkdir_p(WORK)

failures = []

CORPUS.each do |entry|
  out_directory = File.join(WORK, entry[:name])
  binary_path = File.join(out_directory, "bin")
  FileUtils.mkdir_p(out_directory)

  warn "[text gl perf corpus] compile #{entry[:name]}"
  _stdout, stderr, status = run_capture(MLCC, "-o", out_directory, entry[:entry])
  unless status.success?
    failures << "#{entry[:name]}: mlcc failed\n#{stderr}"
    next
  end

  _stdout, stderr, status = run_capture("bash", BUILD_BIN, out_directory, binary_path)
  unless status.success?
    failures << "#{entry[:name]}: build_bin failed\n#{stderr}"
    next
  end

  env = {
    "MLC_GLFW_VISIBLE" => "0",
    entry[:env_name] => "1"
  }
  started = Process.clock_gettime(Process::CLOCK_MONOTONIC)
  _stdout, stderr, status = run_capture(binary_path, env: env)
  elapsed = Process.clock_gettime(Process::CLOCK_MONOTONIC) - started

  unless status.success?
    failures << "#{entry[:name]}: binary exit=#{status.exitstatus}\n#{stderr}"
    next
  end

  limit = entry[:max_wall_seconds]
  if elapsed > limit
    failures << format(
      "%s: wall %.2fs > threshold %.2fs",
      entry[:name],
      elapsed,
      limit
    )
    next
  end

  warn format(
    "[text gl perf corpus] ok %s wall=%.2fs (max=%.2fs)",
    entry[:name],
    elapsed,
    limit
  )
end

if failures.empty?
  warn "[text gl perf corpus] PASS (#{CORPUS.length} demos)"
  exit 0
end

warn "[text gl perf corpus] FAIL"
failures.each { |message| warn message }
exit 1
