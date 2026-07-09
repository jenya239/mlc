#!/usr/bin/env ruby
# frozen_string_literal: true

# OpenAPI 3.x → MLC codegen (MVP).
# Usage: ruby scripts/openapi_codegen.rb <openapi.yaml|json> [out.mlc]

require "json"
require "yaml"

module OpenapiCodegen
  module_function

  def load_spec(path)
    text = File.read(path)
    if path.end_with?(".json")
      JSON.parse(text)
    else
      YAML.safe_load(text, aliases: true)
    end
  end

  def pascal_case(name)
    name.to_s.split(/[^A-Za-z0-9]+/).reject(&:empty?).map { |part| part[0].upcase + part[1..] }.join
  end

  def snake_case(name)
    name.to_s
        .gsub(/([a-z0-9])([A-Z])/, '\1_\2')
        .gsub(/[^A-Za-z0-9]+/, "_")
        .downcase
        .gsub(/\A_+|_+\z/, "")
  end

  def resolve_ref(spec, node)
    return node unless node.is_a?(Hash) && node["$ref"]

    ref = node["$ref"]
    raise "unsupported $ref: #{ref}" unless ref.start_with?("#/")

    parts = ref.split("/")[1..]
    parts.reduce(spec) { |current, part| current.fetch(part) }
  end

  def schema_name_from_ref(ref)
    ref.split("/").last
  end

  def reset_collectors!
    @pending_types = []
    @emitted_type_names = {}
  end

  def mlc_type_from_schema(spec, schema, anonymous_name)
    return "JsonValue" if schema.nil?

    if schema.is_a?(Hash) && schema["$ref"]
      return pascal_case(schema_name_from_ref(schema["$ref"]))
    end

    schema = resolve_ref(spec, schema)

    if schema["oneOf"].is_a?(Array) && !schema["oneOf"].empty?
      return pascal_case(anonymous_name)
    end

    case schema["type"]
    when "string"
      if schema["enum"].is_a?(Array) && !schema["enum"].empty?
        pascal_case(anonymous_name)
      else
        "string"
      end
    when "integer"
      schema["format"] == "int32" ? "i32" : "i64"
    when "number"
      "f64"
    when "boolean"
      "bool"
    when "array"
      inner = mlc_type_from_schema(spec, schema.fetch("items"), "#{anonymous_name}Item")
      "Array<#{inner}>"
    when "object", nil
      if schema["properties"].is_a?(Hash)
        pascal_case(anonymous_name)
      else
        "JsonValue"
      end
    else
      "JsonValue"
    end
  end

  def collect_named_schema(spec, name, schema)
    return if schema.nil?

    if schema.is_a?(Hash) && schema["$ref"]
      ref_name = schema_name_from_ref(schema["$ref"])
      return collect_named_schema(spec, ref_name, resolve_ref(spec, schema))
    end

    schema = resolve_ref(spec, schema)
    type_name = pascal_case(name)
    return if @emitted_type_names[type_name]

    @emitted_type_names[type_name] = true

    if schema["oneOf"].is_a?(Array) && !schema["oneOf"].empty?
      variants = schema["oneOf"].each_with_index.map do |variant, index|
        if variant.is_a?(Hash) && variant["$ref"]
          variant_name = pascal_case(schema_name_from_ref(variant["$ref"]))
          collect_named_schema(spec, variant_name, variant)
          "#{variant_name}(#{variant_name})"
        else
          nested = "#{type_name}Alt#{index}"
          collect_named_schema(spec, nested, variant)
          "#{pascal_case(nested)}(#{pascal_case(nested)})"
        end
      end
      @pending_types << { kind: :sum, name: type_name, variants: variants }
      return
    end

    if schema["type"] == "string" && schema["enum"].is_a?(Array)
      variants = schema["enum"].map { |value| pascal_case(value.to_s) }
      @pending_types << { kind: :sum, name: type_name, variants: variants }
      return
    end

    if schema["type"] == "object" || schema["properties"]
      collect_object_type(spec, type_name, schema)
      return
    end

    if schema["type"] == "array" && schema["items"].is_a?(Hash)
      collect_named_schema(spec, "#{type_name}Item", schema["items"])
    end
  end

  def collect_object_type(spec, type_name, schema)
    schema = resolve_ref(spec, schema)
    properties = schema["properties"] || {}
    required = Array(schema["required"]).map(&:to_s)
    fields = properties.map do |field_name, field_schema|
      nested_name = "#{type_name}#{pascal_case(field_name)}"
      if field_schema.is_a?(Hash) && field_schema["$ref"]
        collect_named_schema(spec, schema_name_from_ref(field_schema["$ref"]), field_schema)
      elsif field_schema.is_a?(Hash) && (field_schema["properties"] || field_schema["oneOf"] || field_schema["enum"])
        collect_named_schema(spec, nested_name, field_schema)
      elsif field_schema.is_a?(Hash) && field_schema["type"] == "array" && field_schema["items"].is_a?(Hash)
        items = field_schema["items"]
        if items["$ref"]
          collect_named_schema(spec, schema_name_from_ref(items["$ref"]), items)
        elsif items["properties"] || items["enum"] || items["oneOf"]
          collect_named_schema(spec, "#{nested_name}Item", items)
        end
      end

      mlc_type = mlc_type_from_schema(spec, field_schema, nested_name)
      unless required.include?(field_name.to_s)
        mlc_type = "Option<#{mlc_type}>" unless mlc_type.start_with?("Option<")
      end
      "  #{snake_case(field_name)}: #{mlc_type}"
    end
    @pending_types << { kind: :record, name: pascal_case(type_name), fields: fields }
  end

  def render_types
    lines = []
    @pending_types.each do |entry|
      case entry[:kind]
      when :record
        body = entry[:fields].empty? ? "" : "\n#{entry[:fields].join(",\n")}\n"
        lines << "type #{entry[:name]} = {#{body}} derive { Json }"
      when :sum
        lines << "type #{entry[:name]} = #{entry[:variants].join(" | ")} derive { Json }"
      end
      lines << ""
    end
    lines.join("\n")
  end

  def path_parameter_type(spec, parameter)
    schema = parameter["schema"] || { "type" => "string" }
    mlc_type_from_schema(spec, schema, parameter["name"] || "Param")
  end

  def response_schema(operation)
    responses = operation["responses"] || {}
    success = responses["200"] || responses["201"] || responses.values.find { |value| value.is_a?(Hash) }
    return nil unless success.is_a?(Hash)

    content = success["content"] || {}
    json = content["application/json"] || content.values.find { |value| value.is_a?(Hash) }
    return nil unless json.is_a?(Hash)

    json["schema"]
  end

  def collect_operation_schemas(spec)
    paths = spec["paths"] || {}
    paths.each_value do |methods|
      next unless methods.is_a?(Hash)

      methods.each do |http_method, operation|
        next unless operation.is_a?(Hash)
        next if %w[parameters servers].include?(http_method)

        operation_id = operation["operationId"] || "#{http_method}_op"
        if operation["requestBody"].is_a?(Hash)
          body = resolve_ref(spec, operation["requestBody"])
          content = body["content"] || {}
          json = content["application/json"] || content.values.find { |value| value.is_a?(Hash) }
          if json.is_a?(Hash) && json["schema"]
            schema = json["schema"]
            if schema["$ref"]
              collect_named_schema(spec, schema_name_from_ref(schema["$ref"]), schema)
            elsif schema["properties"] || schema["oneOf"] || schema["enum"]
              collect_named_schema(spec, "#{operation_id}Body", schema)
            end
          end
        end

        schema = response_schema(operation)
        next unless schema

        if schema["$ref"]
          collect_named_schema(spec, schema_name_from_ref(schema["$ref"]), schema)
        elsif schema["properties"] || schema["oneOf"] || schema["enum"]
          collect_named_schema(spec, "#{operation_id}Response", schema)
        end
      end
    end
  end

  def render_client_functions(spec)
    paths = spec["paths"] || {}
    lines = []
    lines << "type ApiClient = { base_url: string }"
    lines << ""
    lines << "type ApiError = string"
    lines << ""

    paths.each do |path, methods|
      next unless methods.is_a?(Hash)

      methods.each do |http_method, operation|
        next unless operation.is_a?(Hash)
        next if %w[parameters servers].include?(http_method)

        operation_id = operation["operationId"] || "#{http_method}_#{path}"
        function_name = snake_case(operation_id)
        parameters = Array(operation["parameters"])
        path_item_params = Array(methods["parameters"])
        all_params = path_item_params + parameters

        args = ["client: ApiClient"]
        all_params.each do |parameter|
          parameter = resolve_ref(spec, parameter)
          next unless parameter["in"] == "path" || parameter["in"] == "query"

          args << "#{snake_case(parameter["name"])}: #{path_parameter_type(spec, parameter)}"
        end

        if operation["requestBody"].is_a?(Hash)
          body = resolve_ref(spec, operation["requestBody"])
          content = body["content"] || {}
          json = content["application/json"] || content.values.find { |value| value.is_a?(Hash) }
          if json.is_a?(Hash) && json["schema"]
            body_type = mlc_type_from_schema(spec, json["schema"], "#{operation_id}Body")
            args << "body: #{body_type}"
          end
        end

        return_type = "Result<JsonValue, ApiError>"
        schema = response_schema(operation)
        if schema
          if schema["$ref"]
            return_type = "Result<#{pascal_case(schema_name_from_ref(schema["$ref"]))}, ApiError>"
          elsif schema["properties"] || schema["oneOf"] || schema["enum"]
            return_type = "Result<#{pascal_case("#{operation_id}Response")}, ApiError>"
          end
        end

        lines << "// #{http_method.upcase} #{path}"
        lines << "fn #{function_name}(#{args.join(", ")}) -> #{return_type} = do"
        lines << "  let _ = client.base_url"
        lines << "  Err(\"openapi_codegen: #{function_name} not wired to fetch yet\")"
        lines << "end"
        lines << ""
      end
    end
    lines.join("\n")
  end

  def generate(spec)
    reset_collectors!
    schemas = (((spec["components"] || {})["schemas"]) || {})
    schemas.each do |name, schema|
      collect_named_schema(spec, name, schema)
    end
    collect_operation_schemas(spec)
    header = [
      "// Generated by scripts/openapi_codegen.rb — do not edit by hand.",
      "import Json",
      "",
    ].join("\n")
    header + render_types + render_client_functions(spec)
  end
end

if $PROGRAM_NAME == __FILE__
  input_path = ARGV[0]
  output_path = ARGV[1]
  abort "usage: openapi_codegen.rb <openapi.yaml|json> [out.mlc]" if input_path.nil?

  spec = OpenapiCodegen.load_spec(input_path)
  source = OpenapiCodegen.generate(spec)
  if output_path
    File.write(output_path, source)
    warn "wrote #{output_path} (#{source.lines.size} lines)"
  else
    print source
  end
end
