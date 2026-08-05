# Track: Editor frame / paint architecture path (§110)

Parent: [../PLAN.md](../PLAN.md) §110.
Authorized **2026-08-04** as the **path** (not an instant rewrite): user —
«так я не прошу сразу, главное путь» after asking why a maximally efficient
and reliable architecture is hard.

**Queue rule:** §109 epic **CLOSED** 2026-08-05. This track is the next editor
architecture priority (ahead of §103f+ / §104 Wave 2 under the standing
perf/architecture/testing directive), unless the user overrides.

Standing discipline: [AGENTS.md](../../AGENTS.md) Performance workflow —
measure → one hypothesis → one cut → remasure. No “optimize GUI broadly”.

## Status: **open** 2026-08-06 — queue head **§110b** (STEP=0 Decision frozen; Red next)

## Destination (plain)

```text
input / commands
    → single frame state (dirty generations)
    → layout (only if layout_generation dirty)
    → paint / display list (CPU structs, no GL)
    → clip + cull + merge batches
    → few streamed VBO/IBO uploads
    → small number of draw calls
    → swap
```

Not: each widget/`demo_live` branch calling `gl*` / reshaping O(doc) every frame.

Reliability = same path under **scripted** gates (inject + dogfood suite).
Efficiency = idle does ~nothing; scroll/type O(visible); draw calls batched.

## Why not one jump

`demo_live.mlc` still owns dispatch + paint orchestration. §97/§108/§109
fixed measured hotspots and built the harness. The remaining gap is
**ownership of the frame**, not another minimap tweak. Jumping to the full
pipeline without phased gates recreates false-green history.

## Path (ordered phases)

Each phase: Decision → Red harness → Green → Critic. One bottleneck class
per Green. Dogfood gate (`run_editor_perf_dogfood_gate.sh`) must stay green
unless a phase Decision explicitly replaces a member.

| Phase | Name | What ships | Gate idea |
|-------|------|------------|-----------|
| **0** | Finish §109 | §109k Critic closes epic | **done** 2026-08-05 — dogfood gate ×2 + sabotages |
| **A** | Headless-visible measure | All `MLC_GLFW_VISIBLE=1` dogfood/perf scripts runnable under **Xvfb** (or `DISPLAY` isolate) so agents do not steal `:0` keyboard/mouse | **done** §110a — wrapper + wake isolate; dogfood-under-xvfb scroll residual |
| **B** | Frame ownership | One `EditorFrame` (or equivalent): dirty generations (`content` / `chrome` / `present` / `layout` / `paint` / `geometry`); **all** live ticks go through it; no new ad-hoc caches in `demo_live` | L1: unchanged UI → layout_generation and paint_generation do not bump; sabotage always-dirty fails — **§110b** |
| **C** | Paint list | Chrome + text + overlays emit **paint commands** (rects/glyphs/scissors); GL only in one submit path | Counter: `gl_call_from_widget == 0`; draw_calls / paint_ops reported; dogfood non-regress |
| **D** | Batch + stream | Merge compatible commands; orphaning / multi-buffer upload; no per-quad `glBufferData` | draw_calls and bytes_uploaded ceilings measured-then-written; idle upload ≈0 |
| **E** | Glyph damage residual | Row-level Y-adjust / newly-visible-only reshape (leftover from §109e) | scroll_cpu ceiling tightened below §109’s 60 with numbers; shape O(newly visible) |
| **F** | Overlay metrics (optional) | In-process counters already in perf scripts; optional on-screen overlay behind env flag | Does not replace script gates |
| **G** | Archive / handoff | Point GUI_ARCHITECTURE + EDITOR at this path; resume §103f | docs only |

Non-goals for the whole track (unless user re-opens):

- SceneNode chrome migration (GUI_ARCHITECTURE Deviation).
- Flutter/Qt/Skia/GPUI.
- Big-bang delete of `demo_live` in one commit.
- Nsight on Pascal as required tooling (`apitrace` optional diagnosis only).

## Relation to §109

§109 = **stop the bleeding + honest instruments**.  
§110 = **walk to the architecture** those instruments can police.

Do not reopen §109 numbered STEPs for architecture; fold residuals into
phases B–E with new harness names.

## §110a `EDITOR_PERF_XVFB_ISOLATE` — **CLOSED** 2026-08-05 (Critic OK)

