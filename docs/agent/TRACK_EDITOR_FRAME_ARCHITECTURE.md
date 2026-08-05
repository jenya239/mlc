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

## Status: **open** 2026-08-05 — queue head; §109 CLOSED; first STEP **§110a** Decision

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
| **0** | Finish §109 | §109k Critic closes epic | dogfood gate ×2 + sabotages |
| **A** | Headless-visible measure | All `MLC_GLFW_VISIBLE=1` dogfood/perf scripts runnable under **Xvfb** (or `DISPLAY` isolate) so agents do not steal `:0` keyboard/mouse | `MLC_EDITOR_PERF_XVFB=1` → same pass/fail as today; fail if falls back to skip-green |
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

## First Driver STEP (when unblocked)

After §109 Critic close: **§110a Decision** = freeze Xvfb wrapper contract
(phase A) — smallest reliability win for continued agent work, zero renderer
redesign.

## Diff / notes

2026-08-04: path written; no code.
