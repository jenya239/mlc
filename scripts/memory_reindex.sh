#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT="${CODEBASE_MEMORY_PROJECT:-home-jenya-workspaces-current-mlc}"
npx -y codebase-memory-mcp cli index_repository "{\"repo_path\":\"$ROOT\",\"project_name\":\"mlc\"}" >/dev/null
npx -y codebase-memory-mcp cli index_status "{\"project\":\"$PROJECT\"}"