| Item | Choice |
|------|--------|
| Problem | §109 `MLC_GLFW_VISIBLE=1` dogfood/perf scripts bind ambient `DISPLAY` (usually `:0`). Agent Critic/Driver runs steal keyboard/mouse and share CPU with the interactive session — §109k scroll_cpu noise (61–72 under load) was partly host contention on `:0`, not only editor work |
| Fix | Below (Decision frozen 2026-08-05) |
| Depends on | §109 CLOSED (dogfood gate exists and stays the pass/fail authority for ceilings) |
| Gate | Xvfb wrapper harness green; wake-under-xvfb isolate proof exit 0; missing Xvfb → **fail**, not skip-green. Full dogfood-under-xvfb scroll pacing = residual (not §110a close blocker) |
| Sabotage | (1) `MLC_EDITOR_PERF_XVFB=1` while child still sees real `:0` / no Xvfb process → isolate check fail. (2) Missing `xvfb-run`/`Xvfb` in `PATH` with XVFB=1 → **skip-green or exit 0** → fail. (3) Wrapper swallows child non-zero exit → fail. (4) Under wrapper, open README* / raise scroll ceiling / set `MLC_EDITOR_PERF` → dogfood-gate sabotes still fail |
| Out of scope | Frame ownership (§110b); paint list / batch; changing §109 CPU ceilings; SceneNode; installing `xvfb` into CI images as a separate infra track (Green may document `apt-get install -y xvfb` as host prerequisite — still **fail** if absent) |

### Decision (frozen 2026-08-05)

| Choice | Freeze |
|--------|--------|
| Measure authority | **New** `scripts/run_editor_perf_xvfb.sh` (+ `_red.sh`). Report: `.tmp/editor_perf_xvfb/report.txt`. Opt-in env **`MLC_EDITOR_PERF_XVFB=1`**. Default (unset/0): existing scripts unchanged (still ambient `DISPLAY`) |
| Pre-cut (audit 2026-08-05) | (1) **No** `scripts/run_editor_perf_xvfb.sh` / no `*xvfb*` harness. (2) §109 gate + members hardcode/export `MLC_GLFW_VISIBLE=1` and inherit host `DISPLAY` (this machine: `:0.0`). (3) **`Xvfb` / `xvfb-run` not installed** (`/usr/bin/Xvfb`, `/usr/bin/xvfb-run` absent; Debian package `xvfb` exists in apt-cache). (4) Path table already named `MLC_EDITOR_PERF_XVFB=1` but nothing reads it |
| **Green cut** | Ship wrapper only (no `demo_live` / renderer edits). (1) When `MLC_EDITOR_PERF_XVFB=1`: require `xvfb-run` **or** `Xvfb` on `PATH` — else **FAIL** with explicit message (never skip-green / never fall back to `:0` silently). (2) Prefer `xvfb-run -a -s "-screen 0 1280x800x24"` (or equivalent) wrapping the child argv; else start `Xvfb :N` + `DISPLAY=:N` + cleanup trap. (3) Export `MLC_GLFW_VISIBLE=1`, unset skip-heavy `MLC_EDITOR_PERF`, keep §109 open-path defaults (`demo_live.mlc`). (4) Default child = `scripts/run_editor_perf_dogfood_gate.sh` (ceilings unchanged); allow override argv after `--`. (5) Propagate child exit code; write `display=…`, `xvfb=1`, `child_exit=…`, key dogfood metrics copied from gate report. (6) Optional thin helper `scripts/editor_perf_maybe_xvfb.sh` sourced/exec’d by other `run_editor_perf_*.sh` later — **not** required to rewrite every member in this STEP if the composite gate is the Green proof |
| Green must hit | (1) Red “already present” exit 1 after Green. (2) **Amended 2026-08-05:** with XVFB=1 and Xvfb present, wrapper isolates `DISPLAY≠:0` and runs a VISIBLE=1 load-bearing child exit 0 — proof child = `run_editor_perf_wake_on_hover.sh` (still/jitter ceilings). **Full** `run_editor_perf_dogfood_gate.sh` under Xvfb is **not** Green-blocking yet: measured scroll_cpu **235–294%** (llvmpipe + Xvfb ignores `glfwSwapInterval(1)` → uncapped present); §109 scroll≤60 was calibrated on GPU `:0`. Residual = present-pace under Xvfb (follow-up; may touch runtime swap path — out of this wrapper-only STEP). Default argv remains dogfood gate for when pacing exists. (3) With XVFB=1 and no real Xvfb/`xvfb-run` on `PATH`: exit ≠0, message mentions missing Xvfb. (4) Sabotages (1)–(3) fail; (4) dogfood sabotes still fail when invoked as child. (5) Without XVFB=1: ambient path still works (no forced Xvfb) |
| Counters / report | `xvfb=1\|0`, `display=…`, `child=dogfood_gate\|…`, `child_exit=…`, plus copied `scroll_cpu_percent` / `type_stall_ms` / `member=* status=ok` lines when child is the gate |
| Red | No `run_editor_perf_xvfb.sh` |
| Green | Wrapper + `_red.sh` already-present; paste wake-under-xvfb metrics; note dogfood-under-xvfb scroll residual |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-05 |
| 1 | Red: no xvfb harness | **done** 2026-08-05 — `scripts/run_editor_perf_xvfb_red.sh` exit 1 |
| 2 | Green: wrapper + isolate proof under Xvfb | **done** 2026-08-05 — see Green measured |
| 3 | Critic | **done** 2026-08-05 — CLOSED; see Critic notes |

