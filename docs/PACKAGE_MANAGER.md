# Package manager

Parent: [PLAN.md](PLAN.md) §18, [agent/TRACK_PACKAGE_MANAGER.md](agent/TRACK_PACKAGE_MANAGER.md).
Status: **implemented** 2026-07-13 (TRACK STEPs 1–8; STEP=9 usage docs; STEP=10 verify).

Design freeze (STEPs 1–4) is still the source of truth for scope. Do not expand
without a new TRACK step.

## 1. Goal

Attach third-party `.mlc` packages by **git URL + pinned commit SHA**, without a
central registry. Fetch is explicit; compile stays hermetic.

## 2. Usage

1. Add project-root `mlc.json` with pinned deps (full 40-char `rev` SHA).
2. Fetch into `.mlc_packages/` (do not commit that tree):

```bash
ruby scripts/mlc_pkg_fetch.rb [project_root]
# default project_root = cwd
```

3. Import by package name as the first path segment:

```mlc
import { add } from 'example_pkg/math'

fn main() -> i32 = do
  println(`${add(40, 2)}`)
  0
end
```

4. Compile with `mlcc` from the project root (so `mlc.json` is found):

```bash
compiler/out/mlcc -o out main.mlc
```

Smoke: `bundle exec ruby compiler/tests/e2e/package_manager/run_e2e.rb`.

## 3. Manifest — `mlc.json`

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

## 4. Vendor layout — `.mlc_packages/`

- Path: **`<project_root>/.mlc_packages/<pkg>/`** = git clone root at pinned `rev`.
- Fetched artifact — listed in root `.gitignore` as `/.mlc_packages/`; do not commit by default.
- Fetch CLI: `scripts/mlc_pkg_fetch.rb` — clone + checkout, idempotent if already at
  `rev`. Compile does **not** `git clone` on the fly.
- Ruby API: `MLC::PackageManager::Manifest`, `MLC::PackageManager::Fetcher`.

## 5. Import resolution

Syntax unchanged: `import { … } from '<path>'`.

Order (`resolve_import_path` in `compiler/driver/path_normalize.mlc`):

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

## 6. Version skew

**No compile-time language/stdlib version gate.** Optional future
`"mlc_version"` is deferred. Stdlib bare names always come from the consumer’s
`lib/mlc/common/stdlib`, never from `.mlc_packages/`.

## 7. Implementation map (Steps 5–10)

| Step | Deliverable |
|------|-------------|
| 5 | Ruby: parse/validate `mlc.json` — **done:** `lib/mlc/package_manager/manifest.rb` |
| 6 | Fetch script → `.mlc_packages/`; `.gitignore` — **done:** `Fetcher` + `scripts/mlc_pkg_fetch.rb` |
| 7 | Resolver: first-segment dep → vendor path + prefix check — **done:** `path_normalize.mlc` |
| 8 | E2E: local git fixture + fetch + compile — **done:** `compiler/tests/e2e/package_manager/run_e2e.rb` |
| 9 | Usage section + README one-liner — **done** (this section + README link) |
| 10 | Verify: self-host if `compiler/` touched; else `regression_gate.sh` |

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
