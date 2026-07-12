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

Module: [`lib/mlc/common/stdlib/db/postgres.mlc`](../lib/mlc/common/stdlib/db/postgres.mlc).
Handle-based libpq wrapper (connection / result as `i32` tokens). Demo imports
the `.mlc` path directly.

Demo: [`misc/examples/postgres_select_demo.mlc`](../misc/examples/postgres_select_demo.mlc).
Gate: `scripts/run_postgres_gate.sh` (needs libpq + `DATABASE_URL` / `PGHOST`).

| Name | Signature | Description |
|------|-----------|-------------|
| `PgI32Result` | `PgI32Ok(i32) \| PgI32Err` | Connection or result handle |
| `PgStringResult` | `PgStringOk(string) \| PgStringErr` | Cell text |
| `connect` | `(conninfo: string) -> PgI32Result` | `PQconnectdb`-style connect |
| `exec` | `(connection_handle: i32, sql: string) -> PgI32Result` | Run SQL; returns result handle |
| `ntuples` | `(result_handle: i32) -> i32` | Row count |
| `nfields` | `(result_handle: i32) -> i32` | Column count |
| `getvalue` | `(result_handle: i32, row: i32, column: i32) -> PgStringResult` | Cell as string |
| `clear` | `(result_handle: i32) -> unit` | Free result |
| `finish` | `(connection_handle: i32) -> unit` | Close connection |
| `last_error` | `() -> string` | Last error string |

### Example (excerpt from demo)

Source: [`misc/examples/postgres_select_demo.mlc`](../misc/examples/postgres_select_demo.mlc)

```mlc
  let connection = unwrap_handle(connect("dbname=postgres"))
  if connection < 0 then
    println(last_error())
    return 1
  end
  let result = unwrap_handle(exec(connection, "SELECT 1 AS one"))
  if result < 0 then
    println(last_error())
    finish(connection)
    return 2
  end
  let cell = unwrap_text(getvalue(result, 0, 0))
  println(cell)
  clear(result)
  finish(connection)
```

(`unwrap_handle` / `unwrap_text` are local helpers in the same demo file.)

### Limitations (from STDLIB_BACKEND §1)

Postgres only; blocking libpq. PoC-level API (no prepared statements / pool in
this module). Link requires libpq.

## Crypto

Module: [`lib/mlc/common/stdlib/crypto/crypto.mlc`](../lib/mlc/common/stdlib/crypto/crypto.mlc)
(libsodium). Demo imports the `.mlc` path directly.

Demo: [`misc/examples/crypto_sha256_demo.mlc`](../misc/examples/crypto_sha256_demo.mlc).
Gate: `scripts/run_crypto_gate.sh` (`-lsodium`).

| Name | Signature | Description |
|------|-----------|-------------|
| `CryptoStringResult` | `CryptoStringOk(string) \| CryptoStringErr` | Fallible string (random / pwhash) |
| `sha256` | `(data: string) -> string` | SHA-256 hex digest |
| `hmac_sha256` | `(key: string, data: string) -> string` | HMAC-SHA-256 hex |
| `random_bytes` | `(count: i32) -> CryptoStringResult` | Cryptographic random bytes |
| `pwhash` | `(password: string) -> CryptoStringResult` | Password hash (argon2 via sodium) |
| `pwhash_verify` | `(hashed: string, password: string) -> bool` | Verify password hash |
| `last_error` | `() -> string` | Last error string |

### Example (from demo)

Source: [`misc/examples/crypto_sha256_demo.mlc`](../misc/examples/crypto_sha256_demo.mlc)

```mlc
fn main() -> i32 = do
  let digest = sha256("")
  println(digest)
  let mac = hmac_sha256("key", "message")
  println(mac)
  let _err = last_error()
  if digest == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" then
    0
  else
    1
  end
end
```

### Limitations (from STDLIB_BACKEND §1)

SHA-256 / HMAC / random / `pwhash` present. JWT and TLS are out of scope.

## Log

Module: [`lib/mlc/common/stdlib/log/logger.mlc`](../lib/mlc/common/stdlib/log/logger.mlc)
(`import … from 'Log'`). Structured JSON lines via thin `log_abi.hpp` (fwrite).

Demo (shared with Env): [`misc/examples/env_log_demo.mlc`](../misc/examples/env_log_demo.mlc).
Gate: `scripts/run_env_log_gate.sh`.

| Name | Signature | Description |
|------|-----------|-------------|
| `error` | `(message: string) -> unit` | Log level `error` |
| `warn` | `(message: string) -> unit` | Log level `warn` |
| `info` | `(message: string) -> unit` | Log level `info` |
| `debug` | `(message: string) -> unit` | Log level `debug` |

### Example (excerpt from demo)

Source: [`misc/examples/env_log_demo.mlc`](../misc/examples/env_log_demo.mlc)

```mlc
import { info, warn } from 'Log'

  info("env_log demo start")
  warn("using port " + port)
```

