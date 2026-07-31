# Track: Editor terminal panel — libvterm-backed, reuses existing render/state

Parent: [../PLAN.md](../PLAN.md) §102. Authorized 2026-07-28 (user request: "можно
добавить треки по терминалу"). Emphasis per user directive: performance,
architecture, testing — every sub-track below carries an explicit gate, no
sub-track is "done" without one.

## Status: **open** — §102a/§102b/§102c all **CLOSED**, Critic-audited. §102d
`TERMINAL_INPUT_FORWARD` Driver done (red+green), Critic next.

## §102d Decision (frozen 2026-07-31)

Pure encoder module `misc/editor/terminal/terminal_input_forward.mlc`:
`terminal_keyboard_forward_poll(last_backspace_down, last_enter_down)` reads
the same GLFW poll surface `misc/gui/input.mlc`/`frame_input.mlc` already use
(`glfw_gl_take_text`, `glfw_gl_key_backspace_down`/`_enter_down`,
`glfw_gl_take_binding_key`, `glfw_gl_mod_ctrl_down`) and returns the raw
bytes to forward + the new backspace/enter edge state; caller does the
actual `pty_terminal_write` (same poll/pure-transform split as
`frame_input_poll`). Arrow keys and Tab reuse the *existing*
`glfw_gl_take_binding_key()` "command-bus chords" channel (already used for
editor nav — same convention, no new input surface); Ctrl+C/D/Z read
`glfw_gl_mod_ctrl_down()` + the same channel's `"c"`/`"d"`/`"z"`. One small
addition to that channel: `glfw_window_gl.cpp` never tracked
`GLFW_KEY_D` (only C/Z were wired, for copy/undo) — added for live-input
symmetry, since terminal Ctrl+D has no substitute. Byte values reuse the
existing `mlc::net::string_from_byte_u8` header inline (TRACK_STDLIB_
WEBSOCKET_TO_MLC) via a fresh `extern fn` — no new C++ binding, MLC string
literals have no hex/control-byte escape syntax.

Byte mapping, **empirically measured** against this codebase's own `forkpty`
default termios (a standalone `pty_abi.cpp` probe, not assumed): `stty -a`
on a freshly spawned PTY reports `erase = ^?` (DEL 0x7f) — sending it twice
into a `cat` session correctly erased 2 preceding characters via the tty's
own echo (`ab<08> <08><08> <08>` raw bytes observed). `icrnl` is set, so
Enter sends CR (`\r`, 0x0d) — the tty translates it to LF for the child.
Ctrl+D (0x04) on an empty line correctly EOFs a `cat` child (0 further
bytes). Ctrl+C (0x03) correctly SIGINTs a `sleep 5` child (child exits
immediately, never reaches a trailing `echo`). Ctrl+Z (SUSP per the same
`stty -a`, 0x1a) is encoded but never live-tested with a real suspend — that
would leave a stopped child process behind, unsafe in an automated smoke
test. Tab forwards `\t` (0x09) raw — `stty -a` shows `tab0`, no
tab-expansion post-processing by the tty. Arrows forward VT100 "normal mode"
CSI sequences (`ESC[A/B/C/D`) via the existing `vterm_control_escape_byte()`
(§102a) — the standard convention the track's own text names, application
cursor-key mode (`ESC O A/B/C/D`) is out of scope (v1, no client requests
it).

Gate (per the track file's own §102d spec, headless — the input-forward
hooks work without an open GLFW window per code reading, `glfw_gl_
take_text`/`_key_*_down`/`_take_binding_key` all check their test-override
state before ever touching `context_window()`; a context is still opened
for convention/consistency with `editor_command_bus_live_smoke.mlc`, same
`glfw_gl_input_test_*`-driven "live smoke" shape): type `"echo hi"` +
simulated Enter into a real interactive `sh` PTY session, assert the
resulting vterm screen has a row whose *trimmed* text is exactly `"hi"`
(distinct from the locally-echoed `"echo hi"` line, which also contains the
substring `"hi"` but does not equal it). New test `misc/examples/
terminal_input_forward_smoke.mlc` + runner `scripts/
run_editor_terminal_input_forward_smoke.sh` (same glfw3-pkg-config-skip /
`MLC_GLFW_VISIBLE=0` shape as `run_editor_terminal_cell_grid_render_smoke.sh`).
5 supplementary scenarios beyond the bare gate: printable text + Backspace
correction (`"echo hix"` + Backspace + Enter → line reads `"hi"`, not
`"hix"`); Ctrl+D forwarding EOFs a real `cat` child; Ctrl+C forwarding
SIGINTs a real `sleep` child before it completes; Tab/arrow byte-level
encoding verified against a real `cat` child (raw byte pass-through, not
interactive line-editing semantics — that is the shell/readline's own
concern, out of scope for a forwarding layer); Ctrl+Z byte value
independently re-derived (separate `extern fn` declaration to the same
header, not compared against itself).

Module-touch: `misc/editor/terminal/terminal_input_forward.mlc` (new),
`misc/examples/terminal_input_forward_smoke.mlc` (new),
`scripts/run_editor_terminal_input_forward_smoke.sh` (new),
`runtime/src/gl/glfw_window_gl.cpp` (edit — 4-line `GLFW_KEY_D` addition,
symmetric with the existing C/Z tracking). No `compiler/**` `.mlc` files
touched — no self-host diff/Tier B required (same precedent as
§102a/§102b/§102c).

## §102d Green (2026-07-31)

`scripts/run_editor_terminal_input_forward_smoke.sh`:
`terminal_input_forward_smoke ok` / `[terminal input forward] ok`, exit 0
(all 6 scenarios). Regression checks after the `glfw_window_gl.cpp`
`GLFW_KEY_D` addition: `scripts/run_editor_terminal_libvterm_ffi_unit.sh`
(§102a), `scripts/run_editor_terminal_pty_spawn_unit.sh` (§102b),
`scripts/run_editor_terminal_cell_grid_render_smoke.sh` (§102c) all still
`ok`. `scripts/dev_gate_fast.sh`: 1471 passed, 0 failed, arch lint
failures=0 (first run showed the 2 known stale-`TMPDIR` failures from
`docs/agent/SESSION.md`'s own §102a precedent — a leftover `TMPDIR=.tmp/
critic_102c` exported in this persistent shell session by the prior turn's
Critic verification, confirmed via `git stash` reproducing the same 2
failures on committed `HEAD` with zero of this turn's changes present;
unsetting `TMPDIR`/`MLCC_OBJ_CLEAN`/`MLCC_PCH` and rerunning gave 1471/0
clean). `scripts/run_ux_gate.sh`: all 114 scenarios ok — confirms the new
`GLFW_KEY_D` polling introduces no regression to any existing editor
build/scenario. No `lib/mlc/**` files touched → `scripts/
regression_gate.sh` not required by the standing rule.

## §102c Decision (frozen 2026-07-31)

Bridge module `misc/editor/terminal/terminal_grid_render.mlc`, pure MLC, no
new draw path: `terminal_grid_text_lines` walks the vterm screen row by row,
grouping horizontally-contiguous same-foreground-color cells into one
`StaticTextLine` per run (fed to the existing `static_text_draw_lines_colored`);
`terminal_grid_draw_backgrounds` accumulates one `solid_renderer_rect` call
per cell whose background differs from a caller-supplied default (fed to the
existing `solid_renderer_flush_over`).

Two defects found and fixed in `vterm_abi.cpp` while wiring this (both
required for correct rendering, not cosmetic):
- `vterm_read_screen_cell` read `cell.fg.rgb`/`cell.bg.rgb` directly, which is
  only valid for already-RGB (truecolor) cells — for indexed/default-color
  cells (the overwhelming majority of real terminal output) this returned
  garbage/zero. Fixed by calling `vterm_screen_convert_color_to_rgb` on a
  copy before reading, after capturing the original `is_indexed`/`index`
  diagnostics. `vterm_obtain_screen` now also calls
  `vterm_screen_set_default_colors` (white fg / black bg) — without it,
  "default" color resolves to an unconfigured libvterm fallback (empirically
  `(240,240,240)`, not documented/guaranteed), not the intended white.
- `vterm_set_utf8` was never called, so multi-byte UTF-8 PTY output decoded
  one raw byte per cell (Latin-1), not one codepoint per cell — real PTY
  sessions are UTF-8 by default. Fixed in `vterm_create`.

New `vterm_last_cell_utf8()` (UTF-8-encodes the cell's codepoint, space for
an empty cell) and `VtermCell.text`/`.background_*` fields (getters already
existed in the C++ shim since §102a, just unused by `VtermCell` until now).

New GL binding `gl_read_pixel_component(x, y, component_index)` in
`glad_gl_abi.hpp`/`glad_gl.mlc` (mirrors `gl_get_integer_at`'s convention: no
caching, always re-queries via `glReadPixels`) — the gate's own pixel-probe
primitive, reused by any future L2 pixel-content test, not terminal-specific.

Gate: `misc/examples/terminal_cell_grid_render_smoke.mlc`, offscreen
`MLC_GLFW_VISIBLE=0`. Spawns `echo` (via §102b's `pty_terminal_spawn`)
printing 6 background-colored (200,100,50) spaces then a green "OK" word,
drains the PTY (§102b), feeds it to a vterm (§102a), renders via the new
bridge, then two independent pixel-content assertions on the actual
framebuffer: (1) exact-match `gl_read_pixel_component` at the colored-space
cell's center — deterministic, no font/AA dependency since it is a pure
`solid_renderer_rect` fill; (2) a small region scan over the "OK" glyph cells
asserting at least one non-black sample — the default background is pure
black, so any ink proves `static_text_draw_lines_colored` actually drew
there (exact-pixel glyph assertions are AA/hinting-environment-dependent,
same reasoning as `TRACK_EDITOR_HOVER_SCROLLBAR_PAINT_GAP.md`'s established
pattern). Both assertions independently verified to actually fail when
sabotaged (wrong expected RGB / scanning an empty region) before being
reverted — not tautological.

## §102c Green (2026-07-31)

`scripts/run_editor_terminal_cell_grid_render_smoke.sh`: `[mlc-editor]
terminal_cell_grid_render_smoke ok` / `[terminal cell grid render] ok`, exit
0. Regression checks after the `vterm_abi.cpp` color/UTF-8 fixes:
`scripts/run_editor_terminal_libvterm_ffi_unit.sh` (§102a's own test) still
`ok`; `scripts/run_editor_terminal_pty_spawn_unit.sh` (§102b's own test)
still `ok`. `scripts/dev_gate_fast.sh`: 1471 passed, 0 failed, arch lint
failures=0. `scripts/run_ux_gate.sh`: all 114 scenarios ok. `lib/mlc/
common/stdlib/gl/glad_gl.mlc` touched (new `gl_read_pixel_component`) →
`scripts/regression_gate.sh` run per the standing rule: Ruby-vs-mlcc
regression programs pass, examples compile+link sweep ok=147 fail=0 skip=3
(same 3 pre-existing skips as before this change — `dynrecord_demo`,
`scene_form_live`, `text_glyph_color_smoke` — the new
`terminal_cell_grid_render_smoke.mlc` itself compiles+links in the ok count).
No `compiler/**` `.mlc` files touched — no self-host diff/Tier B required
(same precedent as §102a/§102b).

## §102c Critic audit (2026-07-31)

Independent re-audit, no Driver artifact reused. `git show --stat a021d71e`:
exactly the module-touch files claimed (`terminal_grid_render.mlc` new,
`terminal_cell_grid_render_smoke.mlc` new, its runner script new,
`vterm_ffi.mlc`/`vterm_abi.hpp`/`.cpp`/`glad_gl_abi.hpp`/`glad_gl.mlc` edited)
plus 5 doc files — no stray file. Read the `vterm_abi.cpp` diff directly
(not the Driver's prose): confirms all 3 claimed defects — `vterm_set_utf8`
added in `vterm_create`; `vterm_screen_set_default_colors` added in
`vterm_obtain_screen`; `vterm_screen_convert_color_to_rgb` added in
`vterm_read_screen_cell`, with the original `is_indexed`/`index` captured
*before* the mutating conversion call, matching the comment's claim.

Independent from-scratch Ruby-bootstrap rebuild of `mlcc`
(`TMPDIR=.tmp/critic_102c MLCC_INCREMENTAL=0 compiler/build.sh`) — 0 errors,
only pre-existing `-Wparentheses-equality` warnings. Independent rerun of
`scripts/run_editor_terminal_cell_grid_render_smoke.sh` in a separate output
directory (`TERMINAL_CELL_GRID_RENDER_OUT=.tmp/critic_102c/render_smoke`):
passed.

New independent probe (not reusing the Driver's test, exercising a case it
did not cover): the Driver's own smoke test only used truecolor SGR
(`38;2`/`48;2`), never exercising the indexed-color-resolution path
end-to-end through the render bridge. Wrote a separate scratch probe with
different geometry (6×24 grid, 10×18 cells) using ANSI **indexed** colors
(`ESC[42m` background index 2, `ESC[31m` foreground index 1) — background
rendered as a real resolved color (`(0,224,0)`, not black/garbage) and the
indexed-foreground "ERR" text produced glyph ink. Confirms the color-fix
covers the indexed path, not just truecolor. Probe deleted after
verification, not committed.

Independently sabotaged the actual committed smoke test twice, differently
from the Driver's own sabotage (which shifted expected values/scan range):
(1) deleted the `terminal_grid_draw_backgrounds` call entirely — background
assertion correctly failed (`fail background rgb=0,0,0`, exit 6); (2)
replaced the `terminal_grid_text_lines` result with an empty list — ink
assertion correctly failed (`fail no glyph ink found`, exit 7). Both
confirm the assertions are load-bearing, not tautological. Reverted both
sabotages; reran the unmodified file — passes again, byte-identical to the
committed version (`git status` shows no diff).

Independent `scripts/dev_gate_fast.sh`: 1471 passed, 0 failed, arch lint
failures=0. Independent `scripts/run_ux_gate.sh`: all 114 scenarios ok.
`lib/mlc/common/stdlib/gl/glad_gl.mlc` touched → independent
`scripts/regression_gate.sh` run in the background (~9 min compile-bound,
consistent with prior runs): 20 regression programs pass (ruby vs expected,
mlcc vs expected, ruby vs mlcc all PASS), examples compile+link sweep
`ok=147 fail=0 skip=3` with the same 3 pre-existing skips
(`dynrecord_demo`/`scene_form_live`/`text_glyph_color_smoke`) — matches the
Driver's claim exactly.

Confirmed the non-track WIP files (`CLAUDE.md`/`README.md`/
`capture_analyzer.rb`/`docs/reddit-*`/`.vscode/`) absent from commit
`a021d71e` and still present/uncommitted after this audit. Cleaned up all
scratch artifacts (`.tmp/critic_102c/`, the scratch probe `.mlc` file,
`tmp/examples_sweep/`) after verification, not committed. No
self-host diff/Tier B performed — correctly not required (zero
`compiler/**` files touched — only `runtime/**` C++ shims and non-`compiler`
`.mlc` files, confirmed by the `git show --stat` above, same precedent as
§102a/§102b).

**§102c CLOSED.** No false-done found. All Driver claims independently
reproduced: exact file scope, the 3 `vterm_abi.cpp` fixes verified by
reading the diff directly, new test green from a fresh output dir,
`run_ux_gate.sh`/`dev_gate_fast.sh`/`regression_gate.sh` all green.
Additionally verified the indexed-color path (not truecolor-only) renders
correctly end-to-end, and both smoke-test assertions independently
confirmed load-bearing via a different sabotage than the Driver's own.
Queue advances to **§102d `TERMINAL_INPUT_FORWARD`**.

## §102b Decision (frozen 2026-07-31)

PTY plumbing via `forkpty` (glibc/libutil) — it owns slave tty setup
(`setsid`, `TIOCSCTTY`, stdin/stdout/stderr dup) internally, so no manual
`posix_openpt`/`grantpt`/`unlockpt` needed. The MLC boundary carries the
master fd as an `i32` handle (fds are small non-negative ints; `-1` is the
error sentinel), same style as §102a's `i64` pointer handles.

- `runtime/include/mlc/terminal/pty_abi.hpp` + `runtime/src/terminal/pty_abi.cpp`:
  `pty_spawn(command_line)` (`forkpty` + child `execl("/bin/sh", "sh", "-c",
  command_line)` — running through a shell, not raw `execvp`, avoids needing
  an array-typed extern fn parameter, for which there is no existing
  precedent in this codebase; matches the real future use anyway, since
  §102f's interactive shell panel spawns a shell, not a fixed argv), a
  thread-local `master_fd -> pid_t` map for cleanup, `pty_write`, `pty_read`
  (single `poll(timeout_ms)` + one `read()`, empty string on
  timeout/EOF/error), `pty_read_until_eof` (loops `pty_read`'s poll+read
  until a poll times out with no new data — this also naturally drains a
  burst of output from a still-running process like `cat`, not only a
  process that has actually exited), `pty_close` (`waitpid(WNOHANG)` +
  `close`, avoids zombies without blocking).
- `compiler/build_bin.sh`: added `runtime/src/terminal/pty_abi.cpp`
  unconditionally to `RT_SRC` (no `pkg-config` gate — `forkpty`/`<pty.h>` is
  standard glibc, not an optional external package, consistent with the
  track's own Linux-only non-goal) and an unconditional `-lutil` link
  (confirmed present via `ldconfig -p` on this machine; harmless no-op stub
  on glibc versions where `forkpty` was merged into libc proper).
- `misc/editor/terminal/pty_ffi.mlc`: extern fn declarations (module-private)
  + exported wrappers (`pty_terminal_spawn/write/read/read_until_eof/close`),
  same convention as `vterm_ffi.mlc`.
- Module-touch: `runtime/include/mlc/terminal/pty_abi.hpp` (new),
  `runtime/src/terminal/pty_abi.cpp` (new), `compiler/build_bin.sh` (edit,
  tooling script, self-host diff not applicable), `misc/editor/terminal/pty_ffi.mlc`
  (new).
- Gate (per the track file's own §102b spec): spawn `/bin/echo hello`, read
  the master fd, assert exact byte output; spawn `/bin/cat`, write bytes to
  master, assert echoed back. New test
  `misc/editor/tests/terminal_pty_spawn_unit.mlc` + runner
  `scripts/run_editor_terminal_pty_spawn_unit.sh` (pure FFI/process-plumbing
  unit test, not a `run_ux_*.sh` scenario — same rationale as §102a).

## §102b Steps

Red: confirmed no existing PTY/`fork`/`execvp`/`forkpty` binding anywhere in
`runtime/` (`grep -rl "fork(\|execvp\|posix_openpt\|forkpty\|waitpid\|popen"
runtime/src runtime/include`: zero matches) before this step. Confirmed
`forkpty` links against this system's glibc both with and without
`-lutil` (`gcc ... -lutil` and bare both succeed) — kept `-lutil` anyway for
portability, it is always present (`ldconfig -p | grep libutil`).

Empirically measured (not assumed) the exact bytes a PTY actually produces,
via a standalone `g++` probe linking `pty_abi.cpp` directly (no `mlcc`
round-trip needed for this measurement) before writing test assertions:
- `/bin/echo hello` through a freshly `forkpty`'d master, drained with
  `pty_read_until_eof`: **`"hello\r\n"`** (7 bytes) — the PTY's default
  `termios` output processing (`ONLCR`) translates the child's `\n` to
  `\r\n`; there is no bare `"hello\n"` on a PTY.
- `/bin/cat` with `"ping\n"` written to the master then drained with
  `pty_read_until_eof(fd, 4096, 300)`: **`"ping\r\nping\r\n"`** (12 bytes,
  twice) — the tty line discipline's local echo (`ECHO`/`ICANON` are on by
  default, exactly as a real terminal session leaves them) echoes the
  written input back once, and `cat`'s own copy of its stdin to stdout
  produces a second, independent copy; both go through the same `ONLCR`
  translation. A single non-looping `pty_read` call can observe only one of
  the two copies depending on arrival timing, which is why the test drains
  with `pty_read_until_eof` rather than a single `pty_read`.

Green: implemented the shim + bindings per the Decision above. New unit test
asserts the two exact byte strings measured above. Ran standalone through
the real `mlcc` pipeline (not just the raw probe):
`[mlc-editor] terminal_pty_spawn_unit ok`, exit 0.

Regression check: `scripts/dev_gate_fast.sh` (clean environment, no stale
`TMPDIR`) — `1471 passed, 0 failed`, arch lint failures=0. `bash
scripts/run_ux_gate.sh` run 1 and run 2 both `[ux gate] all ok (114
scenarios)` — confirms the new unconditional `pty_abi.cpp`
compile + `-lutil` link in `build_bin.sh` introduces no regression in any
existing editor build.

## §102b Critic audit (2026-07-31)

Independent re-audit, no Driver artifact reused. `git show --stat 32d8acc8`:
exactly the 5 module-touch files (`pty_abi.hpp`/`.cpp`, `pty_ffi.mlc`, the
new unit test + runner script) plus `compiler/build_bin.sh` and 5 doc
files — no stray file. `grep -rl "fork(\|execvp\|posix_openpt\|forkpty\|
waitpid\|popen" runtime/src runtime/include misc/editor`: exactly those 3
non-doc files, confirmed. Read `pty_abi.hpp`/`.cpp`/`pty_ffi.mlc` directly:
all 5 functions (`pty_spawn`/`pty_write`/`pty_read`/`pty_read_until_eof`/
`pty_close`) line up 1:1 across header ↔ implementation ↔ MLC extern fn
declaration, no drift. Read the `build_bin.sh` diff directly: confirms the
addition is unconditional (no `pkg-config` gate, unlike freetype2/harfbuzz/
vterm — correct, since `forkpty`/`<pty.h>` is standard glibc, not an
optional package) — `pty_abi.cpp` added to `RT_SRC`, `-lutil` added to
`EXTERN_LINK_LIBS` via a new `EXTRA_LINK_LIBS_ALWAYS` array.

Independent from-scratch Ruby-bootstrap rebuild of `mlcc`
(`TMPDIR=.tmp/critic_102b MLCC_INCREMENTAL=0 compiler/build.sh`) — 0 errors,
only pre-existing `-Wparentheses-equality` warnings. Independent rerun of
`scripts/run_editor_terminal_pty_spawn_unit.sh` in a separate output
directory (`EDITOR_TERMINAL_PTY_SPAWN_OUT=/tmp/critic_102b_pty`): passed.

New independent probe (not reusing the Driver's test, exercising cases it
did not cover): `pty_terminal_write`/`pty_terminal_read`/`pty_terminal_close`
on an unopened fd (9999) all return their documented error values (-1/""/-1)
without crashing; spawning a non-existent binary (`/bin/nonexistent_binary_
zzz_12345`) still returns a valid master fd (the shell itself starts fine)
and the drained output contains `/bin/sh`'s own "not found" error line;
double-`pty_terminal_close` on the same fd — first call returns 0, second
call (already closed) returns -1 rather than silently succeeding or
crashing. All passed (`critic_ok`, exit 0) — confirms the shim's error paths
beyond what the Driver's own test exercised.

Independent `scripts/run_ux_gate.sh` run 1 and run 2 (own logs, not reusing
the Driver's): both `[ux gate] all ok (114 scenarios)`, 0 failures —
confirms the new unconditional `pty_abi.cpp` compile + `-lutil` link
doesn't regress any existing editor build. Independent
`scripts/dev_gate_fast.sh` rerun with a clean environment (`unset TMPDIR
MLCC_OBJ_CLEAN MLCC_PCH` first): 1471 passed, 0 failed, arch lint
failures=0.

Confirmed the non-track WIP files (`CLAUDE.md`/`README.md`/
`capture_analyzer.rb`/`docs/reddit-*`/`.vscode/`) absent from commit
`32d8acc8` and still present/uncommitted after this audit. Cleaned up all
scratch artifacts (`.tmp/critic_102b/`, `/tmp/critic_102b_*`, the scratch
probe `.mlc` file) after verification, not committed. No `rake
test_compiler_mlc`/self-host diff performed — correctly not required (only
`compiler/build_bin.sh`, a Bash tooling script, touched under
`compiler/**`; zero `.mlc` checker/codegen files touched, confirmed by the
`git show --stat`/`grep` above, same precedent as §102a/§104-22).

**§102b CLOSED.** No false-done found. All Driver claims independently
reproduced: exact file scope, matching signatures across the ABI boundary,
new test green from a fresh output dir, `run_ux_gate.sh` stable ×2, `-lutil`
link unconditional and correctly scoped. Additionally verified 3 error/edge
paths (invalid-fd operations, spawn-of-nonexistent-command, double-close)
not covered by the Driver's own test. Queue advances to **§102c
`TERMINAL_CELL_GRID_RENDER`**.

## §102a Decision (frozen 2026-07-31)

Bind libvterm via a thin ABI shim following the `freetype_abi`/`harfbuzz_abi`
pattern exactly (raw pointers as `i64` handles, `thread_affine` extern fn,
thread-local "last result" slot for structured reads instead of exposing C
structs across the boundary):

- `runtime/include/mlc/terminal/vterm_abi.hpp` + `runtime/src/terminal/vterm_abi.cpp`:
  `vterm_create`/`vterm_destroy` (`vterm_new`/`vterm_free`), `vterm_obtain_screen`
  (`vterm_obtain_screen` + `vterm_screen_set_callbacks` + `vterm_screen_reset(hard=1)`
  in one call), `vterm_write_input` (`vterm_input_write`), `vterm_read_screen_cell`
  (`vterm_screen_get_cell`, fills a thread-local `LastCellSlot`) + getters, a damage
  callback that increments a per-screen counter (`vterm_damage_count`), and
  `vterm_escape_byte` — a 1-byte ESC string, since MLC string literals have no
  hex/control-byte escape syntax (only `\n \t \r \\ \" \0 \$ \{ \}`) and VT100/ANSI
  sequence construction needs the raw 0x1b byte (SGR here, arrow-key input in §102d).
- `compiler/build_bin.sh`: new `pkg-config --exists vterm` detection block
  (same shape as the existing freetype2/harfbuzz block), unconditionally links
  `-lvterm` into every mlcc-built binary when the system has it — matches how
  freetype/harfbuzz are already handled, no per-program opt-in mechanism exists
  in this build today.
- `misc/editor/terminal/vterm_ffi.mlc`: extern fn declarations (module-private,
  matching `text_shaping.mlc`'s convention) + exported wrapper functions
  (`vterm_terminal_create/destroy/screen/write/damage_count`,
  `vterm_control_escape_byte`, `vterm_cell_at` returning a `VtermCell` record).
- Module-touch: `runtime/include/mlc/terminal/vterm_abi.hpp` (new),
  `runtime/src/terminal/vterm_abi.cpp` (new), `compiler/build_bin.sh` (edit,
  tooling script — not a checker/codegen `.mlc` file, self-host diff not
  applicable), `misc/editor/terminal/vterm_ffi.mlc` (new).
- Gate (per the track file's own §102a spec): feed a fixed byte sequence
  (plain text + one SGR true-color escape) through `vterm_write_input`, read
  back the resulting cell grid via `vterm_read_screen_cell`, assert
  text+foreground-color match expected — no PTY, no rendering. New test
  `misc/editor/tests/terminal_libvterm_ffi_unit.mlc` + runner
  `scripts/run_editor_terminal_libvterm_ffi_unit.sh` (not a `run_ux_*.sh` UX
  scenario — this is a pure FFI round-trip unit test per the track spec;
  `run_ux_gate.sh` only gains its first terminal scenario at §102f).

## Non-goals (binding, per the 2026-07-15 architecture review's own lesson)

- **No second render/state system.** The terminal is a new panel/tab type
  inside the *already-unified* `EditorAppState`/paint path from §97, reusing
  `static_text`/`solid_renderer` for the cell grid and `ShellPanels`/tab
  strip for chrome. Do not build a parallel widget system (repeat of the
  `scene.mlc` vs `misc/editor` split the 2026-07-15 review flagged).
- No full terminal emulator from scratch — **libvterm** (already installed
  on this machine as `libvterm0` 0.3.3, `-dev` headers TBD at §102a) does
  VT100/ANSI/scrollback parsing; MLC code only owns PTY plumbing, the render
  adapter, and panel wiring.
- No multiplexing/tabs-within-terminal (tmux-like) in v1 — one PTY per panel
  instance is enough to prove the architecture.
- No Windows/ConPTY — Linux `posix_openpt`/`forkpty` only, matches project's
  current Linux-only runtime scope.

## Sub-tracks, in order

### §102a `TERMINAL_LIBVTERM_FFI`

Bind `libvterm` via `extern fn`/`extern lib` (same pattern as
`freetype`/`harfbuzz` shims in `runtime/src/`) — `vterm_new`, `vterm_free`,
`vterm_input_write`, `vterm_screen_get_cell`, damage callback. Confirm
`libvterm-dev` headers available (`apt install libvterm-dev` if missing) or
vendor the single-header subset actually used. Gate: feed a fixed byte
sequence (plain text, one SGR color escape) through `vterm_input_write`, read
back the resulting cell grid via `vterm_screen_get_cell`, assert
text+foreground-color match expected — no PTY, no rendering yet, pure FFI
round-trip test.

### §102b `TERMINAL_PTY_SPAWN`

PTY spawn/read/write via existing runtime FFI conventions (`forkpty` or
`posix_openpt`+`grantpt`+`unlockpt`+`fork`+`execvp`). Gate: spawn a fixed
non-interactive command (e.g. `/bin/echo hello`), read the PTY master fd,
assert exact byte output; spawn `/bin/cat`, write bytes to master, assert
echoed back — proves the PTY plumbing independent of libvterm.

### §102c `TERMINAL_CELL_GRID_RENDER`

Wire §102a's vterm screen state into §102b's live PTY output, render the
cell grid by reusing `static_text_draw_lines_colored` (batching renderer,
same one the editor uses) for glyphs and `solid_renderer_rect` for cell
background — no new draw path. Gate: offscreen `MLC_GLFW_VISIBLE=0` +
`glReadPixels` test (same pattern as `msdf_renderer_shim`/editor L2 smokes):
run a fixed command that prints a known colored line, assert the sampled
pixel color at the expected cell position matches.

### §102d `TERMINAL_INPUT_FORWARD`

Keyboard → PTY: printable chars, Enter/Backspace/Tab, arrow keys (VT100
escape sequences), Ctrl+C/D/Z signals. Gate: offscreen scenario types a
fixed shell command (e.g. `echo hi<Enter>`) via the same
`glfw_gl_input_test_*` hooks the editor L2 smokes use, asserts the resulting
vterm screen contains the expected echoed output line.

### §102e `TERMINAL_RESIZE_SCROLLBACK`

`SIGWINCH`/`vterm_set_size` on panel resize; bounded scrollback ring buffer
(fixed line cap, not unbounded growth — this is the perf-relevant part).
Gate: resize test asserts vterm reflow; scrollback test writes N+100 lines
into an N-line-cap buffer, asserts buffer stays at N and oldest lines were
evicted, not silently unbounded.

### §102f `TERMINAL_PANEL_INTEGRATION`

Wire as a new tab/panel kind in `demo_live.mlc`'s existing tab strip and
`EditorAppState` (post-§97c unified state — mutate via `editor_app_set_*`
pattern, do not reintroduce scattered locals). Gate: one new
`run_ux_gate.sh` scenario (open terminal tab, type, see output) added to the
discovered-scenario set; `run_ux_gate.sh` ×2 stays green including this one.

### §102g `TERMINAL_PERF_BUDGET`

Perf smoke analogous to §97a: measure ms/frame while a fixed high-throughput
command runs (e.g. `seq 1 100000` or `yes | head -100000`) inside the
terminal panel, on the *same* shared render loop the editor uses. Gate:
explicit numeric budget (document the measured baseline the same honest way
§97a/§101 did — no unverified claims) and assert the terminal panel does not
regress the editor's own idle/scroll frame budget (§97/§101 gates) when the
terminal tab is not focused.

## §102a Steps

Red: confirmed `libvterm-dev` is already installed (`dpkg -l | grep vterm`:
`libvterm-dev:amd64 0.3.3-2build1`, headers at `/usr/include/vterm.h`,
`pkg-config --exists vterm` succeeds) — no apt install needed. Confirmed no
existing vterm binding (`grep -rl vterm runtime/ misc/` before this step:
zero matches).

Green: implemented the shim + bindings per the Decision above. New unit test
feeds `"hi" + ESC + "[38;2;200;100;50mX"` into a 4×20 `vterm_create`,
asserts cell (0,0)='h' (104), (0,1)='i' (105), (0,2)='X' (88) with
`foreground_is_indexed=false`/RGB=(200,100,50) exactly matching the SGR
true-color escape, and `vterm_damage_count(screen) > 0` (the registered
damage callback fired). Ran standalone: `[mlc-editor]
terminal_libvterm_ffi_unit ok`, exit 0.

Regression check: `scripts/dev_gate_fast.sh` green (1471 compiler unit tests
pass — 2 apparent failures on a first attempt traced to a stale `TMPDIR`
left exported in the shell session by an unrelated prior turn, not a real
regression: confirmed by unsetting it and rerunning, all green). `bash
scripts/run_ux_gate.sh` run 1 and run 2 both `[ux gate] all ok (114
scenarios)` — unchanged from §101's close, confirming the new
`compiler/build_bin.sh` vterm-detection block (which now unconditionally
links `-lvterm` into every editor build on this machine) introduces no
regression.

## §102a Critic audit (2026-07-31)

Independent re-audit, no Driver artifact reused. `git show --stat 57877a1c`:
exactly the 5 module-touch files (`vterm_abi.hpp`/`.cpp`, `vterm_ffi.mlc`,
the new unit test + runner script) plus `compiler/build_bin.sh` and 5 doc
files — no stray file. `grep -rl vterm runtime/ misc/ compiler/build_bin.sh`:
exactly those 5 non-doc files, confirmed. Read `vterm_abi.hpp`/`.cpp`/
`vterm_ffi.mlc` directly: every function signature lines up 1:1 across all
three (header ↔ implementation ↔ MLC extern fn declaration), no drift.
Confirmed the new `build_bin.sh` block is shape-identical to the existing
freetype2 block (same `command -v pkg-config` guard,
`RT_SRC+=`/`TEXT_CFLAGS+=`/`TEXT_LIBS+=` idiom).

Independent from-scratch Ruby-bootstrap rebuild of `mlcc`
(`MLCC_INCREMENTAL=0 compiler/build.sh`, fresh `TMPDIR` under
`.tmp/critic_102a/`, not reusing the Driver's binary) — 0 errors, only
pre-existing `-Wparentheses-equality` warnings. Independent rerun of
`scripts/run_editor_terminal_libvterm_ffi_unit.sh` in a separate output
directory (`EDITOR_TERMINAL_LIBVTERM_FFI_OUT=/tmp/critic_102a_ffi`): passed.

New independent probe (not reusing the Driver's test, exercising cases it
did not cover): invalid-handle `vterm_terminal_destroy`/`vterm_terminal_screen`/
`vterm_terminal_damage_count` all return their documented error codes; an
**indexed** SGR escape (`ESC[31mR` — ANSI red, index 1, as opposed to the
Driver's true-color test) correctly reads back `foreground_is_indexed=true`,
`foreground_index=1`, codepoint 'R'; an out-of-range cell read
(`vterm_cell_at(screen, 999, 999)`) returns the documented empty/default
cell rather than crashing or returning garbage. All passed
(`critic_ok`, exit 0) — confirms the shim's error paths and a second color
mode beyond what the Green step's own test exercised.

Independent `scripts/run_ux_gate.sh` run 1 and run 2 (own logs, not reusing
the Driver's): both `[ux gate] all ok (114 scenarios)`, 0 failures —
confirms the new unconditional `-lvterm` link doesn't regress any existing
editor build. Independent `scripts/dev_gate_fast.sh` rerun with a
deliberately clean environment (`unset TMPDIR MLCC_OBJ_CLEAN MLCC_PCH`
first): 0 failures, arch lint failures=0 — reconfirms the Driver's own
diagnosis that the 2 apparent failures on their first attempt were a
stale-`TMPDIR` false alarm from an unrelated prior turn, not a real
regression.

Confirmed the non-track WIP files (`CLAUDE.md`/`README.md`/
`capture_analyzer.rb`/`docs/reddit-*`/`.vscode/`) absent from commit
`57877a1c` and still present/uncommitted after this audit. Cleaned up all
scratch artifacts (`.tmp/critic_102a/`, `/tmp/critic_102a_*`) after
verification, not committed. No `rake test_compiler_mlc`/self-host diff
performed — correctly not required (only `compiler/build_bin.sh`, a Bash
tooling script, touched under `compiler/**`; zero `.mlc` checker/codegen
files touched, confirmed by the `git show --stat`/`grep` above).

**§102a CLOSED.** No false-done found. All Driver claims independently
reproduced: exact file scope, matching signatures across the ABI boundary,
new test green from a fresh output dir, `run_ux_gate.sh` stable ×2, and the
disclosed `TMPDIR` false-alarm explanation reconfirmed. Additionally
verified 3 error/edge paths and a second color mode not covered by the
Driver's own test. Queue advances to **§102b `TERMINAL_PTY_SPAWN`**.

## Verification discipline

Same as every `misc/editor/**` track: scenario-first, `run_ux_gate.sh` ×2
before Critic close, `dev_gate_fast.sh` green. §102g's perf smoke becomes a
permanent regression gate once it exists, same role as §97a's.
