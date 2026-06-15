# Regression suite (frozen baselines)

Ruby bootstrap (`bin/mlc`) is the source of truth for expected stdout.

## Layout

| Path | Purpose |
|------|---------|
| `programs/*.mlc` | Runnable programs |
| `expected/*.txt` | Frozen stdout (one line per `println`, trailing newline) |
| `manifest.tsv` | `program_name<TAB>expected_file` |

## Coverage

| Program | Topic | Origin |
|---------|-------|--------|
| hello | println, main | `compiler/tests/e2e/hello.mlc` |
| fibonacci | recursion, if | e2e |
| result | Result, `?`, match | e2e |
| sum_types | sum types, match | e2e |
| match_guard | match guard | e2e |
| record_update | record, spread, extend | e2e |
| literals | int, string, bool | derived from `test/mlc/` patterns |
| conditionals | if/else | derived |
| while_sum | while, mut | `test/mlc/while_loop_test.rb` |
| for_loop | for-in | `test/mlc/control_flow_statement_test.rb` |
| method_call | extend method | `test/mlc/instance_method_dispatch_test.rb` |
| array_ops | array, push, index | `test/mlc/array_*` |
| char_classify | string compare | `test/integration/self_hosted_e2e_test.rb` |
| lexer_scan | while, char_at | self_hosted_e2e (lexer scan) |

Self-host round-trip: covered by `compiler/main.mlc` in the standard self-host diff gate; `lexer_scan` mirrors compiler lexer patterns.

## Refresh baselines (Ruby only)

```bash
scripts/capture_regression_baselines.sh
```

## Gate (Ruby vs mlcc vs frozen expected)

```bash
scripts/regression_gate.sh
```