### Limitations (from STDLIB_BACKEND §1)

JSON-line logging in MLC; thin fwrite ABI. No log levels filter / sinks beyond
stderr-style write.

## Env

Module: [`lib/mlc/common/stdlib/env/env.mlc`](../lib/mlc/common/stdlib/env/env.mlc)
(`import … from 'Env'`). Thin `getenv` via `env_abi.hpp`.

Demo (shared with Log): [`misc/examples/env_log_demo.mlc`](../misc/examples/env_log_demo.mlc).

| Name | Signature | Description |
|------|-----------|-------------|
| `get` | `(key: string) -> Option<string>` | Lookup env var |
| `get_or` | `(key: string, default: string) -> string` | Lookup or default |
| `has` | `(key: string) -> bool` | Presence check |

All three are `export extern` + `blocking`.

### Example (from demo)

Source: [`misc/examples/env_log_demo.mlc`](../misc/examples/env_log_demo.mlc)

```mlc
import { get_or, has } from 'Env'

fn main() -> i32 = do
  let port = get_or("PORT", "8080")
  info("env_log demo start")
  warn("using port " + port)
  println(port)
  if has("PATH") then
    0
  else
    1
  end
end
```

### Limitations (from STDLIB_BACKEND §1)

Thin getenv wrapper only (no typed config files / dotenv loader in this module).

## Validate

Module: [`lib/mlc/common/stdlib/validate/validate.mlc`](../lib/mlc/common/stdlib/validate/validate.mlc)
(`import … from 'Validate'`). Pure MLC predicates.

Demo: [`misc/examples/validate_demo.mlc`](../misc/examples/validate_demo.mlc).
Gate: `scripts/run_validate_gate.sh`.

| Name | Signature | Description |
|------|-----------|-------------|
| `Result<T, E>` | `Ok(T) \| Err(E)` | Local result type in this module |
| `ValidateSuccess` | `{}` | Empty success payload |
| `non_empty` | `(value: string) -> Result<ValidateSuccess, string>` | Reject empty string |
| `min_length` | `(value: string, minimum: i32) -> Result<…>` | Min string length |
| `max_length` | `(value: string, maximum: i32) -> Result<…>` | Max string length |
| `range_i32` | `(value: i32, minimum: i32, maximum: i32) -> Result<…>` | Inclusive i32 range |

### Example (from demo)

Source: [`misc/examples/validate_demo.mlc`](../misc/examples/validate_demo.mlc)

```mlc
fn main() -> i32 = do
  match non_empty("name") {
    Err(message) => do
      println(message)
      1
    end,
    Ok(_) =>
      match range_i32(42, 1, 100) {
        Ok(_) => 0,
        Err(message) => do
          println(message)
          2
        end
      }
  }
end
```

### Limitations (from STDLIB_BACKEND §1)

No derive / JSON Schema. Small predicate set only.

## Json

Module: [`lib/mlc/common/stdlib/data/json.mlc`](../lib/mlc/common/stdlib/data/json.mlc)
(`import … from "Json"`). Runtime: `mlc/json/json.hpp`. No `misc/examples/*json*`
demo — snippet from program string in
[`test/mlc/derive_json_test.rb`](../test/mlc/derive_json_test.rb).

| Name | Signature | Description |
|------|-----------|-------------|
| `JsonError` | `MissingField(string) \| TypeMismatch(string, string)` | Typed decode errors for `derive { Json }` |
| `JsonValue` | `JsonNull \| JsonBool \| JsonNumber(f64) \| JsonString \| JsonArray \| JsonObject` | JSON AST |
| `parse_json` | `(json_str: str) -> JsonValue` | Parse |
| `stringify_json` | `(value: JsonValue) -> str` | Compact serialize |
| `stringify_json_pretty` | `(value: JsonValue, indent: i32) -> str` | Pretty serialize |
| `json_get` / `json_set` / `json_has_key` / `json_keys` | object helpers | Object field access |
| `json_array_length` / `json_array_get` / `json_array_push` | array helpers | Array access |
| `is_*` / `as_*` | predicates / extractors | Type tests and Option unwraps |
| `json_null` / `json_bool` / `json_number` / `json_string` / `json_array` / `json_object` | constructors | Build `JsonValue` |

### Example (from derive test fixture)

Source: program string in
[`test/mlc/derive_json_test.rb`](../test/mlc/derive_json_test.rb)
(`test_derive_json_round_trip_option_and_array`):

```mlc
type User = {
  id: i64,
  name: string,
  email: Option<string>,
  tags: string[]
} derive { Json }

fn main() -> i32 = 0
```

(Generates `User_to_json` / `User_from_json`; see also [API_CLIENT.md](API_CLIENT.md).)

### Limitations (from STDLIB_BACKEND §1)

Core `JsonValue` parse/stringify in C++ runtime. Typed record/sum (de)serialization
via `derive { Json }` (API_CLIENT track closed). No dedicated misc demo for
manual `parse_json` calls.
