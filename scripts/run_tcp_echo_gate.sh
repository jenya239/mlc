#!/usr/bin/env bash
# TRACK_STDLIB_NET_SERVER STEP=4 — MLC echo server + C++ client gate.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"
# Runtime C++ smoke (bind/accept/echo in-process)
g++ -std=c++20 -pthread -I runtime/include -o /tmp/mlc_test_tcp runtime/test/test_tcp.cpp
/tmp/mlc_test_tcp
rm -f /tmp/mlc_test_tcp
# MLC server + client roundtrip
bundle exec ruby -Ilib:test test/mlc/tcp_stdlib_test.rb -n test_echo_server_roundtrip_via_client
