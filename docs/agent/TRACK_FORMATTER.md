# Track: Formatter (`mlc fmt`)

Parent: [../PLAN.md](../PLAN.md) §Phase 3 п.1; follows [TRACK_CODE_QUALITY.md](TRACK_CODE_QUALITY.md)

## Status: **closed** (STEP=5 done)

**Depends on:** TRACK_PARSE_PROGRAM_RESULT closed; TRACK_CODE_QUALITY closed (`36a6e8cc`).

**Baseline:** build_tests **1030/0**; **diff_exit=0**.

**Goal:** deterministic `.mlc` formatting from AST; foundation for LSP.

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | AST printer module — `compiler/fmt/ast_printer.mlc`; round-trip smoke on literals/idents | done |
| 2 | Indent rules — 2-space `do`/`end`, fn bodies, nested blocks | done |
| 3 | Record alignment — field lists, multiline records | done |
| 4 | Tests — `test_formatter.mlc`; idempotent `fmt(fmt(source)) == fmt(source)` | done |
| 5 | CLI integrate — `mlcc fmt` or `mlc fmt` subcommand; close track | done |

### Out of scope

- Full style config / `.mlc-fmt.toml`
- LSP format-on-save (TRACK_LSP)
