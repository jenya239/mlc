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
