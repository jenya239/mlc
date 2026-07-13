# frozen_string_literal: true

require "json"

module MLC
  module PackageManager
    class ManifestError < StandardError
    end

    Dependency = Struct.new(:name, :git, :rev, keyword_init: true)

    # Parses and validates project-root `mlc.json` (PACKAGE_MANAGER design freeze).
    class Manifest
      PACKAGE_NAME_PATTERN = /\A[a-z][a-z0-9_]*\z/
      COMMIT_SHA_PATTERN = /\A[0-9a-f]{40}\z/
      RESERVED_PACKAGE_NAMES = ["mlc_packages"].freeze

      attr_reader :name, :version, :dependencies, :path

      def initialize(name:, version:, dependencies:, path: nil)
        @name = name
        @version = version
        @dependencies = dependencies
        @path = path
      end

      def self.empty(path: nil)
        new(name: nil, version: nil, dependencies: {}, path: path)
      end

      # Load `<project_root>/mlc.json`. Missing file → empty dependencies.
      def self.load(project_root)
        path = File.join(project_root, "mlc.json")
        return empty(path: path) unless File.file?(path)

        parse(File.read(path), path: path)
      end

      def self.parse(json_text, path: nil)
        begin
          data = JSON.parse(json_text)
        rescue JSON::ParserError => error
          raise ManifestError, "invalid JSON in #{path || "mlc.json"}: #{error.message}"
        end

        unless data.is_a?(Hash)
          raise ManifestError, "#{path || "mlc.json"}: root must be a JSON object"
        end

        name = optional_string(data, "name", path)
        version = optional_string(data, "version", path)
        dependencies = parse_dependencies(data["dependencies"], path)

        new(name: name, version: version, dependencies: dependencies, path: path)
      end

      def dependency_names
        dependencies.keys
      end

      def dependency(package_name)
        dependencies[package_name]
      end

      def self.optional_string(data, key, path)
        return nil unless data.key?(key)

        value = data[key]
        unless value.is_a?(String)
          raise ManifestError, "#{label(path)}: #{key.inspect} must be a string"
        end

        value
      end
      private_class_method :optional_string

      def self.parse_dependencies(raw, path)
        return {} if raw.nil?

        unless raw.is_a?(Hash)
          raise ManifestError, "#{label(path)}: dependencies must be a JSON object"
        end

        result = {}
        raw.each do |package_name, spec|
          validate_package_name!(package_name, path)
          result[package_name] = parse_dependency(package_name, spec, path)
        end
        result
      end
      private_class_method :parse_dependencies

      def self.validate_package_name!(package_name, path)
        unless package_name.is_a?(String)
          raise ManifestError, "#{label(path)}: dependency key must be a string"
        end
        if RESERVED_PACKAGE_NAMES.include?(package_name)
          raise ManifestError, "#{label(path)}: dependency name #{package_name.inspect} is reserved"
        end
        unless package_name.match?(PACKAGE_NAME_PATTERN)
          raise ManifestError,
                "#{label(path)}: dependency name #{package_name.inspect} must match [a-z][a-z0-9_]*"
        end
      end
      private_class_method :validate_package_name!

      def self.parse_dependency(package_name, spec, path)
        unless spec.is_a?(Hash)
          raise ManifestError,
                "#{label(path)}: dependency #{package_name.inspect} must be a JSON object"
        end

        git = spec["git"]
        rev = spec["rev"]
        unless git.is_a?(String) && !git.empty?
          raise ManifestError,
                "#{label(path)}: dependency #{package_name.inspect} requires non-empty string git"
        end
        unless rev.is_a?(String) && rev.match?(COMMIT_SHA_PATTERN)
          raise ManifestError,
                "#{label(path)}: dependency #{package_name.inspect} requires full 40-char lowercase hex rev"
        end

        Dependency.new(name: package_name, git: git, rev: rev)
      end
      private_class_method :parse_dependency

      def self.label(path)
        path || "mlc.json"
      end
      private_class_method :label
    end
  end
end
