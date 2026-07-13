# frozen_string_literal: true

require "test_helper"
require "json"
require "tmpdir"
require "fileutils"
require "open3"

class PackageManagerFetcherTest < Minitest::Test
  def test_clones_and_is_idempotent
    Dir.mktmpdir do |directory|
      remote_path, first_revision, _second_revision = build_remote_repository(directory)
      project_root = File.join(directory, "app")
      FileUtils.mkdir_p(project_root)
      write_manifest(project_root, "demo_pkg", remote_path, first_revision)

      results = MLC::PackageManager::Fetcher.new(project_root: project_root).fetch_all
      assert_equal 1, results.length
      assert_equal :cloned, results[0].status
      package_directory = File.join(project_root, ".mlc_packages", "demo_pkg")
      assert File.file?(File.join(package_directory, "math.mlc"))
      assert_equal first_revision, git_head(package_directory)

      again = MLC::PackageManager::Fetcher.new(project_root: project_root).fetch_all
      assert_equal :already_at_rev, again[0].status
    end
  end

  def test_updates_when_rev_changes
    Dir.mktmpdir do |directory|
      remote_path, first_revision, second_revision = build_remote_repository(directory)
      project_root = File.join(directory, "app")
      FileUtils.mkdir_p(project_root)
      write_manifest(project_root, "demo_pkg", remote_path, first_revision)
      MLC::PackageManager::Fetcher.new(project_root: project_root).fetch_all

      write_manifest(project_root, "demo_pkg", remote_path, second_revision)
      results = MLC::PackageManager::Fetcher.new(project_root: project_root).fetch_all
      assert_equal :updated, results[0].status
      assert_equal second_revision, git_head(File.join(project_root, ".mlc_packages", "demo_pkg"))
      assert File.file?(File.join(project_root, ".mlc_packages", "demo_pkg", "extra.mlc"))
    end
  end

  def test_cli_prints_status_lines
    Dir.mktmpdir do |directory|
      remote_path, first_revision, _second = build_remote_repository(directory)
      project_root = File.join(directory, "app")
      FileUtils.mkdir_p(project_root)
      write_manifest(project_root, "demo_pkg", remote_path, first_revision)

      script = File.expand_path("../../../scripts/mlc_pkg_fetch.rb", __dir__)
      stdout, status = Open3.capture2(RbConfig.ruby, script, project_root)
      assert status.success?
      assert_match(/demo_pkg\tcloned\t#{first_revision}/, stdout)
    end
  end

  private

  def build_remote_repository(directory)
    source = File.join(directory, "source")
    remote = File.join(directory, "remote.git")
    FileUtils.mkdir_p(source)
    File.write(File.join(source, "math.mlc"), "fn add(a: i32, b: i32) -> i32 = a + b\n")
    run!(source, "git", "init")
    run!(source, "git", "config", "user.email", "test@example.com")
    run!(source, "git", "config", "user.name", "Test")
    run!(source, "git", "add", "math.mlc")
    run!(source, "git", "commit", "-m", "first")
    first_revision = git_head(source)

    File.write(File.join(source, "extra.mlc"), "fn one() -> i32 = 1\n")
    run!(source, "git", "add", "extra.mlc")
    run!(source, "git", "commit", "-m", "second")
    second_revision = git_head(source)

    run!(directory, "git", "clone", "--bare", source, remote)
    [remote, first_revision, second_revision]
  end

  def write_manifest(project_root, package_name, git_url, revision)
    File.write(
      File.join(project_root, "mlc.json"),
      JSON.pretty_generate(
        "name" => "app",
        "version" => "0.0.1",
        "dependencies" => {
          package_name => { "git" => git_url, "rev" => revision }
        }
      )
    )
  end

  def git_head(directory)
    stdout, status = Open3.capture2("git", "-C", directory, "rev-parse", "HEAD")
    raise "rev-parse failed" unless status.success?

    stdout.strip
  end

  def run!(directory, *command)
    _stdout, stderr, status = Open3.capture3(*command, chdir: directory)
    raise "command failed: #{command.join(" ")}: #{stderr}" unless status.success?
  end
end
