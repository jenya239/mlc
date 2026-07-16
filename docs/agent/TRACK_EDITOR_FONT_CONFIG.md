# Track: Editor font config path (DejaVuMono default)

Parent: [../PLAN.md](../PLAN.md) §39; gap from [../EDITOR.md](../EDITOR.md)
(«No font discovery API»).

## Status: **active** (2026-07-16) — queue head

After §38 folder browser closed. Next editor gap: resolve font path without
Fontconfig — config / env / well-known DejaVuMono fallback.

## Next step

**STEP=0** — Decision freeze: resolution order + gates.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| API | Pure helper e.g. `editor_resolve_font_path(...)` → `string` |
| Order | explicit arg / `MLC_EDITOR_FONT` / well-known DejaVuMono paths |
| Default | `/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf` (existing demos) |
| Wire | `demo_live` / chrome use resolver (one call site) |
| REG | Prefer no `lib/mlc/`; if touched → REG before Critic |
| Non-goals | Fontconfig; font enumeration UI; tree-sitter; SCRIPT_VM; MIR Epic 5; `compiler/` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Land resolver module + unit | unit token |
| 2 | Wire one consumer (`demo_live` or chrome) | compile gate |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze order + exact export names.
2. List well-known path candidates (Linux DejaVu).

**STEP=1**
1. `misc/editor/...` resolver; unit covers env override + default.
2. Gate: `run_editor_font_config_unit.sh` (name TBD at STEP=0).

**STEP=2**
1. Replace hardcoded DejaVu path in one live entry.
2. `run_editor_demo_live_fs_compile.sh` or equivalent.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- Fontconfig / system font enumeration
- Per-theme font picker UI
- SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
