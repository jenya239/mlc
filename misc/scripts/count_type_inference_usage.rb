#!/usr/bin/env ruby
# frozen_string_literal: true

# Count TypeInferenceService method usage
methods = %w[
  infer_variable_type
  infer_call_type
  infer_binary_type
  infer_unary_type
  expected_lambda_param_types
  infer_iterable_type
  infer_member_type
  unify_types
  substitute_type
  infer_type_arguments
  types_compatible?
  numeric_type?
  string_type?
  void_type?
  float_type?
  function_param_type
  combine_numeric_type
  infer_member_call_type
  infer_array_member_type
  infer_string_member_type
  lambda_return_type
  instantiate_generic_member_type
  function_placeholder_type
  function_type_from_info
]

search_dir = File.expand_path('../lib/mlc', __dir__)

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
