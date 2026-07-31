# Track: Editor terminal panel — libvterm-backed, reuses existing render/state

Parent: [../PLAN.md](../PLAN.md) §102. Authorized 2026-07-28 (user request: "можно
добавить треки по терминалу"). Emphasis per user directive: performance,
architecture, testing — every sub-track below carries an explicit gate, no
sub-track is "done" without one.

## Status: **open** — §102a done (Red+Green), awaiting Critic. §102b next.

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

## Verification discipline

Same as every `misc/editor/**` track: scenario-first, `run_ux_gate.sh` ×2
before Critic close, `dev_gate_fast.sh` green. §102g's perf smoke becomes a
permanent regression gate once it exists, same role as §97a's.
