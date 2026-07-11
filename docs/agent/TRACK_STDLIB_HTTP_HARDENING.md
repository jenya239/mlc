# Track: HTTP server hardening (keep-alive, limits, static files, graceful shutdown)

Parent: [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md), [../archive/tracks/TRACK_STDLIB_HTTP_MLC.md](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md),
[../archive/tracks/TRACK_CONCURRENCY_SPAWN_DETACH.md](../archive/tracks/TRACK_CONCURRENCY_SPAWN_DETACH.md).
Trigger: 2026-07-11 — user demo session built `misc/examples/http_scope_accept_loop_demo.mlc`
(4-connection scope+spawn accept loop) and `misc/examples/http_server_forever_demo.mlc`
(unbounded accept loop, `Connection: close` on every response). Both work,
but the HTTP layer is still minimal: no keep-alive, no request size/time
limits, no static file serving, no documented shutdown story for a `scope`
that never returns (the forever-demo's `scope |s| { while true { ... } }`
never joins its spawned tasks during normal operation — only on process
kill/panic).

## Status: open, средний приоритет

## Goal

Take the already-working MLC HTTP server (`HttpServer` module +
`Tcp`+`spawn`+`scope`) from "demo that curl can hit" to "small utility
server someone could actually point at a directory or a few routes for
real, short-lived, low-traffic use" — not aiming for nginx-class production
readiness (out of scope, see below).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `Connection: keep-alive` support in `HttpServer` (`lib/mlc/common/stdlib/net/http_server.mlc`): parse `Connection` request header, loop `read`/`parse`/`write_all` on the same `stream` while the client keeps it open and no `Connection: close` was requested, instead of one request per accepted connection | pending |
| 2 | Request size limit enforcement already exists for header block (65536) and body (1MB) inside `parse_http_request` — verify these are actually wired to a `400`/`413` response in the demo/server loop, not just a parse-failure fallthrough; add explicit test for oversized body → `413` | pending |
| 3 | Idle-connection timeout: a keep-alive connection with no next request within N seconds should close, not hang a thread forever (needs a `read` with timeout — check if `Tcp.read` supports one; if not, this sub-step first adds `SO_RCVTIMEO` to the `bind`/`accept`-returned socket via `runtime/include/mlc/net/tcp_bridge.hpp`) | pending |
| 4 | Static file serving: a `serve_static(directory: string, request: HttpRequest) -> HttpResponse` helper in `HttpServer` — path traversal guard (`..` rejection), `Content-Type` by extension (small hardcoded table: html/css/js/json/png/svg/txt), `404` on missing file | pending |
| 5 | Graceful shutdown doc + pattern: since `scope |s| { while true { accept... } }` never returns during normal operation, document the actual shutdown story (process kill = OS closes sockets, in-flight `scope`-spawned tasks are abandoned, not joined) in `STDLIB_BACKEND.md`; if a bounded-iteration variant is wanted for tests, that's what `http_scope_accept_loop_demo.mlc` already demonstrates — do not conflate the two patterns in docs | pending |
| 6 | Minimal load-test script (`scripts/http_load_smoke.sh` or Ruby, per scripts-language rule): N concurrent short-lived curl-equivalent requests against the forever-demo, assert all succeed and wall time roughly matches expected parallelism (same style as `scripts/run_http_scope_accept_loop_gate.sh`) | pending |
| 7 | Docs: `STDLIB_BACKEND.md` §1 table — update HTTP server row with keep-alive/static-file/timeout status | pending |
| 8 | Verify: self-host diff identical if `lib/mlc/common/stdlib/net/http_server.mlc` changes propagate through `compiler/`; `scripts/regression_gate.sh` green | pending |

## Out of scope

- TLS/HTTP2/HTTP3 (unchanged from `STDLIB_BACKEND.md` §1).
- Event-loop / epoll-based I/O multiplexing (thread-per-connection via
  `scope.spawn` stays the concurrency model; revisit only if a real
  benchmark shows thread-per-connection is the bottleneck for a concrete
  use case, not speculatively).
- Reverse-proxy features (load balancing, caching) — not a goal for MLC's
  stdlib HTTP server.
- Route-table API (`[HttpRoute]`, `HttpRouter.add(...)`) beyond what already
  exists as if/else in demos — separate track if/when a real app needs it.
