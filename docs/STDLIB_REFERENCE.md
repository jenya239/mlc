# MLC Stdlib Reference

Parent: [PLAN.md](PLAN.md) §28; [STDLIB_BACKEND.md](STDLIB_BACKEND.md);
track: [agent/TRACK_STDLIB_DOCS.md](agent/TRACK_STDLIB_DOCS.md).

This is a **module API reference**, not a tutorial and not the language
manual ([LANGUAGE_REFERENCE.md](LANGUAGE_REFERENCE.md)). Each section will
hold: exported `fn`/`type` signatures (table), one short usage snippet lifted
from a cited demo or fixture (never invented), and limitations copied from
[STDLIB_BACKEND.md](STDLIB_BACKEND.md) §1. JobQueue / Supervisor have no MLC
module surface — out of scope here.

## Contents

- [Tcp](#tcp)
- [HttpServer](#httpserver)
- [WebSocket](#websocket)
- [Postgres](#postgres)
- [Crypto](#crypto)
- [Log](#log)
- [Env](#env)
- [Validate](#validate)
- [Json](#json)

---

## Tcp

Module: [`lib/mlc/common/stdlib/net/tcp.mlc`](../lib/mlc/common/stdlib/net/tcp.mlc)
(`import … from 'Tcp'`). Opaque `i32` tokens are real fds (fd-as-token).

Demo: [`misc/examples/tcp_echo_demo.mlc`](../misc/examples/tcp_echo_demo.mlc).
Tcp+`spawn` under mlcc: [`tcp_spawn_echo_mlcc.mlc`](../misc/examples/tcp_spawn_echo_mlcc.mlc).

| Name | Signature | Description |
|------|-----------|-------------|
| `bind` | `(host: string, port: i32) -> Option<i32>` | Listen socket; `None` → `last_error()` |
| `accept` | `(listener: i32) -> Option<i32>` | Blocking accept |
| `read` | `(stream: i32, max_bytes: i32) -> Option<string>` | Blocking read up to `max_bytes` |
| `set_recv_timeout` | `(stream: i32, timeout_seconds: i32) -> bool` | `SO_RCVTIMEO` idle timeout |
| `write_all` | `(stream: i32, data: string) -> bool` | Write full buffer |
| `close_listener` | `(listener: i32) -> unit` | Close listen fd |
| `close_stream` | `(stream: i32) -> unit` | Close connection fd |
| `port` | `(listener: i32) -> i32` | Bound port (useful after `port: 0`) |
| `last_error` | `() -> string` | Last bridge error string |

All listed fns are `export extern` + `blocking` via `mlc/net/tcp_bridge.hpp`.

### Example (excerpt from demo)

Source: [`misc/examples/tcp_echo_demo.mlc`](../misc/examples/tcp_echo_demo.mlc)

```mlc
import { bind, accept, read, write_all, close_listener, close_stream, port, last_error } from 'Tcp'

fn main() -> i32 = do
  let listener_option = bind("127.0.0.1", 0)
  if !listener_option.is_some() then
    println(last_error())
    return 1
  end
  let listener = listener_option.unwrap()
  File.write("tcp_echo_port.txt", `${port(listener)}`)
  let stream_option = accept(listener)
  if !stream_option.is_some() then
    println(last_error())
    close_listener(listener)
    return 2
  end
  let stream = stream_option.unwrap()
  let data_option = read(stream, 64)
  let data = if data_option.is_some() then data_option.unwrap() else "" end
  let wrote = write_all(stream, data)
  close_stream(stream)
  close_listener(listener)
  if wrote then 0 else 3 end
end
```

### Limitations (from STDLIB_BACKEND §1)

Blocking TCP only; reachable from Ruby and mlcc. No TLS. Errors surface as
`None`/`false` plus `last_error()`.

## HttpServer

Module: [`lib/mlc/common/stdlib/net/http_server.mlc`](../lib/mlc/common/stdlib/net/http_server.mlc)
(`import … from 'HttpServer'`). Pure MLC HTTP/1.1 parse + response format;
I/O stays on `Tcp`.

Demo: [`misc/examples/http_server_forever_demo.mlc`](../misc/examples/http_server_forever_demo.mlc).
Bounded accept (joins on scope end):
[`http_scope_accept_loop_demo.mlc`](../misc/examples/http_scope_accept_loop_demo.mlc).

| Name | Signature | Description |
|------|-----------|-------------|
| `HttpHeader` | `{ name: string, value: string }` | One header line |
| `HttpRequest` | `{ method, path, headers, body }` | Parsed request |
| `HttpResponse` | `{ status: i32, headers, body }` | Response to format |
| `HttpParseResult` | `HttpParseOk(HttpRequest) \| HttpParseErr \| HttpParseTooLarge` | Parse outcome |
| `parse_http_request` | `(raw: string) -> HttpParseResult` | Parse HTTP/1.1 request bytes |
| `format_http_response` | `(response: HttpResponse) -> string` | Serialize response |
| `find_header_value` | `(headers: [HttpHeader], name: string) -> string` | Case-insensitive header lookup |
| `http_request_wants_keep_alive` | `(request: HttpRequest) -> i32` | Keep-alive preference |
| `http_response_with_connection` | `(response, keep_alive: i32) -> HttpResponse` | Set `Connection` header |
| `serve_static` | `(directory: string, request: HttpRequest) -> HttpResponse` | Static files (`GET`; traversal → 400; missing → 404) |
| `http_bad_request` | `() -> HttpResponse` | 400 |
| `http_payload_too_large` | `() -> HttpResponse` | 413 |
| `http_not_found` | `() -> HttpResponse` | 404 |
| `http_method_not_allowed` | `() -> HttpResponse` | 405 |

### Example (excerpt from forever demo)

Source: [`misc/examples/http_server_forever_demo.mlc`](../misc/examples/http_server_forever_demo.mlc)
(accept loop + `scope.spawn`; `handle_one` / `route_http` in the same file.)

```mlc
  scope |server_scope| do
    while true do
      let stream_option = accept(listener)
      if !stream_option.is_some() then
        println("[mlc-http] accept failed: " + last_error())
      else
        let stream = stream_option.unwrap()
        server_scope.spawn do handle_one(stream) end
      end
    end
  end
```

Parse + format path inside `handle_one` (same file):

```mlc
  let response = match parse_http_request(raw) {
    HttpParseErr => …,
    HttpParseTooLarge => …,
    HttpParseOk(request) => route_http(request)
  }
  let wrote = write_all(stream, format_http_response(response))
```

### Limitations (from STDLIB_BACKEND §1)

HTTP/1.1 only; keep-alive, body limit → `HttpParseTooLarge` (413), idle recv
timeout via `Tcp.set_recv_timeout`, `serve_static` with traversal/`404`/
`Content-Type`. No `[HttpRoute]` table API. Forever accept has no MLC drain
API (process signal abandons in-flight handlers — see STDLIB_BACKEND §1
shutdown subsection). TLS / HTTP/2 out of scope.

## WebSocket

Module: [`lib/mlc/common/stdlib/net/websocket.mlc`](../lib/mlc/common/stdlib/net/websocket.mlc).
Upgrade / frames / handshake in MLC; thin `websocket_bridge.hpp` for connection
table + TCP write. Demo imports the `.mlc` path directly (not `from 'WebSocket'`).

Demo: [`misc/examples/websocket_echo_demo.mlc`](../misc/examples/websocket_echo_demo.mlc).
Gate: `scripts/run_websocket_gate.sh`.

| Name | Signature | Description |
|------|-----------|-------------|
| `WsHandleResult` | `WsHandleOk(i32) \| WsHandleNone` | Connection handle after upgrade |
| `WsTextResult` | `WsTextOk(string) \| WsTextNone` | One text frame payload |
| `WsFrameDecode` | `WsFrameIncomplete \| WsFrameOk(opcode, payload, consumed) \| WsFrameTooLarge` | Incremental frame decode |
| `WsUpgradeCheck` | `WsUpgradeOk(key) \| WsUpgradeErr(message)` | Header validation |
| `WsHandshakeResult` | `WsHandshakeOk(response) \| WsHandshakeErr(message)` | 101 response body or error |
| `upgrade` | `(stream: i32) -> WsHandleResult` | Read HTTP upgrade on Tcp stream → WS handle |
| `read_text` | `(connection: i32) -> WsTextResult` | Blocking read of next text payload |
| `write_text` | `(connection: i32, data: string) -> bool` | Send text frame (payload ≤ 1 MiB) |
| `close` | `(connection: i32) -> unit` | Close frame + release handle |
| `last_error` | `() -> string` | Last bridge/table error |
| `check_websocket_upgrade` | `(request: HttpRequest) -> WsUpgradeCheck` | Validate upgrade headers |
| `build_websocket_upgrade_response` | `(request: HttpRequest) -> WsHandshakeResult` | Build 101 response |
| `build_websocket_upgrade_from_raw` | `(raw: string) -> WsHandshakeResult` | Parse raw HTTP then upgrade |
| `format_websocket_upgrade_response` | `(accept_key: string) -> string` | Format 101 bytes |
| `sec_websocket_accept` | `(client_key: string) -> string` | `Sec-WebSocket-Accept` value |
| `try_decode_frame` | `(buffer: string) -> WsFrameDecode` | Decode one frame from buffer |
| `encode_text_frame` | `(payload: string) -> string` | Unmasked text frame bytes |
| `encode_close_frame` | `() -> string` | Close frame bytes |
| `encode_unmasked_frame` / `encode_masked_frame` | `(opcode, payload[, mask]) -> string` | Low-level frame encode |
| `sha1_hex` | `(message: string) -> string` | SHA-1 hex (handshake helper) |

### Example (from demo)

Source: [`misc/examples/websocket_echo_demo.mlc`](../misc/examples/websocket_echo_demo.mlc)

```mlc
  let connection = match upgrade(stream) {
    WsHandleOk(handle) => handle,
    WsHandleNone => -1
  }
  if connection < 0 then
    println("websocket upgrade failed")
    close_listener(listener)
    return 3
  end
  let text = match read_text(connection) {
    WsTextOk(payload) => payload,
    WsTextNone => ""
  }
  let wrote = write_text(connection, text)
  close(connection)
```

### Limitations (from STDLIB_BACKEND §1)

Protocol on MLC; thin bridge residual includes Ruby `:extern` stubs. Gate is
MLC echo + Ruby client (`run_websocket_gate.sh`). No TLS. Payload size capped
(1 MiB on `write_text` / decode path).

## Postgres

Status: pending — filled in STEP=4.

Pinned source: [`misc/examples/postgres_select_demo.mlc`](../misc/examples/postgres_select_demo.mlc).

Planned: API table; snippet from select demo; limitations from STDLIB_BACKEND §1
(blocking libpq; Postgres only).

## Crypto

Status: pending — filled in STEP=4.

Pinned source: [`misc/examples/crypto_sha256_demo.mlc`](../misc/examples/crypto_sha256_demo.mlc).

Planned: API table; snippet from sha256 demo; limitations from STDLIB_BACKEND §1
(SHA-256/HMAC/random/pwhash; JWT/TLS out).

## Log

Status: pending — filled in STEP=5.

Pinned source: [`misc/examples/env_log_demo.mlc`](../misc/examples/env_log_demo.mlc).

Planned: API table; snippet from env_log demo; limitations from STDLIB_BACKEND §1.

## Env

Status: pending — filled in STEP=5.

Pinned source: [`misc/examples/env_log_demo.mlc`](../misc/examples/env_log_demo.mlc).

Planned: API table; snippet from env_log demo; limitations from STDLIB_BACKEND §1.

## Validate

Status: pending — filled in STEP=5.

Pinned source: [`misc/examples/validate_demo.mlc`](../misc/examples/validate_demo.mlc).

Planned: API table; snippet from validate demo; limitations from STDLIB_BACKEND §1.

## Json

Status: pending — filled in STEP=5.

Pinned sources: API table from
[`lib/mlc/common/stdlib/data/json.mlc`](../lib/mlc/common/stdlib/data/json.mlc);
snippet from program string in
[`test/mlc/derive_json_test.rb`](../test/mlc/derive_json_test.rb)
(no `misc/examples/*json*` demo).

Planned: API table; cited test fixture snippet; limitations from STDLIB_BACKEND §1.
