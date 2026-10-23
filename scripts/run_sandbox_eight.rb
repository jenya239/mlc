#!/usr/bin/env ruby

ARGV.replace([File.expand_path("../script_host/tools/sandbox_eight.mlc", __dir__)])
load File.expand_path("run_script_vm_sandbox_child.rb", __dir__)
