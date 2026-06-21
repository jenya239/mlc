# frozen_string_literal: true

require "test_helper"

# Full toolchain: Ruby builds mlcc, mlcc emits compiler/out/bootstrap/*.cpp, g++ links mlcc_bootstrap.
# Slow (~minutes); enable with MLC_REQUIRE_BOOTSTRAP_LINK=1 (CI gate when bootstrap must stay green).
class SelfHostedMlccBootstrapLinkTest < Minitest::Test
  def test_mlcc_bootstrap_binary_links
    skip "set MLC_REQUIRE_BOOTSTRAP_LINK=1 to run" unless ENV["MLC_REQUIRE_BOOTSTRAP_LINK"] == "1"

    root = File.expand_path("../..", __dir__)
    compiler = File.join(root, "compiler")
    build_sh = File.join(compiler, "build.sh")
    bootstrap_exe = File.join(compiler, "out", "bootstrap", "mlcc_bootstrap")

    Dir.chdir(root) do
      assert_equal true, system("MLCC_FORCE_RUBY=1 compiler/build.sh")
      output = `cd #{compiler} && MLCC_BOOTSTRAP=1 ./build.sh 2>&1`
      assert_equal 0, $?.exitstatus, "bootstrap build failed:\n#{output}"

      assert_path_exists bootstrap_exe, "expected #{bootstrap_exe}"
      assert_equal true, File.executable?(bootstrap_exe), "#{bootstrap_exe} not executable"
    end
  end
end
