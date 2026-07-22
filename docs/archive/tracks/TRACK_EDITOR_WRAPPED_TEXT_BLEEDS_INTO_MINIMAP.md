# Track: Editor wrapped text bleeds into minimap

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#37**
(pulled forward 2026-07-22 — live screenshot). Size **M**.

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=4):** Re-ran stable + `demo_live` compile + HEAD `run_ux_gate` (14 scenarios).
Anti-false-done: `9310f1e2`…`81fb086a` (STEP=0–3); `editor_ux_content_clip_rect` + glyph batch
`editor_gl_scissor_enable_rect(height, content_clip)`; `misc/editor/**` only → REG skip; no
`compiler/`/`lib/mlc/`. L2: clip right edge == `minimap_rect.x` when enabled. **reopen: none**.

Honest residual: solid-pass still scissors full `editor_rect` (selection/minimap indicator under
that pass); wrap budget still uniform `char_width` (secondary, L2 green without it). Historical
`run_ux_minimap_content_clip_red.sh` expects pre-wire demo_live and fails post-green — use
`run_ux_minimap_content_clip_stable.sh` only. Early-return in `do` body was dropped by codegen
(STEP=2 used if/else expression form).

| Gate | Result |
|------|--------|
| `run_ux_minimap_content_clip_stable.sh` | `ux_ok minimap_content_clip_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (14 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor-only) |

## Next step

**closed** — Critic OK. §46 backlog items #1…#38 all done. Queue → Planner (select next PLAN item).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9310f1e2` | Decision: clip-primary at minimap edge |
| 1 | `88780293` | Red L2 stub + red/stable scripts |
| 2 | `b70b5f45` | `editor_ux_content_clip_rect` green |
| 3 | `81fb086a` | `demo_live` glyph batch → `content_clip` |
| 4 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | Full-size content glyphs appear inside `minimap_rect`; content draw scissored to full `editor_rect` |
| Strategy (v1) | Clip glyph batch to non-minimap strip via `editor_ux_content_clip_rect` |
| Primary gate | Clip right edge == `minimap_rect.x` when minimap enabled |
| Module touch | `misc/editor/**` only |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: clip / bleed probe | **done** |
| 2 | Clip implement | **done** |
| 3 | `demo_live` wire; compile | **done** |
| 4 | Critic: gates; archive | **done** |

## Out of scope

`#38` reopen; full HarfBuzz wrap rewrite; `lib/mlc/`.
