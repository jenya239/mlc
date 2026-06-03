# Track: Structured struct_using (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_EMPTY_FRAGMENT_BRIDGE.md](TRACK_DECL_EMPTY_FRAGMENT_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, 2026-06-02)

**Goal:** eliminate `cpp_decl_from_string_output(line)` fallback in `cpp_using_from_struct_using_line` — **done** (step 3). Structured assoc-using via `struct_using_entries`; no string re-parse on native path.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Reachability audit — fallback site; `build_struct_using_lines`; callers | done |
| 2 | Structured struct_using in `context.mlc` + `module.mlc` | done |
| 3 | `struct_using_declarations_cpp` — native `CppUsing` from entries; drop parse fallback | done |
| 4 | Parity tests — AssocBind extend → record struct_using | done |
| 5 | Survivors audit; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal — struct_using string re-parse fallback | **0** — `cpp_using_from_struct_using_line` removed step 3 |
| `cpp_decl_from_string_output` in `decl_cpp.mlc` (struct_using) | **0** |
| `cpp_decl_from_string_output` export | **1** — `:27-28` (TU scaffolding API) |
| `cpp_decls_from_string_parts` | export `:33-34`; **0** codegen production callers |
| Native struct_using producer | `build_struct_using_data` → `struct_using_entries` (`module.mlc`) |
| Native struct_using consumer | `struct_using_declarations_cpp` → `CppUsing` (`decl_cpp.mlc`) |
| String struct_using consumer | `struct_extra_using` → `struct_using_lines` (`type_gen.mlc`) — parallel path, not bridged |
| Tests | `test_decl_gen.mlc` — entries/lines + native `CppUsing` (step 4) |

Result: **913 pass; DIFF_EXIT=0**.

## Deferred (out of track)

- `module.mlc` — `cpp_declarations_from_text_parts` (guards, includes, namespace scaffolding).
- `derive_methods_cpp` hash specialization `make_fragment_cpp_declaration`.
- `cpp_decl_from_string_output` export + `cpp_decls_from_string_parts` (no production consumer).
- `type_gen.mlc` — inline `struct_using_lines` in string struct body (layout ≠ native sibling `CppUsing`).
- `decl.mlc` / `decl_extend.mlc` string parity bundle.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.

## Legacy context (step 1 audit, pre-bridge)

<!-- step 1 sub-steps:
  1) grep `cpp_using_from_struct_using_line` and `struct_using_lines`
  2) trace `build_struct_using_lines` line format (`using X = Y;\\n`)
  3) note when fallback `:192` can fire
-->

## Step 1 audit (2026-06-02, historical)

| Check | Result |
|-------|--------|
| Fallback site | **1** — `:192` in `cpp_using_from_struct_using_line` |
| Last `cpp_decl_from_string_output` in `decl_cpp.mlc` | **1** — `:192` only (+ export `:27-34`) |
| `cpp_using_from_struct_using_line` callers | **1** — `struct_using_declarations_cpp:200` |
| `struct_using_declarations_cpp` callers | **1** — `append_type_body_struct_declarations:236` (single-variant record) |

| Producer | Site | Format |
|----------|------|--------|
| `build_struct_using_lines` | `module.mlc:56,77` | `'using ' + assoc + ' = ' + sem_type_to_cpp + ';\n'` |
| Sources | `SDeclExtend` / exported `SDeclExtend` | `SDeclAssocBind` only |

| Consumer | Site | Use |
|----------|------|-----|
| Native | `decl_cpp.mlc:235-236` | append `CppUsing` decls after record struct |
| String | `type_gen.mlc:192-206` | `struct_extra_using` — join lines into struct body |

| Fallback when | Production |
|---------------|------------|
| Line has no ` = ` in scan range | **unreachable** — builder always emits ` = ` |
| Malformed / manual `struct_using_lines` | defensive only |

| Tests | Gap (step 4) |
|-------|--------------|
| AssocBind extend | `:346-356` — extend loop only; **no** `struct_using_lines` in context |
| Native struct_using | **0** — no test populates `context.struct_using_lines` |

**Step 2 recommendation:** `StructUsingEntry { alias, type_cpp }`; `Map<string, [StructUsingEntry]>` in context; populate in `build_struct_using_data`; drop string re-parse + `:192` fallback. **Implemented steps 2–4.**
