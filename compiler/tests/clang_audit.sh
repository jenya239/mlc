#!/usr/bin/env bash
# Parallel clang -fsyntax-only audit of every generated .cpp in a directory.
# Usage: compiler/tests/clang_audit.sh <cpp_dir>
set -e

CPP_DIR="${1:?usage: clang_audit.sh <cpp_dir>}"
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"

ls "$CPP_DIR"/*.cpp | xargs -P "$(nproc)" -I{} \
  clang++ -std=c++20 -fsyntax-only -I "$CPP_DIR" -I "$ROOT_DIR/runtime/include" {}
