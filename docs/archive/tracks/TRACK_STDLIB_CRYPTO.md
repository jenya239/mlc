# Track: Crypto stdlib (libsodium)

Parent: [../../PLAN.md](../../PLAN.md), [../../STDLIB_BACKEND.md](../../STDLIB_BACKEND.md) §5.3,
[../../FFI_LAYER.md](../../FFI_LAYER.md).
Trigger: Postgres **closed**; auth/backend needs hash/HMAC/password primitives.
STDLIB_BACKEND prefers **libsodium** over OpenSSL (simpler C API, less global
state).

## Status: **closed** (2026-07-10) — STEP=1–5 **done**

**Driver 2026-07-10:** STEP=5 — `MLC.md` Crypto note + pipeline matrix;
`misc/examples/crypto_sha256_demo.mlc`; `STDLIB_BACKEND` closed; gate re-run
OK; no `compiler/**` (regression_gate N/A); track archived.

## Decision (STEP=1, 2026-07-10)

### Library

- **libsodium only** (not OpenSSL). Init once via `sodium_init` in runtime
  (lazy on first call).
- Link: `-lsodium` for consumers/tests. Ruby pipeline has no `extern lib`
  (self-hosted only) — gate/scripts pass `-lsodium` explicitly (same class of
  residual as Postgres `-lpq`).

### Pipeline

- MLC: `lib/mlc/common/stdlib/crypto/crypto.mlc`, module `Crypto`.
- Runtime: `runtime/include/mlc/crypto/sodium.hpp` under `mlc::crypto`.
- Soft bridge `postgres_bridge`-style so core codegen does not require sodium
  headers when unused.

### Methods (v1)

```
Crypto.sha256(data: string) -> string
Crypto.hmac_sha256(key: string, data: string) -> string
Crypto.random_bytes(count: i32) -> Option<string>
Crypto.pwhash(password: string) -> Option<string>
Crypto.pwhash_verify(hashed: string, password: string) -> bool
Crypto.last_error() -> string
```

| Method | Semantics |
|--------|-----------|
| `sha256` / `hmac_sha256` | Always succeed for valid inputs; return **lowercase hex** `string` (32→64 / 32→64 chars). Empty input allowed. |
| `random_bytes` | `count` in 1..1024; return lowercase hex (`2*count` chars) or `None`+`last_error`. |
| `pwhash` | `crypto_pwhash_str` (ops/mem moderate interactive); encoded string or `None`. |
| `pwhash_verify` | `crypto_pwhash_str_verify`; `false` on mismatch/error (set `last_error` on lib error). |

### Error model

- Fallible → `Option` / `bool` + `last_error()` (Postgres/Tcp shipped pattern).
- **No** Decision-only `Result` for MLC v1.
- No panic on crypto failure.

### libsodium calls in v1

| Call | Use |
|------|-----|
| `sodium_init` | once |
| `crypto_hash_sha256` | sha256 |
| `crypto_auth_hmacsha256` | hmac_sha256 |
| `randombytes_buf` | random_bytes |
| `crypto_pwhash_str` / `crypto_pwhash_str_verify` | password |

### Build / CI

- Core `mlcc`/runtime build: **no** hard `-lsodium`.
- Smoke/gate: skip with clear message if `sodium.h` / lib missing.
- Known-answer vectors in gate (empty-string SHA-256, RFC HMAC sample).

### Non-goals (locked)

JWT, TLS, OpenSSL, raw binary returns, `crypto_pwhash` low-level (non-`_str`),
WebSocket.

### Open questions — resolved

1. **Option vs Result?** Option/`bool`+`last_error` (shipped pattern).
2. **Password API?** `_str` / `_str_verify` only.
3. **Hex vs bytes?** Lowercase hex `string` for all digests/random in v1.

## Goal

Minimal crypto surface for backend auth building blocks: SHA-256, HMAC-SHA256,
secure random bytes, password hash/verify (`crypto_pwhash`). Validate
`-lsodium` link path. Not a full JWT/TLS stack.

## Constraints (STDLIB_BACKEND §3)

- Thin C++ runtime + `std/crypto/*.mlc` (Postgres/Tcp pattern).
- Errors → `Option`/`bool`+`last_error` (Decision); no panic on crypto API failure.
- Default core build must **not** hard-require libsodium; consumers/tests link
  `-lsodium`. Skip smoke clearly if headers/libs missing.
- No panic on crypto API failure.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: public API (`Crypto.sha256` / `hmac_sha256` / `random_bytes` / `pwhash`+`pwhash_verify`); error model; libsodium vs OpenSSL (prefer sodium); link `-lsodium`. Document in «Decision». | **done** (2026-07-10: see Decision) |
| 2 | Runtime: `runtime/include/mlc/crypto/sodium.hpp` — wrappers; C++ smoke compile+link `-lsodium`. | **done** (2026-07-10: `mlc::crypto::*`; smoke 9/0) |
| 3 | Stdlib: `lib/mlc/common/stdlib/crypto/crypto.mlc` + registry/scanner; codegen bridge include; Ruby codegen smoke. | **done** (2026-07-10: `crypto_stdlib_test` 1/14) |
| 4 | Gate: `scripts/run_crypto_gate.sh` — link smoke + stdlib test; known-answer vectors (SHA-256 / HMAC). | **done** (2026-07-10: gate OK; smoke 9/0 + codegen 1/14) |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md` note) + example; close (regression_gate only if `compiler/**` touched). | **done** (2026-07-10: MLC.md + example; no compiler/**) |

## Out of scope (this track)

- JWT encode/verify (follow-up TRACK after primitives exist)
- TLS / HTTPS server
- OpenSSL dual-backend
- WebSocket, job-queue, env/logging

## Open questions

None for v1 — resolved in Decision above.
