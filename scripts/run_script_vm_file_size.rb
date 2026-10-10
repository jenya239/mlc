#!/usr/bin/env ruby

root_directory = File.expand_path("..", __dir__)
line_limit = 400

source_paths = Dir.glob(File.join(root_directory, "script_vm", "**", "*.mlc")).sort
violations = []

source_paths.each do |absolute_path|
  relative_path = absolute_path.delete_prefix("#{root_directory}/")
  line_count = File.read(absolute_path).count("\n")
  next if line_count <= line_limit

  violations << "#{relative_path} #{line_count} > #{line_limit}"
end

if violations.empty?
  puts "script_vm file size: ok"
  exit 0
end

warn violations.join("\n")
exit 1
