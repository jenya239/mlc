# Track: Editor terminal panel — libvterm-backed, reuses existing render/state

Parent: [../PLAN.md](../PLAN.md) §102. Authorized 2026-07-28 (user request: "можно
добавить треки по терминалу"). Emphasis per user directive: performance,
architecture, testing — every sub-track below carries an explicit gate, no
sub-track is "done" without one.

## Status: **open** — §102a Decision next

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

## Verification discipline

Same as every `misc/editor/**` track: scenario-first, `run_ux_gate.sh` ×2
before Critic close, `dev_gate_fast.sh` green. §102g's perf smoke becomes a
permanent regression gate once it exists, same role as §97a's.
