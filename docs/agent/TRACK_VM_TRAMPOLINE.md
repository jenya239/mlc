# Track: MIR VM stack-overflow crash (~1500 steps) — trampoline fix

Parent: [../PLAN.md](../PLAN.md), [TRACK_MIR_VM_FULL.md](TRACK_MIR_VM_FULL.md).

Trigger: manual benchmarking of `mlcc --run` (2026-07-10) found the VM
crashes with a real segfault on **any** program executing more than
~1500-2000 MIR steps — loop or user recursion, doesn't matter. This was not
caught by Epic 0-4 corpus (`vm_cpp_diff_programs.txt`, `run_examples_vm_gate.sh`)
because every fixture is a handful of steps. Epic 4's "single/multi-file
`--run` stable" claim (STEP=10-12, closed 2026-07-09/10) is **not actually
true** for any program bigger than a toy example — this bug makes the VM
currently unusable for its own north star (running real programs without
g++, eventually the compiler itself).

Приоритет: **критический — выше `TEXT_RENDERING` и `TRACK_CLI_STDIN`**.
Blocks the entire practical value of `TRACK_MIR_VM_FULL`. Small, well-scoped,
root cause already found (see below) — no further investigation needed
before starting the fix.

## Status: **open** — STEP=1–3 **done**; STEP=4 next (re-benchmark)

**Driver 2026-07-10:** STEP=1 — trampoline; STEP=2 — corpus; STEP=3 — `vm_deep_loop.mlc`
(100k iters) + `run_vm_trampoline_depth_gate.sh` under default `ulimit -s`.

## Root cause (found, confirmed, not fixed)

Repro:

```bash
cat > /tmp/loop.mlc << 'EOF'
fn main() -> i32 = do
  let mut total = 0
  let mut i = 0
  while i < 1700 do
    total = i
    i = i + 1
  end
  total
end
EOF
compiler/out/mlcc --run /tmp/loop.mlc
# n=1500: exits with mod-256 value, correct
# n=1700: Segmentation fault (core dumped)
```

Same ceiling for pure user recursion (no loop at all):

```mlc
fn sum_rec(n: i32, acc: i32) -> i32 =
  if n <= 0 then acc else sum_rec(n - 1, acc + n) end
fn main() -> i32 = do sum_rec(1000, 0) end   // works
fn main() -> i32 = do sum_rec(1200, 0) end   // Segmentation fault
```

`(ulimit -s unlimited; ...)` makes the crash go away but a 2,000,000-iteration
loop still does not finish in 15s — this is not "stack a bit too small", it
is unbounded stack growth proportional to steps executed.

**File:** `compiler/vm/execute.mlc`.

```
26:  Ok(locals) => vm_run_frames(program, [vm_frame_new(...)], trace_enabled)
29:export fn vm_run_frames(program: MirProgram, frames: [VmFrame], trace_enabled: bool) -> VmRunOutcome = do
  ...
38:        VmRunContinue(next_frames) => vm_run_frames(program, next_frames, trace_enabled),
  ...
44:        VmRunContinue(next_frames) => vm_run_frames(program, next_frames, trace_enabled),
  ...
195: vm_run_frames(program, vm_frame_advance_statement(caller_frames, caller_index), trace_enabled)
```

`vm_run_frames` calls **itself** once per executed MIR statement/terminator
(one MIR step = one self-call), including through `vm_pop_return` (line 195,
executed once per user function return). This is a tail call at the MLC
source level, but the generated C++ almost certainly does not get sibling/
tail-call elimination from g++/clang here (`VmRunOutcome`/`[VmFrame]` are
non-trivial types — `mlc::Array` COW, variant matches, `match` codegen shape
in this project frequently nests through helper calls) — every MIR step
executed adds one real C++ stack frame in `mlcc`'s own process. Default
8 MB thread stack / observed frame size ⇒ crash around 1500-2000 steps.

## Fix approach

Rewrite `vm_run_frames` as an explicit host-level loop instead of
self-recursion — a trampoline, not an architecture change:

```mlc
export fn vm_run_frames(program: MirProgram, frames_initial: [VmFrame], trace_enabled: bool) -> VmRunOutcome = do
  let mut frames = frames_initial
  while true do
    if frames.length() == 0 then return VmRunFailed(['vm: empty frame stack']) end
    const frame_index = frames.length() - 1
    const frame = vm_frame_at(frames, frame_index)
    const step_outcome = match vm_find_block(frame.function, frame.block_id) {
      Err(errors) => VmRunFailed(errors),
      Ok(block) =>
        if frame.statement_index < block.stmts.length() then
          vm_execute_statement(program, frames, frame_index, block.stmts[frame.statement_index], trace_enabled)
        else
          vm_execute_terminator(program, frames, frame_index, block.term, trace_enabled)
        end
    }
    match step_outcome {
      VmRunContinue(next_frames) => frames = next_frames,   // loop again, no recursion
      VmRunReturn(value) => return VmRunReturn(value),
      VmRunFailed(errors) => return VmRunFailed(errors)
    }
  end
end
```

`vm_pop_return` (line 195) must stop calling `vm_run_frames` itself — it
should return `VmRunContinue(next_frames)` up to the trampoline loop instead
(check its current signature/callers; it is called from inside
`vm_execute_terminator`'s `MirReturn` arm, which is itself invoked from
inside the loop body above — as long as `vm_pop_return` returns a
`VmRunOutcome` rather than directly recursing, the loop naturally continues).

Exact refactor of `vm_execute_statement`/`vm_execute_terminator` signatures
is NOT required — only the outer driver (`vm_run_frames` + `vm_pop_return`'s
self-call) needs to stop recursing.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Rewrite `vm_run_frames` in `compiler/vm/execute.mlc` as an explicit `while` loop (see Fix approach). Change `vm_pop_return` to return `VmRunOutcome` instead of calling `vm_run_frames` directly. | **done** (2026-07-10: trampoline + step helper; loop 10k / rec 1200 no segfault) |
| 2 | Regression: full existing corpus (`vm_cpp_diff_programs.txt`, `run_single_file_vm_gate.sh`, `run_examples_vm_gate.sh`) must still pass — this is a pure refactor, no behavior change for small programs. | **done** (2026-07-10: cpp_diff 18, single-file 18+diff, examples 28) |
| 3 | New regression fixture: loop/recursion depth **≥ 100,000** steps (well past the old ~1700 ceiling) must complete without crashing and without `ulimit -s unlimited`. Add to `compiler/tests/` corpus permanently — this exact class of bug must never regress silently again. | **done** (2026-07-10: `misc/examples/vm_deep_loop.mlc` 100k + `run_vm_trampoline_depth_gate.sh`; exit=7, ulimit -s=8192, ~0.34s) |
| 4 | Re-benchmark throughput after the fix (same 1000/2,000,000-iteration loops as the 2026-07-10 manual benchmark) — confirm no major regression from the refactor itself (trampoline should be same speed or faster, no recursion call overhead). Record numbers in this track or `TRACK_MIR_VM_FULL.md` §4. | pending |
| 5 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`. Update `TRACK_MIR_VM_FULL.md` — Epic 4's stability claim was false until this fix; note the correction there with a link to this track. | pending |

## Out of scope

- Throughput optimization beyond "fix the crash + don't regress speed" — a
  separate, lower-priority follow-up (`TRACK_MIR_VM_FULL.md` already
  documents realistic throughput expectations, 20-80× slower than native).
- Any other VM native-coverage gaps found during the same benchmarking
  session (e.g. `.to_string()` on `i32` unsupported in VM method dispatch)
  — separate, smaller, non-critical follow-up, not blocking this fix.

## Per-turn template

```
| step | <1-5> |
| done | <one line> |
| verify | <числа/факт> |
| next | ROLE=Driver STEP=<n+1> TRACK_VM_TRAMPOLINE |
```
