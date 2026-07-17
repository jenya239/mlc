# Track: Editor large-file — no repeated full stringify

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#1d**
(pulled forward from #22 — frame-latency / Sublime-parity).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L2 + compile. Anti-false-done:
`7e796d7a`…`62425957` (STEP=0–2). Hot path: no `line_index_rebuild`; no
triple `document_to_string` in frame loop — only `document_frame_snapshot`
(+ save-path stringify). Snapshot body: one flatten + `line_index_from_string`.
End match-guard skips redundant resnapshot when index matches. **reopen: none**.

Honest residual: each **edit** still resnapshots (one flatten per mutation) —
required until incremental line_index; zero-flatten piece ranges remain out of
scope.

| Gate | Result |
|------|--------|
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#2 EDITOR_KEYBOARD_NAV_WIRE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `7e796d7a` | Decision freeze + open |
| 1 | `1d0d5c2d` | L2 red harness |
| 2 | `62425957` | snapshot API + demo_live wire |
| 3 | this Critic | close + archive |

## Delivered

`document_frame_snapshot` + demo_live reuse; L2 gate on flatten_count=1.

## Out of scope (unchanged)

Zero-flatten ranges; #2 keyboard nav; `compiler/`.
