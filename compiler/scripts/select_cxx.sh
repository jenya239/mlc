#!/usr/bin/env bash
# Shared C++ compiler selection: MLC_CXX override > ccache clang++ > clang++.
# Source this file (do not execute); it sets the CXX_CMD array.
if [ -n "${MLC_CXX:-}" ]; then
  CXX_CMD=($MLC_CXX)
elif command -v ccache &>/dev/null && command -v clang++ &>/dev/null; then
  CXX_CMD=(ccache clang++)
elif command -v clang++ &>/dev/null; then
  CXX_CMD=(clang++)
else
  echo "select_cxx: clang++ not found" >&2
  return 1 2>/dev/null || exit 1
fi
