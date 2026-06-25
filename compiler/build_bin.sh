#!/usr/bin/env bash
# Fast parallel build of a directory of .cpp files into a binary.
# Usage: build_bin.sh <cpp_dir> <binary_out> [extra_include_dir]
# Respects MLC_CXX (default: ccache clang++ > ccache g++ > g++) and MLC_JOBS.
# Object files persist in <cpp_dir>/obj/<tag>/ for incremental rebuild (ccache-friendly).
# MLCC_OBJ_CLEAN=1 wipes obj/<tag>/ before compile.
# MLCC_DEV=1 → compile with -O0 -g (obj tag: dev).
# MLCC_OPT=<n> → compile with -On (default 2; obj tag: On).
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

JOBS="${MLC_JOBS:-$(nproc 2>/dev/null || echo 4)}"

if [ -n "$MLC_CXX" ]; then
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

if [ "${MLCC_DEV:-0}" = "1" ]; then
  CXX_OPTIMIZE_FLAGS=(-O0 -g)
  OBJECT_STORE_TAG=dev
else
  OPTIMIZATION_LEVEL="${MLCC_OPT:-2}"
  CXX_OPTIMIZE_FLAGS=(-O"$OPTIMIZATION_LEVEL")
  OBJECT_STORE_TAG="O${OPTIMIZATION_LEVEL}"
fi

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

INC_FLAGS=(-I "$CPP_DIR" -I "$RT_INC")
[ -n "$EXTRA_INC" ] && INC_FLAGS+=(-I "$EXTRA_INC")

GENERATED_CPP=()
shopt -s nullglob
for candidate_cpp in "$CPP_DIR"/*.cpp; do
  case "$(basename "$candidate_cpp")" in
    tests_main.cpp) continue ;;
  esac
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
  if [ -f "$object_path" ] && [ ! "$cpp" -nt "$object_path" ]; then
    continue
  fi
  "${CXX_CMD[@]}" -std=c++20 "${CXX_OPTIMIZE_FLAGS[@]}" "${INC_FLAGS[@]}" -c "$cpp" -o "$object_path" &
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
command -v ld.lld &>/dev/null && LINK_FLAGS+=(-fuse-ld=lld)

"${CXX_CMD[@]}" -std=c++20 "${LINK_FLAGS[@]}" -o "$BIN_OUT" "${OBJS[@]}"
