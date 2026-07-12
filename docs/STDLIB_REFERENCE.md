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

Status: pending — filled in STEP=2.

Pinned source: [`misc/examples/tcp_echo_demo.mlc`](../misc/examples/tcp_echo_demo.mlc)
(optional cross-cite: [`tcp_spawn_echo_mlcc.mlc`](../misc/examples/tcp_spawn_echo_mlcc.mlc)).

Planned: API table (`fn`/`type`, signature, one-line description); snippet from
pinned demo; limitations from STDLIB_BACKEND §1 (blocking TCP; no TLS).

## HttpServer

Status: pending — filled in STEP=2.

Pinned source: [`misc/examples/http_server_forever_demo.mlc`](../misc/examples/http_server_forever_demo.mlc).

Planned: API table; snippet from forever demo; limitations from STDLIB_BACKEND §1
(keep-alive / limits / no TLS/HTTP/2; shutdown story for forever vs bounded).

## WebSocket

Status: pending — filled in STEP=3.

Pinned source: [`misc/examples/websocket_echo_demo.mlc`](../misc/examples/websocket_echo_demo.mlc).

Planned: API table; snippet from echo demo; limitations from STDLIB_BACKEND §1.

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
