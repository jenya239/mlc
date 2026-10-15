#include "mlc/gl/clipboard_files.hpp"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#if __has_include(<X11/Xlib.h>) && __has_include(<GLFW/glfw3.h>)
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#define MLC_CLIPBOARD_FILES_X11 1
#else
#define MLC_CLIPBOARD_FILES_X11 0
#endif

namespace mlc {
namespace gl {

#if MLC_CLIPBOARD_FILES_X11

namespace {

// XChangeProperty with format 32 stores C long values, not 32-bit ints.
struct ClipboardFiles {
  Display* display;
  Window selection_window;
  Atom clipboard_atom;
  Atom utf8_string_atom;
  Atom targets_atom;
  Atom text_plain_atom;
  Atom text_plain_utf8_atom;
  Atom resource_list_atom;
  Atom gnome_copied_atom;
  Atom property_atom;
  Atom pending_target;
  Atom notify_property;
  std::string plain_text;
  std::string resource_list_text;
  std::string gnome_copied_text;
  bool owns_selection;
  bool copies_files;
  bool cutting_files;
  bool notify_seen;
};

void install_clipboard_x_error_handler();

ClipboardFiles& clipboard_files_state() {
  static ClipboardFiles state = {
    nullptr, 0, None, None, None, None, None, None, None, None, None, None,
    std::string(), std::string(), std::string(), false, false, false, false
  };
  return state;
}

bool byte_unreserved(unsigned char byte) {
  if (byte >= 'a' && byte <= 'z') {
    return true;
  }
  if (byte >= 'A' && byte <= 'Z') {
    return true;
  }
  if (byte >= '0' && byte <= '9') {
    return true;
  }
  return byte == '-' || byte == '.' || byte == '_' || byte == '~' || byte == '/';
}

char hex_digit(unsigned int value) {
  if (value < 10) {
    return static_cast<char>('0' + value);
  }
  return static_cast<char>('A' + (value - 10));
}

int hex_value(char character) {
  if (character >= '0' && character <= '9') {
    return character - '0';
  }
  if (character >= 'a' && character <= 'f') {
    return character - 'a' + 10;
  }
  if (character >= 'A' && character <= 'F') {
    return character - 'A' + 10;
  }
  return -1;
}

std::string percent_decode(const std::string& text) {
  std::string decoded;
  std::size_t index = 0;
  while (index < text.size()) {
    if (text[index] == '%' && index + 2 < text.size()) {
      const int high = hex_value(text[index + 1]);
      const int low = hex_value(text[index + 2]);
      if (high >= 0 && low >= 0) {
        decoded.push_back(static_cast<char>((high << 4) | low));
        index += 3;
        continue;
      }
    }
    decoded.push_back(text[index]);
    index += 1;
  }
  return decoded;
}

std::string path_to_resource(const std::string& path) {
  std::string resource = "file://";
  for (unsigned char byte : path) {
    if (byte_unreserved(byte)) {
      resource.push_back(static_cast<char>(byte));
    } else {
      resource.push_back('%');
      resource.push_back(hex_digit(byte >> 4));
      resource.push_back(hex_digit(byte & 0x0f));
    }
  }
  return resource;
}

std::string path_from_resource(const std::string& resource) {
  const std::string prefix = "file://";
  if (resource.size() < prefix.size() || resource.compare(0, prefix.size(), prefix) != 0) {
    return "";
  }
  std::string rest = resource.substr(prefix.size());
  const std::string local_host = "localhost";
  if (rest.compare(0, local_host.size(), local_host) == 0) {
    rest = rest.substr(local_host.size());
  }
  if (rest.empty() || rest[0] != '/') {
    return "";
  }
  return percent_decode(rest);
}

std::vector<std::string> split_lines(const std::string& text) {
  std::vector<std::string> lines;
  std::string current;
  for (char character : text) {
    if (character == '\n') {
      if (!current.empty() && current.back() == '\r') {
        current.pop_back();
      }
      if (!current.empty()) {
        lines.push_back(current);
      }
      current.clear();
    } else {
      current.push_back(character);
    }
  }
  if (!current.empty() && current.back() == '\r') {
    current.pop_back();
  }
  if (!current.empty()) {
    lines.push_back(current);
  }
  return lines;
}

std::string join_lines(const std::vector<std::string>& lines) {
  std::string joined;
  for (std::size_t index = 0; index < lines.size(); ++index) {
    if (index != 0) {
      joined.push_back('\n');
    }
    joined += lines[index];
  }
  return joined;
}

std::string file_header_text(bool cutting, const std::string& paths) {
  if (paths.empty()) {
    return "";
  }
  if (cutting) {
    return std::string("mlc-file-cut\n") + paths;
  }
  return std::string("mlc-file-copy\n") + paths;
}

std::string paths_from_resources(const std::vector<std::string>& lines, std::size_t first) {
  std::vector<std::string> paths;
  for (std::size_t index = first; index < lines.size(); ++index) {
    if (!lines[index].empty() && lines[index][0] == '#') {
      continue;
    }
    const std::string path = path_from_resource(lines[index]);
    if (!path.empty()) {
      paths.push_back(path);
    }
  }
  return join_lines(paths);
}

std::string header_from_gnome(const std::string& payload) {
  const std::vector<std::string> lines = split_lines(payload);
  if (lines.empty()) {
    return "";
  }
  bool cutting = false;
  std::size_t first = 0;
  if (lines[0] == "cut") {
    cutting = true;
    first = 1;
  } else if (lines[0] == "copy") {
    first = 1;
  }
  return file_header_text(cutting, paths_from_resources(lines, first));
}

std::string header_from_resource_list(const std::string& payload) {
  return file_header_text(false, paths_from_resources(split_lines(payload), 0));
}

void store_payloads(ClipboardFiles& state, const std::string& plain, bool cutting) {
  state.plain_text = plain;
  state.copies_files = true;
  state.cutting_files = cutting;
  const std::vector<std::string> paths = split_lines(plain);
  state.gnome_copied_text = cutting ? "cut\n" : "copy\n";
  state.resource_list_text.clear();
  for (const std::string& path : paths) {
    const std::string resource = path_to_resource(path);
    state.gnome_copied_text += resource;
    state.gnome_copied_text.push_back('\n');
    state.resource_list_text += resource;
    state.resource_list_text += "\r\n";
  }
}

bool ensure_window(ClipboardFiles& state) {
  install_clipboard_x_error_handler();
  if (state.selection_window != 0) {
    return true;
  }
  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    return false;
  }
  const Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
  if (window == 0) {
    XCloseDisplay(display);
    return false;
  }
  state.display = display;
  state.selection_window = window;
  state.clipboard_atom = XInternAtom(display, "CLIPBOARD", False);
  state.utf8_string_atom = XInternAtom(display, "UTF8_STRING", False);
  state.targets_atom = XInternAtom(display, "TARGETS", False);
  state.text_plain_atom = XInternAtom(display, "text/plain", False);
  state.text_plain_utf8_atom = XInternAtom(display, "text/plain;charset=utf-8", False);
  state.resource_list_atom = XInternAtom(display, "text/uri-list", False);
  state.gnome_copied_atom = XInternAtom(display, "x-special/gnome-copied-files", False);
  state.property_atom = XInternAtom(display, "MLC_CLIPBOARD_DATA", False);
  return true;
}

void write_bytes(Display* display, Window requestor, Atom property, Atom type, const std::string& bytes) {
  XChangeProperty(
    display,
    requestor,
    property,
    type,
    8,
    PropModeReplace,
    reinterpret_cast<const unsigned char*>(bytes.data()),
    static_cast<int>(bytes.size())
  );
}

bool write_target(ClipboardFiles& state, Window requestor, Atom property, Atom target) {
  if (target == state.targets_atom) {
    std::vector<long> atoms;
    atoms.push_back(static_cast<long>(state.targets_atom));
    atoms.push_back(static_cast<long>(state.utf8_string_atom));
    atoms.push_back(static_cast<long>(XA_STRING));
    atoms.push_back(static_cast<long>(state.text_plain_atom));
    atoms.push_back(static_cast<long>(state.text_plain_utf8_atom));
    if (state.copies_files) {
      atoms.push_back(static_cast<long>(state.resource_list_atom));
      atoms.push_back(static_cast<long>(state.gnome_copied_atom));
    }
    XChangeProperty(
      state.display,
      requestor,
      property,
      XA_ATOM,
      32,
      PropModeReplace,
      reinterpret_cast<const unsigned char*>(atoms.data()),
      static_cast<int>(atoms.size())
    );
    return true;
  }
  if (target == state.utf8_string_atom || target == state.text_plain_atom || target == state.text_plain_utf8_atom || target == XA_STRING) {
    write_bytes(state.display, requestor, property, target, state.plain_text);
    return true;
  }
  if (state.copies_files && target == state.resource_list_atom) {
    write_bytes(state.display, requestor, property, target, state.resource_list_text);
    return true;
  }
  if (state.copies_files && target == state.gnome_copied_atom) {
    write_bytes(state.display, requestor, property, target, state.gnome_copied_text);
    return true;
  }
  return false;
}

void handle_selection_request(ClipboardFiles& state, const XSelectionRequestEvent& request) {
  XEvent reply;
  std::memset(&reply, 0, sizeof(reply));
  reply.xselection.type = SelectionNotify;
  reply.xselection.display = state.display;
  reply.xselection.requestor = request.requestor;
  reply.xselection.selection = request.selection;
  reply.xselection.target = request.target;
  reply.xselection.property = None;
  reply.xselection.time = request.time;
  if (request.selection == state.clipboard_atom && state.owns_selection) {
    Atom property = request.property;
    if (property == None) {
      property = request.target;
    }
    if (write_target(state, request.requestor, property, request.target)) {
      reply.xselection.property = property;
    }
  }
  if (request.requestor == state.selection_window) {
    if (request.target == state.pending_target) {
      state.notify_seen = true;
      state.notify_property = reply.xselection.property;
    }
    return;
  }
  XSendEvent(state.display, request.requestor, False, 0, &reply);
  XFlush(state.display);
}

XErrorHandler previous_x_error_handler = nullptr;

int clipboard_x_error(Display* display, XErrorEvent* error) {
  if (error->request_code == 25 && error->error_code == BadWindow) {
    return 0;
  }
  if (previous_x_error_handler != nullptr) {
    return previous_x_error_handler(display, error);
  }
  return 0;
}

void install_clipboard_x_error_handler() {
  static bool installed = false;
  if (installed) {
    return;
  }
  previous_x_error_handler = XSetErrorHandler(clipboard_x_error);
  installed = true;
}

Bool selection_window_event(Display*, XEvent* event, XPointer argument) {
  const Window* window = reinterpret_cast<const Window*>(argument);
  if (event->xany.window == *window) {
    return True;
  }
  return False;
}

void pump_window(ClipboardFiles& state) {
  if (state.display == nullptr || state.selection_window == 0) {
    return;
  }
  XEvent event;
  while (XCheckIfEvent(state.display, &event, selection_window_event, reinterpret_cast<XPointer>(&state.selection_window))) {
    if (event.type == SelectionRequest) {
      handle_selection_request(state, event.xselectionrequest);
    } else if (event.type == SelectionClear && event.xselectionclear.selection == state.clipboard_atom) {
      state.owns_selection = false;
      state.copies_files = false;
      state.cutting_files = false;
      state.plain_text.clear();
      state.resource_list_text.clear();
      state.gnome_copied_text.clear();
    } else if (event.type == SelectionNotify && event.xselection.target == state.pending_target) {
      state.notify_seen = true;
      state.notify_property = event.xselection.property;
    }
  }
}

std::string read_property(Display* display, Window window, Atom property) {
  Atom actual_type = None;
  int actual_format = 0;
  unsigned long item_count = 0;
  unsigned long bytes_remaining = 0;
  unsigned char* data = nullptr;
  const int status = XGetWindowProperty(
    display,
    window,
    property,
    0,
    1024 * 1024,
    True,
    AnyPropertyType,
    &actual_type,
    &actual_format,
    &item_count,
    &bytes_remaining,
    &data
  );
  if (status != Success || data == nullptr || actual_format != 8) {
    if (data != nullptr) {
      XFree(data);
    }
    return "";
  }
  const std::string result(reinterpret_cast<char*>(data), item_count);
  XFree(data);
  return result;
}

std::string request_target(ClipboardFiles& state, Atom target) {
  if (!ensure_window(state)) {
    return "";
  }
  state.notify_seen = false;
  state.notify_property = None;
  state.pending_target = target;
  XConvertSelection(
    state.display,
    state.clipboard_atom,
    target,
    state.property_atom,
    state.selection_window,
    CurrentTime
  );
  XFlush(state.display);
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(250);
  while (!state.notify_seen && std::chrono::steady_clock::now() < deadline) {
    pump_window(state);
    if (!state.notify_seen && XPending(state.display) == 0) {
      timespec delay;
      delay.tv_sec = 0;
      delay.tv_nsec = 2 * 1000 * 1000;
      nanosleep(&delay, nullptr);
    }
  }
  state.pending_target = None;
  if (!state.notify_seen || state.notify_property == None) {
    return "";
  }
  return read_property(state.display, state.selection_window, state.notify_property);
}

void claim_owner(ClipboardFiles& state) {
  XSetSelectionOwner(state.display, state.clipboard_atom, state.selection_window, CurrentTime);
  XFlush(state.display);
  state.owns_selection = XGetSelectionOwner(state.display, state.clipboard_atom) == state.selection_window;
}

} // namespace

void clipboard_files_pump() {
  pump_window(clipboard_files_state());
}

void clipboard_files_release() {
  ClipboardFiles& state = clipboard_files_state();
  // Do not XSetSelectionOwner(None): a later glfwSetClipboardString on the other
  // X connection can lose the race if None is timestamped after it.
  state.owns_selection = false;
  state.copies_files = false;
  state.cutting_files = false;
  state.plain_text.clear();
  state.resource_list_text.clear();
  state.gnome_copied_text.clear();
}

void clipboard_files_shutdown() {
  ClipboardFiles& state = clipboard_files_state();
  clipboard_files_release();
  if (state.display != nullptr && state.selection_window != 0) {
    XDestroyWindow(state.display, state.selection_window);
    state.selection_window = 0;
    XCloseDisplay(state.display);
    state.display = nullptr;
  }
}

bool clipboard_files_owns() {
  return clipboard_files_state().owns_selection;
}

String clipboard_files_plain() {
  return String(clipboard_files_state().plain_text);
}

void clipboard_files_claim(String paths, int32_t cutting) {
  const std::string plain(paths.raw_data(), paths.size());
  if (plain.empty()) {
    clipboard_files_release();
    return;
  }
  ClipboardFiles& state = clipboard_files_state();
  if (!ensure_window(state)) {
    GLFWwindow* window = glfwGetCurrentContext();
    if (window != nullptr) {
      glfwSetClipboardString(window, plain.c_str());
    }
    return;
  }
  store_payloads(state, plain, cutting != 0);
  claim_owner(state);
  if (!state.owns_selection) {
    state.copies_files = false;
    GLFWwindow* window = glfwGetCurrentContext();
    if (window != nullptr) {
      glfwSetClipboardString(window, plain.c_str());
    }
  }
}

String clipboard_files_read() {
  ClipboardFiles& state = clipboard_files_state();
  pump_window(state);
  if (state.owns_selection && state.copies_files) {
    return String(file_header_text(state.cutting_files, state.plain_text));
  }
  if (!ensure_window(state)) {
    return String();
  }
  const std::string gnome_payload = request_target(state, state.gnome_copied_atom);
  if (!gnome_payload.empty()) {
    return String(header_from_gnome(gnome_payload));
  }
  return String(header_from_resource_list(request_target(state, state.resource_list_atom)));
}

String clipboard_files_served() {
  ClipboardFiles& state = clipboard_files_state();
  if (!ensure_window(state)) {
    return String();
  }
  return String(request_target(state, state.gnome_copied_atom));
}

#else

void clipboard_files_pump() {}
void clipboard_files_release() {}
void clipboard_files_shutdown() {}
bool clipboard_files_owns() { return false; }
String clipboard_files_plain() { return String(); }
void clipboard_files_claim(String, int32_t) {}
String clipboard_files_read() { return String(); }
String clipboard_files_served() { return String(); }

#endif

} // namespace gl
} // namespace mlc
