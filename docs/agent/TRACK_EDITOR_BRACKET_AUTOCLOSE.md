# Track: Editor Bracket Autoclose

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#18**.
Typing `(` / `[` / `{` / `"` inserts only the opener; closing char is not
paired; typing a closer always inserts a second copy even when the next
character is already that closer. Review gate: `typing_paren_autocloses` (L1).
Size **M**.

## Status: **active** (2026-07-19) — STEP=1 done; next STEP=2

## Next step

**STEP=2** — autoclose API + demo_live / app insert wire.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Opener chars insert alone; no skip-over when caret sits before an existing closer |
| Pairs | `(`→`)`, `[`→`]`, `{`→`}`, `"`→`"` only (no `'` / backtick this track) |
| Open (collapsed) | Single-char insert of opener → insert **opener+closer**, caret **between** |
| Skip-over (collapsed) | Single-char insert of closer when next document byte is that closer → **do not insert**; advance caret past it |
| Selection wrap | Non-empty selection + opener → wrap selection (`open` + selected + `close`); caret after close (selection collapsed after wrap). Closer with selection → plain insert (no special wrap) |
| Multi-char | `text.length() != 1` → plain `editor_ux_insert_text` (paste/batch unchanged) |
| API | `editor_ux_insert_text_autoclose(state, text) -> EditorUxState` (or thin edit helper + ux wrapper); L1 calls API without GLFW |
| Live | `demo_live` / `editor_app_insert_text` path uses autoclose helper for typed buffer |
| Scenarios | L1: `typing_paren_autocloses` — type `(` → text `()` caret at 1; type `)` → skip to after `)`; token `ux_ok typing_paren_autocloses` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` document/ux edit (+ scenario/script), thin app/demo_live. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok typing_paren_autocloses` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_enter_keeps_indent.sh` → `ux_ok enter_keeps_indent` (insert/newline path) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`typing_paren_autocloses`) | **done** (2026-07-19) — stub red: `ux_fail paren autoclose text` |
| 2 | autoclose API + demo_live / app insert wire | pending |
| 3 | Critic: gates; archive | pending |

## Out of scope

Single-quote / backtick; language-aware rules; triple-quote; smart delete of empty pair; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
