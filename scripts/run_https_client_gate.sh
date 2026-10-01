#!/usr/bin/env bash
# HTTPS client v1 gate. Runs steps 1–5. Step 6 (live CA) stays manual.
# Invariants:
# - runtime/include/mlc.hpp, compiler/mlcc_precompiled.hpp, and
#   runtime/include/mlc/net/http.hpp have an empty diff against HEAD
# - compiler/build_bin.sh diff must not mention curl
# - lib/mlc/common/stdlib/net/http.mlc diff is empty or the single HttpsClient pointer comment
# - compiler/out/mlcc does not link libcurl and the mlcc build does not include curl.h
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
cd "$ROOT_DIR"

if [[ ! -x "$MLCC" ]]; then
  echo "[https gate] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

echo "[https gate] check=forbidden_diff" >&2
for path in runtime/include/mlc.hpp compiler/mlcc_precompiled.hpp runtime/include/mlc/net/http.hpp; do
  if ! git diff --quiet -- "$path"; then
    git diff --stat -- "$path" >&2
    echo "[https gate] FAIL: $path differs from HEAD" >&2
    exit 1
  fi
done

if git diff -- compiler/build_bin.sh | grep -E 'curl\.h|-lcurl|libcurl' >/dev/null; then
  git diff --stat -- compiler/build_bin.sh >&2
  echo "[https gate] FAIL: compiler/build_bin.sh diff mentions curl" >&2
  exit 1
fi

http_added="$(git diff -- lib/mlc/common/stdlib/net/http.mlc | grep -E '^\+' | grep -v '^+++' || true)"
http_removed="$(git diff -- lib/mlc/common/stdlib/net/http.mlc | grep -E '^-' | grep -v '^---' || true)"
if [[ -n "$http_removed" ]]; then
  echo "[https gate] FAIL: http.mlc deletes lines" >&2
  exit 1
fi
if [[ -n "$http_added" && "$http_added" != "+// HTTPS requests: HttpsClient in https_client.mlc; gate scripts/run_https_client_gate.sh." ]]; then
  echo "[https gate] FAIL: http.mlc diff is not the HttpsClient pointer" >&2
  exit 1
fi

echo "[https gate] check=mlcc_without_curl" >&2
if ldd "$MLCC" | grep -q 'libcurl'; then
  echo "[https gate] FAIL: mlcc links libcurl" >&2
  exit 1
fi
if grep -n -E 'curl\.h|-lcurl' \
  compiler/build.sh runtime/include/mlc.hpp compiler/mlcc_precompiled.hpp >/dev/null; then
  echo "[https gate] FAIL: mlcc build pulls curl" >&2
  exit 1
fi

export HTTPS_CLIENT_REQUIRE=1
unset HTTPS_MLC_OUT HTTPS_FAILURE_OUT HTTPS_PURE_OUT || true
rm -rf "$ROOT_DIR/tmp/https_client_mlc" "$ROOT_DIR/tmp/https_client_failure"

echo "[https gate] step=1 link_probe" >&2
bash "$ROOT_DIR/scripts/run_https_link_probe_smoke.sh"
echo "[https gate] step=2 pure" >&2
bash "$ROOT_DIR/scripts/run_https_client_pure_smoke.sh"
echo "[https gate] step=3 curl_abi" >&2
bash "$ROOT_DIR/scripts/run_curl_abi_cpp_smoke.sh"
echo "[https gate] step=4 mlc_client" >&2
bash "$ROOT_DIR/scripts/run_https_client_mlc_smoke.sh"
echo "[https gate] step=5 failure" >&2
bash "$ROOT_DIR/scripts/run_https_client_failure_smoke.sh"

echo "[https gate] ok" >&2
