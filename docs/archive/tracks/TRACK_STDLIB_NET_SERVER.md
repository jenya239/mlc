# Track: TCP/HTTP server stdlib

Parent: [../../PLAN.md](../../PLAN.md), [../../STDLIB_BACKEND.md](../../STDLIB_BACKEND.md) §5.1.
Trigger: backend-class apps need listen/accept; today only HTTP **client**
(`mlc/net/http.hpp` + curl). No `bind`/`listen`/`accept` in runtime/stdlib.

## Status: **closed** (2026-07-10) — STEP=1–8 **done**

**Driver 2026-07-10:** STEP=8 — `STDLIB_BACKEND.md`/`MLC.md` note;
`misc/examples/tcp_echo_demo.mlc`; `regression_gate` 20/0; track archived.

**Critic 2026-07-10:** `critique-audit` — commits `e27a7092`…`d0d3206a`
match STEP 1–8 claims; PLAN §11/queue/archive path synced; smokes
tcp 12/0, http_request 14/0, http_router 14/0, http_server 5/0;
codegen `tcp_stdlib_test` 1/18. Residuals (no reopen): Decision
`Result<TcpListener,_>` vs MLC `Option<i32>`+`last_error`; HTTP
parse/router/serve C++-only (no MLC stdlib); example not compile-gated
in STEP=8; archive links to SPAWN/RUBY_PARITY fixed to `../../agent/`.

## Decision (STEP=1, 2026-07-10)

### Types (MLC surface; C++ mirrors under `mlc::net`)

| Type | Role |
|------|------|
| `TcpListener` | Bound listening socket. Opaque handle (no raw fd in MLC). |
| `TcpStream` | Accepted (or client) connection. Opaque; owns fd; drop/close. |
| `HttpRequest` | `method: string`, `path: string`, `headers: Array<{name,value}>`, `body: string` (may be empty). |
| `HttpResponse` | `status: i32`, `headers: Array<{name,value}>`, `body: string`. |
| `NetError` | `string` message for v1 (`Result<T, string>`); no errno enum yet. |

### Methods (v1)

```
TcpListener.bind(host: string, port: i32) -> Result<TcpListener, string>
TcpListener.accept(self) -> Result<TcpStream, string>   # blocking
TcpListener.close(self) -> unit

TcpStream.read(self, max_bytes: i32) -> Result<string, string>
TcpStream.write_all(self, data: string) -> Result<(), string>
TcpStream.close(self) -> unit

HttpRequest.parse(raw: string) -> Result<HttpRequest, string>   # STEP=5
HttpResponse.write_to(self, stream: TcpStream) -> Result<(), string>  # STEP=6
```

Routing (STEP=6): explicit `Array` of `{ method, path, handler }` where
`handler: fn(HttpRequest) -> HttpResponse` — **sync only** in v1. Match is
exact method + exact path (no glob/regex). Default 404 response built by
runtime helper if no match.

### Error model

- Socket/bind/listen/accept/read/write failures → `Err(string)` (message may
  include `strerror`). **No panic** on I/O errors; no C++ exceptions across
  the MLC boundary.
- Programmer bugs (use after close) → debug panic acceptable; release: Err.

### Accept / concurrency

- STEP=2–6: **single-threaded blocking** accept loop in user code
  (`while true { let stream = listener.accept()? … }`).
- STEP=7: optional `accept` + `ThreadPool.submit` per connection; pool is
  existing `mlc::concurrency::ThreadPool`. No epoll/reactor in this track.
  **Use an `extern fn` stdlib wrapper in the same Ruby `lib/mlc/common/stdlib/`
  module as `Tcp`, not the language-level `spawn`** — `spawn`/`Mutex`/`Channel`
  exist only in self-hosted `compiler/`, unreachable from the Ruby pipeline
  this track's TCP stdlib depends on; see
  [TRACK_CONCURRENCY_RUBY_PARITY](../../agent/TRACK_CONCURRENCY_RUBY_PARITY.md) (found
  2026-07-10). Also: self-hosted `spawn do <tail-call> end` currently
  double-executes the block body — [TRACK_LANG_SPAWN_DOUBLE_EXEC](TRACK_LANG_SPAWN_DOUBLE_EXEC.md),
  another reason to avoid `spawn` here until fixed.

