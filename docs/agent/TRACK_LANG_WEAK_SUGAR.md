# Track: `.weak()`/`.upgrade()` sugar on `Shared`/`Weak`

Parent: [../PLAN.md](../PLAN.md), [../MEMORY_MODEL.md](../MEMORY_MODEL.md).
Source: [../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #4
(Уровень 0 из части «память и связность»).

## Status: **open**

## STEP=1 — inventory Ruby `lib/mlc/` — **done** (2026-07-09)

| Piece | Status |
|-------|--------|
| Type `Weak<T>` | Declared in `lib/mlc/common/stdlib/core/memory.mlc`; mapped `Weak`→`std::weak_ptr` in `type_mapper.rb` |
| Free fns | `shared_downgrade`, `weak_lock`, `weak_null`, `weak_is_valid` in `memory.mlc` + codegen in `call_rule.rb` (`:shared_downgrade`, `:weak_lock`) |
| Extend API | `Shared.downgrade`, `Weak.lock` / `Weak.null` / `Weak.is_valid` in `memory.mlc` |
| Auto-import | **No** — `Shared.new` is builtin (`static_method_call_rule`); `shared_downgrade` / import `std/core/memory` failed probe (`Unknown identifier`) |
| Working path today | Inline `extend Shared/Weak` in program (see `test/integration/smart_pointers_e2e_test.rb`: `Shared.downgrade`, `Weak.lock`) |
| Sugar `.weak()` / `.upgrade()` | **Missing** — probe: `Unknown member 'weak'/'upgrade' for type Shared` |
| Instance `.downgrade()` | **Missing** as instance method — e2e uses **static** `Shared.downgrade(shared)` |

**Conclusion:** Ruby already has Weak codegen + types; ergonomic gap is (1) sugar aliases `.weak()`/`.upgrade()`, (2) reliable stdlib surface without re-declaring extend. Prefer aliases over renaming `downgrade`/`lock` (keep e2e).

## STEP=2 — Ruby: `.weak()` / `.upgrade()` sugar (lib/mlc only)

**Action:**
- Accept instance methods `weak` on `Shared<T>` → same lowering as `shared_downgrade` / `Shared.downgrade`.
- Accept instance methods `upgrade` on `Weak<T>` → same as `weak_lock` / `Weak.lock` → `Option<Shared<T>>`.
- TDD: extend `test/integration/smart_pointers_e2e_test.rb` or add `test/mlc/` case with `.weak()`/`.upgrade()` (no inline extend if possible; else document).
- Do **not** touch `compiler/` this step.

**Repro (after):**

```mlc
type Node = { value: i32 }
fn main() -> i32 = do
  let root = Shared.new(Node { value: 1 })
  let weak_ref = root.weak()
  match weak_ref.upgrade() {
    Some(p) => p.value,
    None => -1
  }
end
```

## STEP=3 — mlcc parity (compiler/ only)

Mirror sugar in checker (`infer` / builtin method types for `Shared`/`Weak`) + codegen. Separate turn.

## STEP=4 — verify-gate + optional compiler AST use of `Weak`

Self-host diff + one real `Weak` use if a cycle site exists.

## Verify gate (track close)

```bash
bundle exec rake test_mlc   # after STEP=2
# after STEP=3:
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
…
```
