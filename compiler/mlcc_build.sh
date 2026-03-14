#!/usr/bin/env bash
# Build binary from mlcc output: mlcc <source> -o out && g++ compile+link
set -e

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
RUNTIME_DIR="${RUNTIME_DIR:-$ROOT_DIR/runtime}"
RUNTIME_INCLUDE="$RUNTIME_DIR/include"
RUNTIME_CPP="$RUNTIME_DIR/src/core/string.cpp $RUNTIME_DIR/src/io/io.cpp"

SOURCE="${1:?Usage: mlcc_build.sh <source.mlc> [out_dir] [binary_name]}"
OUT_DIR="${2:-${OUT_DIR:-out}}"
BINARY_NAME="${3:-app}"
ENTRY_BASE="$(basename "$SOURCE" .mlc)"

mkdir -p "$OUT_DIR"
if [ "$MLCC" = "ruby" ]; then
  bundle exec ruby -I"$ROOT_DIR/lib" -e '
    require "mlc/common/index"
    require "mlc/common/modular_compilation/modular_compiler"
    c = MLC::Common::ModularCompilation::ModularCompiler.new(
      entry_path: ARGV[0], out_dir: ARGV[1], root_dir: File.dirname(ARGV[0])
    )
    c.compile
  ' "$SOURCE" "$OUT_DIR"
else
  "$MLCC" "$SOURCE" -o "$OUT_DIR" || exit 1
fi

# Wrap main
MAIN_CPP="$OUT_DIR/$ENTRY_BASE.cpp"
_base="$(echo "$ENTRY_BASE" | tr '[:upper:]' '[:lower:]')"
case "$_base" in main|stdin|stdout|stderr|errno) NS="mlc_$_base" ;; *) NS="$_base" ;; esac
{
  echo '#include "mlc.hpp"'
  echo '#define main mlc_user_main'
  cat "$MAIN_CPP"
  echo '#undef main'
  echo 'static void mlc_cli_set_args(int argc, char** argv) {'
  echo '  std::vector<mlc::String> arguments;'
  echo '  arguments.reserve(argc > 0 ? argc - 1 : 0);'
  echo '  for (int i = 1; i < argc; ++i) arguments.emplace_back(argv[i]);'
  echo '  mlc::io::set_args(std::move(arguments));'
  echo '}'
  echo 'int main(int argc, char** argv) {'
  echo "  mlc_cli_set_args(argc, argv);"
  echo "  return ::${NS}::mlc_user_main(argc, argv);"
  echo '}'
} > "$MAIN_CPP.tmp" && mv "$MAIN_CPP.tmp" "$MAIN_CPP"

# Compile
OBJ_DIR="$OUT_DIR/obj"
mkdir -p "$OBJ_DIR"
for cpp in "$OUT_DIR"/*.cpp; do
  [ -f "$cpp" ] || continue
  obj="$OBJ_DIR/$(basename "$cpp" .cpp).o"
  g++ -std=c++20 -I "$OUT_DIR" -I "$RUNTIME_INCLUDE" -c "$cpp" -o "$obj"
done
for rcpp in $RUNTIME_CPP; do
  [ -f "$rcpp" ] || continue
  obj="$OBJ_DIR/runtime_$(basename "$rcpp" .cpp).o"
  g++ -std=c++20 -I "$RUNTIME_INCLUDE" -c "$rcpp" -o "$obj"
done

# Link
BIN="$OUT_DIR/$BINARY_NAME"
g++ -std=c++20 -o "$BIN" "$OBJ_DIR"/*.o
echo "Built: $BIN"
