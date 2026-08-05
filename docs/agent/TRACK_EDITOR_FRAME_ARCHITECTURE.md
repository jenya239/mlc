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

## Status: **open** 2026-08-05 — queue head **§110a** (STEP=1 Red done; Green next)

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
| **A** | Headless-visible measure | All `MLC_GLFW_VISIBLE=1` dogfood/perf scripts runnable under **Xvfb** (or `DISPLAY` isolate) so agents do not steal `:0` keyboard/mouse | `MLC_EDITOR_PERF_XVFB=1` → same pass/fail as today; fail if falls back to skip-green — **§110a** |
| **B** | Frame ownership | One `EditorFrame` (or equivalent): dirty generations (`content` / `chrome` / `present` / `layout` / `paint` / `geometry`); **all** live ticks go through it; no new ad-hoc caches in `demo_live` | L1: unchanged UI → layout_generation and paint_generation do not bump; sabotage always-dirty fails |
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

## §110a `EDITOR_PERF_XVFB_ISOLATE` — **queue head**

| Item | Choice |
|------|--------|
| Problem | §109 `MLC_GLFW_VISIBLE=1` dogfood/perf scripts bind ambient `DISPLAY` (usually `:0`). Agent Critic/Driver runs steal keyboard/mouse and share CPU with the interactive session — §109k scroll_cpu noise (61–72 under load) was partly host contention on `:0`, not only editor work |
| Fix | Below (Decision frozen 2026-08-05) |
| Depends on | §109 CLOSED (dogfood gate exists and stays the pass/fail authority for ceilings) |
| Gate | Xvfb wrapper harness green; `run_editor_perf_dogfood_gate.sh` under wrapper exit 0; missing Xvfb → **fail**, not skip-green |
| Sabotage | (1) `MLC_EDITOR_PERF_XVFB=1` while child still sees real `:0` / no Xvfb process → isolate check fail. (2) Missing `xvfb-run`/`Xvfb` in `PATH` with XVFB=1 → **skip-green or exit 0** → fail. (3) Wrapper swallows child non-zero exit → fail. (4) Under wrapper, open README* / raise scroll ceiling / set `MLC_EDITOR_PERF` → dogfood-gate sabotes still fail |
| Out of scope | Frame ownership (§110b); paint list / batch; changing §109 CPU ceilings; SceneNode; installing `xvfb` into CI images as a separate infra track (Green may document `apt-get install -y xvfb` as host prerequisite — still **fail** if absent) |

### Decision (frozen 2026-08-05)

| Choice | Freeze |
|--------|--------|
| Measure authority | **New** `scripts/run_editor_perf_xvfb.sh` (+ `_red.sh`). Report: `.tmp/editor_perf_xvfb/report.txt`. Opt-in env **`MLC_EDITOR_PERF_XVFB=1`**. Default (unset/0): existing scripts unchanged (still ambient `DISPLAY`) |
| Pre-cut (audit 2026-08-05) | (1) **No** `scripts/run_editor_perf_xvfb.sh` / no `*xvfb*` harness. (2) §109 gate + members hardcode/export `MLC_GLFW_VISIBLE=1` and inherit host `DISPLAY` (this machine: `:0.0`). (3) **`Xvfb` / `xvfb-run` not installed** (`/usr/bin/Xvfb`, `/usr/bin/xvfb-run` absent; Debian package `xvfb` exists in apt-cache). (4) Path table already named `MLC_EDITOR_PERF_XVFB=1` but nothing reads it |
| **Green cut** | Ship wrapper only (no `demo_live` / renderer edits). (1) When `MLC_EDITOR_PERF_XVFB=1`: require `xvfb-run` **or** `Xvfb` on `PATH` — else **FAIL** with explicit message (never skip-green / never fall back to `:0` silently). (2) Prefer `xvfb-run -a -s "-screen 0 1280x800x24"` (or equivalent) wrapping the child argv; else start `Xvfb :N` + `DISPLAY=:N` + cleanup trap. (3) Export `MLC_GLFW_VISIBLE=1`, unset skip-heavy `MLC_EDITOR_PERF`, keep §109 open-path defaults (`demo_live.mlc`). (4) Default child = `scripts/run_editor_perf_dogfood_gate.sh` (ceilings unchanged); allow override argv after `--`. (5) Propagate child exit code; write `display=…`, `xvfb=1`, `child_exit=…`, key dogfood metrics copied from gate report. (6) Optional thin helper `scripts/editor_perf_maybe_xvfb.sh` sourced/exec’d by other `run_editor_perf_*.sh` later — **not** required to rewrite every member in this STEP if the composite gate is the Green proof |
| Green must hit | (1) Red “no xvfb harness” exit 1. (2) With XVFB=1 and Xvfb present: wrapper runs dogfood gate → exit 0 (one quiet pass OK for Green; Critic may require ×2). (3) With XVFB=1 and Xvfb/`xvfb-run` removed from `PATH`: exit ≠0, message mentions missing Xvfb — **not** exit 0. (4) Sabotages (1)–(4) each fail. (5) Without XVFB=1: ambient path still works (no forced Xvfb) |
| Counters / report | `xvfb=1\|0`, `display=…`, `child=dogfood_gate\|…`, `child_exit=…`, plus copied `scroll_cpu_percent` / `type_stall_ms` / `member=* status=ok` lines when child is the gate |
| Red | No `run_editor_perf_xvfb.sh` |
| Green | Wrapper + `_red.sh` already-present; paste one green gate-under-xvfb run (metrics) under this §110a |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-05 |
| 1 | Red: no xvfb harness | **done** 2026-08-05 — `scripts/run_editor_perf_xvfb_red.sh` exit 1 |
| 2 | Green: wrapper + dogfood gate under Xvfb | **open** |
| 3 | Critic | **open** |

## Diff / notes

2026-08-04: path written; no code.
2026-08-05: §109 CLOSED; §110a Decision frozen (Xvfb wrapper contract); Red harness added.
