# Track: Stdlib module reference

Parent: [../PLAN.md](../PLAN.md) §28, [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md),
[TRACK_LANG_DOCS](../archive/tracks/TRACK_LANG_DOCS.md).
Trigger: 2026-07-11 backlog review. Every stdlib module (`Tcp`, `HttpServer`,
`WebSocket`, `Postgres`, `Crypto`, `Log`, `Env`, `Validate`, `Json`) has a
working demo under `misc/examples/` (Json: no misc demo — see STEP=5) but no
one-page API reference — discoverability relies on reading the `.mlc` source
directly.
Predecessor closed (Critic OK 2026-07-12):
[TRACK_LANG_DOCS](../archive/tracks/TRACK_LANG_DOCS.md).

## Status: **active** (Planner 2026-07-12) — очередь §28

## Next step

**STEP=1** — create `docs/STDLIB_REFERENCE.md` skeleton + per-module section stubs.

## Goal

`docs/STDLIB_REFERENCE.md` — one section per module: exported functions/types
with signatures, one short usage snippet per module lifted from its existing
demo (not invented), known limitations (blocking-only, no TLS, etc. — copy
accurately from `STDLIB_BACKEND.md` §1, do not re-derive).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Skeleton + module list; one stub section per module (table placeholder: `fn`/`type` name, signature, one-line description) | pending |
| 2 | `Tcp` + `HttpServer` — snippets from `misc/examples/tcp_echo_demo.mlc` + `misc/examples/http_server_forever_demo.mlc` (Tcp+spawn: `tcp_spawn_echo_mlcc.mlc` optional cross-cite) | pending |
| 3 | `WebSocket` — snippet from `misc/examples/websocket_echo_demo.mlc` (filename verified 2026-07-12) | pending |
| 4 | `Postgres` + `Crypto` — `misc/examples/postgres_select_demo.mlc` + `misc/examples/crypto_sha256_demo.mlc` | pending |
| 5 | `Log` + `Env` + `Validate` + `Json` — Log/Env: `misc/examples/env_log_demo.mlc`; Validate: `misc/examples/validate_demo.mlc`; Json: **no** `misc/examples/*json*` — API table from `lib/mlc/common/stdlib/data/json.mlc`, snippet from program string in `test/mlc/derive_json_test.rb` (cite path; do not invent) | pending |
| 6 | Cross-link from `STDLIB_BACKEND.md` §1 table (replace inline snippets with links to this doc where it gets long) | pending |
| 7 | Verify: every snippet appears verbatim (or near-verbatim, noted if trimmed) in the attributed file — grep-check, not visual-only | pending |

### STEP=1 sub-steps (Driver)

1. Create `docs/STDLIB_REFERENCE.md` with title, one-paragraph scope (module API
   reference; snippets only from cited demos/fixtures; limitations from
   `STDLIB_BACKEND.md` §1), parent links to PLAN §28 + STDLIB_BACKEND + this TRACK.
2. TOC + stub `##` headings only (no filled API tables yet) for: Tcp; HttpServer;
   WebSocket; Postgres; Crypto; Log; Env; Validate; Json. Under each: `Status:
   pending — filled in STEP=N` and the pinned Source path from the Steps table.
3. Do **not** invent example code in STEP=1; do **not** edit STDLIB_BACKEND /
   README (STEP=6). Limitations prose waits for filled sections.
4. Verify: file exists; TOC anchors match headings; TRACK Status **active**,
   Next=STEP=2 after mark STEP=1 done.

## Progress

- **Planner** (2026-07-12): activated after LANG_DOCS Critic OK; demo paths
  pinned; Json exception documented; STEP=1 next.

## Out of scope

- Full language reference — see [TRACK_LANG_DOCS](../archive/tracks/TRACK_LANG_DOCS.md).
- Generated/automated doc extraction — manual curation only, same as that track.
- JobQueue / Supervisor MLC modules (C++-only; no MLC surface to document here).
