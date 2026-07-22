# Track: Editor solid-pass content clip (minimap)

Parent: [../PLAN.md](../PLAN.md) §49.
Residual of [TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP](../archive/tracks/TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md)
(§46 #37). Size **S**.

## Status: **active** (2026-07-22) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: solid-pass still scissors `editor_rect` (gap probe).

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | #37 clipped the **glyph** batch to `content_clip`, but the **solid** pass still does `editor_gl_scissor_enable_rect(height, editor_rect)` (`demo_live.mlc` ~1907). Selection / current-line / find highlights and content scrollbar may paint into `minimap_rect`. Minimap viewport indicator is drawn in the same pass (~2066–2077), so a naive clip would hide it |
| Strategy (v1) | (1) Solid content overlays (selection, current-line, find, caret band, content thumb) use `content_clip`. (2) Minimap viewport indicator: draw under `minimap_rect` scissor (brief enable/disable around that rect only, or after solid content flush before minimap glyphs). Do **not** change wrap budget (still out of scope) |
| Primary gate | Red: demo_live solid pass still has `editor_gl_scissor_enable_rect(height, editor_rect)` (gap). Green: that site uses `content_clip`; minimap indicator still drawn (grep `minimap_indicator` under minimap scissor or separate clip). `demo_live_fs_compile_ok`; `run_ux_gate` includes prior minimap/idle stables |
| Module touch | `misc/editor/demo_live.mlc` only (helper already exists) |
| REG | no |
| Out of scope | wrap `char_width` budget; idle caret-blink dirty; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: solid-pass `editor_rect` gap | open |
| 2 | Wire solid → `content_clip`; minimap indicator clip | open |
| 3 | Critic: compile + full `run_ux_gate` | open |

<!-- STEP=1: red script greps solid-pass editor_rect scissor still present -->
<!-- STEP=2: content_clip solid; minimap_rect for indicator; demo_live_fs_compile_ok -->
<!-- STEP=3: Critic archive -->

## Out of scope

Reopen #37/#38 numbered STEPs; HarfBuzz wrap rewrite.
