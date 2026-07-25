# Track: Wheel-scroll-on-hover independent of focus — protective regression scenario

Parent: [../PLAN.md](../PLAN.md) §96. **Superseded by §97** — executed as
§97c add-on under
[TRACK_EDITOR_RENDER_ARCHITECTURE](TRACK_EDITOR_RENDER_ARCHITECTURE.md)
(§96 wheel-hover Decision frozen 2026-07-25). Do not reopen numbered STEPs here.

## Status: **closed** via §97c add-on (Critic OK 2026-07-25)

## Code-read finding (2026-07-25, no fix expected)

`demo_live.mlc` already implements this correctly: the tree wheel-scroll
gate (`tree_hovered`) and the main-content wheel-scroll gate
(`point_in_rect(mouse, editor_rect)`) both check only mouse
position, never `app.editor_focused`. Protective L0 harness lives under §97c.
