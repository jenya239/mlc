# Track: GUI framework (native UI on MLC)

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md) §8,
[TRACK_TEXT_RENDERING_NATIVE.md](../archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md).

## Status: **backlog** (not active) — NATIVE Critic OK 2026-07-11

Opened as a pointer from `TRACK_TEXT_RENDERING_NATIVE` STEP=7 (2026-07-11).
NATIVE closed + Critic OK; activate only via Planner promote.

## Problem

`TRACK_TEXT_RENDERING_NATIVE` delivers a GLFW window + MLC text/GL orchestration
(foundation). A full GUI stack (layout, widgets, input routing, easing,
hit-testing) is explicitly out of scope there.

## Goal (future)

Native fast UI on MLC: layout/widgets, animations/easing, effects — built on
the windowed GL + TextRenderer path, not a C++ scene graph.

## Non-goals (v0 pointer)

- No steps yet; no code in this track until activated.
- Does not replace headless text golden tests.

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Activate after NATIVE close; write Decision (layout model, event loop). | pending |
