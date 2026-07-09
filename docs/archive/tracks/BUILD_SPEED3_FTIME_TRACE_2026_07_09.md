# ftime-trace STEP=2 (TRACK_BUILD_SPEED3) — 2026-07-09

## Target TU

Largest by line count: `compiler/out/exprs.cpp` (1245 lines).
(Previous SPEED2 used `transform.cpp`; current largest is `exprs.cpp`.)

## Command

```bash
clang++ -std=c++20 -ftime-trace=.tmp_ftime/exprs.json \
  -I compiler/out -I runtime/include \
  -c compiler/out/exprs.cpp -o .tmp_ftime/exprs.o
```

Wall: **9.95s** (no PCH, default -O0-ish compile of single TU).

## Top cumulative event names

| Event | Cumulative | Count |
|-------|------------|------:|
| InstantiateFunction | 38627 ms | 7220 |
| Source | 18109 ms | 285 |
| Frontend (Total) | 8287 ms | 1 |
| InstantiateClass | 8267 ms | 2818 |
| Backend (Total) | 1220 ms | 1 |

`Total InstantiateFunction` wall-attributed: **4437 ms** (nested events over-count when summed raw).

## InstantiateFunction by type family (args.detail)

| Family | Cumulative | Count |
|--------|------------|------:|
| other | 22391 ms | 5511 |
| variant/visit | 13794 ms | 1268 |
| shared_ptr/Shared | 2055 ms | 358 |
| map/HashMap | 155 ms | 41 |
| mlc::Array | 132 ms | 10 |
| std::function | 89 ms | 22 |
| optional | 11 ms | 10 |

Keyword rollup on detail strings: `variant` ~26886 ms / 5057; `shared_ptr` ~3046 ms; `String` ~2223 ms; `Array` ~745 ms; **`function` ~89 ms**.

Top symbols are `std::__detail::__variant::__gen_vtable_impl` / visit over **AST Expr** and **TokenKind** variants — not `std::function` / `mlc::Array`.

## Qualitative vs TRACK_LANG_CLOSURE_ESCAPE

Non-escaping fn params → `template<class F>` reduced `std::function` cost to noise (**89 ms** / 22 instantiations on this TU).  
`Shared`/`Array` are secondary. Dominant cost remains **large `std::variant` AST/token instantiations**.

## Implication for STEP=3

- (a) `extern template` for Shared/Array/std::function — **not justified** by this table.
- (b) ninja/bash orchestration — not indicated by single-TU profile (Frontend+Instantiate dominate).
- (c) **chosen (2026-07-09 STEP=3):** no further work in this track without a new measured bottleneck matching (a) or (b). Remaining compile cost is large AST/`TokenKind` `std::variant` — that would be a **separate** design track (tagged unions / smaller IR), not `extern template` and not ninja.
