# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-10-vm-block-id-gate` |
| agent_token_last | — |
| driver_turns_since_plan | 18 |
| step_last | 4 |
| active_track | TRACK_CLI_STDIN STEP=2 |
| test_gate | ok (DIFF identical; regression 20/0; corpus) |

### Turn 2026-07-10 15:00 (Driver TRACK_VM_LOWERING_GAPS STEP=4 — verify-gate + close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_VM_LOWERING_GAPS |
| started | 2026-07-10 14:40 |
| elapsed | ~20 min |
| done    | Self-host p1→mlcc2→p2 DIFF identical; `regression_gate` 20/0; VM corpus (elif6, depth, examples 28, single 18, cpp_diff 18); archived track; PLAN 13a-3 done; MIR_VM_FULL note. |
| verify  | DIFF:IDENTICAL (p1≈6.3s, link≈112s, p2≈7.4s); regression_gate 20/0; corpus all ok. |
| result  | STEP=4 done; track **closed**. Plain: unary + mid-block/rvalue `if` lowering accepted. |
| issues  | Dirty `compiler/out/*` left. New `vm_if_*`/`vm_unary_*` not yet in examples manifest (28). |
| next    | ROLE=Driver STEP=2 TRACK_CLI_STDIN — `read_all` builtin registration |

### Turn 2026-07-10 14:50 (Driver TRACK_VM_LOWERING_GAPS STEP=3 — if-as-rvalue)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_VM_LOWERING_GAPS |
| started | 2026-07-10 14:36 |
| elapsed | ~14 min |
| done    | `mir_lower_if_to_local`/`if_into_local`/`expression_into_local`; wired operand+rvalue+expression_to_local; examples `vm_if_rvalue{,_else,_block}.mlc`. Archived pre-14:05 SESSION turns → `SESSION_HISTORY.md`. |
| verify  | BUILD:0; rvalue=1 else=2 block=11 nested=3; stmt 1/2; unary; elif gate 6 ok. |
| result  | STEP=3 done. Plain: `const y = if … end` materializes via shared local. |
| issues  | Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=4 TRACK_VM_LOWERING_GAPS — self-host diff + regression_gate + VM corpus; close track |

### Turn 2026-07-10 14:40 (Driver TRACK_VM_LOWERING_GAPS STEP=2 — mir_lower_if_statement)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_VM_LOWERING_GAPS |
| started | 2026-07-10 14:27 |
| elapsed | ~13 min |
| done    | `mir_lower_if_statement` (CondJump→then/else→continue); `mir_lower_discard_expression` (Block/Unit/empty Tuple); statement Expr: Block/Unit/empty Tuple; operand empty Tuple→Unit; examples `vm_if_stmt{,_else}.mlc`. |
| verify  | BUILD:0; `--run` then=1 else=2; unary not/neg; elif gate 6 ok. |
| result  | STEP=2 done. Plain: mid-block `if` with `do`/`()` arms runs in VM. |
| issues  | Bare `if then x=1 else x=2` still E015 (unit vs i32); use `do`+`()`. Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=3 TRACK_VM_LOWERING_GAPS — if-as-rvalue (shared local + continue) |

### Turn 2026-07-10 14:30 (Driver TRACK_VM_LOWERING_GAPS STEP=1 — MirRvalueUnary)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_VM_LOWERING_GAPS |
| started | 2026-07-10 14:21 |
| elapsed | ~9 min |
| done    | `MirRvalueUnary` in mir_types; lower_fn operand+rvalue; mir_eval `!`/`-`/`+`/`~`; const_fold + mir_to_cpp; examples `vm_unary_{not,neg}.mlc`. |
| verify  | BUILD:0; `--run` not→0, neg→7; elif gate 6 ok; hello=7. |
| result  | STEP=1 done. Plain: VM can evaluate unary `!` and `-`. |
| issues  | Foreign CLI_STDIN WIP stashed/restored. Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=2 TRACK_VM_LOWERING_GAPS — mir_lower_if_statement (mid-block if) |

