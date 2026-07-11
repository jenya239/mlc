#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_TEXT_MSDF_TO_MLC STEP=2 — dump/verify C++ msdf_shim golden RGB.
# Default: write fixture. --check: regenerate to temp and compare to committed.

require "digest"
require "fileutils"
require "tmpdir"

root = File.expand_path("..", __dir__)
font = ENV.fetch("MSDF_FONT", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")
fixture_dir = File.join(root, "misc/examples/fixtures")
rgb_path = File.join(fixture_dir, "msdf_A_32_4.rgb")
meta_path = File.join(fixture_dir, "msdf_A_32_4.meta.txt")
codepoint = 65
field_size = 32
px_range = 4
expected_bytes = field_size * field_size * 3
check_only = ARGV.include?("--check")

abort("[msdf golden] FAIL: missing font #{font}") unless File.file?(font)

runtime_include = File.join(root, "runtime/include")
shim_cpp = File.join(root, "runtime/src/text/msdf_shim.cpp")
string_cpp = File.join(root, "runtime/src/core/string.cpp")
abort("[msdf golden] FAIL: missing #{shim_cpp}") unless File.file?(shim_cpp)

ft_cflags = `pkg-config --cflags freetype2`.strip
ft_libs = `pkg-config --libs freetype2`.strip
abort("[msdf golden] FAIL: pkg-config freetype2") if ft_cflags.empty? || ft_libs.empty?

Dir.mktmpdir("msdf_golden_") do |workdir|
  dump_cpp = File.join(workdir, "dump_msdf_golden.cpp")
  binary = File.join(workdir, "dump_msdf_golden")
  out_rgb = File.join(workdir, "out.rgb")

  File.write(dump_cpp, <<~CPP)
    #include "mlc/text/msdf_shim.hpp"
    #include <cstdio>
    #include <cstdint>
    #include <fstream>
    #include <vector>

    int main(int argc, char** argv) {
      if (argc != 3) {
        std::fprintf(stderr, "usage: dump_msdf_golden <font> <out.rgb>\\n");
        return 1;
      }
      const int status = mlc::text::msdf_generate(
        mlc::String(argv[1]), #{codepoint}, #{field_size}, #{px_range});
      if (status != 0) {
        std::fprintf(stderr, "msdf_generate failed: %d\\n", status);
        return 2;
      }
      if (mlc::text::msdf_width() != #{field_size}
          || mlc::text::msdf_height() != #{field_size}
          || mlc::text::msdf_px_range() != #{px_range}
          || mlc::text::msdf_byte_count() != #{expected_bytes}) {
        std::fprintf(stderr, "unexpected dimensions/count\\n");
        return 3;
      }
      const std::uint8_t* data = mlc::text::msdf_rgb_data();
      if (data == nullptr) {
        return 4;
      }
      std::ofstream out(argv[2], std::ios::binary);
      out.write(reinterpret_cast<const char*>(data), #{expected_bytes});
      if (!out) {
        return 5;
      }
      return 0;
    }
  CPP

  compile = [
    "c++", "-std=c++20", "-O2",
    "-I#{runtime_include}",
    *ft_cflags.split,
    dump_cpp, shim_cpp, string_cpp,
    *ft_libs.split,
    "-o", binary
  ]
  warn "[msdf golden] compile"
  system(*compile) or abort("[msdf golden] FAIL: compile")
  system(binary, font, out_rgb) or abort("[msdf golden] FAIL: dump run")

  bytes = File.binread(out_rgb)
  abort("[msdf golden] FAIL: size #{bytes.bytesize} != #{expected_bytes}") if bytes.bytesize != expected_bytes
  abort("[msdf golden] FAIL: R!=G or G!=B at [0]") unless bytes[0] == bytes[1] && bytes[1] == bytes[2]

  digest = Digest::SHA256.hexdigest(bytes)
  meta = <<~META
    font=#{font}
    codepoint=#{codepoint}
    field_size=#{field_size}
    px_range=#{px_range}
    byte_count=#{expected_bytes}
    sha256=#{digest}
    layout=RGB8 interleaved, R=G=B single-channel SDF (msdf_shim)
    source=runtime/src/text/msdf_shim.cpp
  META

  if check_only
    abort("[msdf golden] FAIL: missing #{rgb_path}") unless File.file?(rgb_path)
    committed = File.binread(rgb_path)
    if committed != bytes
      abort("[msdf golden] FAIL: live C++ RGB differs from committed fixture")
    end
    warn "[msdf golden] CHECK ok (#{expected_bytes} bytes, sha256=#{digest[0, 12]}…)"
  else
    FileUtils.mkdir_p(fixture_dir)
    File.binwrite(rgb_path, bytes)
    File.write(meta_path, meta)
    warn "[msdf golden] wrote #{rgb_path} (#{expected_bytes} bytes)"
    warn "[msdf golden] wrote #{meta_path}"
    warn "[msdf golden] sha256=#{digest}"
  end
end
