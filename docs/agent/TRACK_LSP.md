# Track: LSP server (`mlc lsp`)

Parent: [../PLAN.md](../PLAN.md) §Phase 3 п.2; follows [TRACK_SELF_HOST_BOOTSTRAP.md](TRACK_SELF_HOST_BOOTSTRAP.md)

## Status: **planned** (blocked: depends on TRACK_SELF_HOST_BOOTSTRAP)

**Depends on:** TRACK_SELF_HOST_BOOTSTRAP closed; TRACK_FORMATTER done (formatting optional for v1).

**Baseline:** `--check-only` + rustc-style diagnostics.

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
| 1 | LSP skeleton — `compiler/lsp/server.mlc`; stdio JSON-RPC loop; initialize/shutdown | pending |
| 2 | Go-to-definition — resolve ident via checker names; single-file scope first | pending |
| 3 | Hover — type at cursor from checker/infer snapshot | pending |
| 4 | Diagnostics — map checker errors → LSP `publishDiagnostics` | pending |
| 5 | Integration test + `mlcc lsp` CLI; close track | pending |

### Out of scope

- Multi-module workspace / import graph
- Completion / rename / format-on-save (later)
