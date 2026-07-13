# Debugging MLC (source map via `#line`)

Generated C++ carries `#line <N> "<path.mlc>"` before each statement with a
known span (Ruby `lib/mlc/backends/cpp/` and self-hosted `compiler/codegen/`).
Unknown spans (`line <= 0` or empty file) emit nothing.

## Build with DWARF

```bash
compiler/out/mlcc -o /tmp/out program.mlc
MLCC_DEV=1 compiler/build_bin.sh /tmp/out /tmp/program   # -O0 -g
```

`MLCC_DEV=1` selects `-O0 -g`. `#line` is always on when the span is known;
it does not require a separate mlcc flag.

## Panic → `.mlc` in gdb

Division by zero / signed overflow (debug) / `panic` call `mlc::io::panic`.
With `-g`, gdb maps the caller frame to the original `.mlc` file and line:

```bash
gdb -batch -ex 'set pagination off' -ex 'set debuginfod enabled off' \
  -ex 'break mlc::io::panic(mlc::String const&)' -ex 'run' \
  -ex 'bt' -ex 'frame 2' -ex 'info line *$pc' -ex 'quit' \
  --args /tmp/program
```

Expect a frame like `… at program.mlc:3` and `info line` pointing at that
`.mlc` line.

Verified 2026-07-13 (`TRACK_DEBUG_SOURCE_MAP` STEP=4): div0 probe →
`crash.mlc:3`.

## Limits

- Not a source-level debugger (no MLC breakpoints / watches guaranteed).
- MIR VM stacks are out of scope (separate from C++ DWARF).
- IDE breakpoint UI may still resolve to generated `.cpp`; trust backtrace /
  `info line`, not every IDE stop.

Track: [archive/tracks/TRACK_DEBUG_SOURCE_MAP.md](archive/tracks/TRACK_DEBUG_SOURCE_MAP.md).
