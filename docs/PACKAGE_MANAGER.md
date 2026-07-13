# Package manager (design freeze)

Parent: [PLAN.md](PLAN.md) §18, [agent/TRACK_PACKAGE_MANAGER.md](agent/TRACK_PACKAGE_MANAGER.md).
Status: **design frozen** 2026-07-13 (TRACK STEPs 1–4). Implementation = STEPs 5–10.

This document is the source of truth for what Steps 5–10 may build. Do not
expand scope here without a new TRACK step.

## 1. Goal

Attach third-party `.mlc` packages by **git URL + pinned commit SHA**, without a
central registry. Fetch is explicit; compile stays hermetic.

## 2. Manifest — `mlc.json`

- Location: **project root** (directory treated as compile root).
- Format: JSON (not TOML — no extra Ruby gem).
- Missing file ⇒ zero external packages (current behavior).

```json
{
  "name": "my_app",
  "version": "0.1.0",
  "dependencies": {
    "example_pkg": {
      "git": "https://github.com/org/example_pkg.git",
      "rev": "0123456789abcdef0123456789abcdef01234567"
    }
  }
}
```

| Field | Rule |
|-------|------|
| `name` / `version` | This project’s identity (strings; semver recommended, not enforced) |
| `dependencies` | Map keyed by package name |
| Package name | Exact charset **`[a-z][a-z0-9_]*`**; not `mlc_packages` |
| `git` | URL string |
| `rev` | **Full 40-char commit SHA** (tags/branches may be fetch tips only; stored pin = SHA) |
| Unknown keys | Ignore (forward-compat) |
| Empty `dependencies` | Valid |

No registry. No semver range solver. Diamond conflicts = author edits the pin.

## 3. Vendor layout — `.mlc_packages/`

- Path: **`<project_root>/.mlc_packages/<pkg>/`** = git clone root at pinned `rev`.
- Fetched artifact — add to `.gitignore` (implement with STEP=6); do not commit by default.
- Fetch CLI (STEP=6): `scripts/mlc_pkg_fetch.rb` (name may vary) — clone + checkout,
  idempotent if already at `rev`. Compile must **not** `git clone` on the fly.

## 4. Import resolution

Syntax unchanged: `import { … } from '<path>'`.

Order (STEP=7 in `resolve_import_path` / loader):

1. Path starts with `.` → relative to importer (unchanged).
2. No `/` → bare stdlib table (`Tcp`, `HttpServer`, `Env`, `Log`, `Validate`) unchanged.
3. Contains `/`, not starting with `.`, **first segment ∈ root `dependencies`**:
   - `from 'example_pkg/math'` → `.mlc_packages/example_pkg/math.mlc`
   - Append `.mlc` if missing.
4. Else → join with importer `dirname` (backward compat).

No `pkg:…` / `@pkg/…` in v1.

### Security

Resolved package path must stay under `<project_root>/.mlc_packages/<pkg>/`
(prefix check after `resolve_dotdot`; reuse `driver_source_path_is_safe`).

### Flat dependencies (v1)

Only the **root** `mlc.json` `dependencies` are resolvable. A package’s own
`mlc.json` is ignored. Transitive packages must be listed at the root.

## 5. Version skew

**No compile-time language/stdlib version gate.** Optional future
`"mlc_version"` is deferred; STEP=5 must not require it. Stdlib bare names always
come from the consumer’s `lib/mlc/common/stdlib`, never from `.mlc_packages/`.

## 6. Status quo (before Steps 5–7)

| File | Today |
|------|--------|
| `compiler/driver/path_normalize.mlc` | Relative + bare stdlib only |
| `compiler/driver/module_loader.mlc` | Loads by filesystem path; no manifest |

## 7. Implementation map (Steps 5–10)

| Step | Deliverable |
|------|-------------|
| 5 | Ruby: parse/validate `mlc.json` — **done:** `lib/mlc/package_manager/manifest.rb` |
| 6 | Fetch script → `.mlc_packages/`; `.gitignore` — **done:** `Fetcher` + `scripts/mlc_pkg_fetch.rb` |
| 7 | Resolver: first-segment dep → vendor path + prefix check |
| 8 | E2E: local git fixture + fetch + compile |
| 9 | Usage section + README one-liner (this file expanded) |
| 10 | Self-host if `compiler/` touched; else `regression_gate.sh` |

## 8. Out of scope

- Central registry / discovery
- Semver ranges / diamond solving
- Nested per-package lockfiles
- Auto-clone at compile time
- `mlc.toml`

## 9. Rejected alternatives (summary)

Registry proxies, path-only vendor without manifest, live git in `mlcc`,
hard version fail, embedding stdlib in each package — see TRACK Decisions
STEP=1–3.