### Turn 2026-07-10 14:25 (Driver TRACK_VM_BLOCK_ID_COLLISION STEP=5 — verify-gate + close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_VM_BLOCK_ID_COLLISION |
| started | 2026-07-10 14:06 |
| elapsed | ~19 min |
| done    | Self-host p1→mlcc2→p2; `regression_gate.sh`; archive track; MIR_VM_FULL note; PLAN 13a-2 done. |
| verify  | DIFF:IDENTICAL (p1=7.45s, link=456.85s, p2=5.77s); regression_gate 20/0 (303s). |
| result  | STEP=5 done; track **closed**. Plain: nested elif no longer hangs VM; self-host still bit-identical. |
| issues  | Foreign CLI_STDIN WIP stashed/restored. Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=1 TRACK_VM_LOWERING_GAPS — lower unary `!`/`-` in MIR |

### Turn 2026-07-10 14:15 (Driver TRACK_VM_BLOCK_ID_COLLISION STEP=4 — VM corpus)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_VM_BLOCK_ID_COLLISION |
| started | 2026-07-10 14:00 |
| elapsed | ~15 min |
| done    | Ran elif/depth/examples/single-file(+cpp_diff) gates on post-fix `mlcc` (no code change). |
| verify  | elif 6 ok; trampoline depth ok; examples 28 ok; single-file 18 + cpp_diff 18 ok. |
| result  | STEP=4 done. Plain: one-line block-id fix does not regress VM corpus. |
| issues  | Foreign CLI_STDIN WIP + dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=5 TRACK_VM_BLOCK_ID_COLLISION — self-host diff + regression_gate; close track |

### Turn 2026-07-10 14:10 (Driver TRACK_VM_BLOCK_ID_COLLISION STEP=3 — deeper nesting)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_VM_BLOCK_ID_COLLISION |
| started | 2026-07-10 13:58 |
| elapsed | ~12 min |
| done    | `vm_elif_chain_{mid,last}.mlc`, `vm_if_in_then.mlc`; extended `run_vm_elif_block_id_gate.sh` to 6 cases. |
| verify  | gate ok: 100/200/44/40/50/22; no timeout. |
| result  | STEP=3 done. Plain: long elif and if-in-then also safe after block-id fix. |
| issues  | Foreign CLI_STDIN WIP + dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=4 TRACK_VM_BLOCK_ID_COLLISION — full VM corpus gates |

### Turn 2026-07-10 14:05 (Driver TRACK_VM_BLOCK_ID_COLLISION STEP=2 — classify fixture)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_VM_BLOCK_ID_COLLISION |
| started | 2026-07-10 13:56 |
| elapsed | ~9 min |
| done    | `misc/examples/vm_elif_classify_{0,1,5}.mlc`; `compiler/tests/run_vm_elif_block_id_gate.sh` (timeout 5s). |
| verify  | gate ok: classify0=100, classify1=200, classify5=44. |
| result  | STEP=2 done. Plain: elif hang class permanently gated. |
| issues  | Foreign CLI_STDIN WIP + dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=3 TRACK_VM_BLOCK_ID_COLLISION — deeper nesting (elif×5+, if-in-then) |

### Turn 2026-07-10 14:00 (Driver TRACK_VM_BLOCK_ID_COLLISION STEP=1 — else_block_step.state)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_VM_BLOCK_ID_COLLISION |
| started | 2026-07-10 13:42 |
| elapsed | ~18 min |
| done    | `compiler/mir/lower_fn.mlc`: If-arm `mir_lower_finish_current_block(else_block_step.state, …)`. |
| verify  | BUILD:0; `--dump-mir` blocks 0–4 unique; `--run` classify(0)=100, (1)=200, (5)=44 (=300%256). |
| result  | STEP=1 done. Plain: elif second branch no longer infinite-loops in VM. |
| issues  | Foreign CLI_STDIN STEP=2 WIP stashed/restored. Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=2 TRACK_VM_BLOCK_ID_COLLISION — permanent classify fixture in compiler/tests |
