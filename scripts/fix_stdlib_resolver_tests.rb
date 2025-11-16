#!/usr/bin/env ruby
# Fix StdlibResolver references in tests

files = [
  "test/mlc/stdlib_scanner_integration_test.rb",
  "test/mlc/stdlib_resolution_test.rb"
]

files.each do |file|
  next unless File.exist?(file)

  content = File.read(file)
  original = content.dup

  # Replace MLC::Common::StdlibResolver with MLC::Common::Stdlib::Resolver
  content.gsub!(/MLC::Common::StdlibResolver/, 'MLC::Common::Stdlib::Resolver')

  if content != original
    File.write(file, content)
    puts "✓ Updated #{file}"
  else
    puts "  Skipped #{file} (no changes)"
  end
end

puts "\nDone!"
