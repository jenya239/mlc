#pragma once

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace gl {

void clipboard_files_pump();
void clipboard_files_release();
void clipboard_files_shutdown();
bool clipboard_files_owns();
String clipboard_files_plain();
void clipboard_files_claim(String paths, int32_t cutting);
String clipboard_files_read();
String clipboard_files_served();

} // namespace gl
} // namespace mlc
