# Шаг 444 — остаток после 301–443

Ограничение (В)/JIT: **не раньше lifetime модуля, slicing и security boundary**.

Lifetime модуля закрыт Phase 2. Slicing закрыт Phase 3. Security boundary закрыт Phase 7. Это условие фиксации трёх границ, не разрешение начинать (В). Разрешение — отдельное решение следующей очереди. Ниже перечислено всё, что не вошло ни в один шаг 301–443.

## (В) и JIT — не начато

- **quickening** — не начато: опирается на стабильный opcode-набор, который фиксируется только после Phase 6.
- **shapes** — не начато: требует property access pattern, которого в текущем ISA нет.
- **inline caches** — не начато: требует call-site profiling, не введённого.
- **baseline JIT x86-64** — не начато: требует материального VM frame и guard contract сверх текущего Execution.
- **tier-2 optimizing JIT** — не начато: требует профилей реальных приложений, которых после этого плана ещё не будет.

## SVGUI той же волны, не JIT, не в этом плане

- **SVGUI-24** production acceptance (§14.5 полностью) — открыто; нужен отдельный security review.
- **SVGUI-30** MCP gateway — открыто; нужен decision о версии MCP.

## (Б), не реализованное в 301–443

- **Generics в методах** (карточки 361–374): blocked после шага 302 исход (b). `lower_generic_method_unit` и `lower_generic_method.mlc` не созданы. Bounds и where-clause по-прежнему отказ. Правка `compiler/` / `lib/mlc/` этой очередью запрещена.

## Вне очереди по правилам исполнителя

- Шаг 228 не открывался.
- `misc/editor/` не трогался.
- Бюджет hover не поднимался (`hover_damage_min_us` ≤ 1000).
- MIR VM в `compiler/vm/` не становился GUI sandbox.
- `compiler/` и `lib/mlc/` не правились.

Из прежнего остатка (Б) среза 201–300 в 301–440 вошло и закрыто: живая память REPL, dump VM, fibers/slicing, инкрементальный GC, лимит байт, волны SVGUI-01…23 и SVGUI-25…29. Это не остаток.
