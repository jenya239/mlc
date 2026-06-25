#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
TURNLOG="$ROOT/docs/agent/TURNLOG.jsonl"
CR_DB="${CR_DB:-$HOME/.cursor/cr.db}"

echo "[turn_audit_gate] TURNLOG"
if [ ! -s "$TURNLOG" ]; then
  echo "FAIL: missing or empty $TURNLOG" >&2
  exit 1
fi
tail -1 "$TURNLOG" | python3 -c "import json,sys; json.load(sys.stdin)" >/dev/null
lines="$(wc -l < "$TURNLOG")"
echo "OK: TURNLOG lines=$lines"

echo "[turn_audit_gate] SESSION done lint"
python3 scripts/session_turn_lint.py
echo "OK: session_turn_lint"

if [ -f "$CR_DB" ]; then
  echo "[turn_audit_gate] cr.db turn_audit_events"
  count="$(sqlite3 "$CR_DB" "SELECT COUNT(*) FROM turn_audit_events;" 2>/dev/null || echo 0)"
  if [ "${count:-0}" -lt 1 ]; then
    echo "WARN: turn_audit_events empty (sync optional)" >&2
  else
    echo "OK: turn_audit_events count=$count"
  fi
fi

echo "[turn_audit_gate] pass"
