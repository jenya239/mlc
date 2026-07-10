# Track: TCP/HTTP server stdlib

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.1.
Trigger: backend-class apps need listen/accept; today only HTTP **client**
(`mlc/net/http.hpp` + curl). No `bind`/`listen`/`accept` in runtime/stdlib.

## Status: **open** — STEP=1 next (API design)

## Goal

Minimal blocking TCP listener + HTTP/1.1 request parse + explicit route table,
usable from MLC without a framework. Concurrent accept via existing
`ThreadPool`/`Isolate` — after single-threaded smoke works.

## Constraints (from STDLIB_BACKEND §3)

- No reflection DI, no decorator routing, no hot-reload.
- Routes: explicit table or codegen from a declarative structure.
- Prefer C++ runtime header + thin `std/net/*.mlc` (same pattern as HTTP client),
  not a large pure-MLC reimplementation of sockets.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: public API (`TcpListener` / `TcpStream` / `HttpRequest` / `HttpResponse`); error model (`Result` vs panic); blocking-first accept loop; what v1 HTTP supports (request-line + headers, fixed body limit). Document in this TRACK «Decision». | pending |
| 2 | Runtime: `runtime/include/mlc/net/tcp.hpp` — wrap POSIX `socket`/`bind`/`listen`/`accept`/`read`/`write`/`close`/`setsockopt(SO_REUSEADDR)`; IPv4 `127.0.0.1` + port; RAII close. | pending |
| 3 | Stdlib: `lib/mlc/common/stdlib/net/tcp.mlc` + registry/scanner; smoke compile of listen+accept+write from MLC. | pending |
| 4 | Gate: echo server test (connect via `nc`/`curl` or Ruby `TCPSocket`); script under `scripts/` or `test/mlc/`. | pending |
| 5 | HTTP/1.1: parse request-line + headers into `HttpRequest` (C++ helper or MLC); reject oversized headers; no chunked/body streaming in v1 beyond optional `Content-Length` read up to cap. | pending |
| 6 | Routing: explicit `Array`/`Map` of `(method, path) → handler`; write status-line + headers + body; 404 default. | pending |
| 7 | Concurrency: accept loop dispatches connection handler on `ThreadPool` (or document single-thread-only if pool wiring is blocked). | pending |
| 8 | Docs (`STDLIB_BACKEND.md` / short `MLC.md` note) + example under `misc/examples/`; verify-gate (`regression_gate.sh`; self-host if `compiler/**` touched) + close. | pending |

<!-- sub-steps STEP=1: 1) list types/methods in Decision table; 2) pick Result vs errno panic; 3) list non-goals (TLS, HTTP/2, WS) -->
<!-- sub-steps STEP=2: 1) tcp.hpp listen/accept/read/write; 2) C++ unit or smoke main; 3) include from mlc.hpp if needed -->
<!-- sub-steps STEP=3: 1) tcp.mlc externs; 2) registry entry; 3) MLC compile smoke -->
<!-- sub-steps STEP=4: 1) echo fixture; 2) automated connect+assert; 3) SESSION verify line -->

## Out of scope (this track)

- TLS / HTTPS
- HTTP/2, WebSocket (separate track after this closes)
- Postgres / crypto
- Full async event loop (use ThreadPool, not epoll reactor in v1)

## Open questions (resolve in STEP=1)

1. Expose raw `i32` fd in MLC vs opaque `TcpStream` handle only?
2. Handler signature: `fn(HttpRequest) -> HttpResponse` sync only for v1?
3. Default bind address: `127.0.0.1` only vs `0.0.0.0`?
