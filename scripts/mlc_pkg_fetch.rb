#!/usr/bin/env ruby
# frozen_string_literal: true

# Fetch pinned git deps from project-root mlc.json into .mlc_packages/.
# Usage: ruby scripts/mlc_pkg_fetch.rb [project_root]

require_relative "../lib/mlc/common/index"

project_root = ARGV[0] || Dir.pwd

begin
  results = MLC::PackageManager::Fetcher.new(project_root: project_root).fetch_all
  if results.empty?
    warn "mlc_pkg_fetch: no dependencies in #{File.join(project_root, "mlc.json")}"
    exit 0
  end

  results.each do |result|
    puts "#{result.name}\t#{result.status}\t#{result.rev}\t#{result.path}"
  end
rescue MLC::PackageManager::ManifestError, MLC::PackageManager::FetchError => error
  warn "mlc_pkg_fetch: #{error.message}"
  exit 1
end
