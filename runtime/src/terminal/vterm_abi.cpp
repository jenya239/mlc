#include "mlc/terminal/vterm_abi.hpp"

#include <vterm.h>

#include <cstdint>
#include <unordered_map>

namespace mlc {
namespace terminal {
namespace {

struct LastCellSlot {
  std::int32_t codepoint = 0;
  std::int32_t width = 0;
  bool foreground_is_indexed = false;
  std::uint8_t foreground_red = 0;
  std::uint8_t foreground_green = 0;
  std::uint8_t foreground_blue = 0;
  std::uint8_t foreground_index = 0;
  bool background_is_indexed = false;
  std::uint8_t background_red = 0;
  std::uint8_t background_green = 0;
  std::uint8_t background_blue = 0;
  std::uint8_t background_index = 0;
};

LastCellSlot& last_cell_slot() {
  static thread_local LastCellSlot slot;
  return slot;
}

std::unordered_map<VTermScreen*, std::int32_t>& damage_counts() {
  static thread_local std::unordered_map<VTermScreen*, std::int32_t> counts;
  return counts;
}

int on_damage(VTermRect, void* user) {
  VTermScreen* screen = static_cast<VTermScreen*>(user);
  damage_counts()[screen] += 1;
  return 1;
}

VTermScreenCallbacks damage_only_callbacks() {
  VTermScreenCallbacks callbacks{};
  callbacks.damage = on_damage;
  return callbacks;
}

VTerm* i64_to_vterm(std::int64_t vterm_handle) {
  return reinterpret_cast<VTerm*>(vterm_handle);
}

VTermScreen* i64_to_screen(std::int64_t screen_handle) {
  return reinterpret_cast<VTermScreen*>(screen_handle);
}

std::int64_t pointer_to_i64(void* pointer) {
  return reinterpret_cast<std::int64_t>(pointer);
}

} // namespace

std::int64_t vterm_create(std::int32_t rows, std::int32_t columns) {
  if (rows <= 0 || columns <= 0) {
    return 0;
  }
  VTerm* vterm = vterm_new(rows, columns);
  if (vterm == nullptr) {
    return 0;
  }
  // Real PTY sessions overwhelmingly emit UTF-8 today; without this,
  // multi-byte input decodes as raw Latin-1 bytes, one per cell.
  vterm_set_utf8(vterm, 1);
  return pointer_to_i64(vterm);
}

std::int32_t vterm_destroy(std::int64_t vterm_handle) {
  VTerm* vterm = i64_to_vterm(vterm_handle);
  if (vterm == nullptr) {
    return -1;
  }
  vterm_free(vterm);
  return 0;
}

std::int64_t vterm_obtain_screen(std::int64_t vterm_handle) {
  VTerm* vterm = i64_to_vterm(vterm_handle);
  if (vterm == nullptr) {
    return 0;
  }
  VTermScreen* screen = ::vterm_obtain_screen(vterm);
  if (screen == nullptr) {
    return 0;
  }
  static const VTermScreenCallbacks callbacks = damage_only_callbacks();
  vterm_screen_set_callbacks(screen, &callbacks, screen);
  vterm_screen_reset(screen, 1);

  VTermColor default_foreground;
  VTermColor default_background;
  vterm_color_rgb(&default_foreground, 255, 255, 255);
  vterm_color_rgb(&default_background, 0, 0, 0);
  // vterm_screen_convert_color_to_rgb (used by vterm_read_screen_cell) reads
  // the screen's own default colors, distinct from vterm_state_*'s — set
  // both would be redundant/inconsistent, this is the one that matters here.
  vterm_screen_set_default_colors(screen, &default_foreground, &default_background);
  return pointer_to_i64(screen);
}

std::int32_t vterm_write_input(std::int64_t vterm_handle, String bytes) {
  VTerm* vterm = i64_to_vterm(vterm_handle);
  if (vterm == nullptr) {
    return -1;
  }
  const std::string_view view = bytes.view();
  return static_cast<std::int32_t>(vterm_input_write(vterm, view.data(), view.size()));
}

String vterm_escape_byte() {
  return String(std::string(1, '\x1b'));
}

std::int32_t vterm_read_screen_cell(std::int64_t screen_handle, std::int32_t row, std::int32_t column) {
  VTermScreen* screen = i64_to_screen(screen_handle);
  if (screen == nullptr || row < 0 || column < 0) {
    return -1;
  }
  VTermPos position{};
  position.row = row;
  position.col = column;
  VTermScreenCell cell{};
  if (vterm_screen_get_cell(screen, position, &cell) == 0) {
    return -2;
  }
  LastCellSlot& slot = last_cell_slot();
  slot.codepoint = static_cast<std::int32_t>(cell.chars[0]);
  slot.width = cell.width;

  // Capture original color-type diagnostics before resolving to concrete
  // RGB below (convert_color_to_rgb mutates the color's type in place).
  slot.foreground_is_indexed = VTERM_COLOR_IS_INDEXED(&cell.fg);
  slot.foreground_index = cell.fg.indexed.idx;
  slot.background_is_indexed = VTERM_COLOR_IS_INDEXED(&cell.bg);
  slot.background_index = cell.bg.indexed.idx;

  // Resolves RGB/indexed/default alike to a concrete RGB triple using the
  // screen's palette + default colors (set in vterm_obtain_screen) — a
  // no-op for already-RGB colors (e.g. SGR truecolor).
  vterm_screen_convert_color_to_rgb(screen, &cell.fg);
  vterm_screen_convert_color_to_rgb(screen, &cell.bg);
  slot.foreground_red = cell.fg.rgb.red;
  slot.foreground_green = cell.fg.rgb.green;
  slot.foreground_blue = cell.fg.rgb.blue;
  slot.background_red = cell.bg.rgb.red;
  slot.background_green = cell.bg.rgb.green;
  slot.background_blue = cell.bg.rgb.blue;
  return 0;
}

namespace {
std::string utf8_encode_codepoint(std::uint32_t codepoint) {
  std::string encoded;
  if (codepoint == 0) {
    encoded.push_back(' ');
  } else if (codepoint <= 0x7F) {
    encoded.push_back(static_cast<char>(codepoint));
  } else if (codepoint <= 0x7FF) {
    encoded.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
    encoded.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint <= 0xFFFF) {
    encoded.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
    encoded.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    encoded.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else {
    encoded.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
    encoded.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
    encoded.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    encoded.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
  return encoded;
}
} // namespace

std::int32_t vterm_last_cell_codepoint() { return last_cell_slot().codepoint; }
String vterm_last_cell_utf8() {
  return String(utf8_encode_codepoint(static_cast<std::uint32_t>(last_cell_slot().codepoint)));
}
std::int32_t vterm_last_cell_width() { return last_cell_slot().width; }
std::int32_t vterm_last_cell_foreground_is_indexed() { return last_cell_slot().foreground_is_indexed ? 1 : 0; }
std::int32_t vterm_last_cell_foreground_red() { return last_cell_slot().foreground_red; }
std::int32_t vterm_last_cell_foreground_green() { return last_cell_slot().foreground_green; }
std::int32_t vterm_last_cell_foreground_blue() { return last_cell_slot().foreground_blue; }
std::int32_t vterm_last_cell_foreground_index() { return last_cell_slot().foreground_index; }
std::int32_t vterm_last_cell_background_is_indexed() { return last_cell_slot().background_is_indexed ? 1 : 0; }
std::int32_t vterm_last_cell_background_red() { return last_cell_slot().background_red; }
std::int32_t vterm_last_cell_background_green() { return last_cell_slot().background_green; }
std::int32_t vterm_last_cell_background_blue() { return last_cell_slot().background_blue; }
std::int32_t vterm_last_cell_background_index() { return last_cell_slot().background_index; }

std::int32_t vterm_damage_count(std::int64_t screen_handle) {
  VTermScreen* screen = i64_to_screen(screen_handle);
  if (screen == nullptr) {
    return -1;
  }
  auto& counts = damage_counts();
  auto found = counts.find(screen);
  return found == counts.end() ? 0 : found->second;
}

} // namespace terminal
} // namespace mlc
