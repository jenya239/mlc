# Track: FFI safety contract

Parent: [../../PLAN.md](../../PLAN.md) §14, [../../FFI_LAYER.md](../../FFI_LAYER.md) §9
(инфраструктура уже реализована, `TRACK_FFI_LAYER` closed 2026-07-09 — этот
трек только про безопасность использования, без нового codegen).

## Status: **closed** (2026-07-11) — awaiting Critic

**Driver 2026-07-11 STEP=5:** `regression_gate` 20/0; self-host p1↔p2 DIFF 0
(`mlcc`→`mlcc2` via `build_bin.sh`); arity+concurrency smokes ok. TRACK
archived. Close → Critic.

**Driver 2026-07-11 STEP=4:** Rewrote `FFI_LAYER.md` §9 — contract table
(`W-EXTERN-ATTR` / `W-EXTERN-ARITY`; lifetime/null = author duty; Decision B).

**Driver 2026-07-11 STEP=3:** `W-EXTERN-ARITY` in `extern_header_arity_lint.mlc`
(hook after concurrency lint). Header stubs tagged
`ExprExtern(c_name, "__mlc_header_import__", …)` including `CppFnProto` path;
`PatternUnit` params (avoid E074). Smoke `run_extern_header_arity_smoke.sh` ok.

**Driver 2026-07-11 STEP=2:** `W-EXTERN-ATTR` warning in
`extern_concurrency_lint.mlc` (hook in `gather_program_check`). Warn only when
`from "<header>"` present (shorthand cannot carry attrs). Annotated stdlib
full-form: `env.mlc` + `tcp.mlc` → `blocking` (11 sites). Smoke
`run_extern_concurrency_attr_smoke.sh` ok. Tests also in `test_checker.mlc`
(run_tests rebuild blocked: Ruby parse `spawn_capture` / mlcc tests path).

**Driver 2026-07-11 STEP=1:** Decision **locked**. Inventory 255 shorthand /
0 attrs; full-form was 11.

**Planner 2026-07-11:** Activated after GLAD Critic OK.

## Проблема

`extern fn`/`RawPointer[T]`/`extern type` — полный unsafe escape hatch без
единого маркера в языке. Риски: lifetime, null, ABI mismatch, missing
concurrency attrs.

## Задача трека

Не менять codegen — диагностики checker + документация. Синтаксический
`unsafe` **не** вводится (Decision B).

## Decision (STEP=1) — **locked 2026-07-11**

| Вопрос | Locked |
|--------|--------|
| Синтаксический маркер `unsafe` | **B** — не нужен |
| STEP=2 severity | **warning** (`W-EXTERN-ATTR`) |
| Default при отсутствии attr | **нет implicit attr** |
| Scope of warning | Only decls with non-empty `from "<header>"` (attrs syntactically possible) |
| Stdlib full-form | Annotated `blocking` on env+tcp (11) |

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design-решение: marker vs diagnostics-only. | **done** |
| 2 | Checker warning missing concurrency attr + annotate full-form stdlib. | **done** |
| 3 | Checker: базовая ABI sanity-проверка arity vs header_import (best-effort). | **done** |
| 4 | Документация: Safety contract в `FFI_LAYER.md` §9. | **done** |
| 5 | Verify-gate + close: self-host diff identical, `regression_gate.sh`. | **done** |

## Out of scope

- Полный borrow checker для `RawPointer`.
- Автогенерация ABI из `.h` без `header_import`.
- Новый синтаксис `unsafe { ... }` / `unsafe extern fn`.
- Mass-annotate shorthand `extern fn` (cannot carry attrs today).

## Residuals (for Critic)

- Arity lint best-effort by C symbol name only (no `from` path ↔ import path link).
- `misc` full-form may still emit `W-EXTERN-ATTR` if unannotated.
- Ruby `spawn_capture` parse / `build_tests_self` path pre-broken (not this track).
