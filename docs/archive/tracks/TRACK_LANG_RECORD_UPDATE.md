# Track: Lang Record Update (anonymous)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#29**.
Compiler track (not editor-only). Size **M**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=5):** Re-audit STEP=0–4 (`10f7fb89`…`9067d04e`).
Wire present: bare `{` → `parse_record_expr`; `named_type_name_from_type`;
infer+transform fill empty update type from base; e2e
`record_update_anonymous` in `run_e2e.sh`; `scroll.mlc`
`state_with_scroll_y/x` → `{ ...state, … }`. No `lib/mlc/` → REG skipped.
Self-host + Tier B: STEP=2 (`efd8d9c6`) identical + `build_tests.sh` exit 0;
no compiler `*.mlc` after STEP=2. **reopen: none**.

Honest residual: multi-spread RecordLit B4 still OOS; mass editor/gui
migrate not done (only scroll helpers as proof).

| Gate | Result |
|------|--------|
| `run_e2e.sh` | 10 passed, 0 failed (incl. `record_update` + `record_update_anonymous`) |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| `run_ux_wheel_scroll.sh` | `ux_ok wheel_scroll_keeps_caret_visible` EXIT=0 |
| `run_ux_shift_wheel_scrolls_horizontal.sh` | `ux_ok shift_wheel_scrolls_horizontal` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#30 EDITOR_UX_L1_BACKFILL`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `10f7fb89` | Decision freeze + open |
| 1 | `e3e6c72c` | Red e2e + harness |
| 2 | `efd8d9c6` | bare `{` parse + infer/transform type from base |
| 3 | `be970c68` | wire `run_e2e.sh`; retire check script |
| 4 | `9067d04e` | `scroll.mlc` state_with_scroll_* → anonymous update |
| 5 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Editor/gui hand-copy all record fields on partial update. Backlog wants `{ ...state, field: v }`. **Named** `TypeName { ...base, field: v }` already works. **Anonymous** was the gap |
| Goal | Anonymous single-spread + overrides → same semantics as named update; type from base |
| Surface | `{ ...base, field: value, ... }` → `ExprRecordUpdate` with type filled from base |
| Named | Unchanged |
| Ruby / B4 | Multi-spread RecordLit = out of scope |
| Editor proof | `state_with_scroll_*` migrated |
| REG | only if `lib/mlc/` touched |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red e2e harness | **done** |
| 2 | Parser + checker | **done** — Tier B + self-host |
| 3 | Wire `run_e2e.sh` | **done** — 10/0 |
| 4 | Editor scroll helpers | **done** — demo + wheel gates |
| 5 | Critic: gates; archive | **done** (closed) |

## Out of scope

Multi-spread / mid-literal spreads (B4); mass-migrate all editor/gui records; `lib/mlc/` unless REG forced.
