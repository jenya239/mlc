# MLC agent instructions

Turn workflow: [docs/agent/CONTINUITY.md](docs/agent/CONTINUITY.md)  
Development: [docs/agent/DEVELOPMENT.md](docs/agent/DEVELOPMENT.md)

## Regression gate (`compiler/**` tracks)

Before marking **closed** any track that touched `compiler/**`, run:

```bash
scripts/regression_gate.sh
```

Exit 0 required. Suite: `specs/regression/` — Ruby bootstrap stdout is source of truth; mlcc output must match frozen expected and Ruby.

Docs-only tracks (no `compiler/**` changes) are exempt.
