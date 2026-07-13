# frozen_string_literal: true

require "fileutils"
require "open3"

module MLC
  module PackageManager
    class FetchError < StandardError
    end

    # Clones/checks out pinned deps into `<project_root>/.mlc_packages/<pkg>/`.
    class Fetcher
      VENDOR_DIRECTORY_NAME = ".mlc_packages"

      Result = Struct.new(:name, :status, :path, :rev, keyword_init: true)

      def initialize(project_root:)
        @project_root = File.expand_path(project_root)
      end

      def vendor_root
        File.join(@project_root, VENDOR_DIRECTORY_NAME)
      end

      def fetch_all
        manifest = Manifest.load(@project_root)
        FileUtils.mkdir_p(vendor_root)
        manifest.dependencies.values.map { |dependency| fetch_one(dependency) }
      end

      def fetch_one(dependency)
        package_directory = File.join(vendor_root, dependency.name)
        assert_under_vendor!(package_directory)

        if git_repository?(package_directory)
          current_revision = git_head(package_directory)
          if current_revision == dependency.rev
            return Result.new(
              name: dependency.name,
              status: :already_at_rev,
              path: package_directory,
              rev: dependency.rev
            )
          end

          sync_existing(package_directory, dependency)
          return Result.new(
            name: dependency.name,
            status: :updated,
            path: package_directory,
            rev: dependency.rev
          )
        end

        FileUtils.rm_rf(package_directory) if Dir.exist?(package_directory)
        clone_and_checkout(package_directory, dependency)
        Result.new(
          name: dependency.name,
          status: :cloned,
          path: package_directory,
          rev: dependency.rev
        )
      end

      private

      def assert_under_vendor!(package_directory)
        vendor = File.expand_path(vendor_root) + File::SEPARATOR
        expanded = File.expand_path(package_directory)
        return if expanded == File.expand_path(vendor_root)
        return if expanded.start_with?(vendor)

        raise FetchError, "refusing path outside #{VENDOR_DIRECTORY_NAME}: #{package_directory}"
      end

      def git_repository?(directory)
        Dir.exist?(File.join(directory, ".git"))
      end

      def git_head(directory)
        stdout, status = Open3.capture2("git", "-C", directory, "rev-parse", "HEAD")
        raise FetchError, "git rev-parse failed in #{directory}" unless status.success?

        stdout.strip
      end

      def sync_existing(package_directory, dependency)
        run_git!(package_directory, "fetch", "--force", "origin", dependency.rev)
        run_git!(package_directory, "checkout", "--force", dependency.rev)
      end

      def clone_and_checkout(package_directory, dependency)
        parent = File.dirname(package_directory)
        FileUtils.mkdir_p(parent)
        run_git!(nil, "clone", dependency.git, package_directory)
        run_git!(package_directory, "checkout", "--force", dependency.rev)
      end

      def run_git!(directory, *arguments)
        command = ["git"]
        command.concat(["-C", directory]) if directory
        command.concat(arguments)
        stdout, stderr, status = Open3.capture3(*command)
        return if status.success?

        raise FetchError,
              "git #{arguments.join(" ")} failed: #{stderr.empty? ? stdout : stderr}".strip
      end
    end
  end
end
