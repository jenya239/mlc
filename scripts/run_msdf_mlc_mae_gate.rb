#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_TEXT_MSDF_TO_MLC STEP=4 — MAE(MLC msdf_sdf_rgb_from_mask, C++ golden) ≤ 8.0/255.

require "fileutils"
require "tmpdir"

root = File.expand_path("..", __dir__)
font = ENV.fetch("MSDF_FONT", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")
fixture_dir = File.join(root, "misc/examples/fixtures")
rgb_golden = File.join(fixture_dir, "msdf_A_32_4.rgb")
mask_txt = File.join(fixture_dir, "msdf_A_32_4.mask.txt")
mask_mlc = File.join(fixture_dir, "msdf_A_32_4_mask.mlc")
codepoint = 65
field_size = 32
pixel_count = field_size * field_size
rgb_count = pixel_count * 3
mae_limit = 8.0
write_mask = ARGV.include?("--write-mask")
mlcc = ENV.fetch("MLCC", File.join(root, "compiler/out/mlcc"))

abort("[msdf mae] FAIL: missing font #{font}") unless File.file?(font)
abort("[msdf mae] FAIL: missing golden #{rgb_golden}") unless File.file?(rgb_golden)
abort("[msdf mae] FAIL: mlcc not executable") unless File.executable?(mlcc)

ft_cflags = `pkg-config --cflags freetype2`.strip
ft_libs = `pkg-config --libs freetype2`.strip
abort("[msdf mae] FAIL: pkg-config freetype2") if ft_cflags.empty? || ft_libs.empty?

def write_mask_mlc(mask_mlc, mask_lines)
  body = mask_lines.each_slice(32).map { |row| "  " + row.join(", ") }.join(",\n")
  File.write(mask_mlc, <<~MLC)
    // Auto-generated FreeType mask for DejaVuSans U+0041 field=32 (TRACK_TEXT_MSDF_TO_MLC).
    // Regenerate: ruby scripts/run_msdf_mlc_mae_gate.rb --write-mask
    export fn msdf_a_32_4_mask() -> [i32] = [
    #{body}
    ]
  MLC
end

Dir.mktmpdir("msdf_mae_", File.join(root, "tmp")) do |workdir|
  dump_cpp = File.join(workdir, "dump_mask.cpp")
  dump_bin = File.join(workdir, "dump_mask")
  live_mask = File.join(workdir, "live_mask.txt")

  File.write(dump_cpp, <<~CPP)
    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include <algorithm>
    #include <cstdint>
    #include <fstream>
    #include <vector>

    static bool render_glyph_mask(
      const char* font_path,
      int32_t codepoint,
      int32_t field_size,
      std::vector<uint8_t>& mask
    ) {
      FT_Library library = nullptr;
      if (FT_Init_FreeType(&library) != 0) return false;
      FT_Face face = nullptr;
      if (FT_New_Face(library, font_path, 0, &face) != 0) {
        FT_Done_FreeType(library);
        return false;
      }
      const int32_t pixel_size = std::max(8, field_size - 8);
      if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return false;
      }
      const FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(codepoint));
      if (glyph_index == 0
          || FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0
          || FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return false;
      }
      const int32_t glyph_width = static_cast<int32_t>(face->glyph->bitmap.width);
      const int32_t glyph_rows = static_cast<int32_t>(face->glyph->bitmap.rows);
      const int pitch = face->glyph->bitmap.pitch;
      mask.assign(static_cast<size_t>(field_size * field_size), 0);
      const int32_t origin_x = (field_size - glyph_width) / 2;
      const int32_t origin_y = (field_size - glyph_rows) / 2;
      for (int32_t row = 0; row < glyph_rows; row += 1) {
        for (int32_t column = 0; column < glyph_width; column += 1) {
          const uint8_t coverage =
            face->glyph->bitmap.buffer[static_cast<size_t>(row * pitch + column)];
          if (coverage < 128) continue;
          const int32_t x = origin_x + column;
          const int32_t y = origin_y + row;
          if (x < 0 || y < 0 || x >= field_size || y >= field_size) continue;
          mask[static_cast<size_t>(y * field_size + x)] = 1;
        }
      }
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return true;
    }

    int main(int argc, char** argv) {
      if (argc != 3) return 1;
      std::vector<uint8_t> mask;
      if (!render_glyph_mask(argv[1], #{codepoint}, #{field_size}, mask)) return 2;
      std::ofstream out(argv[2]);
      for (uint8_t bit : mask) out << static_cast<int>(bit) << '\\n';
      return out ? 0 : 3;
    }
  CPP

  warn "[msdf mae] dump mask"
  system("c++", "-std=c++20", "-O2", *ft_cflags.split, dump_cpp, *ft_libs.split, "-o", dump_bin) \
    or abort("[msdf mae] FAIL: mask dump compile")
  system(dump_bin, font, live_mask) or abort("[msdf mae] FAIL: mask dump run")

  live_lines = File.readlines(live_mask).map(&:strip)
  abort("[msdf mae] FAIL: mask size #{live_lines.size}") if live_lines.size != pixel_count

  if write_mask || !File.file?(mask_txt)
    FileUtils.mkdir_p(fixture_dir)
    File.write(mask_txt, live_lines.join("\n") + "\n")
    write_mask_mlc(mask_mlc, live_lines)
    warn "[msdf mae] wrote #{mask_txt} + #{mask_mlc}"
  end

  committed = File.readlines(mask_txt).map(&:strip)
  if committed != live_lines
    abort("[msdf mae] FAIL: live FreeType mask differs from committed fixture")
  end

  entry = File.join(root, "misc/examples/msdf_mlc_mae_gate.mlc")
  out_dir = File.join(workdir, "mlc_out")
  bin_out = File.join(workdir, "mae_bin")
  FileUtils.mkdir_p(out_dir)

  warn "[msdf mae] mlcc + link"
  env = {
    "TMPDIR" => File.join(root, "tmp"),
    "MLCC_PCH" => "0",
    "MLCC_DEV" => "1",
    "MLCC_ENTRY_BASENAME" => "msdf_mlc_mae_gate",
    "MLCC_OBJ_CLEAN" => "1"
  }
  system(env, mlcc, "-o", out_dir, entry) or abort("[msdf mae] FAIL: mlcc")
  system(env, File.join(root, "compiler/build_bin.sh"), out_dir, bin_out) \
    or abort("[msdf mae] FAIL: link")

  Dir.chdir(workdir) do
    system(bin_out) or abort("[msdf mae] FAIL: MLC binary exit")
  end

  rgb_out = File.join(workdir, "mlc.rgb.txt")
  mlc_bytes = File.readlines(rgb_out).map { |line| Integer(line.strip) }
  abort("[msdf mae] FAIL: MLC rgb count #{mlc_bytes.size}") if mlc_bytes.size != rgb_count

  golden_bytes = File.binread(rgb_golden).bytes
  sum_abs = 0.0
  rgb_count.times { |index| sum_abs += (mlc_bytes[index] - golden_bytes[index]).abs }
  mae = sum_abs / rgb_count.to_f
  warn format("[msdf mae] MAE=%.4f limit=%.1f", mae, mae_limit)
  abort("[msdf mae] FAIL: MAE #{mae} > #{mae_limit}") if mae > mae_limit
  warn "[msdf mae] PASS"
end
