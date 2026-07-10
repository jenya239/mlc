# Track: Crypto stdlib (libsodium)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.3,
[../FFI_LAYER.md](../FFI_LAYER.md).
Trigger: Postgres **closed**; auth/backend needs hash/HMAC/password primitives.
STDLIB_BACKEND prefers **libsodium** over OpenSSL (simpler C API, less global
state).

## Status: **open** — STEP=1 next (API Decision)

**Planner 2026-07-10:** opened after closed STDLIB_POSTGRES Critic. Pipeline
(Decision C / Tcp+Postgres pattern): MLC via Ruby `common/stdlib`; runtime
C++ under `mlc::crypto`. No language `spawn` for v1.

## Goal

Minimal crypto surface for backend auth building blocks: SHA-256, HMAC-SHA256,
secure random bytes, password hash/verify (`crypto_pwhash`). Validate
`-lsodium` link path. Not a full JWT/TLS stack.

## Constraints (STDLIB_BACKEND §3)

- Thin C++ runtime + `std/crypto/*.mlc` (Postgres/Tcp pattern).
- Errors → `Option`+`last_error` (shipped pattern) unless Decision picks
  otherwise — **lock in STEP=1**; avoid Decision/ship split.
- Default core build must **not** hard-require libsodium; consumers/tests link
  `-lsodium`. Skip smoke clearly if headers/libs missing.
- No panic on crypto API failure.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: public API (`Crypto.sha256` / `hmac_sha256` / `random_bytes` / `pwhash`+`pwhash_verify`); error model; libsodium vs OpenSSL (prefer sodium); link `-lsodium`. Document in «Decision». | **pending** |
| 2 | Runtime: `runtime/include/mlc/crypto/sodium.hpp` (or `hash.hpp`) — wrappers; C++ smoke compile+link `-lsodium`. | pending |
| 3 | Stdlib: `lib/mlc/common/stdlib/crypto/crypto.mlc` + registry/scanner; codegen bridge include; Ruby codegen smoke. | pending |
| 4 | Gate: `scripts/run_crypto_gate.sh` — link smoke + stdlib test; known-answer vectors (SHA-256 / HMAC). | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md` note) + example; close (regression_gate only if `compiler/**` touched). | pending |

<!-- sub-steps STEP=1: 1) API table; 2) Option+last_error vs Result; 3) v1 vs JWT/argon2-only/TLS out -->
<!-- sub-steps STEP=2: 1) header wrap sodium; 2) test_crypto.cpp; 3) soft include bridge -->
<!-- sub-steps STEP=3: 1) crypto.mlc; 2) scanner namespace mlc::crypto; 3) codegen test -->
<!-- sub-steps STEP=4: 1) gate script; 2) NIST/RFC vectors; 3) SESSION verify -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic next -->

## Out of scope (this track)

- JWT encode/verify (follow-up TRACK after primitives exist)
- TLS / HTTPS server
- OpenSSL dual-backend
- WebSocket, job-queue, env/logging

## Open questions (resolve in STEP=1)

1. Option+`last_error` (Postgres/Tcp) vs `Result<string, string>` for hash outputs?
2. Password API: `crypto_pwhash_str` (encoded string) only, or raw ops too?
3. Hex vs raw bytes for hash return in MLC v1 (`string` hex for simplicity)?
