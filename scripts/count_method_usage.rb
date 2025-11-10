#!/usr/bin/env ruby
# frozen_string_literal: true

# Count TypeChecker method usage
methods = %w[
  type_name
  normalized_type_name
  describe_type
  type_error
  ensure_type!
  ensure_compatible_type
  ensure_boolean_type
  ensure_numeric_type
  numeric_type?
  generic_type_name?
  unit_like?
  io_return_type
  validate_function_call
  ensure_argument_count
  module_member_info
  assign_expression_type
  validate_constraint_name
  type_satisfies_constraint?
  extract_actual_type_name
  validate_type_constraints
  record_field_type_map
  normalize_type_params
  infer_type_kind
]

search_dir = File.expand_path('../lib/mlc/semantic_gen', __dir__)

results = {}
methods.each do |method|
  pattern = /\.#{Regexp.escape(method)}\(/
  count = 0

  Dir.glob("#{search_dir}/**/*.rb").each do |file|
    content = File.read(file)
    count += content.scan(pattern).length
  end

  results[method] = count
end

results.sort_by { |_, count| -count }.each do |method, count|
  puts "#{method}: #{count}"
end
