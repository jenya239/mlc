# Track: Editor solid-pass content clip (minimap)

Parent: [../../PLAN.md](../../PLAN.md) §49.
Residual of [TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP](TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md)
(§46 #37). Size **S**.

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=3):** Re-ran stable + `demo_live` compile + HEAD `run_ux_gate`
(64 scenarios). Anti-false-done: `915dc910`…`eb7ba4f6` (STEP=0–2); solid pass
`editor_gl_scissor_enable_rect(height, content_clip)`; minimap indicator under
`minimap_rect` scissor; glyph batch still `content_clip`; no `editor_rect` solid
scissor. `misc/editor/**` only → REG skip; no `compiler/`/`lib/mlc/`. **reopen: none**.

Honest residual: wrap budget still uniform `char_width` (out of scope). Historical
`run_ux_solid_pass_content_clip_red.sh` expects pre-wire demo_live and fails
post-green — use `run_ux_solid_pass_content_clip_stable.sh` only.

| Gate | Result |
|------|--------|
| `run_ux_solid_pass_content_clip_stable.sh` | `ux_ok solid_pass_content_clip_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (64 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor-only) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; select next or document idle).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `915dc910` | Decision: solid `content_clip` + minimap indicator clip |
| 1 | `63382e9f` | Red/stable harnesses |
| 2 | `eb7ba4f6` | `demo_live` solid → `content_clip`; indicator under `minimap_rect` |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | #37 clipped the **glyph** batch to `content_clip`, but the **solid** pass still did `editor_gl_scissor_enable_rect(height, editor_rect)`. Selection / current-line / find highlights and content scrollbar could paint into `minimap_rect`. Minimap viewport indicator is drawn in the same pass, so a naive clip would hide it |
| Strategy (v1) | (1) Solid content overlays use `content_clip`. (2) Minimap viewport indicator under `minimap_rect` scissor. Do **not** change wrap budget |
| Primary gate | Green: solid uses `content_clip`; minimap indicator under minimap scissor; `demo_live_fs_compile_ok`; `run_ux_gate` |
| Module touch | `misc/editor/demo_live.mlc` only |
| REG | no |
| Out of scope | wrap `char_width` budget; idle caret-blink dirty; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: solid-pass `editor_rect` gap | **done** |
| 2 | Wire solid → `content_clip`; minimap indicator clip | **done** |
| 3 | Critic: compile + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Reopen #37/#38 numbered STEPs; HarfBuzz wrap rewrite.