### Green measured (2026-08-05)

Host: `xvfb` package installed (`Xvfb`/`xvfb-run` on PATH). Wrapper: `scripts/run_editor_perf_xvfb.sh`.

| Check | Result |
|-------|--------|
| Red already present | exit 1 |
| Missing Xvfb (stub first on PATH) | exit 1, message mentions install xvfb |
| Isolate smoke `XVFB=1 -- bash -c 'echo DISPLAY=…'` | exit 0, `display=:99` |
| Propagate child fail `-- false` | exit 1 |
| Ambient `XVFB=0` | exit 0, `xvfb=0`, ambient `DISPLAY` |
| Wake under Xvfb | exit 0, `display=:99`, still=1% jitter=1% |
| Full dogfood gate under Xvfb | **fail** scroll median **242** (samples 235/242/256) — residual present-pace |

### Critic notes (2026-08-05)

Independent remasure (system `/usr/bin` Xvfb, not `.tmp` prefix):

| Check | Result |
|-------|--------|
| Red already present | exit 1 |
| Missing stubs on PATH | exit 1, install message |
| Isolate smoke | exit 0, `display=:99` |
| `-- false` | exit 1 |
| Ambient XVFB=0 | exit 0, `xvfb=0` |
| Wake under Xvfb | exit 0, `display=:99`, still=1% jitter=1%, L1 deltas 0 |
| Sab4 `MLC_EDITOR_PERF=1` via `-- env … dogfood_gate` | exit 1 skip-heavy |
| Sab4 `SCROLL_CPU_MAX=100` via `-- env …` | exit 1 |
| Sab4 README via `-- env …` | exit 1 basename |
| Note | Outer `MLC_EDITOR_PERF=1` on default child is **unset by wrapper** before exec — sab4 must use `-- env …` to reach the gate (not a false-green) |

Residual (not blocking §110a close): dogfood-under-xvfb scroll≫60 — present-pace follow-up (runtime/`SwapInterval` under Xvfb).

## §110b `EDITOR_FRAME_OWNERSHIP` — **queue head**

| Item | Choice |
|------|--------|
| Problem | Path phase B: `demo_live.mlc` still owns the GLFW loop and schedules layout/paint by hand. Dirty intent is **booleans** (`content_dirty` / `pointer_dirty` / `chrome_dirty` / `paint_dirty` / `layout_skip`). Retained batches (§108) and `EditorFrameLayout` ticks (§97) exist, but there is **no** single `EditorFrame` and **no** `layout_generation` / `paint_generation`. ~30 scattered `frame_layout_tick_*` call sites in `demo_live` — new ad-hoc caches can still land outside one owner |
| Fix | Below (Decision frozen 2026-08-06) |
| Depends on | §110a CLOSED (measure isolate available); §108 retained chrome/text layers; §109 dogfood gate stays authority for epic CPU ceilings |
| Gate | Frame-ownership harness green; unchanged UI → generation deltas 0; sabotage always-dirty fails; dogfood gate non-regress |
| Sabotage | (1) Force bump `layout_generation` and/or `paint_generation` every poll while UI unchanged → L1 fail. (2) Bypass `EditorFrame` and call bare `frame_layout_tick_*` from a load-bearing live path while claiming ownership → static/harness fail. (3) Claim green while `run_editor_perf_wake_on_hover.sh` rebuild deltas or `run_editor_perf_dogfood_gate.sh` regress |
| Out of scope | SceneNode; full paint-list / GL submit merge (§110c/D); wholesale delete of `demo_live`; Xvfb dogfood scroll pacing residual (§110a); changing §109 CPU ceilings; glyph row-Y damage (§110e) |

