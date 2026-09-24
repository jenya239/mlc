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

## Performance workflow (GUI / OpenGL)

Low-level UI cannot be “vibe-coded” without a measurable loop. Architecture that looks right can still mean tens of thousands of shapes/draws per frame. Rules:

- Never claim a performance win without before/after numbers from the same harness.
- Prefer **measure → one hypothesis → one cut → remasure**. No unrelated refactor in that commit.
- **Native editor (`misc/editor/`, PLAN §33–§111) is a failed experiment (2026-09-20).** Do not resume it. Do not use `scripts/run_editor_perf_*.sh` / `demo_live` as authority. Restart surface: PLAN §32 GlyphCache + `text_ide_panels_demo`. New product track: PLAN §112 `TRACK_TEXTUI_FILE_MANAGER` (`misc/textui/`, file manager) — [docs/archive/EDITOR_EXPERIMENT_FAILED.md](docs/archive/EDITOR_EXPERIMENT_FAILED.md). Gate: `scripts/run_text_gl_perf_corpus.rb`.
- Report at least: CPU frame parts you touch (`layout_us` / `draw_us` / `total_us` or named counters); keep GPU guesses separate unless timed asynchronously.
- Do not add `glFinish`, framebuffer readback, or synchronous query reads to the normal frame path.
- Widgets / chrome produce paint data; only the shared renderer path issues OpenGL.
- **`misc/textui/`:** one concern per file, hard max 400 lines (`scripts/run_textui_file_size.rb`). No god-host (`app.mlc` wires the frame only). Split in the same STEP; no size allowlist.
- A static UI (idle, hit-stable pointer) must not reshape text / rebuild O(doc) glyph lists every frame.
- **textui hover budget (2026-09-23).** A hover / selection change paints only the rows that changed (`submit_ui_frame` → `paint_hover_damage`) and the gallery presents with `glfw_gl_swap_interval(0)`. Harness: `bash scripts/run_textui_hover_bench.sh` at 2560×1440, 80 rows, 10 samples. Gate: `hover_damage_min_us` ≤ **1000** and `hover_damage_min_us * 3 < hover_full_min_us` (`hover_damage_*` is the gallery path, not a hand-rolled submit). Run this bench after any `misc/textui/` paint, submit, hit, or frame-loop change; exit 0 required. Do not raise the budget. Do not restore SwapInterval 1 on that window.
- One measured bottleneck per Green step. If the target metric does not move → revert or narrow; do not ship “cleaner” without numbers.
- Optional deeper GL traces (`apitrace`) are allowed for diagnosis; they do not replace scripted pass/fail gates.

