#!/usr/bin/env bash
# Compile+link smoke for misc/examples + misc/gui entries with `fn main`.
# TRACK_EXAMPLES_CI — no GUI run; STEP=2 soft-skips missing glfw3/freetype/fonts.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
SWEEP_ROOT="${EXAMPLES_SWEEP_ROOT:-$ROOT_DIR/tmp/examples_sweep}"
DRY_RUN="${EXAMPLES_SWEEP_DRY:-0}"
ONLY_GLOBS="${EXAMPLES_SWEEP_ONLY:-}"
MAX_ENTRIES="${EXAMPLES_SWEEP_MAX:-0}"
ALLOWLIST_FILE="${EXAMPLES_SWEEP_ALLOWLIST:-$ROOT_DIR/scripts/examples_compile_sweep_allowlist.txt}"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

if [[ ! -x "$MLCC" ]]; then
  echo "[examples sweep] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

has_pkg() {
  local name="$1"
  command -v pkg-config >/dev/null 2>&1 && pkg-config --exists "$name"
}

discover_entries() {
  local path
  for path in "$ROOT_DIR"/misc/examples/*.mlc "$ROOT_DIR"/misc/gui/*.mlc; do
    [[ -f "$path" ]] || continue
    if grep -qE '^fn main' "$path"; then
      printf '%s\n' "$path"
    fi
  done | sort
}

# Print skip reason to stdout, or nothing if entry should run.
soft_skip_reason() {
  local path="$1"
  local base
  base="$(basename "$path")"

  if [[ -f "$ALLOWLIST_FILE" ]] && grep -qxF "$base" "$ALLOWLIST_FILE"; then
    printf 'allowlist:%s' "$base"
    return 0
  fi

  if grep -qE 'glfw|gl_window|glad_gl|Glfw|GLAD' "$path"; then
    if ! has_pkg glfw3; then
      printf 'missing pkg-config glfw3'
      return 0
    fi
  fi

  if grep -qE 'freetype|harfbuzz|FreeType|HarfBuzz|msdf' "$path"; then
    if ! has_pkg freetype2; then
      printf 'missing pkg-config freetype2'
      return 0
    fi
  fi

  if grep -qE 'harfbuzz|HarfBuzz' "$path"; then
    if ! has_pkg harfbuzz; then
      printf 'missing pkg-config harfbuzz'
      return 0
    fi
  fi

  local font_path
  while IFS= read -r font_path; do
    [[ -n "$font_path" ]] || continue
    if [[ ! -f "$font_path" ]]; then
      printf 'missing font file %s' "$font_path"
      return 0
    fi
  done < <(grep -oE '"/usr/share/fonts[^"]+"' "$path" | tr -d '"' || true)

  return 1
}

entries=()
while IFS= read -r path; do
  [[ -n "$path" ]] || continue
  if [[ -n "$ONLY_GLOBS" ]]; then
    base="$(basename "$path")"
    match=0
    for pattern in $ONLY_GLOBS; do
      if [[ "$base" == $pattern || "$path" == $pattern ]]; then
        match=1
        break
      fi
    done
    [[ "$match" -eq 1 ]] || continue
  fi
  entries+=("$path")
  if [[ "$MAX_ENTRIES" -gt 0 && "${#entries[@]}" -ge "$MAX_ENTRIES" ]]; then
    break
  fi
done < <(discover_entries)

ok_count=0
fail_count=0
skip_count=0
fail_paths=()
skip_paths=()

echo "[examples sweep] discovered=${#entries[@]} dry=$DRY_RUN" >&2
has_gui_button=0
for path in "${entries[@]}"; do
  if [[ "$(basename "$path")" == "gui_button_demo.mlc" ]]; then
    has_gui_button=1
  fi
done
if [[ "$has_gui_button" -eq 0 && -z "$ONLY_GLOBS" && "$MAX_ENTRIES" -eq 0 ]]; then
  echo "[examples sweep] FAIL: gui_button_demo.mlc missing from discovery" >&2
  exit 1
fi

if [[ "$DRY_RUN" == "1" ]]; then
  for path in "${entries[@]}"; do
    reason=""
    if reason="$(soft_skip_reason "$path")"; then
      echo "SKIP $path ($reason)"
      skip_count=$((skip_count + 1))
    else
      echo "RUN  $path"
    fi
  done
  echo "[examples sweep] DRY run=$((${#entries[@]} - skip_count)) skip=$skip_count gui_button_demo=$has_gui_button" >&2
  exit 0
fi

mkdir -p "$SWEEP_ROOT"

for path in "${entries[@]}"; do
  reason=""
  if reason="$(soft_skip_reason "$path")"; then
    skip_count=$((skip_count + 1))
    skip_paths+=("$path")
    echo "[examples sweep] SKIP $path ($reason)" >&2
    continue
  fi

  stem="$(basename "$path" .mlc)"
  out_dir="$SWEEP_ROOT/$stem"
  bin_out="$out_dir/bin"
  log_file="$out_dir/sweep.log"
  rm -rf "$out_dir"
  mkdir -p "$out_dir"

  echo "[examples sweep] compile+link $path" >&2
  set +e
  "$MLCC" -o "$out_dir" "$path" >"$log_file" 2>&1
  compile_status=$?
  if [[ "$compile_status" -eq 0 ]]; then
    "$COMPILER_DIR/build_bin.sh" "$out_dir" "$bin_out" >>"$log_file" 2>&1
    link_status=$?
  else
    link_status=1
  fi
  set -e

  if [[ "$compile_status" -eq 0 && "$link_status" -eq 0 ]]; then
    ok_count=$((ok_count + 1))
    echo "[examples sweep] OK $path" >&2
  else
    fail_count=$((fail_count + 1))
    fail_paths+=("$path")
    echo "[examples sweep] FAIL $path (compile=$compile_status link=$link_status)" >&2
    tail -n 20 "$log_file" >&2 || true
  fi
done

echo "[examples sweep] summary ok=$ok_count fail=$fail_count skip=$skip_count" >&2
if [[ "$skip_count" -gt 0 ]]; then
  echo "[examples sweep] skipped:" >&2
  printf '  %s\n' "${skip_paths[@]}" >&2
fi
if [[ "$fail_count" -gt 0 ]]; then
  echo "[examples sweep] failures:" >&2
  printf '  %s\n' "${fail_paths[@]}" >&2
  exit 1
fi
exit 0
