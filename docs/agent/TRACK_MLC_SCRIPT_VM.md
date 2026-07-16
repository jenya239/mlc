# Track: MLC Script VM — embeddable dynamic-profile VM (design-only)

Parent: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md), [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-13 — концепт embeddable VM для динамического
профиля языка (`mlc script`, implicit `dyn`), предложенный внешней моделью.

## Status: **design-only, реализация НЕ авторизована**

**НЕ путать с [TRACK_MIR_VM_FULL](TRACK_MIR_VM_FULL.md)** — это про другой
объект. Полная таблица различий: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md)
§0. Кратко: MIR VM = `compiler/vm/`, debug-интерпретатор `mlcc --run`, без
bytecode/GC/JIT, existing track. MLC Script VM = design-only, отдельный
top-level каталог `script_vm/` (design-only, каталога нет), bytecode+GC+
опциональный JIT, embedding ABI — этот трек.

**HARD STOP GATE (по прецеденту TRACK_MIR_VM_FULL Epic 5):** не открывать
STEP=1 без явной новой команды пользователя в чате. Это ~200–400+
agent-hours (полная VM с GC/JIT/embedding), на порядок больше самого
крупного текущего резервуара (`GUI_CANVAS_GRAPH`, ~100+ шагов). Planner не
подставляет этот трек в очередь `PLAN.md` автоматически — только по
явному запросу пользователя «начинай реализацию MLC Script VM» или
аналогичному.

## Non-goals (пока)

- Замена статического `native` MLC / C++ release backend — не цель, третий
  путь исполнения, не замена AOT.
- Замена MIR VM (`compiler/vm/`, `mlcc --run`) — разные назначения, оба
  остаются.
- BigInt в core, concurrent/moving GC, trace JIT (LuaJIT-style side exits),
  optimizing JIT до рабочего tier 0/0.5 — см. `MLC_SCRIPT_VM.md` §14.

## Открытые вопросы дизайна (закрыть на STEP=1, если/когда авторизуют)

1. `dyn` — новый тип в checker native-профиля (явная граница) или только
   внутри `script module`? Черновой ответ в `MLC_SCRIPT_VM.md` §3: явный в
   native, default для неаннотированных в script.
2. `script module` / `native module` — новый синтаксис на уровне модуля или
   отдельное расширение файла/манифеста? Не решено.
3. Value ABI: NaN-boxing vs tagged union — какая версия x86-64/ARM64
   реализуется первой.
4. Verifier: обязателен с первого коммита (см. `MLC_SCRIPT_VM.md` §10,
   LuaJIT precedent) — не откладывать на потом.
5. Где физически живёт код: подтвердить `script_vm/` как sibling
   `compiler/`/`runtime/`/`lib/`/`misc/`, не подкаталог ни одного из них.

## Фазы (из `MLC_SCRIPT_VM.md` §12, только для справки — не steps до авторизации)

| Фаза | Содержание | Оценка (agent-hours, грубо) |
|------|------------|------------------------------|
| 1 | VM без JIT: register bytecode, verifier, closures, arrays/records, tagged `Value`, non-moving GC, embedding ABI, native type descriptors, fibers, bytecode dump/trace | 60–100 |
| 2 | Адаптивный интерпретатор: counters, quickening, shapes, inline caches, specialized arithmetic/native calls | 40–70 |
| 3 | Baseline JIT x86-64: whole-function compile, guards+slow stubs, материальный frame, bounded code cache | 50–90 |
| 4 | Opcode DSL (interpreter+JIT+verifier из одного описания, см. `MLC_SCRIPT_VM.md` §9) | 30–50 |
| 5 | Tier 2 (function/region JIT, SSA) — только по профилям реальных приложений | не оценено, возможно не понадобится |

**Итого до фазы 4 включительно: ~180–310 agent-hours** — не начинать без
явного приоритета от пользователя; это больше, чем весь текущий backlog
`PLAN.md` §21-30 вместе взятый.

## Связь со static MLC (не нарушает существующее решение)

[LANGUAGE_DESIGN.md](../LANGUAGE_DESIGN.md) §«Что не войдёт в язык»
отклоняет встроенный `any`/`dynamic` тип для static/native профиля — это
решение остаётся в силе. `dyn`/MLC Script VM — отдельный profile с
собственным lowering, не тип, просачивающийся в native inference по
умолчанию. Обновлено cross-reference в `LANGUAGE_DESIGN.md` (см. коммит).

## Steps

Не создавать до авторизации. Когда авторизуют — Planner создаёт STEP=1 по
Decision-вопросам выше, дальше по фазам §"Фазы".
