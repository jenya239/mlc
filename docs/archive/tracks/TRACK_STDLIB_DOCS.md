# Track: Stdlib module reference

Parent: [../../PLAN.md](../../PLAN.md) §28, [../../STDLIB_BACKEND.md](../../STDLIB_BACKEND.md),
[TRACK_LANG_DOCS](TRACK_LANG_DOCS.md).
Trigger: 2026-07-11 backlog review. Every stdlib module (`Tcp`, `HttpServer`,
`WebSocket`, `Postgres`, `Crypto`, `Log`, `Env`, `Validate`, `Json`) has a
working demo under `misc/examples/` (Json: no misc demo — see STEP=5) but no
one-page API reference — discoverability relies on reading the `.mlc` source
directly.
Predecessor closed (Critic OK 2026-07-12):
[TRACK_LANG_DOCS](TRACK_LANG_DOCS.md).

## Status: **closed** (2026-07-12) — awaiting Critic

**Planner** (2026-07-12): activated after LANG_DOCS Critic OK; demo paths pinned.
**Driver** (2026-07-12): STEP=1–7 done — `docs/STDLIB_REFERENCE.md` filled;
`STDLIB_BACKEND.md` §1 cross-links; snippet grep-verify 10/0. Docs-only;
regression_gate N/A.

## Next step

— (closed; `ROLE=Critic STEP=critique-audit` then Planner → next PLAN queue item)

## Goal

`docs/STDLIB_REFERENCE.md` — one section per module: exported functions/types
with signatures, one short usage snippet per module lifted from its existing
demo (not invented), known limitations (blocking-only, no TLS, etc. — copy
accurately from `STDLIB_BACKEND.md` §1, do not re-derive).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Skeleton + module list; one stub section per module (table placeholder: `fn`/`type` name, signature, one-line description) | **done** (2026-07-12) — `docs/STDLIB_REFERENCE.md` TOC + 9 stubs |
| 2 | `Tcp` + `HttpServer` — snippets from `misc/examples/tcp_echo_demo.mlc` + `misc/examples/http_server_forever_demo.mlc` (Tcp+spawn: `tcp_spawn_echo_mlcc.mlc` optional cross-cite) | **done** (2026-07-12) — API tables + demos; limitations from STDLIB_BACKEND §1 |
| 3 | `WebSocket` — snippet from `misc/examples/websocket_echo_demo.mlc` (filename verified 2026-07-12) | **done** (2026-07-12) — API table + upgrade/echo excerpt |
| 4 | `Postgres` + `Crypto` — `misc/examples/postgres_select_demo.mlc` + `misc/examples/crypto_sha256_demo.mlc` | **done** (2026-07-12) — API tables + demo excerpts |
| 5 | `Log` + `Env` + `Validate` + `Json` — Log/Env: `misc/examples/env_log_demo.mlc`; Validate: `misc/examples/validate_demo.mlc`; Json: **no** `misc/examples/*json*` — API table from `lib/mlc/common/stdlib/data/json.mlc`, snippet from program string in `test/mlc/derive_json_test.rb` (cite path; do not invent) | **done** (2026-07-12) — four sections filled |
| 6 | Cross-link from `STDLIB_BACKEND.md` §1 table (replace inline snippets with links to this doc where it gets long) | **done** (2026-07-12) — §1 intro + per-module API links; HTTP row condensed |
| 7 | Verify: every snippet appears verbatim (or near-verbatim, noted if trimmed) in the attributed file — grep-check, not visual-only | **done** (2026-07-12) — 10 fences / 0 missing lines; fixed HttpServer Ok-arm excerpt |

## Progress

- **Planner** (2026-07-12): activated after LANG_DOCS Critic OK; demo paths
  pinned; Json exception documented; STEP=1 next.
- **STEP=1–6** (2026-07-12): reference filled + STDLIB_BACKEND cross-links.
- **STEP=7** (2026-07-12): snippet grep-verify 10/0; track closed.

## Out of scope

- Full language reference — see [TRACK_LANG_DOCS](TRACK_LANG_DOCS.md).
- Generated/automated doc extraction — manual curation only, same as that track.
- JobQueue / Supervisor MLC modules (C++-only; no MLC surface to document here).
