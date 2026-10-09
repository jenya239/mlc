#!/usr/bin/env ruby

root_directory = File.expand_path("..", __dir__)
line_limit = 400
ratchet_limits = {
  "script_vm/interpreter.mlc" => 977,
  "script_vm/heap.mlc" => 496,
  "script_vm/embedding.mlc" => 410,
}

source_paths = Dir.glob(File.join(root_directory, "script_vm", "**", "*.mlc")).sort
violations = []

ratchet_limits.each_key do |relative_path|
  absolute_path = File.join(root_directory, relative_path)
  unless File.file?(absolute_path)
    violations << "#{relative_path} missing"
  end
end

source_paths.each do |absolute_path|
  relative_path = absolute_path.delete_prefix("#{root_directory}/")
  line_count = File.read(absolute_path).count("\n")
  allowed_maximum = ratchet_limits.fetch(relative_path, line_limit)
  next if line_count <= allowed_maximum

  violations << "#{relative_path} #{line_count} > #{allowed_maximum}"
end

if violations.empty?
  puts "script_vm file size: ok"
  exit 0
end

warn violations.join("\n")
exit 1
