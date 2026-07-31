#pragma once

// Thin libvterm ABI for TRACK_EDITOR_TERMINAL §102a.
// Handles cross the MLC boundary as i64 (freetype_abi/harfbuzz_abi pattern).
// MLC owns PTY plumbing and rendering; this shim only owns VT100/ANSI parsing
// via libvterm.

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace terminal {

// vterm_new. 0 = error.
std::int64_t vterm_create(std::int32_t rows, std::int32_t columns);

// vterm_free.
std::int32_t vterm_destroy(std::int64_t vterm_handle);

// vterm_obtain_screen + vterm_screen_reset(hard=1) + register damage
// callback + vterm_screen_set_default_colors (white fg / black bg, so
// uncolored output resolves to a visible color via
// vterm_screen_convert_color_to_rgb instead of an unconfigured fallback).
// 0 = error.
std::int64_t vterm_obtain_screen(std::int64_t vterm_handle);

// vterm_input_write. Returns bytes consumed.
std::int32_t vterm_write_input(std::int64_t vterm_handle, String bytes);

// Single-byte string holding ASCII ESC (0x1b) — MLC string literals have no
// hex/control-byte escape syntax, so VT100/ANSI sequence construction (SGR
// colors, arrow-key input in §102d) composes this with plain text.
String vterm_escape_byte();

// vterm_screen_get_cell at (row, column), fills the last-cell slot.
// 0 = ok, negative = error.
std::int32_t vterm_read_screen_cell(std::int64_t screen_handle, std::int32_t row, std::int32_t column);

// First Unicode codepoint of the last-read cell (0 = empty cell).
std::int32_t vterm_last_cell_codepoint();
// UTF-8 encoding of the last-read cell's codepoint (a single space for an
// empty/unwritten cell, codepoint 0).
String vterm_last_cell_utf8();
std::int32_t vterm_last_cell_width();
// Foreground/background RGB below are always resolved concrete colors
// (vterm_screen_convert_color_to_rgb) — valid for the RGB/indexed/default
// color types alike. is_indexed/index reflect the color's original type,
// captured before resolution, for diagnostics only.
std::int32_t vterm_last_cell_foreground_is_indexed();
std::int32_t vterm_last_cell_foreground_red();
std::int32_t vterm_last_cell_foreground_green();
std::int32_t vterm_last_cell_foreground_blue();
std::int32_t vterm_last_cell_foreground_index();
std::int32_t vterm_last_cell_background_is_indexed();
std::int32_t vterm_last_cell_background_red();
std::int32_t vterm_last_cell_background_green();
std::int32_t vterm_last_cell_background_blue();
std::int32_t vterm_last_cell_background_index();

// Count of damage callback invocations since the screen was obtained.
std::int32_t vterm_damage_count(std::int64_t screen_handle);

// vterm_set_size. The screen obtained via vterm_obtain_screen (reflow
// enabled there) adjusts automatically: growing/shrinking preserves
// in-bounds content, rejoining previously auto-wrapped rows when widened
// (empirically confirmed against this vterm build). 0 = ok, negative =
// error.
std::int32_t vterm_resize(std::int64_t vterm_handle, std::int32_t rows, std::int32_t columns);
std::int32_t vterm_size_rows(std::int64_t vterm_handle);
std::int32_t vterm_size_columns(std::int64_t vterm_handle);

// Bounded scrollback ring buffer, fed by libvterm's own sb_pushline screen
// callback (fires once per line scrolled off the top of the visible grid,
// during normal output or a row-shrinking resize alike — same mechanism).
// Default capacity is 1000 lines; vterm_screen_set_scrollback_capacity
// changes it, evicting oldest-first immediately if shrinking below the
// current line count. sb_popline (restoring scrollback into newly-grown
// rows) is intentionally not wired — out of scope for this sub-track, no
// functional loss (grown rows are simply blank instead of backfilled).
std::int32_t vterm_screen_set_scrollback_capacity(std::int64_t screen_handle, std::int32_t capacity);
std::int32_t vterm_screen_scrollback_capacity(std::int64_t screen_handle);
std::int32_t vterm_screen_scrollback_line_count(std::int64_t screen_handle);
// index counts from the oldest retained line (0 = oldest); out-of-range
// returns an empty string.
String vterm_screen_scrollback_line_text(std::int64_t screen_handle, std::int32_t index);

} // namespace terminal
} // namespace mlc
