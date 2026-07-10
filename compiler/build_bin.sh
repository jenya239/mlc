#!/usr/bin/env bash
# Fast parallel build of a directory of .cpp files into a binary.
# Usage: build_bin.sh <cpp_dir> <binary_out> [extra_include_dir]
# Respects MLC_CXX (default: ccache clang++ > ccache g++ > g++) and MLC_JOBS.
# Object files persist in <cpp_dir>/obj/<tag>/ for incremental rebuild (ccache-friendly).
# MLCC_OBJ_CLEAN=1 wipes obj/<tag>/ before compile.
# MLCC_DEV=1 → compile with -O0 -g (obj tag: dev).
# MLCC_OPT=<n> → compile with -On (default 2; obj tag: On).
# MLCC_PCH=0 → disable precompiled mlcc_precompiled.hpp (default: on).
set -e

CPP_DIR="$1"
BIN_OUT="$2"
EXTRA_INC="${3:-}"

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC=(
  "$ROOT_DIR/runtime/src/io/io.cpp"
  "$ROOT_DIR/runtime/src/core/string.cpp"
  "$ROOT_DIR/runtime/src/core/profile.cpp"
)
# Optional text/GL shims (TRACK_TEXT_RENDERING): FreeType / HarfBuzz / EGL loader.
TEXT_CFLAGS=()
TEXT_LIBS=()
if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists freetype2; then
  RT_SRC+=("$ROOT_DIR/runtime/src/text/freetype_shim.cpp")
  RT_SRC+=("$ROOT_DIR/runtime/src/text/msdf_shim.cpp")
  # shellcheck disable=SC2207
  TEXT_CFLAGS+=($(pkg-config --cflags freetype2))
  # shellcheck disable=SC2207
  TEXT_LIBS+=($(pkg-config --libs freetype2))
  if pkg-config --exists harfbuzz; then
    RT_SRC+=("$ROOT_DIR/runtime/src/text/harfbuzz_shim.cpp")
    # shellcheck disable=SC2207
    TEXT_CFLAGS+=($(pkg-config --cflags harfbuzz))
    # shellcheck disable=SC2207
    TEXT_LIBS+=($(pkg-config --libs harfbuzz))
  fi
fi
if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists egl && pkg-config --exists glesv2; then
  RT_SRC+=("$ROOT_DIR/runtime/src/gl/loader_shim.cpp")
  # shellcheck disable=SC2207
  TEXT_CFLAGS+=($(pkg-config --cflags egl glesv2))
  # shellcheck disable=SC2207
  TEXT_LIBS+=($(pkg-config --libs egl glesv2))
  if pkg-config --exists freetype2; then
    RT_SRC+=("$ROOT_DIR/runtime/src/gl/msdf_renderer_shim.cpp")
  fi
  if pkg-config --exists freetype2 && pkg-config --exists harfbuzz; then
    RT_SRC+=("$ROOT_DIR/runtime/src/gl/text_renderer_shim.cpp")
  fi
fi

JOBS="${MLC_JOBS:-$(nproc 2>/dev/null || echo 4)}"
ENTRY_BASENAME="${MLCC_ENTRY_BASENAME:-main}"

source "$(dirname "${BASH_SOURCE[0]}")/scripts/select_cxx.sh"

# clang's -include-pch defeats ccache direct mode unless this sloppiness is set
# (ccache refuses to hash-match PCH-based invocations otherwise: "You have to
# specify time_macros sloppiness when using precompiled headers to get direct
# hits"). Safe: only relaxes __TIME__/__DATE__ + PCH-defines sensitivity, which
# mlcc-generated sources never rely on.
if [[ "${CXX_CMD[*]}" == ccache* ]]; then
  export CCACHE_SLOPPINESS="${CCACHE_SLOPPINESS:-pch_defines,time_macros}"
fi

if [ "${MLCC_DEV:-0}" = "1" ]; then
  CXX_OPTIMIZE_FLAGS=(-O0 -g)
  OBJECT_STORE_TAG=dev
else
  OPTIMIZATION_LEVEL="${MLCC_OPT:-2}"
  CXX_OPTIMIZE_FLAGS=(-O"$OPTIMIZATION_LEVEL")
  OBJECT_STORE_TAG="O${OPTIMIZATION_LEVEL}"
