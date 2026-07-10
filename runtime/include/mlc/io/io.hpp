#ifndef MLC_IO_HPP
#define MLC_IO_HPP

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"

namespace mlc {
namespace io {

// Basic console output
int print(const String& s);
int println(const String& s);

// Overloads for non-String types (auto-convert via to_string)
template <typename T>
inline int print(const T& v) { return print(mlc::to_string(v)); }
template <typename T>
inline int println(const T& v) { return println(mlc::to_string(v)); }

// Error output
int eprint(const String& s);
int eprintln(const String& s);

// Overloads for non-String types
template <typename T>
inline int eprint(const T& v) { return eprint(mlc::to_string(v)); }
template <typename T>
inline int eprintln(const T& v) { return eprintln(mlc::to_string(v)); }

// I/O functions
String read_line();
String read_all();

// Command line arguments
Array<String> args();
void set_args(std::vector<String>&& new_args);

// Process control
[[noreturn]] inline int exit(int code) { std::exit(code); }

// Abort with message. Header-only (no link to io.cpp) for arith helpers.
[[noreturn]] inline void panic(const String& message) {
  std::fprintf(stderr, "PANIC: %s\n", message.c_str());
  std::exit(1);
}

} // namespace io
} // namespace mlc

#endif // MLC_IO_HPP