### Bind defaults

- `TcpListener.bind(host, port)` — caller passes host. Examples/tests use
  `127.0.0.1`. Binding `0.0.0.0` allowed when caller asks (no API restriction).
- IPv4 only in v1. `SO_REUSEADDR` on by default.

### HTTP v1 scope

| In | Out |
|----|-----|
| Request-line + headers (`\r\n` / `\n`) | TLS, HTTP/2, WebSocket |
| Body if `Content-Length` present, cap **1 MiB** | Chunked transfer, trailers |
| Header block cap **64 KiB** | Pipelining, keep-alive reuse (may close after one response) |
| Response: status-line + headers + body | Absolute-URI form, CONNECT |

### Open questions — resolved

1. **Raw fd?** No — opaque `TcpStream` / `TcpListener` only.
2. **Handler?** Sync `fn(HttpRequest) -> HttpResponse` only in v1.
3. **Default bind?** No silent default host; caller supplies; docs/examples use `127.0.0.1`.

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
| 1 | Design: public API (`TcpListener` / `TcpStream` / `HttpRequest` / `HttpResponse`); error model (`Result` vs panic); blocking-first accept loop; what v1 HTTP supports (request-line + headers, fixed body limit). Document in this TRACK «Decision». | **done** (2026-07-10: see Decision) |
| 2 | Runtime: `runtime/include/mlc/net/tcp.hpp` — wrap POSIX `socket`/`bind`/`listen`/`accept`/`read`/`write`/`close`/`setsockopt(SO_REUSEADDR)`; IPv4 `127.0.0.1` + port; RAII close. | **done** (2026-07-10: tcp.hpp + test_tcp 12/0; `result::ok` move) |
| 3 | Stdlib: `lib/mlc/common/stdlib/net/tcp.mlc` + registry/scanner; smoke compile of listen+accept+write from MLC. | **done** (2026-07-10: Option+i32 handles + last_error; codegen smoke) |
| 4 | Gate: echo server test (connect via `nc`/`curl` or Ruby `TCPSocket`); script under `scripts/` or `test/mlc/`. | **done** (2026-07-10: MLC server + C++ client; gate script) |
| 5 | HTTP/1.1: parse request-line + headers into `HttpRequest` (C++ helper or MLC); reject oversized headers; no chunked/body streaming in v1 beyond optional `Content-Length` read up to cap. | **done** (2026-07-10: `parse_http_request`; smoke 14/0) |
| 6 | Routing: explicit `Array`/`Map` of `(method, path) → handler`; write status-line + headers + body; 404 default. | **done** (2026-07-10: `HttpRouter` + `write_http_response`; smoke 14/0) |
| 7 | Concurrency: accept loop dispatches connection handler on `ThreadPool` (or document single-thread-only if pool wiring is blocked). | **done** (2026-07-10: `serve_http_with_thread_pool`; smoke 5/0) |
| 8 | Docs (`STDLIB_BACKEND.md` / short `MLC.md` note) + example under `misc/examples/`; verify-gate (`regression_gate.sh`; self-host if `compiler/**` touched) + close. | **done** (2026-07-10: docs + tcp_echo_demo; regression 20/0; closed) |

<!-- sub-steps STEP=1: 1) list types/methods in Decision table; 2) pick Result vs errno panic; 3) list non-goals (TLS, HTTP/2, WS) -->
<!-- sub-steps STEP=2: 1) tcp.hpp listen/accept/read/write; 2) C++ unit or smoke main; 3) include from mlc.hpp if needed -->
<!-- sub-steps STEP=3: 1) tcp.mlc externs; 2) registry entry; 3) MLC compile smoke -->
<!-- sub-steps STEP=4: 1) echo fixture; 2) automated connect+assert; 3) SESSION verify line -->

## Out of scope (this track)

- TLS / HTTPS
- HTTP/2, WebSocket (separate track after this closes)
- Postgres / crypto
- Full async event loop (use ThreadPool, not epoll reactor in v1)

## Open questions

None for v1 — resolved in Decision above.
