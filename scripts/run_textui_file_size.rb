#!/usr/bin/env ruby
# PLAN §112: no god-files under misc/textui/.
# New kit modules: hard max 400 lines. Prefer splitting before 250.
# Historical §32 demo misc/textui/main.mlc must not grow.

require "pathname"

ROOT = Pathname.new(__dir__).parent
TEXTUI = ROOT.join("misc/textui")
MAX_KIT_LINES = 400
MAIN_DEMO = "misc/textui/main.mlc"
MAIN_DEMO_MAX_LINES = 637

failures = []
Pathname.glob(TEXTUI.join("**/*.mlc")).sort.each do |absolute_path|
  relative_path = absolute_path.relative_path_from(ROOT).to_s
  line_count = absolute_path.readlines.size
  if relative_path == MAIN_DEMO
    if line_count > MAIN_DEMO_MAX_LINES
      failures << "#{relative_path}: #{line_count} lines (demo freeze #{MAIN_DEMO_MAX_LINES}; do not grow)"
    end
    next
  end
  if line_count > MAX_KIT_LINES
    failures << "#{relative_path}: #{line_count} lines (max #{MAX_KIT_LINES}; split, no allowlist)"
  end
end

if failures.empty?
  puts "textui file size: ok (kit max #{MAX_KIT_LINES}, #{MAIN_DEMO} freeze #{MAIN_DEMO_MAX_LINES})"
  exit 0
end

warn "textui file size FAIL:"
failures.each { |line| warn "  #{line}" }
exit 1
