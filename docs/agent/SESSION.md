# Agent session log

Turns before 2026-08-01 (Planner §107 open) archived — see [../archive/SESSION_HISTORY.md](../archive/SESSION_HISTORY.md).

## Entries

### Turn 2026-08-01 (Planner/Scribe docs sync — §102 close, §106 record, §107 open)

| field   | value |
|---------|-------|
| role    | Planner/Scribe |
| step    | n/a (docs sync, no code) |
| track   | docs (PLAN / CONTINUITY / SESSION / TRACK_EDITOR_TERMINAL / TRACK_EDITOR_IDLE_POINTER_DIRTY / TRACK_EDITOR_HYGIENE) |
| started | 2026-08-01 |
| instructions_rev | 2026-08-01-editor-hygiene-queue |
| done    | Three drifts fixed, no code touched. (1) **§102 `TRACK_EDITOR_TERMINAL` epic recorded CLOSED**: the track file already carries the §102g Critic audit (independent re-measure in `.tmp/critic_102g/`, doc `total_us=521074` / term `total_us=677933` under the documented `DOC_TOTAL_US_MAX=1500000` / `TERM_TOTAL_US_MAX=2000000` ceilings, 4 sabotages incl. a source mutation of the perf tag string, `dev_gate_fast` 1471/0, `run_ux_gate` ×2 = 115/115) while PLAN's §102 status cell, the CONTINUITY authorized-queue cell and the SESSION tail still said "§102g next / awaiting Critic" — PLAN row, queue chain, CONTINUITY queue cell and HARD LIMIT rows updated to CLOSED; no new verification claimed, only the existing Critic result propagated. (2) **New §106 `TRACK_EDITOR_IDLE_POINTER_DIRTY`** written as a **closed** track documenting the already-shipped idle/hover CPU fix (commit `47b4f134`): `pointer_dirty` split on `EditorFrameInput`, out-of-client-area mouse-jitter filter, idle `wait_events_timeout(0.05)` moved *before* layout/chrome work, `layout_skip` reusing `cached_visual_rows` on pointer-only frames, `glfwSwapInterval(1)` only when `MLC_GLFW_VISIBLE`. Track records honestly that **no dedicated gate exists** for this fix (`run_ux_idle_cpu_budget_stable.sh` measures idle without pointer motion and would have stayed green through the incident; the perf smoke cannot see it because `MLC_EDITOR_PERF=1` forces `skip_full_pixel_wrap_now`) and that **no Critic turn was run** — residuals routed to §107e/§107i/§107j/§107r and to P2 backlog B7, not silently closed. (3) **New §107 `TRACK_EDITOR_HYGIENE` epic opened** from `mlc-support/responses/editor_hygiene_audit_20260801_103839.md` (Opus 5, 2026-08-01, EHA-01…28): P0 §107a `EDITOR_SAVE_ACTIVE_FILE` → §107b `EDITOR_SESSION_ORIGINAL_PATHS` → §107c `EDITOR_VISIBLE_ROWS_PREFIX_JUMP` → §107d `EDITOR_PERF_SMOKE_FULL_PATH` → §107e `EDITOR_DOCUMENT_VERSION`, then P1 §107f…§107r in the audit's own roadmap order, then a P2 backlog table (B1–B11) that is deliberately **not** expanded into PLAN rows. §107a carries a Planner-proposed Decision table (frozen-ready: `CmdSave` → `open_buffer_save` on the active tab's original path + `tab_set_update_active_buffer`, session dump split to `CmdSaveSession`, gate `run_ux_save_writes_file_to_disk` reading the file back **from disk**, 2 required sabotages, atomic write/fsync explicitly out of scope → B10) which the Driver confirms or re-freezes at STEP=0. Two catalog findings the audit's own roadmap dropped were recorded rather than lost: EHA-10 (`pty_spawn` shell-exec, catalog P1) as B11, and the audit §6.1 non-atomic save as B10. Queue order after this sync: **§107a (head) → §107b…§107r → §103a `SCRIPT_VM_VALUE_REP` → §104 Wave 2**; §104 Wave 2 is explicitly **not** pulled ahead of hygiene P0 — doing so would require a new user override. `INSTRUCTIONS_REV` bumped `2026-07-28-compiler-architecture-hygiene-priority` → `2026-08-01-editor-hygiene-queue`; new override (f) recorded (2026-08-01: hygiene audit authorized as queue head ahead of §103) |
| verify  | Docs-only turn — no build, no gate run, and none claimed. Every status change is a propagation of a verification already recorded elsewhere: §102g Critic evidence from `TRACK_EDITOR_TERMINAL.md` / git `44b66105`+; §106 from the shipped commit `47b4f134` with its gaps stated explicitly rather than papered over. §107 sub-tracks are all **pending** — no Decision is marked frozen, no Step is marked done, no Critic close is asserted for anything unshipped |
| result  | **§102 epic CLOSED (recorded). §106 CLOSED (recorded, with disclosed gaps). §107 OPEN, head §107a.** Agent state now matches the tree |
| issues  | §106 ships without a gate — accepted and documented, its missing idle-CPU-under-pointer-load gate is owned by §107r. §107d exists precisely because today's perf number is measured with the hot phases off; until it closes, no editor perf claim from `MLC_EDITOR_PERF=1` should be treated as characterising a real frame |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_EDITOR_HYGIENE (§107a `EDITOR_SAVE_ACTIVE_FILE` — read `TRACK_EDITOR_HYGIENE.md` §107a and the audit's EHA-01/EHA-02 entries before confirming/re-freezing the proposed Decision; red first: the new scenario must fail on today's tree by reading the fixture back from disk after Ctrl+S, not merely by checking a dirty flag) |


### Turn 2026-08-02 (Driver TRACK_EDITOR_HYGIENE STEP=0+1, §107a Decision+Red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0+1 |
| track   | TRACK_EDITOR_HYGIENE |
| started | 2026-08-02 |
| instructions_rev | 2026-08-01-editor-hygiene-queue |
| done    | Froze §107a Decision (confirms Planner table: `CmdSave` → `editor_app_save_active_buffer` → `open_buffer_save` on original path; `CmdSaveSession` split; gate reads disk). Red: added stub `editor_app_save_active_buffer` that still dumps `.tmp/editor_live_*` (CmdSave bug shape), scenario `save_writes_file_to_disk.mlc` + `scripts/run_ux_save_writes_file_to_disk.sh`. Ran gate: `ux_fail save_writes_file_to_disk disk_content`, binary exit=5 — Red proven. demo_live CmdSave not wired yet (Green) |
| verify  | `bash scripts/run_ux_save_writes_file_to_disk.sh` → FAIL exit 1 / scenario exit 5 `disk_content` |
| result  | **§107a STEP=0/1 done (Decision+Red).** Green next |
| issues  | New `run_ux_*` script will fail `run_ux_gate` until Green — expected for Red |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_EDITOR_HYGIENE (§107a Green — replace stub with `open_buffer_save` on active path + clear dirty; wire `CmdSave`/`CmdSaveSession` in `demo_live`+`bus.mlc`; scenario green; `dev_gate_fast` + `run_ux_gate` ×2; then Critic) |

### Turn 2026-08-02 (Driver TRACK_EDITOR_HYGIENE STEP=2, §107a Green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_EDITOR_HYGIENE |
| started | 2026-08-02 |
| instructions_rev | 2026-08-01-editor-hygiene-queue |
| done    | Green §107a: `editor_app_save_active_buffer` → `open_buffer_save` + `tab_set_update_active_buffer` (terminal tab refused). Added `CmdSaveSession` (Ctrl+Shift+S; toolbar tool 9). `CmdSave` (Ctrl+S) → active-file save in `demo_live`. Scenario `save_writes_file_to_disk` green. `dev_gate_fast` 1471/0. `run_ux_gate` ×2 = 116/116 (was 115). `demo_live` links |
| verify  | `run_ux_save_writes_file_to_disk.sh` ok; `run_editor_command_bus_unit.sh` ok; `dev_gate_fast` 1471/0; `run_ux_gate` ×2 all ok (116) |
| result  | **§107a STEP=2 done (Green), awaiting Critic before CLOSE** |
| issues  | Prompt INSTRUCTIONS_REV still 2026-07-28; CONTINUITY/SESSION use 2026-08-01-editor-hygiene-queue — followed CONTINUITY |
| next    | ROLE=Critic STEP=3 TRACK=TRACK_EDITOR_HYGIENE (§107a — independent rebuild+rerun of save scenario; sabotages: (1) dump/session path instead of open_buffer_save, (2) no-op open_buffer_save success; `run_ux_gate` ×2; close §107a → §107b Decision) |

### Turn 2026-08-03 (Critic TRACK_EDITOR_HYGIENE STEP=3, §107a close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_EDITOR_HYGIENE |
| started | 2026-08-03 |
| instructions_rev | 2026-08-01-editor-hygiene-queue |
| done    | Independent re-audit of §107a Green `03179bcc`. Fresh scenario rebuild in `.tmp/critic_107a_save`: `ux_ok`. Sabotages (Decision-required, distinct): (1) `editor_app_save_active_buffer` dumps `.tmp/editor_live_save_red.txt` → `disk_content` exit 5; (2) `open_buffer_save` no-op clears dirty without write → `disk_content` exit 5; both reverted (`git checkout` clean). `dev_gate_fast` 1471/0; `run_ux_gate` ×2 = 116/116. Closed §107a |
| verify  | critic scenario ok; sabotages 5/5; ux gate ×2 116/116; track files clean after revert |
| result  | **§107a CLOSED.** Advances to §107b |
| issues  | Prompt INSTRUCTIONS_REV still 2026-07-28; followed CONTINUITY `2026-08-01-editor-hygiene-queue` |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_EDITOR_HYGIENE (§107b `EDITOR_SESSION_ORIGINAL_PATHS` — read track §107b; freeze Decision; red: session restore must fail on today's tree when paths are `.tmp/editor_live_*` copies / tab loss) |

### Turn 2026-08-03 (Driver TRACK_EDITOR_HYGIENE STEP=0+1, §107b Decision+Red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0+1 |
| track   | TRACK_EDITOR_HYGIENE |
| started | 2026-08-03 |
| instructions_rev | 2026-08-01-editor-hygiene-queue |
| done    | Archived pre-2026-08-01 SESSION → `docs/archive/SESSION_HISTORY.md`. Froze §107b Decision (session save stores original absolute paths; drop `dump_tab_file` from CmdSaveSession path). Red: `session_restores_original_paths.mlc` + `scripts/run_ux_session_restores_original_paths.sh` mirrors today's dump-based `save_demo_session`; two `*.txt` fixtures collide on `.tmp/editor_live_buffer.txt` |
| verify  | `bash scripts/run_ux_session_restores_original_paths.sh` → FAIL exit 1 / scenario exit 6 `ux_fail session_restores_original_paths count` |
| result  | **§107b STEP=0/1 done (Decision+Red).** Green next |
| issues  | New `run_ux_*` fails `run_ux_gate` until Green — expected for Red. Prompt INSTRUCTIONS_REV stale; followed CONTINUITY `2026-08-01-editor-hygiene-queue` |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_EDITOR_HYGIENE (§107b Green — `save_demo_session` uses `workspace_session_from_tabs` on live tabs, no dump; scenario green; `dev_gate_fast` + `run_ux_gate` ×2; then Critic) |
