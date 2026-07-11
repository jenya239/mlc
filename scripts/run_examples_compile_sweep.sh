#!/usr/bin/env bash
# Compile+link smoke for misc/examples + misc/gui entries with `fn main`.
# TRACK_EXAMPLES_CI STEP=1 — no GUI run (display/pkg-config soft-skip → Step 2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
SWEEP_ROOT="${EXAMPLES_SWEEP_ROOT:-$ROOT_DIR/tmp/examples_sweep}"
DRY_RUN="${EXAMPLES_SWEEP_DRY:-0}"
ONLY_GLOBS="${EXAMPLES_SWEEP_ONLY:-}"
MAX_ENTRIES="${EXAMPLES_SWEEP_MAX:-0}"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

if [[ ! -x "$MLCC" ]]; then
  echo "[examples sweep] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

discover_entries() {
  local path
  for path in "$ROOT_DIR"/misc/examples/*.mlc "$ROOT_DIR"/misc/gui/*.mlc; do
    [[ -f "$path" ]] || continue
    if grep -qE '^fn main' "$path"; then
      printf '%s\n' "$path"
    fi
  done | sort
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
  printf '%s\n' "${entries[@]}"
  echo "[examples sweep] DRY ok=${#entries[@]} fail=0 skip=0 gui_button_demo=$has_gui_button" >&2
  exit 0
fi

mkdir -p "$SWEEP_ROOT"

for path in "${entries[@]}"; do
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
if [[ "$fail_count" -gt 0 ]]; then
  echo "[examples sweep] failures:" >&2
  printf '  %s\n' "${fail_paths[@]}" >&2
  exit 1
fi
exit 0
