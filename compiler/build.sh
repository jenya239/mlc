#!/usr/bin/env bash
set -e

# Progress during modular compile/link (semantic → codegen → g++): stderr lines prefixed with "[mlc build]".
# Example: MLCC_BUILD_VERBOSE=1 compiler/build.sh
#
# Default: mlcc-only (skip when fresh, rebuild via mlcc + build_bin.sh when stale).
# Cold start (no mlcc): MLCC_FORCE_RUBY=1 compiler/build.sh
# Bootstrap: MLCC_BOOTSTRAP=1 compiler/build.sh
# Incremental codegen (default on): MLCC_INCREMENTAL=0 forces full wipe + mlcc emit.
# When module stamp matches: skip mlcc codegen; build_bin.sh only.

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

MAIN="$COMPILER_DIR/main.mlc"
BIN_OUT="$OUT_DIR/mlcc"

echo "[mlcc build] modular compile → ${BIN_OUT} (MAIN=${MAIN})" >&2

_verbose_lowercase="$(printf '%s' "${MLCC_BUILD_VERBOSE:-}" | tr '[:upper:]' '[:lower:]')"
if [[ -n "${MLCC_BUILD_VERBOSE:-}" ]] && [[ "${_verbose_lowercase}" != "0" ]] && [[ "${_verbose_lowercase}" != "false" ]] && [[ "${_verbose_lowercase}" != "no" ]]; then
  echo "compiler/build.sh: MLCC_BUILD_VERBOSE=${MLCC_BUILD_VERBOSE} (progress on stderr)" >&2
fi
unset _verbose_lowercase

mlcc_binary_is_fresh() {
  [ -x "$BIN_OUT" ] || return 1
  ! find "$COMPILER_DIR" -name '*.mlc' -newer "$BIN_OUT" -print -quit | grep -q .
}

MODULE_STAMP="$OUT_DIR/.mlcc_module_stamp"

incremental_enabled() {
  [ "${MLCC_INCREMENTAL:-1}" != "0" ]
}

compute_module_fingerprint() {
  find "$COMPILER_DIR" -name '*.mlc' -printf '%P\n' 2>/dev/null | LC_ALL=C sort | sha256sum | awk '{print $1}'
}

module_stamp_matches() {
  [ -f "$MODULE_STAMP" ] || return 1
  [ "$(head -1 "$MODULE_STAMP")" = "$(compute_module_fingerprint)" ]
}

write_module_stamp() {
  {
    compute_module_fingerprint
    find "$OUT_DIR" -maxdepth 1 -name '*.cpp' -printf '%f\n' 2>/dev/null | LC_ALL=C sort
  } > "$MODULE_STAMP"
}

clear_generated_sources() {
  find "$OUT_DIR" -maxdepth 1 \( -name '*.cpp' -o -name '*.hpp' \) -delete
}

prune_orphan_generated_sources() {
  local stamp_file="$1"
  local expected_list
  expected_list="$(mktemp)"
  tail -n +2 "$stamp_file" > "$expected_list"
  local base_name
  while IFS= read -r base_name; do
    [ -n "$base_name" ] || continue
    if ! grep -qxF "$base_name" "$expected_list"; then
      rm -f "$OUT_DIR/$base_name" "$OUT_DIR/${base_name%.cpp}.hpp"
    fi
  done < <(find "$OUT_DIR" -maxdepth 1 -name '*.cpp' -printf '%f\n' 2>/dev/null)
  rm -f "$expected_list"
}

should_skip_codegen() {
  incremental_enabled && module_stamp_matches
}

prepare_codegen_output() {
  if should_skip_codegen; then
    echo "[mlcc build] incremental codegen (keep existing sources)" >&2
    return
  fi
  clear_generated_sources
}

build_via_ruby() {
  echo "[mlcc build] Ruby ModularCompiler" >&2
  bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "mlc/common/index"
require "mlc/common/modular_compilation/modular_compiler"

compiler = MLC::Common::ModularCompilation::ModularCompiler.new(
  entry_path: ARGV[0],
  out_dir: ARGV[1],
  root_dir: File.dirname(ARGV[0]),
  binary_name: "mlcc"
)
result = compiler.build
puts "Built: #{result[:binary]}"
' "$MAIN" "$OUT_DIR"
}

build_via_mlcc() {
  echo "[mlcc build] mlcc codegen + build_bin.sh" >&2
  if should_skip_codegen; then
    echo "[mlcc build] skip mlcc codegen (module stamp)" >&2
  else
    prepare_codegen_output
    "$BIN_OUT" "$MAIN" -o "$OUT_DIR"
    write_module_stamp
    if incremental_enabled; then
      prune_orphan_generated_sources "$MODULE_STAMP"
    fi
  fi
  "$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
}

if mlcc_binary_is_fresh; then
  echo "[mlcc build] ${BIN_OUT} up to date (skip)" >&2
elif [ "${MLCC_FORCE_RUBY:-0}" = "1" ]; then
  build_via_ruby
elif [ ! -x "$BIN_OUT" ]; then
  echo "[mlcc build] no ${BIN_OUT}; cold start requires MLCC_FORCE_RUBY=1" >&2
  exit 1
elif [ "${MLCC_BOOTSTRAP:-0}" = "1" ]; then
  echo "[mlcc build] ${BIN_OUT} stale; skip main rebuild (bootstrap only)" >&2
else
  build_via_mlcc
fi

echo "[mlcc build] binary ready: ${BIN_OUT}" >&2

if [ "${MLCC_BOOTSTRAP:-0}" = "1" ]; then
  BS_DIR="$OUT_DIR/bootstrap"
  BS_BIN="$BS_DIR/mlcc_bootstrap"
  mkdir -p "$BS_DIR"
  echo "[mlcc build] bootstrap: mlcc codegen → ${BS_DIR}" >&2
  if ! "$BIN_OUT" "$MAIN" -o "$BS_DIR"; then
    echo "[mlcc build] bootstrap: mlcc codegen failed" >&2
    exit 1
  fi
  if ! ls "$BS_DIR"/*.cpp 1>/dev/null 2>&1; then
    echo "[mlcc build] bootstrap: no .cpp in ${BS_DIR}" >&2
    exit 1
  fi
  if ! "$COMPILER_DIR/build_bin.sh" "$BS_DIR" "$BS_BIN"; then
    echo "[mlcc build] bootstrap: build_bin.sh failed" >&2
    exit 1
  fi
  echo "[mlcc build] bootstrap: built ${BS_BIN}" >&2
fi
