#!/usr/bin/env bash
# Capture stdout baselines via Ruby bootstrap (bin/mlc).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROGRAMS="$ROOT/specs/regression/programs"
EXPECTED="$ROOT/specs/regression/expected"
MLC="$ROOT/bin/mlc"
RUN_MLC=(bundle exec ruby "$MLC")

mkdir -p "$EXPECTED"

while IFS=$'\t' read -r name _expected_path; do
  [[ -z "$name" || "$name" == \#* ]] && continue
  source_file="$PROGRAMS/${name}.mlc"
  output_file="$EXPECTED/${name}.txt"
  if [[ ! -f "$source_file" ]]; then
    echo "missing program: $source_file" >&2
    exit 1
  fi
  tmpdir="$(mktemp -d)"
  if ! "${RUN_MLC[@]}" -o "$tmpdir" "$source_file" >"$output_file" 2>"$tmpdir/err.log"; then
    echo "mlc failed for $name:" >&2
    cat "$tmpdir/err.log" >&2
    rm -rf "$tmpdir"
    exit 1
  fi
  rm -rf "$tmpdir"
  echo "captured $name"
done < "$ROOT/specs/regression/manifest.tsv"
