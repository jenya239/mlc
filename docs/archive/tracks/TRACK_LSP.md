# Track: LSP server (`mlc lsp`)

Parent: [../PLAN.md](../PLAN.md) §Phase 3 п.2; follows [TRACK_SELF_HOST_BOOTSTRAP.md](TRACK_SELF_HOST_BOOTSTRAP.md)

## Status: **closed** (STEP=1-5 done)

**Depends on:** TRACK_SELF_HOST_BOOTSTRAP closed; TRACK_FORMATTER done.

**Baseline:** `build_tests` **1234/0**; `build_bin` mlcc2 ok; `mlcc2 --check-only compiler/main.mlc` ok; `run_lsp_smoke.sh` ok.

**Goal:** minimal Language Server: go-to-definition, hover types, publish diagnostics.

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 --check-only compiler/main.mlc
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | LSP skeleton — `compiler/lsp/server.mlc`; stdio JSON-RPC loop; initialize/shutdown | done |
| 2 | Go-to-definition — resolve ident; single-file scope | done |
| 2a | `symbols.mlc` index + `resolve_definition_in_source`; `test_lsp_server.mlc` | done |
| 2b | `lsp_protocol.mlc` string helpers — no char/string mix | done |
| 2c | `server.mlc` definition handler wired | done |
| 3 | Hover — type at cursor from checker/infer snapshot | done |
| 4 | Diagnostics — map checker errors → LSP `publishDiagnostics` | done |
| 5 | Integration test + `mlcc lsp` CLI; close track | done |

### STEP=5 notes (2026-06-24)

- `lsp_cli.mlc` + `main.mlc` `mlcc lsp` (stdio JSON-RPC).
- `server.mlc`: `lsp_process_messages_for_test` (didOpen, definition, hover, diagnostics).
- `test_lsp_server.mlc`: integration didOpen+definition; `run_lsp_smoke.sh` (initialize/shutdown/exit).
- `build_tests.sh` phase 8/8: lsp smoke. Gate: **1234/0**; track **closed**.

### STEP=4 notes (2026-06-24)

- `check.mlc`: `gather_program_check` + export `program_diagnostics`.
- `diagnostics.mlc`: `collect_diagnostics_in_source` → checker.
- `lsp_protocol.mlc`: `lsp_build_publish_diagnostics_notification`.
- `server.mlc`: `didOpen` → push `textDocument/publishDiagnostics`.
- `test_lsp_server.mlc`: type-mismatch fixture + publish JSON asserts.

### STEP=3 notes (2026-06-22)

- `hover.mlc`: env snapshot via `check_context_new` + `type_environment.set` in same scope (no `ref mut` in `CheckContext`).
- `symbols.mlc`: `ref mut Map` for symbol table mutation (COW).
- Void match wildcard: `_ => do end` (not `_ => ()` — avoids `return std::make_tuple()` in void lambda).
- `lsp_protocol.mlc`: `"\"" + field + "\"" + ":"`; `+ "}"` not `+ '}'` (char).

### Out of scope

- Multi-module workspace / import graph
- Completion / rename / format-on-save (later)
