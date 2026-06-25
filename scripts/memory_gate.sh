#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT="${CODEBASE_MEMORY_PROJECT:-home-jenya-workspaces-current-mlc}"
cd "$ROOT"

echo "[memory_gate] reindex"
bash scripts/memory_reindex.sh

echo "[memory_gate] codebase-memory search_graph"
result="$(npx -y codebase-memory-mcp cli search_graph \
  "{\"project\":\"$PROJECT\",\"query\":\"compiler\",\"limit\":3}")"
total="$(echo "$result" | python3 -c "import json,sys; print(json.load(sys.stdin).get('total',0))")"
if [ "${total:-0}" -lt 1 ]; then
  echo "FAIL: search_graph total=$total" >&2
  exit 1
fi
echo "OK: search_graph total=$total"

echo "[memory_gate] mlc-memory memory_search"
count="$(python3 -c "
import sys
sys.path.insert(0, 'tools/mcp')
from mlc_memory_server import handle_memory_search
import json
rows = json.loads(handle_memory_search({'query': 'build', 'limit': 5}))
print(len(rows))
")"
if [ "${count:-0}" -lt 1 ]; then
  echo "FAIL: memory_search count=$count (run scripts/memory_bootstrap.py)" >&2
  exit 1
fi
echo "OK: memory_search count=$count"

echo "[memory_gate] sync obsolete memories"
sync_report="$(python3 scripts/memory_sync_obsolete.py --apply)"
updated="$(echo "$sync_report" | python3 -c "import json,sys; print(json.load(sys.stdin).get('updated',0))")"
echo "OK: memory_sync_obsolete updated=$updated"

echo "[memory_gate] task metrics"
python3 scripts/memory_task_metrics.py --report --limit 3 >/dev/null
echo "OK: task_metrics"

if [ "${GRAPHITI_GATE:-0}" = "1" ]; then
  echo "[memory_gate] graphiti health"
  curl -sf "http://localhost:8000/health" >/dev/null || { echo "FAIL: Graphiti not on :8000" >&2; exit 1; }
  echo "OK: graphiti health"
fi

echo "[memory_gate] pass"
