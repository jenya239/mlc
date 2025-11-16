#!/usr/bin/env ruby
# Fix all references to SemanticGen::Pipeline

files = [
  "test/mlc/import_service_integration_test.rb",
  "test/mlc/function_registry_integration_test.rb",
  "test/mlc/rules_rule_engine_test.rb",
  "test/mlc/type_inference_architecture_test.rb",
  "lib/mlc/representations/semantic/gen/services/inference/type_inference_service.rb"
]

files.each do |file|
  next unless File.exist?(file)

  content = File.read(file)
  original = content.dup

  # Replace all variations of SemanticGen::Pipeline
  content.gsub!(/MLC::SemanticGen::Pipeline/, 'MLC::Representations::Semantic::Gen::Pipeline')
  content.gsub!(/SemanticGen::Pipeline/, 'MLC::Representations::Semantic::Gen::Pipeline')

  if content != original
    File.write(file, content)
    puts "✓ Updated #{file}"
  else
    puts "  Skipped #{file} (no changes)"
  end
end

puts "\nDone!"
