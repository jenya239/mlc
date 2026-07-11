#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_TEXT_MSDF_TO_MLC — golden RGB was frozen at STEP=2 from C++ msdf_shim.
# After STEP=5 shim is gone: --check delegates to MLC MAE gate (byte-identical).

root = File.expand_path("..", __dir__)
if ARGV.include?("--check")
  warn "[msdf golden] delegating --check to run_msdf_mlc_mae_gate.rb"
  exec("ruby", File.join(root, "scripts/run_msdf_mlc_mae_gate.rb"))
end

abort <<~MSG
  [msdf golden] RGB fixture is frozen (misc/examples/fixtures/msdf_A_32_4.rgb).
  Re-check: ruby scripts/dump_msdf_golden.rb --check
  (runs MLC MAE gate vs golden; C++ msdf_shim removed in STEP=5)
MSG
