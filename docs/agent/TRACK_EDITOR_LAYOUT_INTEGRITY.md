# Track: Editor product shell + agent visual integrity (§111)

Parent: [../PLAN.md](../PLAN.md) §111.
Authorized **2026-08-06** by user: live feels faster but still «криво»;
«почему это demo_live… полноценный подпроект»; need a system so AI agents
build a **clear interface** (nothing overlaps); first **study how agents can
do this themselves** — many visual artifacts remain.

**Not** a big-bang rewrite of paint (§110 closed). **Not** SceneNode chrome
migration. Focus: **layout contract + agent-checkable visual invariants**.

Standing: [AGENTS.md](../../AGENTS.md) Performance workflow still applies to
CPU. This track adds the missing **geometry / overlap / bleed** loop.

## Status: **open** 2026-08-07 — §111a1 **CLOSED**; §111a study when queued

([TRACK_EDITOR_SCROLL_GLYPH_X](TRACK_EDITOR_SCROLL_GLYPH_X.md) Critic OK).
§111a0 CLOSED. Residual: S2 wrap-aware caret/scroll, S4 shell layout, S5 minimap scale.

## Why it still feels crooked

§109/§110 fixed **cost** (idle, O(doc) glyphs, paint submit). They did **not**
own **spatial truth**:

| Layer we have | What agents can prove today | Gap |
|---------------|----------------------------|-----|
| L0/L1 semantic UX | tabs/focus/selection via inject | Does not see pixels overlapping |
| Overflow/scissor probes | text bleeds past clip (some) | Not a full shell partition |
| Dogfood CPU gates | still/scroll budgets | Green while chrome looks wrong |
| `demo_live.mlc` | product orchestration (~3k lines) | Named like a throwaway; layout math scattered |

Visual artifacts (overlap, bleed into minimap, tab titles through neighbors,
misaligned selection vs glyphs) are a **different failure class** than lag.
Agents keep shipping “reasonable” local geometry because **no gate fails**.

## Destination (plain)

```text
named product entry (not “demo”)
    → shell layout solver (one function: window → non-overlapping region map)
    → hit-test + paint consume the same map
    → agent gates: partition / contain / no-bleed / optional pixel strip
```

Agents must be able to change chrome **only** by editing the solver +
scenarios — not inventing new ad-hoc rects in the paint loop.

## How agents can do this themselves (study — freeze in §111a)

### Principle

Give the agent the same loop as perf: **hypothesis → change → machine
pass/fail**. For visuals, pass/fail must be **geometry-first**, pixels rare.

### Required instruments (build in order)

1. **`EditorShellLayout` record** — every chrome region as named rects:
   `window`, `menu/title`, `tab_strip`, `toolbar`, `tree`, `divider`,
   `gutter`, `text`, `minimap`, `h_scroll`, `v_scroll`, `status`,
   `overlay` (modal; may cover, but must be explicit).
2. **Partition checker** (pure MLC, no GPU):
   - pairwise **interior overlap area == 0** for exclusive regions;
   - each child **contained** in parent;
   - scrollbars abut content edges within 1px tolerance;
   - optional: sum of exclusive areas ≤ window (no “lost” space required).
3. **Paint/hit consume one map** — sabotage gate: paint path that uses a
   rect not taken from the current shell layout fails static/L1 check.
4. **Bleed scenarios** (extend overflow): wrapped text must not draw inside
   `minimap_rect` / tree; tab labels clipped to tab slot.
5. **Agent playbook** (short, in AGENTS or this track):
   - Before UI chrome change: run partition + named bleed scenarios.
   - One region family per Green (tabs **or** tree **or** minimap…).
   - Screenshots / MAE only for frozen strips when L1 cannot see the bug.
6. **Rename product surface** — `misc/editor/app_main.mlc` (or
   `mlc_editor.mlc`) + `scripts/run_editor.sh`; keep `demo_live.mlc` as
   thin alias **one** release, then delete alias. Docs/EDITOR say “editor
   app”, not demo.

### What does **not** work for agents (avoid)

- “Look at the screenshot and fix” as the only gate.
- Full-window PNG golden every commit (flaky DPI).
- Asking the LLM to “make layout nice” without a rect map API.
- Parallel geometry: probe sizes ≠ live sizes (already a known disease).

### Optional later

- Xvfb + small **region** FBO MAE (caret strip, tab strip) — L3, rare.
- Constraint DSL (Row/Column/Split) generating `EditorShellLayout` —
  only after partition gates exist.

## Path (ordered)

| Phase | Name | Ships | Gate |
|-------|------|-------|------|
| **§111a** | Study freeze + inventory | Decision: region set, exclusive vs overlay, alias rename plan; inventory current live rect sources in `demo_live` / `chrome` / `shell` | Decision table in track; Red = no inventory / no partition API stub |
| **§111b** | `EditorShellLayout` + partition L1 | Pure layout fn + `shell_rects_no_overlap` (+ contain) scenarios | L1 green; sabotage overlapping tree/text fails |
| **§111c** | Live wire one map | Live frame builds shell layout once per resize/layout_gen; hit + chrome paint read it | Static/L1: no ad-hoc tree/text rect literals outside solver (allowlist) |
| **§111d** | Bleed pack | Scenarios: wrap↛minimap, tab title↛neighbor, selection x vs metrics | Fail on known artifact classes |
| **§111e** | Product rename | `run_editor.sh` + app entry name; docs; deprecate `demo_live` alias | Scripts/docs use product name; dogfood gate still green |
| **§111f** | Agent checklist | Short AGENTS § Visual layout; fold partition into `run_ux_gate` or `run_editor_layout_gate.sh` | Gate ×2; sabotage overlap fails |

Non-goals: rewriting glyph batch; Script VM; compiler §104; SceneNode chrome.

## Queue

After current §104-6 work **or** when user prioritizes editor UX again —
Planner places §111a. Do not silently preempt §104 without override.

## Relation

- §109/§110 = speed + frame/paint plumbing.
- §111 = **spatial product integrity** + agent autonomy on chrome.

## Notes

2026-08-06: path + study written; no code.
