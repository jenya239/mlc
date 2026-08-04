# MLC agent instructions

Turn workflow: [docs/agent/CONTINUITY.md](docs/agent/CONTINUITY.md)  
Development: [docs/agent/DEVELOPMENT.md](docs/agent/DEVELOPMENT.md)

## On-demand specs

Read [docs/specs/index.md](docs/specs/index.md) before loading product docs. Load only rows whose **tags** match the active track; do not bulk-read `docs/`.

Language syntax reference (human-oriented, not agent cheatsheet):
[docs/LANGUAGE_REFERENCE.md](docs/LANGUAGE_REFERENCE.md).

## Regression gate (`compiler/**` tracks)

Before marking **closed** any track that touched `compiler/**`, run:

```bash
scripts/regression_gate.sh
```

Exit 0 required. Suite: `specs/regression/` — Ruby bootstrap stdout is source of truth; mlcc output must match frozen expected and Ruby.

Docs-only tracks (no `compiler/**` changes) are exempt.

## Performance workflow (GUI / editor / OpenGL)

Low-level UI cannot be “vibe-coded” without a measurable loop. Architecture that looks right can still mean tens of thousands of shapes/draws per frame. Rules:

- Never claim a performance win without before/after numbers from the same harness.
- Prefer **measure → one hypothesis → one cut → remasure**. No unrelated refactor in that commit.
- Editor dogfood authority while §109 is open: scripts under `scripts/run_editor_perf_*.sh` + PERF_FULL / dogfood metrics written into [docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md](docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md). Manual “launch and look” is not verification.
- Report at least: CPU frame parts you touch (`layout_us` / `draw_us` / `total_us` or named counters); keep GPU guesses separate unless timed asynchronously.
- Do not add `glFinish`, framebuffer readback, or synchronous query reads to the normal frame path.
- Widgets / chrome produce paint data; only the shared renderer path issues OpenGL.
- A static UI (idle, hit-stable pointer) must not reshape text / rebuild O(doc) glyph lists every frame.
- One measured bottleneck per Green step. If the target metric does not move → revert or narrow; do not ship “cleaner” without numbers.
- Optional deeper GL traces (`apitrace`) are allowed for diagnosis; they do not replace scripted pass/fail gates.
