#!/usr/bin/env ruby
# frozen_string_literal: true

# Systematic check: does mlcc translation time scale ~ O(N^b)? Estimate b via log-log regression.
#
# Usage:
#   ruby run_quadratic_probe.rb [suite] [min] [max] [steps]
#   ruby run_quadratic_probe.rb functions 64 4096 7
#
# Repeats each scale REPEATS times, uses median wall ms (mlcc only, no g++).
# Exponent b > SOFT suggests super-linear growth worth profiling; b > HARD is a strong red flag.

require "fileutils"

REPEATS = (ENV["SCALING_REPEATS"] || "3").to_i
SOFT_EXP = (ENV["SCALING_SOFT_EXP"] || "1.35").to_f
HARD_EXP = (ENV["SCALING_HARD_EXP"] || "1.65").to_f

ROOT = File.expand_path("../..", __dir__)
MLCC = File.join(ROOT, "compiler/out/mlcc")
GEN = File.join(ROOT, "benchmarks/synthetic/gen.rb")

suite = ARGV[0] || "functions"
n_min = (ARGV[1] || "64").to_i
n_max = (ARGV[2] || "4096").to_i
steps = (ARGV[3] || "8").to_i

abort "mlcc missing: #{MLCC}" unless File.executable?(MLCC)

def run_mlcc_ms(mlcc, main_path, out_dir)
  t0 = Process.clock_gettime(Process::CLOCK_MONOTONIC)
  system(mlcc, "-o", out_dir, main_path, out: File::NULL, err: File::NULL)
  t1 = Process.clock_gettime(Process::CLOCK_MONOTONIC)
  return nil unless $?.success?

  ((t1 - t0) * 1000.0).round(1)
end

def median(array)
  sorted = array.sort
  mid = sorted.length / 2
  sorted.length.odd? ? sorted[mid] : (sorted[mid - 1] + sorted[mid]) / 2.0
end

# Geometric progression from n_min to n_max with `steps` points
def scales(n_min, n_max, steps)
  ratio = (n_max.to_f / n_min)**(1.0 / (steps - 1))
  (0...steps).map { |k| (n_min * (ratio**k)).round.clamp(1, n_max) }.uniq.sort
end

def log_regress(xs, ys)
  pairs = xs.zip(ys).select { |x, y| x.positive? && y.positive? }
  return [nil, nil] if pairs.length < 3

  lx = pairs.map { |x, _| Math.log(x) }
  ly = pairs.map { |_, y| Math.log(y) }
  n = lx.length
  sx = lx.sum
  sy = ly.sum
  sxx = lx.sum { |v| v * v }
  sxy = lx.zip(ly).sum { |a, b| a * b }
  denom = n * sxx - sx * sx
  return [nil, nil] if denom.abs < 1e-12

  b = (n * sxy - sx * sy) / denom
  a = (sy - b * sx) / n
  mean_y = sy / n
  ss_tot = ly.sum { |v| (v - mean_y)**2 }
  pred = lx.map { |x| a + b * x }
  ss_res = ly.zip(pred).sum { |y, p| (y - p)**2 }
  r2 = ss_tot.positive? ? 1.0 - (ss_res / ss_tot) : nil
  [b, r2]
end

scale_list = scales(n_min, n_max, steps)
tmpdir = File.join(ROOT, ".tmp_scaling_probe")
FileUtils.mkdir_p(tmpdir)

rows = []
scale_list.each do |n|
  gen_dir = File.join(tmpdir, "#{suite}_#{n}")
  FileUtils.rm_rf(gen_dir)
  FileUtils.mkdir_p(gen_dir)
  ok = system("ruby", GEN, suite, n.to_s, gen_dir, out: File::NULL, err: File::NULL)
  abort "gen.rb failed for #{suite} #{n}" unless ok

  main_path = File.join(gen_dir, "main.mlc")
  samples = []
  REPEATS.times do
    out_dir = File.join(gen_dir, "out_#{samples.length}")
    FileUtils.rm_rf(out_dir)
    ms = run_mlcc_ms(MLCC, main_path, out_dir)
    samples << ms if ms
  end
  abort "mlcc failed at scale #{n}" if samples.empty?

  med = median(samples)
  rows << { n: n, ms: med }
end

printf "%-10s %8s %12s\n", "suite", "N", "median(ms)"
rows.each { |r| printf "%-10s %8d %12.1f\n", suite, r[:n], r[:ms] }

ns = rows.map { |r| r[:n] }
ts = rows.map { |r| [r[:ms], 0.1].max }
b, r2 = log_regress(ns, ts)

puts "---"
if b.nil?
  puts "log-log fit: insufficient points"
else
  printf "fitted exponent b = %.3f (log-time ~ b * log-N + const), R² = %.4f\n", b, r2 || 0.0
  if b > HARD_EXP
    puts "ALERT: exponent >= #{HARD_EXP} — likely super-linear (investigate profiler)."
  elsif b > SOFT_EXP
    puts "hint: exponent >= #{SOFT_EXP} — worth checking hotspots as N grows."
  else
    puts "OK: exponent below #{SOFT_EXP} (roughly sub-quadratic in this band)."
  end
end

puts "---"
puts "pair ratios T(n)/T(prev) for adjacent scales (quadratic-ish gives ~4 when n doubles):"
rows.each_cons(2) do |a, c|
  r = c[:ms] / [a[:ms], 0.1].max
  nd = c[:n].to_f / a[:n]
  printf "  N %5d -> %5d (×%.2f): time ×%.2f\n", a[:n], c[:n], nd, r
end
