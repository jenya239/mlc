#!/usr/bin/env bash
# IR boundary + clean code gates. See docs/agent/TRACK_CLEAN_ARCHITECTURE.md
set -u

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ALLOWLIST="$COMPILER_DIR/tests/architecture_lint_allowlist.txt"
MAX_FILE_LINES=800
FAIL=0
WARN=0

allowed() {
  local rule="$1" path="$2"
  [[ -f "$ALLOWLIST" ]] && grep -qF "${rule}:${path}" "$ALLOWLIST"
}

check_cross_imports() {
  echo "[arch lint] IR boundary imports" >&2
  local hits=0
  while IFS= read -r f; do
    rel="${f#"$COMPILER_DIR/"}"
    if allowed cross_import "$rel"; then continue; fi
    if grep -qE "from ['\"].*codegen/" "$f" 2>/dev/null; then
      echo "FAIL cross_import: $rel imports codegen/" >&2
      hits=$((hits + 1))
    fi
  done < <(find "$COMPILER_DIR/frontend" "$COMPILER_DIR/checker/check" -name '*.mlc' 2>/dev/null)

  if grep -q "transform_load_items" "$COMPILER_DIR/codegen/module.mlc" 2>/dev/null; then
    if ! allowed cross_import "codegen/module.mlc" "transform_load_items"; then
      echo "FAIL cross_import: codegen/module.mlc still imports transform_load_items" >&2
      hits=$((hits + 1))
    fi
  fi

  if grep -q "acceptable_for_codegen" "$COMPILER_DIR/checker/check/check.mlc" 2>/dev/null; then
    if ! allowed cross_import "checker/check/check.mlc" "acceptable_for_codegen"; then
      echo "FAIL cross_import: checker/check knows about codegen constraints" >&2
      hits=$((hits + 1))
    fi
  fi

  FAIL=$((FAIL + hits))
}

check_file_sizes() {
  echo "[arch lint] file size (max $MAX_FILE_LINES)" >&2
  while IFS= read -r f; do
    [[ "$f" == *"/out/"* ]] && continue
    [[ "$f" == *"/tests/"* ]] && continue
    local n lines
    n=$(wc -l < "$f")
    rel="${f#"$COMPILER_DIR/"}"
    if [[ "$n" -gt "$MAX_FILE_LINES" ]]; then
      if allowed file_size "$rel"; then
        echo "WARN file_size (allowlisted): $rel ($n lines)" >&2
        WARN=$((WARN + 1))
      else
        echo "FAIL file_size: $rel ($n lines > $MAX_FILE_LINES)" >&2
        FAIL=$((FAIL + 1))
      fi
    fi
  done < <(find "$COMPILER_DIR" -name '*.mlc' ! -path '*/out/*')
}

check_survivor_trends() {
  echo "[arch lint] survivor patterns (warn only)" >&2
  local wp kw
  wp=$(rg -c 'while index <|while i <|while mut index' "$COMPILER_DIR" --glob '*.mlc' --glob '!out/**' 2>/dev/null | awk -F: '{s+=$2} END{print s+0}')
  kw=$(rg -c 'else if word ==|else if name ==' "$COMPILER_DIR" --glob '*.mlc' --glob '!out/**' 2>/dev/null | awk -F: '{s+=$2} END{print s+0}')
  echo "INFO while-loop sites (compiler): ${wp:-0}" >&2
  echo "INFO else-if keyword/name chains: ${kw:-0}" >&2
}

check_cross_imports
check_file_sizes
check_survivor_trends

echo "[arch lint] failures=$FAIL warnings=$WARN" >&2
if [[ "$FAIL" -gt 0 ]]; then exit 1; fi
exit 0
