#!/usr/bin/env bash
# Optional FalkorDB + Graphiti MCP (STEP=2). Requires Docker + OPENAI_API_KEY.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GRAPHITI_DIR="${GRAPHITI_DIR:-$ROOT/.vendor/graphiti}"
GROUP_ID="${GRAPHITI_GROUP_ID:-project:mlc}"
COMPOSE_FILE="docker/docker-compose-falkordb-combined.yml"

if ! command -v docker >/dev/null 2>&1; then
  echo "docker not found; skip Graphiti (SQLite mlc-memory remains default)" >&2
  exit 0
fi

if [ -z "${OPENAI_API_KEY:-}" ]; then
  echo "OPENAI_API_KEY unset; export it or add to $GRAPHITI_DIR/mcp_server/.env" >&2
  exit 1
fi

if [ ! -d "$GRAPHITI_DIR/.git" ]; then
  echo "[graphiti] clone into $GRAPHITI_DIR"
  mkdir -p "$(dirname "$GRAPHITI_DIR")"
  git clone --depth 1 https://github.com/getzep/graphiti.git "$GRAPHITI_DIR"
fi

cd "$GRAPHITI_DIR/mcp_server"
if [ ! -f .env ]; then
  cat > .env <<EOF
OPENAI_API_KEY=${OPENAI_API_KEY}
GRAPHITI_GROUP_ID=${GROUP_ID}
SEMAPHORE_LIMIT=5
EOF
  echo "[graphiti] wrote .env (GROUP_ID=$GROUP_ID)"
fi

if [ ! -f "$COMPOSE_FILE" ]; then
  COMPOSE_FILE="docker-compose.yml"
fi

echo "[graphiti] docker compose -f $COMPOSE_FILE up -d"
docker compose -f "$COMPOSE_FILE" up -d

echo "[graphiti] MCP http://localhost:8000/mcp/  group_id=$GROUP_ID"
echo "[graphiti] migrate: python3 scripts/memory_export_graphiti.py | see docs/agent/TRACK_MEMORY_MCP.md"
