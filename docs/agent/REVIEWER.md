# Reviewer role

**STEP:** `history-review`  
**Frequency:** every 35 Driver turns (see ROLES.md)

## Goal

Look at recently closed tracks and find what was missed, deferred, or should spawn a new track.

## Process

1. Read the last 5�7 closed TRACK_*.md files (sort by `## Status: **closed**` date).
2. For each closed track scan for:
   - Steps marked `pending` or never committed (compare status vs `git log --oneline`).
   - `TODO`, `FIXME`, `deferred`, `later`, `workaround` comments in touched source files.
   - Known limitations noted in the track but without a follow-up track created.
   - Self-host failures or non-empty diffs that were noted but not resolved.
3. Check PLAN.md for any item that should have been created as a TRACK after closure but wasn't.
4. For each gap found, either:
   - Create a new `TRACK_*.md` with pending steps (if ready to implement), OR
   - Add an entry to `RESEARCH.md` (if needs investigation first), OR
   - Log a note in mlc-memory `known_limitations`.
5. Write a brief summary of findings in SESSION.md (`done` field).
6. Enqueue Driver or Planner as appropriate.

## Scope

- Read-only on `compiler/` and `lib/mlc/` � no code changes.
- May create/update `docs/agent/TRACK_*.md`, `docs/agent/RESEARCH.md`.
- One `reviewer: history-review` commit for doc changes.
