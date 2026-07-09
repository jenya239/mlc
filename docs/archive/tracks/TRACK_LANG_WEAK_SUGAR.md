# Track: `.weak()`/`.upgrade()` sugar on `Shared`/`Weak`

Parent: [../PLAN.md](../PLAN.md), [../MEMORY_MODEL.md](../MEMORY_MODEL.md).
Source: [../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #4
(Уровень 0 из части «память и связность»).

## Status: **closed** (2026-07-09)

## STEP=1 — inventory Ruby `lib/mlc/` — **done** (2026-07-09)

| Piece | Status |
|-------|--------|
| Type `Weak<T>` | Declared in `lib/mlc/common/stdlib/core/memory.mlc`; mapped `Weak`→`std::weak_ptr` in `type_mapper.rb` |
| Free fns | `shared_downgrade`, `weak_lock`, `weak_null`, `weak_is_valid` in `memory.mlc` + codegen in `call_rule.rb` (`:shared_downgrade`, `:weak_lock`) |
| Extend API | `Shared.downgrade`, `Weak.lock` / `Weak.null` / `Weak.is_valid` in `memory.mlc` |
| Auto-import | **No** — `Shared.new` is builtin (`static_method_call_rule`); `shared_downgrade` / import `std/core/memory` failed probe (`Unknown identifier`) |
| Working path today | Inline `extend Shared/Weak` in program (see `test/integration/smart_pointers_e2e_test.rb`: `Shared.downgrade`, `Weak.lock`) |
| Sugar `.weak()` / `.upgrade()` | Shipped in STEP=2 (Ruby) + STEP=3 (mlcc) |

**Conclusion:** Ruby already had Weak codegen + types; ergonomic gap closed via `.weak()`/`.upgrade()` aliases.

## STEP=2 — Ruby: `.weak()` / `.upgrade()` sugar (lib/mlc only) — **done** (2026-07-09)

| Piece | Change |
|-------|--------|
| Infer | `type_inference_service.rb`: `infer_smart_pointer_method_type` + `resolve_smart_pointer_member` — Shared `weak`/`downgrade` → `Weak<T>`; Weak `upgrade`/`lock` → `Option<Shared<T>>` |
| Codegen | `call_rule.rb`: `Shared_weak`/`Weak_upgrade` aliases; `lower_smart_pointer_instance_method` before Result/Option dispatch |
| Test | `smart_pointers_e2e_test.rb` `test_shared_weak_upgrade_sugar` — no inline extend; exit 77 |

## STEP=3 — mlcc parity (compiler/ only) — **done** (2026-07-09)

| Piece | Change |
|-------|--------|
| Infer | `infer_weak_method.mlc`: Shared.new → `TShared`; `.weak`/`.downgrade` → `Weak<T>`; `.upgrade`/`.lock` → `Option<Shared<T>>` |
| Transform | `transform.mlc`: Shared.new + weak/upgrade result types |
| Codegen | `weak_method_gen.mlc` + `method_gen.mlc` dispatch; `type_gen.mlc` Weak→`std::weak_ptr` |
| Test | `test_weak_sugar.mlc` (check + codegen contains) |

## STEP=4 — verify-gate + optional compiler AST use of `Weak` — **done** (2026-07-09)

| Check | Result |
|-------|--------|
| `compiler/build.sh` | ok |
| self-host p1 / mlcc2 / p2 | identical (`diff -rq` empty) |
| `scripts/regression_gate.sh` | 20/0 |
| Optional `Weak` in compiler AST | **skipped** — no parent/cycle ownership site in `compiler/**/*.mlc`; adoption deferred to TRACK_LANG_CYCLE_LINT |

## Verify gate (track close)

```bash
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
MLC_CXX=g++ compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2 --exclude=obj
scripts/regression_gate.sh
```
