# MLC Stdlib Reference

Parent: [PLAN.md](PLAN.md) §28; [STDLIB_BACKEND.md](STDLIB_BACKEND.md);
track: [archive/tracks/TRACK_STDLIB_DOCS.md](archive/tracks/TRACK_STDLIB_DOCS.md).

This is a **module API reference**, not a tutorial and not the language
manual ([LANGUAGE_REFERENCE.md](LANGUAGE_REFERENCE.md)). Each section will
hold: exported `fn`/`type` signatures (table), one short usage snippet lifted
from a cited demo or fixture (never invented), and limitations copied from
[STDLIB_BACKEND.md](STDLIB_BACKEND.md) §1. JobQueue / Supervisor have no MLC
module surface — out of scope here.

## Contents

- [Tcp](#tcp)
- [HttpServer](#httpserver)
- [HttpsClient](#httpsclient)
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

Parse + format path inside `handle_one` (same file; Err/TooLarge arms
omitted — see demo for full `match`):

```mlc
    HttpParseOk(request) => do
      let handled = route_http(request)
      println("[mlc-http] " + request.method + " " + request.path + " -> " + handled.status.to_string())
      handled
    end
  }
  let wrote = write_all(stream, format_http_response(response))
```

### Limitations (from STDLIB_BACKEND §1)

HTTP/1.1 only; keep-alive, body limit → `HttpParseTooLarge` (413), idle recv
timeout via `Tcp.set_recv_timeout`, `serve_static` with traversal/`404`/
`Content-Type`. No `[HttpRoute]` table API. Forever accept has no MLC drain
API (process signal abandons in-flight handlers — see STDLIB_BACKEND §1
shutdown subsection). TLS / HTTP/2 out of scope.

## HttpsClient

Modules: [`lib/mlc/common/stdlib/net/https_client.mlc`](../lib/mlc/common/stdlib/net/https_client.mlc)
(`HttpsClient`) and [`lib/mlc/common/stdlib/net/https_request.mlc`](../lib/mlc/common/stdlib/net/https_request.mlc)
(`HttpsRequests`). TLS and HTTP/1.1 go through libcurl in
`runtime/include/mlc/net/curl_abi.hpp` (`mlc::curl_abi`). Import the `.mlc`
paths directly.

Demo: [`misc/examples/https_get_demo.mlc`](../misc/examples/https_get_demo.mlc).
Gate: `HTTPS_CLIENT_REQUIRE=1 bash scripts/run_https_client_gate.sh` (steps 1–5).
Live default-CA check is separate: `HTTPS_LIVE=1 bash scripts/run_https_client_live_smoke.sh`.

| Name | Signature | Description |
|------|-----------|-------------|
| `HttpsHeader` | `{ name: string, value: string }` | One header |
| `HttpsRequest` | `{ method, url, headers: [HttpsHeader], body, timeout_milliseconds: i32, max_response_bytes: i32, ca_bundle_path }` | Request. Defaults at `https_get` / `https_post`: 30000 ms, 4194304 bytes, empty CA path |
| `HttpsResponse` | `{ status: i32, headers: [HttpsHeader], body: string }` | Response |
| `HttpsFailureKind` | `InvalidRequest \| ResolveFailed \| ConnectFailed \| TimedOut \| CertificateRejected \| ResponseTooLarge \| TransportFailed` | Failure class |
| `HttpsFailure` | `{ kind: HttpsFailureKind, message: string }` | Failure |
| `HttpsResult` | `HttpsOk(HttpsResponse) \| HttpsErr(HttpsFailure)` | Outcome. Not stdlib `Result` |
| `HttpsStatusClass` | `Informational \| Success \| Redirect \| ClientError \| ServerError \| OtherStatus` | Status class |
| `https_request_problem` | `(request: HttpsRequest) -> string` | Empty string means the request is acceptable |
| `https_header_lines` | `(headers: [HttpsHeader]) -> string` | `Name: value\n` lines |
| `parse_response_header_block` | `(raw: string) -> [HttpsHeader]` | Headers of the last response block |
| `https_find_header` | `(headers: [HttpsHeader], name: string) -> string` | Case-insensitive lookup |
| `https_status_class` | `(status: i32) -> HttpsStatusClass` | |
| `https_status_is_retryable` | `(status: i32) -> bool` | 429, 500, 502, 503, 504 |
| `https_failure_kind_from_code` | `(code: i32) -> HttpsFailureKind` | 1..6 map to the specific kinds; any other code is `TransportFailed` |
| `https_send` | `(request: HttpsRequest) -> HttpsResult` | Blocking call |
| `https_get` | `(url: string, headers: [HttpsHeader]) -> HttpsResult` | GET with defaults |
| `https_post` | `(url: string, headers: [HttpsHeader], body: string) -> HttpsResult` | POST with defaults |
| `https_live_handle_count` | `() -> i32` | Live result slots; tests use this for leaks |

### Example (excerpt from demo)

Source: [`misc/examples/https_get_demo.mlc`](../misc/examples/https_get_demo.mlc).
The key is read from the environment and sent as `Authorization`. Status and body length go to stdout. The log line does not contain the key.

```mlc
const key = get_or("HTTPS_DEMO_KEY", "")
println("status=" + ok_status(result).to_string() + " body_bytes=" + ok_body(result).length().to_string())
info("https get ok")
```

### Limitations (from STDLIB_BACKEND §1)

MLC-reachable, blocking, libcurl, no streaming. GET and POST only. Redirects
are off. Proxy environment variables are ignored. Empty `ca_bundle_path` uses
libcurl's default CA store. The failure message is curl's error text and does
not include request headers. `lib/mlc/common/stdlib/net/http.mlc` stays a stub.

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
manual `parse_json` calls. `JsonNumber` is `f64`. Integer fields accept only
finite integral values inside the type range; `i64`, `u64`, and `usize` are
exact only through ±2^53 (9007199254740992). A fraction, NaN, or out-of-range
number is `TypeMismatch` with expected type `integer`. `json.mlc` describes
`JsonValue` as an MLC sum and objects as `Map<str, JsonValue>`; the C++ runtime
is a separate `std::variant`, and objects are `nlohmann::json`. `parse_json` of
text that is not JSON returns `JsonNull`, the same value as JSON `null`.
A non-generic sum with two or more variants that contains itself, or two such
sums that contain each other, is a C++ struct `{ std::variant<...> _; }` with
a converting constructor per variant. A by-value field whose type is one of
those sums is stored as `std::shared_ptr`. The MLC type stays the sum.
`Leaf` lowers to `Tree(Leaf{})`, and `Node(left, right)` lowers to
`Tree(Node{std::make_shared<Tree>(left), std::make_shared<Tree>(right)})`.
`derive { Json }` round-trips that shape. A field the program already wrote as
`Shared<T>` does not make the sum cyclic. It stays `std::shared_ptr` and is
still rejected by `derive { Json }` (E069). A generic sum and a single-variant
sum stay unsupported. `Option` of a cyclic sum is
`std::optional<std::shared_ptr<Sum>>`: `Some(value)` is `std::make_shared`,
and `None` is an empty optional. An array of a cyclic sum is
`mlc::Array<std::shared_ptr<Sum>>`. `derive { Display, Eq, Ord, Hash, Json }`
of those fields reads the child sum: an `Option` through `*(*field)`, an array
element through `*element`. `derive { Hash }` allows those fields when the
enclosing type is that cyclic sum. `match` visits `subject._`. A
pattern binding of a boxed child is the sum: the binding reads `*field`.
A `Some(child)` match on `Option` of a cyclic sum binds `child` as the sum
(`*(*optional)`). A nested `Node(Some(child))` binds `child` as the sum
(`*(*field)`), and `Node(None)` is the empty optional. `Node(Some(Leaf))`
requires that child to be `Leaf` (`holds_alternative<Leaf>` on
`(*(*field))._`). A constructor with fields inside that `Some` is checked the
same way: `Node(Some(Pair(left, _)))` binds `left` as the sum (`*field`), and
`Node(Some(Node(Some(child))))` binds `child` as the sum. `Kids([first])`
binds `first` as the sum (`*element`) when the array length is 1, and
`Kids([])` is the empty array. `Kids([first, ...rest])` binds `first` as the
sum and `rest` as the remaining `[Tree]` (`mlc::Array<std::shared_ptr<Tree>>`
from `cbegin() + 1`). Indexing that tail reads the sum. `Kids([Leaf])`
requires that element to be `Leaf` (`holds_alternative<Leaf>` on
`(*element)._`). `Kids([Node(left, _)])` binds `left` as the sum (`*field`).
`Kids([Node(Node(left, _), _)])` binds that inner `left` as the sum. Another
constructor around it, `Kids([Node(Node(Node(left, _), _), _)])`, binds `left`
the same way. `Kids([Node(Node(left, _), Node(right, _))])` binds both `left`
and `right` as the sum (`*field`). `Kids([Node(left, _), Node(right, _)])`
binds both `left` and `right` as the sum (`*field`).
`Kids([Node(Node(left, _), _), Node(right, _)])` binds both `left` and `right`
as the sum (`*field`). `Kids([Node(left, _), ...rest])` binds `left` as the
sum and `rest` as the remaining `[Tree]`. Indexing that tail reads the sum.
A nested constructor in that prefix, `Kids([Node(Node(left, _), _), ...rest])`,
binds `left` the same way. `Kids([Node(Node(left, _), _), Node(Node(right, _), _)])`
binds both `left` and `right` as the sum (`*field`).
`Kids([Node(Node(left, _), Node(mid, _)), Node(right, _)])` binds `left`,
`mid`, and `right` as the sum (`*field`). `Node(Kids([child]), _)` binds
`child` as the sum (`*element`). `Node(Kids([child, ...rest]), _)` binds
`child` as the sum and `rest` as the remaining `[Tree]`.
`Node(Kids([child]), Node(right, _))` binds both as the sum.
`Kids([Node(Kids([child]), _)])` binds `child` as the sum (`*element`).
`Kids([Node(Kids([child, ...rest]), _)])` binds `child` as the sum and
`rest` as the remaining `[Tree]`. `Kids([Node(Kids([Node(left, _)]), _)])`
binds `left` as the sum (`*field`). `Kids([Node(Kids([Node(Node(left, _), _)]), _)])`
binds that inner `left` as the sum.
`Kids([Node(Kids([Node(Node(left, _), Node(right, _))]), _)])` binds both
`left` and `right` as the sum (`*field`).
`Kids([Node(Kids([Node(Node(left, _), _), Node(Node(right, _), _)]), _)])`
binds both `left` and `right` as the sum (`*field`).
`Kids([Node(Kids([Node(Node(Node(left, _), _), Node(right, _))]), _)])`
binds both `left` and `right` as the sum (`*field`).
`Kids([Node(Kids([Node(Node(left, _), _), ...rest]), _)])` binds `left` as
the sum and `rest` as the remaining `[Tree]`.
`Kids([Node(Kids([Node(Kids([left]), _), Node(Kids([right]), _)]), _)])`
binds both `left` and `right` as the sum (`*element`).
`Kids([Node(Kids([Node(Kids([left]), _), ...rest]), _)])` binds `left` as
the sum and `rest` as the remaining `[Tree]`.
`Kids([Node(Kids([left, Node(Node(child, _), _)]), _)])` binds both `left`
and `child` as the sum (`*element` and `*field`).
`Kids([Node(Kids([left]), Kids([right]))])` binds both `left` and `right`
as the sum (`*element`).
`Kids([Node(Kids([left]), Kids([Node(child, _)])), ...rest])` binds `left`
and `child` as the sum and `rest` as the remaining `[Tree]`.
`Kids([Node(Kids([Node(Leaf, child)]), _)])` binds `child` as the sum
(`*field`). Indexing the tail of
`Kids([Node(Kids([Node(Kids([left]), _), ...rest]), _)])` reads the sum
(`*rest[0]`). An array element of a cyclic sum is the sum.
`derive { Display }` prints that child with `Type_to_string(*field)`.
`derive { Eq }` and `derive { Ord }` compare the child sums, so two separately
allocated values of the same shape compare equal. `derive { Hash }` hashes
that child through `Type_hash(*field)`. Any other field still allows only
`i32`, `bool`, and `string`.

## Yaml

Module: [`lib/mlc/common/stdlib/data/yaml.mlc`](../lib/mlc/common/stdlib/data/yaml.mlc)
(`import … from "Yaml"`). Runtime: `mlc/yaml/yaml.hpp`, linked with `-lyaml`
(libyaml 0.2.5). The header is not part of `mlc.hpp`.

| Name | Signature | Description |
|------|-----------|-------------|
| `YamlValue` | `YamlNull \| YamlBool \| YamlNumber(f64) \| YamlString \| YamlArray \| YamlObject` | One YAML document |
| `parse_yaml` | `(text: str) -> YamlValue` | Parse one document |
| `stringify_yaml` | `(value: YamlValue) -> str` | Emit one document |
| `yaml_get` / `yaml_keys` | object helpers | First matching key, then the key list |

### Limitations

`parse_yaml` reads one YAML 1.1 document through libyaml. Plain scalars use the
1.1 words: `yes`/`no`/`on`/`off`/`y`/`n` are bools, and an empty plain scalar
is null. Quoted text stays a string, including `"true"`. Numbers are `f64`.
A second document, a tag outside `tag:yaml.org,2002:{str,int,float,bool,null,seq,map}`,
an unknown alias, or an alias into a node that is still open fails. The failure
text is `multiple documents`, `tag`, `unknown alias`, `cyclic alias`, `scalar`,
`key`, or the libyaml syntax text. Merge keys stay ordinary keys named `<<`.
Hex and octal plain tokens stay strings. `yaml.mlc` describes `YamlValue` as an
MLC sum and objects as `Map<str, YamlValue>`; the C++ runtime is a separate
`std::variant`, and objects are an ordered list of pairs. `derive { Yaml }` is
not implemented.