### Decision (frozen 2026-08-06)

| Choice | Freeze |
|--------|--------|
| Measure authority | **New** `scripts/run_editor_frame_ownership.sh` (+ `_red.sh`). L1 scenario dumps `layout_generation` / `paint_generation` (and content/chrome gens if shipped) over still/idle/present-only ticks. Side: `run_editor_perf_wake_on_hover.sh` rebuild/frame deltas still 0. Dogfood: existing `run_editor_perf_dogfood_gate.sh` non-regress (ceilings unchanged). Report: `.tmp/editor_frame_ownership/report.txt` |
| Pre-cut (audit 2026-08-06) | (1) **No** type `EditorFrame`; **zero** `layout_generation` / `paint_generation` in repo. (2) Loop + paint orchestration owned by `misc/editor/demo_live.mlc` (~3566 lines; `while glfw_gl_context_should_close()`). (3) Closest types: `EditorFrameInput` (`app/frame_input.mlc`), `EditorFrameLayout` + `frame_layout_tick_*` (`app/frame_layout.mlc`) — layout helpers, not frame owner. (4) Dirty = booleans in input/live locals (`content_dirty` / `pointer_dirty` / `chrome_dirty` / `paint_dirty` / `layout_skip`). (5) ~30 `frame_layout_tick_*` sites in `demo_live` (snapshot/edit/pixel/spans/minimap/max_columns). (6) Retained `TextLayerBatch` / `ChromeLayerBatch` / glyph batches exist (§108/§109) but are not generation-owned. (7) Wake L1 already gates `delta_*_rebuild_count` / `delta_*_frame_count`, **not** generations |
| **Green cut** | Introduce **`EditorFrame`** (name may match `EditorFrameLayout` family) holding dirty **generations** at minimum `layout_generation` + `paint_generation` (add `content` / `chrome` / `present` / `geometry` if needed for the wire without expanding to paint-list). Single live entry (`editor_frame_tick` / equivalent) schedules layout ticks + retained-layer invalidate/replay from gens — **route load-bearing live paths through it** so unchanged UI does not bump layout/paint gens. Edit/open paths mark content (or layout) gen once instead of growing the ad-hoc retick forest. Keep §108 retained batches; **do not** emit paint-command lists (§110c). Files: new small module under `misc/editor/app/` (preferred) + minimal `demo_live.mlc` wire; L1 scenario + harness (+ `_red.sh`) |
| Green must hit | (1) Unchanged UI (still-over-text and/or present-only / hit-stable as harness defines): `layout_generation` delta == 0 and `paint_generation` delta == 0 across the sample window. (2) Sabotage always-dirty fails that assert. (3) Static/harness: load-bearing live layout ticks go through `EditorFrame` (bare `frame_layout_tick_*` from listed live sites fails). (4) `run_editor_perf_wake_on_hover.sh` still/jitter ceilings + rebuild deltas still 0. (5) `run_editor_perf_dogfood_gate.sh` exit 0 (one quiet pass OK for Green; Critic may ×2). (6) Red “already present” after Green |
| Counters / report | `layout_generation=…`, `paint_generation=…`, deltas over sample; optional content/chrome gens; wake `delta_*_rebuild_count`; dogfood `scroll_cpu_percent` / `type_stall_ms` copied when run |
| Red | No `run_editor_frame_ownership.sh` / no generation fields in editor sources |
| Green | Ownership type + wire + harness; paste L1 generation deltas + wake/dogfood side metrics under this §110b |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-06 |
| 1 | Red: no ownership harness / no generations | **done** 2026-08-06 |
| 2 | Green: EditorFrame + generation-stable L1 + dogfood non-regress | **open** |
| 3 | Critic | **open** |

## Diff / notes

2026-08-04: path written; no code.
2026-08-05: §109 CLOSED; §110a Decision/Red/Green/Critic CLOSED (Xvfb wrapper + wake isolate); dogfood-under-xvfb scroll residual.
2026-08-06: §110b Decision frozen (frame ownership / dirty generations).
2026-08-06: §110b Red — `scripts/run_editor_frame_ownership_red.sh` (exit 1: no green harness / no gens).
