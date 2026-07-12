# Track: Stdlib module reference

Parent: [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md), [TRACK_LANG_DOCS](../archive/tracks/TRACK_LANG_DOCS.md).
Trigger: 2026-07-11 backlog review. Every stdlib module (`Tcp`, `HttpServer`,
`WebSocket`, `Postgres`, `Crypto`, `Log`, `Env`, `Validate`, `Json`) has a
working demo under `misc/examples/` but no one-page API reference —
discoverability relies on reading the `.mlc` source directly.

## Status: open, средний приоритет

## Goal

`docs/STDLIB_REFERENCE.md` — one section per module: exported functions/types
with signatures, one short usage snippet per module lifted from its existing
demo (not invented), known limitations (blocking-only, no TLS, etc. — copy
accurately from `STDLIB_BACKEND.md` §1, do not re-derive).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Skeleton + module list (see trigger); one section per module, table format: `fn`/`type` name, signature, one-line description | pending |
| 2 | `Tcp` + `HttpServer` sections — snippet from `misc/examples/http_server_forever_demo.mlc` (or successor) | pending |
| 3 | `WebSocket` section — snippet from `misc/examples/websocket_echo_demo.mlc` (verify exact filename first) | pending |
| 4 | `Postgres` + `Crypto` sections | pending |
| 5 | `Log` + `Env` + `Validate` + `Json` sections | pending |
| 6 | Cross-link from `STDLIB_BACKEND.md` §1 table (replace inline snippets with links to this doc where it gets long) | pending |
| 7 | Verify: every snippet actually appears verbatim (or near-verbatim, noted if trimmed) in the demo file it's attributed to — grep-check, not just visual review | pending |

## Out of scope

- Full language reference — see [TRACK_LANG_DOCS](../archive/tracks/TRACK_LANG_DOCS.md).
- Generated/automated doc extraction — manual curation only, same as that track.
