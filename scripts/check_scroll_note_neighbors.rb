#!/usr/bin/env ruby
# §111a1 — each clamp_scroll_to_content / ensure_caret_visible in demo_live
# must be followed by editor_live_note_scroll_changed within LOOKAHEAD lines.
LOOKAHEAD = 20
demo = ARGV[0] || abort("usage: check_scroll_note_neighbors.rb demo_live.mlc")
lines = File.readlines(demo)
patterns = [
  /editor_app_clamp_scroll_to_content\(/,
  /editor_app_ensure_caret_visible\(/
]
note = /editor_live_note_scroll_changed/
missing = []
lines.each_with_index do |line, index|
  next unless patterns.any? { |pattern| line =~ pattern }

  window = lines[index, LOOKAHEAD] || []
  next if window.any? { |candidate| candidate =~ note }

  missing << (index + 1)
end
if missing.empty?
  puts "ok scroll_note_neighbors"
  exit 0
end
warn "[scroll note neighbors] FAIL: no note within #{LOOKAHEAD} lines after clamp/ensure at lines #{missing.join(',')}"
exit 1
