#!/usr/bin/env bash
# Shared C++ compiler selection chain: MLC_CXX override > ccache clang++ >
# clang++ > ccache g++ > g++. Source this file (do not execute); it sets the
# CXX_CMD array. g++ is kept as the final fallback for environments without
# clang, and remains explicitly reachable via MLC_CXX=g++.
if [ -n "${MLC_CXX:-}" ]; then
  CXX_CMD=($MLC_CXX)
elif command -v ccache &>/dev/null && command -v clang++ &>/dev/null; then
  CXX_CMD=(ccache clang++)
elif command -v clang++ &>/dev/null; then
  CXX_CMD=(clang++)
elif command -v ccache &>/dev/null; then
  CXX_CMD=(ccache g++)
else
  CXX_CMD=(g++)
fi
