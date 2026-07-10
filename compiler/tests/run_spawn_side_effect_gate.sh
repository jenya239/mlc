#!/usr/bin/env bash
# TRACK_LANG_SPAWN_DOUBLE_EXEC STEP=3 — spawn do <call> end must run side effect once.
set -euo pipefail

root="$(cd "$(dirname "$0")/../.." && pwd)"
mlcc="${MLCC:-$root/compiler/out/mlcc}"
source "$root/compiler/scripts/select_cxx.sh"
runtime_include="$root/runtime/include"
runtime_sources=("$root/runtime/src/io/io.cpp" "$root/runtime/src/core/string.cpp")
fixture="$root/compiler/tests/e2e/spawn_side_effect.mlc"
workdir="$(mktemp -d "${TMPDIR:-/tmp}/spawn_side_effect.XXXXXX")"
trap 'rm -rf "$workdir"' EXIT

if [[ ! -x "$mlcc" ]]; then
  echo "[spawn side-effect] FAIL: mlcc not executable at $mlcc" >&2
  exit 1
fi

echo "[spawn side-effect] mlcc=$mlcc" >&2
"$mlcc" -o "$workdir/out" "$fixture"

generated="$(find "$workdir/out" -name '*.cpp' | head -1)"
if [[ -z "$generated" ]]; then
  echo "[spawn side-effect] FAIL: no generated .cpp" >&2
  exit 1
fi

# Bare statement `side();` inside spawn_task = double-emit (return side() is OK).
if awk '
  /spawn_task/ { in_spawn = 1 }
  in_spawn && /^[[:space:]]*side\(\);[[:space:]]*$/ { found = 1 }
  in_spawn && /\}\(\);/ { in_spawn = 0 }
  END { exit found ? 0 : 1 }
' "$generated"; then
  echo "[spawn side-effect] FAIL: codegen still has bare side(); in spawn_task (double emit)" >&2
  grep -n -A8 'spawn_task' "$generated" >&2 || true
  exit 1
fi

if ! grep -q 'return side();' "$generated"; then
  echo "[spawn side-effect] FAIL: expected return side(); in generated C++" >&2
  exit 1
fi

"${CXX_CMD[@]}" -std=c++20 -pthread \
  -I "$workdir/out" -I "$runtime_include" \
  "$workdir/out"/*.cpp "${runtime_sources[@]}" \
  -o "$workdir/prog"

output="$("$workdir/prog" 2>&1 || true)"
count="$(printf '%s\n' "$output" | grep -c 'SIDE_ONCE' || true)"
if [[ "$count" -ne 1 ]]; then
  echo "[spawn side-effect] FAIL: expected SIDE_ONCE once, got count=$count" >&2
  printf '%s\n' "$output" >&2
  exit 1
fi

echo "[spawn side-effect] PASS (codegen + runtime count=1)"
