# Track: Editor stale help text (folder UX copy)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#1c**.
Found 2026-07-17: in-app README/guide/changelog still described tree
expand/collapse; live UI is breadcrumb + back/forward (§38).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=2):** Re-ran compile gate. Anti-false-done:
`c034c66f`…`3bb3707d` (STEP=0–1). Decision met: no `expand/collapse` /
`[+]/[-]` left in `demo_live` samples/header; breadcrumb+nav copy present.
Copy-only; no `compiler/` / `lib/mlc/`. **reopen: none**.

| Gate | Result |
|------|--------|
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#1d EDITOR_LARGE_FILE_NO_FULL_STRINGIFY`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `c034c66f` | Decision freeze + open |
| 1 | `3bb3707d` | sample_* + header copy |
| 2 | this Critic | close + archive |

## Delivered

In-app demo help strings match folder browser breadcrumbs + back/forward.

## Out of scope (unchanged)

Behavior changes; #1d; keyboard nav; `compiler/`.
