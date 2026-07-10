# Track: `mlcc --run` — stdin support + crash fix (as in other interpreters)

Parent: [../PLAN.md](../PLAN.md).

Trigger: `mlcc --run <file>` works (verified manually, 2026-07-10), but there
is no stdin convention like `python -`/`node -`/`ruby < file` — and piping
into it crashes instead of erroring cleanly. Root cause and exact fix
locations already found by manual investigation (2026-07-10, reverted
before commit per user instruction "не делай сам, добавь в планы") — this
track exists so the next Driver turn does not have to re-discover them.

Приоритет: **высокий** — маленький, изолированный, полностью
самодостаточный (не зависит от MIR_VM_FULL/Epic 4, не трогает `compiler/vm/`).
Можно брать сразу, не дожидаясь HARD STOP GATE.

## Status: **open** — STEP=1 **done**; STEP=2 next

**Driver 2026-07-10:** STEP=1 — `file.hpp` streambuf read (no seekg/tellg).

## Root cause (found, fixed in STEP=1)

`compiler/out/mlcc --run /dev/stdin` crashes:

```
terminate called after throwing an instance of 'std::length_error'
  what():  basic_string::_M_create
Aborted (core dumped)
```

`runtime/include/mlc/io/file.hpp`:
- `read_to_string(path)` (free fn, ~line 166) and `File::read_all()` (class
  method, ~line 91) both do `seekg(0, end); size_t size = tellg(); seekg(0,
  beg); std::string content(size, '\0'); read(&content[0], size)`. For a
  non-seekable stream (pipe, `/dev/stdin`), `tellg()` returns `-1`, cast to
  `size_t` becomes a huge number → `std::string(huge, '\0')` throws
  `std::length_error`. This is the exact call path `compiler/driver/module_loader.mlc:42`
  (`File.read(path)`) hits when `path` is a pipe.

**Fix:** replace the seekg/tellg size-probe with a streambuf-based read
(`std::ostringstream oss; oss << stream.rdbuf();`) — works for regular files
and non-seekable streams identically. No behavior change for the normal
(regular file) case; this is a pure correctness fix, not new functionality.

## stdin convention (missing entirely)

`mlcc --run <source.mlc>` requires a real file path (`Usage:` in
`compiler/compile_options.mlc:18` confirms `<source.mlc>` positional, no `-`
handling). No REPL, no `-e`, no piping convention (`python -`, `cat x |
node`, `ruby -` all supported by their respective interpreters — MLC has
none of this).

**Already available and NOT wired into self-hosted `compiler/`:**
`mlc::io::read_all()` exists in `runtime/src/io/io.cpp:44` (reads all of
stdin via `oss << std::cin.rdbuf()` — same safe pattern as the fix above,
already correct) and is **already exposed as a global builtin in the Ruby
bootstrap** (`lib/mlc/backends/cpp/rules/expressions/call_rule.rb:18-19`:
`"read_all" => "mlc::io::read_all"`, alias `"input"`). The self-hosted
`compiler/` checker/codegen simply never registered it — this is a gap
specific to `compiler/`, not a missing runtime feature.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Fix `read_to_string`/`File::read_all()` crash in `runtime/include/mlc/io/file.hpp` (see Root cause above) — streambuf read instead of seekg/tellg. Regression: existing `File.read`/`.read_all()` tests on regular files must still pass byte-for-byte. | **done** (2026-07-10: streambuf; header smoke + file_io_e2e 8/0; `mlcc --run /dev/stdin` exit=7 no length_error) |
| 2 | Register `read_all` as a self-hosted global builtin: `compiler/checker/check/check.mlc` (add to the `names.set(...)` global whitelist near `read_line`), `compiler/checker/registry.mlc` (`builtin_function_types.set('read_all', TFn([], TString))` + `builtin_required_arity.set('read_all', 0)`), `compiler/codegen/cpp_naming.mlc` (`map_builtin`: `"read_all" -> "mlc::io::read_all"`). Mirror the existing `read_line` pattern exactly (same 3 files, same shape). | pending |
| 3 | `compiler/driver/module_loader.mlc:42` — when the entry/import path is exactly `"-"`, call `read_all()` instead of `File.read(path)`, and skip the `File.exists(path)` not-found check for that case (a nonexistent file literally named `-` must not shadow the stdin convention). Convention: `mlcc --run -` reads the program from stdin, matching `python -`/`ruby -`. | pending |
| 4 | Smoke test: `echo '...' \| compiler/out/mlcc --run -` runs correctly; `compiler/out/mlcc --run /dev/stdin` no longer crashes (graceful behavior, same as `-`, or a clear "use -" error — decide at implementation time, do not leave a crash either way). Add to `compiler/tests/` (or `misc/examples/` smoke script per project convention) so it is not silently lost. | pending |
| 5 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`. Update `compiler/compile_options.mlc:18` usage string to mention `-` for stdin. | pending |

## Out of scope

- REPL / `-e 'code'` inline-eval — separate feature, not requested.
- `File.read(path)` accepting arbitrary named pipes as a general feature —
  this track only guarantees no crash + the `-` convention for the CLI
  entry point, not full pipe-transparency everywhere `File.read` is called.

## Per-turn template

```
| step | <1-5> |
| done | <one line> |
| verify | <числа/факт> |
| next | ROLE=Driver STEP=<n+1> TRACK_CLI_STDIN |
```
