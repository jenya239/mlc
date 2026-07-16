# Track: Editor font config path (DejaVuMono default)

Parent: [../PLAN.md](../PLAN.md) §39; gap from [../EDITOR.md](../EDITOR.md)
(«No font discovery API»).

## Status: **active** (2026-07-16) — queue head

## Next step

**STEP=2** — Wire `demo_live`.

### STEP=1 done (2026-07-16)

- Landed `misc/editor/config/font_path.mlc` + unit + gate (default + env override).
- Gate: `bash scripts/run_editor_font_config_unit.sh` → `font_config_unit ok`.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §39 → STEP=1.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in existing hardcoded
`/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf` in `demo_live.mlc` /
`main.mlc` / `gutter_smoke.mlc`, plus `get_or` from stdlib `env`.

| Item | Choice |
|------|--------|
| Module | `misc/editor/config/font_path.mlc` |
| API | `editor_default_font_path() -> string`; `editor_resolve_font_path(explicit: string) -> string` |
| Order | (1) non-empty `explicit`; (2) `get_or("MLC_EDITOR_FONT", "")` if non-empty; (3) first existing well-known path; (4) primary default string even if missing (caller / FreeType fails later) |
| Exists check | `file_exists` from §36 IO (`mlc/io/file_abi.hpp`) for candidates only |
| Primary default | `/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf` |
| Wire (STEP=2) | `demo_live.mlc` only (one call site); leave `main.mlc` / tests unless trivial |
| REG | Prefer **no** `lib/mlc/`; if touched → REG before Critic |
| Namespace | Editor-only under `misc/editor/`; **no** `compiler/` |

### Exact exports

```text
editor_default_font_path() -> string
editor_resolve_font_path(explicit: string) -> string
```

### Well-known candidates (Linux, first existing wins after env)

1. `/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf`
2. `/usr/share/fonts/TTF/DejaVuSansMono.ttf`
3. `/usr/local/share/fonts/truetype/dejavu/DejaVuSansMono.ttf`

### Gate names

| Step | Script / token |
|------|----------------|
| 1 | `scripts/run_editor_font_config_unit.sh` → `[mlc-editor] font_config_unit ok` |
| 2 | `scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

### Non-goals (Decision)

Fontconfig; font enumeration UI; tree-sitter; SCRIPT_VM; LANG_AUTO_CYCLE;
MIR Epic 5; `compiler/` changes; rewriting every demo/smoke path in one STEP.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Land resolver module + unit | `font_config_unit ok` |
| 2 | Wire `demo_live` | `demo_live_fs_compile_ok` |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze order + exact export names — done.
2. Well-known path candidates — done.

**STEP=1**
1. `misc/editor/config/font_path.mlc` + `misc/editor/tests/font_config_unit.mlc`.
2. Gate: `bash scripts/run_editor_font_config_unit.sh`.

**STEP=2**
1. `demo_live.mlc`: replace hardcoded path with `editor_resolve_font_path("")`.
2. `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- Fontconfig / system font enumeration
- Per-theme font picker UI
- SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
