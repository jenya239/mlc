# frozen_string_literal: true

require "test_helper"
require "tmpdir"
require "fileutils"

class PackageManagerManifestTest < Minitest::Test
  VALID_REV = "0123456789abcdef0123456789abcdef01234567"

  def test_missing_file_is_empty_manifest
    Dir.mktmpdir do |directory|
      manifest = MLC::PackageManager::Manifest.load(directory)
      assert_nil manifest.name
      assert_nil manifest.version
      assert_equal({}, manifest.dependencies)
    end
  end

  def test_parses_valid_manifest
    Dir.mktmpdir do |directory|
      write_manifest(directory, <<~JSON)
        {
          "name": "my_app",
          "version": "0.1.0",
          "dependencies": {
            "example_pkg": {
              "git": "https://github.com/org/example_pkg.git",
              "rev": "#{VALID_REV}"
            }
          },
          "mlc_version": "ignored",
          "extra_root": true
        }
      JSON

      manifest = MLC::PackageManager::Manifest.load(directory)
      assert_equal "my_app", manifest.name
      assert_equal "0.1.0", manifest.version
      assert_equal ["example_pkg"], manifest.dependency_names
      dependency = manifest.dependency("example_pkg")
      assert_equal "https://github.com/org/example_pkg.git", dependency.git
      assert_equal VALID_REV, dependency.rev
    end
  end

  def test_empty_dependencies_valid
    manifest = MLC::PackageManager::Manifest.parse('{"dependencies":{}}')
    assert_equal({}, manifest.dependencies)
  end

  def test_rejects_invalid_package_name
    error = assert_raises(MLC::PackageManager::ManifestError) do
      MLC::PackageManager::Manifest.parse(<<~JSON)
        {"dependencies":{"Bad-Name":{"git":"u","rev":"#{VALID_REV}"}}}
      JSON
    end
    assert_match(/must match/, error.message)
  end

  def test_rejects_reserved_package_name
    error = assert_raises(MLC::PackageManager::ManifestError) do
      MLC::PackageManager::Manifest.parse(<<~JSON)
        {"dependencies":{"mlc_packages":{"git":"u","rev":"#{VALID_REV}"}}}
      JSON
    end
    assert_match(/reserved/, error.message)
  end

  def test_rejects_short_rev
    error = assert_raises(MLC::PackageManager::ManifestError) do
      MLC::PackageManager::Manifest.parse(<<~JSON)
        {"dependencies":{"pkg":{"git":"https://x","rev":"abc"}}}
      JSON
    end
    assert_match(/40-char/, error.message)
  end

  def test_rejects_uppercase_rev
    error = assert_raises(MLC::PackageManager::ManifestError) do
      MLC::PackageManager::Manifest.parse(<<~JSON)
        {"dependencies":{"pkg":{"git":"https://x","rev":"#{VALID_REV.upcase}"}}}
      JSON
    end
    assert_match(/40-char/, error.message)
  end

  def test_rejects_missing_git
    error = assert_raises(MLC::PackageManager::ManifestError) do
      MLC::PackageManager::Manifest.parse(<<~JSON)
        {"dependencies":{"pkg":{"rev":"#{VALID_REV}"}}}
      JSON
    end
    assert_match(/git/, error.message)
  end

  def test_rejects_invalid_json
    error = assert_raises(MLC::PackageManager::ManifestError) do
      MLC::PackageManager::Manifest.parse("{")
    end
    assert_match(/invalid JSON/, error.message)
  end

  private

  def write_manifest(directory, contents)
    File.write(File.join(directory, "mlc.json"), contents)
  end
end
