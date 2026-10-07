# TRACK_STDLIB_SQLITE

Status: implemented 2026-10-06. `compiler/**`, Ruby `lib/mlc/**`, and
`compiler/build_bin.sh` were not edited. sqlite is not in `RT_SRC` or
`TEXT_LIBS`. `-lsqlite3` comes from `extern lib "sqlite3"` via
`mlc_link_libs.txt`.

## Linked library

System SQLite **3.45.1** (`sqlite abi smoke OK version=3.45.1`). The shim
requires `SQLITE_VERSION_NUMBER >= 3007015` (`sqlite3_errstr`). Tests use
`:memory:` or `mkstemp`. A missing `sqlite3.h` makes
`scripts/probe_sqlite_link.sh` and `scripts/run_sqlite_gate.sh` exit **2**
with `install libsqlite3-dev`. Exit 2 is not a green run.

## mlcc entry used by the probes

`fn main() -> i32`. A failing check is `return N`. The last line of `main` is
a bare `0`. An `extern fn` that names a header must say `blocking`; without
it mlcc emits `W-EXTERN-ATTR`. The step-0 fixture's `fn main() -> unit` and
`exit(1)` are not this compiler's entry contract.

A local function in a consumer whose name is `importedName_rest` is emitted
as `sqlite::importedName_rest` (`qualify_function_callee` splits on the first
`_`). Probe helpers are `kind_of_step` and `unit_of_prepare` for that reason.

## Ruby pipeline (not patched)

- `MLC.parse` of `lib/mlc/common/stdlib/db/sqlite.mlc` succeeds. The scanner
  therefore indexes the module.
- `import Sqlite::{libversion_number}` lowers to
  `mlc::sqlite::libversion_number()`. `Scanner#infer_namespace` maps `Postgres`
  to `mlc::db` and every other name to `mlc::<downcase>`. That call is not the
  shim symbol `mlc::db::sqlite_libversion_number_i`.
- `MLC.compile_project` of `misc/probe/sqlite_stdlib_probe.mlc` fails:
  `sqlite.mlc:86: function 'i64_from_i32' result expected i64, got i32`.
  mlcc accepts `fn i64_from_i32(value: i32) -> i64 = value`.
- Bare module name `Sqlite` is not in `misc/scripts/generate_stdlib_registry.rb`.
  mlcc probes import the `.mlc` path.

## Gate

`bash scripts/run_sqlite_gate.sh all` exited 0 on 2026-10-06 (link, stdlib
link, shim, module, statement, transaction, `misc/examples/sqlite_demo.mlc`).
`bundle exec rake test_mlc` exited 0 the same day: 1476 runs, 0 failures,
0 errors, 1 skip. `compiler/**` was not changed, so `scripts/regression_gate.sh`
was not run.