fi

SANITIZE_FLAGS=()
case "${MLC_SANITIZE:-}" in
  "") ;;
  address)
    SANITIZE_FLAGS=(-fsanitize=address -g -fno-omit-frame-pointer)
    OBJECT_STORE_TAG="${OBJECT_STORE_TAG}_asan"
    ;;
  undefined)
    SANITIZE_FLAGS=(-fsanitize=undefined -g)
    OBJECT_STORE_TAG="${OBJECT_STORE_TAG}_ubsan"
    ;;
  thread)
    SANITIZE_FLAGS=(-fsanitize=thread -g)
    OBJECT_STORE_TAG="${OBJECT_STORE_TAG}_tsan"
    ;;
  *)
    echo "build_bin.sh: unknown MLC_SANITIZE=${MLC_SANITIZE} (want address|undefined|thread)" >&2
    exit 1
    ;;
esac

OBJ_DIR="$CPP_DIR/obj/$OBJECT_STORE_TAG"
if [ "${MLCC_OBJ_CLEAN:-0}" = "1" ]; then
  rm -rf "$OBJ_DIR"
fi
mkdir -p "$OBJ_DIR"

object_path_for_source() {
  local source_path="$1"
  local base_name
  base_name="$(basename "$source_path" .cpp)"
  if [[ "$source_path" == "$CPP_DIR"/* ]]; then
    printf '%s/%s.o' "$OBJ_DIR" "$base_name"
  else
    printf '%s/_rt_%s.o' "$OBJ_DIR" "$base_name"
  fi
}

# Dependency-aware staleness: consult the compiler-generated depfile (-MMD -MF)
# when present, so edits to a shared header (not just the .cpp itself) force a
# rebuild. Falls back to plain .cpp-vs-.o mtime when no depfile exists yet
# (first build for this object).
dep_file_for_object() {
  printf '%s.d' "${1%.o}"
}

parse_dep_file_paths() {
  sed -e 's/^[^:]*://' -e 's/\\$//' "$1" | tr -s ' \t' '\n' | sed '/^$/d'
}

object_is_stale() {
  local source_path="$1"
  local object_path="$2"
  [ -f "$object_path" ] || return 0
  local dep_file
  dep_file="$(dep_file_for_object "$object_path")"
  if [ -f "$dep_file" ]; then
    local dependency_path
    while IFS= read -r dependency_path; do
      [ -n "$dependency_path" ] || continue
      [ -f "$dependency_path" ] || return 0
      [ "$dependency_path" -nt "$object_path" ] && return 0
    done < <(parse_dep_file_paths "$dep_file")
    return 1
  fi
  [ "$source_path" -nt "$object_path" ]
}

INC_FLAGS=(-I "$CPP_DIR" -I "$RT_INC")
[ -n "$EXTRA_INC" ] && INC_FLAGS+=(-I "$EXTRA_INC")
INC_FLAGS+=("${TEXT_CFLAGS[@]}")

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
PRECOMPILED_SOURCE="$COMPILER_DIR/mlcc_precompiled.hpp"

cxx_is_clang() {
  if [[ "${CXX_CMD[*]}" == *clang* ]]; then
    return 0
  fi
  "${CXX_CMD[@]}" --version 2>&1 | grep -qi clang
}

PCH_FLAGS=()
if [ "${MLCC_PCH:-1}" != "0" ] && [ -f "$PRECOMPILED_SOURCE" ]; then
  PRECOMPILED_HEADER="$OBJ_DIR/mlcc_precompiled.hpp"
  if [ ! -f "$PRECOMPILED_HEADER" ] || [ "$PRECOMPILED_SOURCE" -nt "$PRECOMPILED_HEADER" ]; then
    cp "$PRECOMPILED_SOURCE" "$PRECOMPILED_HEADER"
  fi
  if cxx_is_clang; then
    PRECOMPILED_OUTPUT="$OBJ_DIR/mlcc_precompiled.pch"
  else
    PRECOMPILED_OUTPUT="$PRECOMPILED_HEADER.gch"
  fi
  if [ ! -f "$PRECOMPILED_OUTPUT" ] \
    || [ "$PRECOMPILED_HEADER" -nt "$PRECOMPILED_OUTPUT" ] \
    || [ "$RT_INC/mlc.hpp" -nt "$PRECOMPILED_OUTPUT" ]; then
    "${CXX_CMD[@]}" -std=c++20 "${CXX_OPTIMIZE_FLAGS[@]}" "${SANITIZE_FLAGS[@]}" "${INC_FLAGS[@]}" \
      -x c++-header "$PRECOMPILED_HEADER" -o "$PRECOMPILED_OUTPUT"
  fi
  if cxx_is_clang; then
    PCH_FLAGS=(-include-pch "$PRECOMPILED_OUTPUT")
  else
    PCH_FLAGS=(-include "$PRECOMPILED_HEADER")
  fi
fi

mlcc_only_skip_source() {
  local base_name="$1"
  case "$base_name" in
    tests_main.cpp|suite_registry.cpp|codegen_test_helpers.cpp|golden_harness.cpp)
      return 0
      ;;
    test_*.cpp)
      return 0
      ;;
  esac
  return 1
}

GENERATED_CPP=()
shopt -s nullglob
for candidate_cpp in "$CPP_DIR"/*.cpp; do
  case "$(basename "$candidate_cpp")" in
    tests_main.cpp)
      [ "$ENTRY_BASENAME" = "tests_main" ] || continue
      ;;
  esac
  if [ "$ENTRY_BASENAME" = "main" ] && mlcc_only_skip_source "$(basename "$candidate_cpp")"; then
    continue
  fi
  GENERATED_CPP+=("$candidate_cpp")
done
shopt -u nullglob
if [ ${#GENERATED_CPP[@]} -gt 0 ]; then
  IFS=$'\n' GENERATED_CPP=($(printf '%s\n' "${GENERATED_CPP[@]}" | LC_ALL=C sort))
  unset IFS
fi
ALL_CPP=("${GENERATED_CPP[@]}" "${RT_SRC[@]}")
OBJS=()
PIDS=()
ERRORS=()

for cpp in "${ALL_CPP[@]}"; do
  [ -f "$cpp" ] || continue
  object_path="$(object_path_for_source "$cpp")"
  OBJS+=("$object_path")
  if ! object_is_stale "$cpp" "$object_path"; then
    continue
  fi
  dep_file="$(dep_file_for_object "$object_path")"
  "${CXX_CMD[@]}" -std=c++20 "${CXX_OPTIMIZE_FLAGS[@]}" "${SANITIZE_FLAGS[@]}" "${PCH_FLAGS[@]}" "${INC_FLAGS[@]}" -MMD -MF "$dep_file" -c "$cpp" -o "$object_path" &
  PIDS+=($!)
  if [ ${#PIDS[@]} -ge "$JOBS" ]; then
    for pid in "${PIDS[@]}"; do
      wait "$pid" || ERRORS+=("$pid")
    done
    PIDS=()
  fi
done

for pid in "${PIDS[@]}"; do
  wait "$pid" || ERRORS+=("$pid")
done

if [ ${#ERRORS[@]} -gt 0 ]; then
  echo "build_bin.sh: compile errors" >&2
  exit 1
fi

LINK_FLAGS=()
if command -v mold &>/dev/null || command -v ld.mold &>/dev/null; then
  LINK_FLAGS+=(-fuse-ld=mold)
elif command -v ld.lld &>/dev/null; then
  LINK_FLAGS+=(-fuse-ld=lld)
elif command -v ld.gold &>/dev/null; then
  LINK_FLAGS+=(-fuse-ld=gold)
fi

EXTERN_LINK_LIBS=()
if [ -f "$CPP_DIR/mlc_link_libs.txt" ]; then
  while IFS= read -r library_name || [ -n "$library_name" ]; do
    [ -n "$library_name" ] || continue
    EXTERN_LINK_LIBS+=("-l${library_name}")
  done < "$CPP_DIR/mlc_link_libs.txt"
fi
EXTERN_LINK_LIBS+=("${TEXT_LIBS[@]}")

"${CXX_CMD[@]}" -std=c++20 "${LINK_FLAGS[@]}" "${SANITIZE_FLAGS[@]}" -o "$BIN_OUT" "${OBJS[@]}" "${EXTERN_LINK_LIBS[@]}"
