#!/usr/bin/env bash
set -euo pipefail

MLCC="${1:-$(cd "$(dirname "$0")/.." && pwd)/out/mlcc}"

if [[ ! -x "$MLCC" ]]; then
  echo "[lsp smoke] FAIL: mlcc not found: $MLCC" >&2
  exit 1
fi

INIT_BODY='{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}'
INIT_LEN=${#INIT_BODY}
SHUTDOWN_BODY='{"jsonrpc":"2.0","id":2,"method":"shutdown"}'
SHUTDOWN_LEN=${#SHUTDOWN_BODY}
EXIT_BODY='{"jsonrpc":"2.0","method":"exit"}'
EXIT_LEN=${#EXIT_BODY}

OUTPUT=$(printf 'Content-Length: %d\r\n\r\n%s\r\nContent-Length: %d\r\n\r\n%s\r\nContent-Length: %d\r\n\r\n%s\r\n' \
  "$INIT_LEN" "$INIT_BODY" \
  "$SHUTDOWN_LEN" "$SHUTDOWN_BODY" \
  "$EXIT_LEN" "$EXIT_BODY" | timeout 10 "$MLCC" lsp 2>/dev/null || true)

if ! echo "$OUTPUT" | grep -q '"serverInfo"'; then
  echo "[lsp smoke] FAIL: initialize response missing serverInfo" >&2
  echo "$OUTPUT" | head -5 >&2
  exit 1
fi

echo "[lsp smoke] ok"
