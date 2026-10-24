#!/usr/bin/env bash
# PLAN §112 / Cairo 01-label: visible three-Label gallery via app.mlc.
# Default: show the GLFW window (playground contract). Headless:
#   MLC_GLFW_VISIBLE=0 MLC_TEXTUI_BENCH=1 bash scripts/run_textui_label_gallery.sh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXTUI_GALLERY_ENTRY:-$ROOT_DIR/misc/textui/app.mlc}"
OUT_DIR="${TEXTUI_GALLERY_OUT:-$ROOT_DIR/tmp/textui_label_gallery}"
BIN_OUT="$OUT_DIR/bin"
FONT="${TEXTUI_GALLERY_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"

prepare_extracted_text_packages() {
  local extract="$ROOT_DIR/tmp/debs/extract"
  local package_config="$ROOT_DIR/tmp/debs/pkgconfig"
  local header_root="$extract/usr/include"
  local library_directory="$extract/usr/lib/x86_64-linux-gnu"
  if pkg-config --exists harfbuzz && [ -f /usr/include/cairo/cairo.h ]; then
    return 0
  fi
  if [ ! -f "$header_root/harfbuzz/hb.h" ] || [ ! -f "$header_root/cairo/cairo.h" ]; then
    mkdir -p "$ROOT_DIR/tmp/debs" "$extract"
    (
      cd "$ROOT_DIR/tmp/debs"
      apt-get download libharfbuzz-dev libcairo2-dev libpixman-1-dev
    )
    local archive
    for archive in "$ROOT_DIR"/tmp/debs/*.deb; do
      if [ -f "$archive" ]; then
        dpkg-deb -x "$archive" "$extract"
      fi
    done
  fi
  if [ ! -f "$header_root/harfbuzz/hb.h" ]; then
    return 0
  fi
  mkdir -p "$library_directory" "$package_config"
  if [ -e /usr/lib/x86_64-linux-gnu/libharfbuzz.so.0 ]; then
    ln -sfn /usr/lib/x86_64-linux-gnu/libharfbuzz.so.0 "$library_directory/libharfbuzz.so.0"
  fi
  if [ -e /usr/lib/x86_64-linux-gnu/libcairo.so.2 ]; then
    ln -sfn /usr/lib/x86_64-linux-gnu/libcairo.so.2 "$library_directory/libcairo.so.2"
  fi
  cat > "$package_config/harfbuzz.pc" <<EOF
Name: harfbuzz
Description: HarfBuzz text shaping library
Version: 12.3.2
Libs: -L${library_directory} -lharfbuzz
Cflags: -I${header_root}/harfbuzz
EOF
  if ! pkg-config --exists harfbuzz; then
    if [ -n "${PKG_CONFIG_PATH:-}" ]; then
      export PKG_CONFIG_PATH="${package_config}:${PKG_CONFIG_PATH}"
    else
      export PKG_CONFIG_PATH="${package_config}"
    fi
  fi
  if [ ! -f /usr/include/cairo/cairo.h ] && [ -f "$header_root/cairo/cairo.h" ]; then
    if [ -n "${CPLUS_INCLUDE_PATH:-}" ]; then
      export CPLUS_INCLUDE_PATH="${header_root}:${CPLUS_INCLUDE_PATH}"
    else
      export CPLUS_INCLUDE_PATH="${header_root}"
    fi
  fi
}

prepare_pulse_client() {
  if pkg-config --exists libpulse-simple; then
    return 0
  fi
  if [ ! -e /usr/lib/x86_64-linux-gnu/libpulse-simple.so.0 ]; then
    return 0
  fi
  local extract="$ROOT_DIR/tmp/debs/extract"
  local package_config="$ROOT_DIR/tmp/debs/pkgconfig"
  local header_root="$extract/usr/include"
  local library_directory="$extract/usr/lib/x86_64-linux-gnu"
  if [ ! -f "$header_root/pulse/simple.h" ]; then
    mkdir -p "$ROOT_DIR/tmp/debs" "$extract"
    (
      cd "$ROOT_DIR/tmp/debs"
      apt-get download libpulse-dev
    )
    local archive
    for archive in "$ROOT_DIR"/tmp/debs/libpulse-dev_*.deb; do
      if [ -f "$archive" ]; then
        dpkg-deb -x "$archive" "$extract"
      fi
    done
  fi
  if [ ! -f "$header_root/pulse/simple.h" ]; then
    return 0
  fi
  mkdir -p "$library_directory" "$package_config"
  ln -sfn /usr/lib/x86_64-linux-gnu/libpulse-simple.so.0 "$library_directory/libpulse-simple.so"
  ln -sfn /usr/lib/x86_64-linux-gnu/libpulse.so.0 "$library_directory/libpulse.so"
  cat > "$package_config/libpulse.pc" <<EOF
prefix=${extract}/usr
exec_prefix=\${prefix}
libdir=${library_directory}
includedir=${header_root}

Name: libpulse
Description: PulseAudio Client Interface
Version: 17.0
Libs: -L\${libdir} -lpulse -pthread
Cflags: -I\${includedir} -D_REENTRANT
EOF
  cat > "$package_config/libpulse-simple.pc" <<EOF
prefix=${extract}/usr
exec_prefix=\${prefix}
libdir=${library_directory}
includedir=${header_root}

Name: libpulse-simple
Description: PulseAudio Simplified Synchronous Client Interface
Version: 17.0
Libs: -L\${libdir} -lpulse-simple -pthread
Cflags: -I\${includedir} -D_REENTRANT
Requires: libpulse
EOF
  if [ -n "${PKG_CONFIG_PATH:-}" ]; then
    export PKG_CONFIG_PATH="${package_config}:${PKG_CONFIG_PATH}"
  else
    export PKG_CONFIG_PATH="${package_config}"
  fi
}

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[textui gallery] SKIP: glfw3 not found" >&2
  exit 0
fi
prepare_extracted_text_packages
prepare_pulse_client
if ! pkg-config --exists freetype2 || ! pkg-config --exists harfbuzz; then
  echo "[textui gallery] SKIP: freetype2/harfbuzz not found" >&2
  if [ "${MLC_TEXTUI_REQUIRE:-0}" = "1" ]; then
    exit 1
  fi
  exit 0
fi
if [ ! -f "$FONT" ]; then
  echo "[textui gallery] SKIP: font missing $FONT" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[textui gallery] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-1}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
if [ "${MLC_TEXTUI_COMPILE_ONLY:-0}" = "1" ]; then
  echo "[textui gallery] compiled $BIN_OUT" >&2
  exit 0
fi
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -eq 75 ]; then
  exec ruby "$ROOT_DIR/scripts/launch_textui_gallery.rb"
fi
if [ "$status" -ne 0 ]; then
  echo "[textui gallery] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui gallery] ok MLC_GLFW_VISIBLE=$MLC_GLFW_VISIBLE" >&2
