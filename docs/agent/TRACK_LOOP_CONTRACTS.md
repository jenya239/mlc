# Track: Ralph/G3 loop contracts

Parent: [../PLAN.md](../PLAN.md) §Phase 1

## Status: **open**

**Goal:** три механических улучшения цикла агента:
1. Self-host regression gate как стандарт для всех треков с `compiler/**`
2. `MAX_ITER` лимит в заголовке каждого трека
3. `specs/index.md` — карта доков для on-demand загрузки

**Constraint:** не менять тесты, не менять логику компилятора — только инфраструктура.

---

| Step | Item | Status |
|------|------|--------|
| 1 | Выбрать 10-20 test-программ из `test/`, зафиксировать эталонный вывод в `specs/regression/` | done (`a7c752cd`) |
| 2 | Написать `scripts/regression_gate.sh` — запускает frozen suite, сравнивает вывод Ruby vs C++, exit 1 при расхождении | done (`1ccea8f6`) |
| 3 | Добавить в `AGENTS.md`: любой трек затрагивающий `compiler/**` обязан пройти `scripts/regression_gate.sh` перед закрытием | done |
| 4 | Добавить `MAX_ITER: 20` и `GATE:` поле в заголовки всех открытых треков | pending |
| 5 | Создать `docs/specs/index.md` — карта существующих доков (`LANGUAGE_DESIGN.md`, `CPP_PARSER_DESIGN.md`, `MEMORY_MODEL.md` и др.) с тегами по теме | pending |
| 6 | Обновить `AGENTS.md` и `CONTINUITY.md` — агент читает `specs/index.md` и грузит только релевантные доки для текущего трека | pending |

---

## Verify gate

```bash
scripts/regression_gate.sh   # exit 0 (добавляется на шаге 2)
```

## Заметки

**Regression suite — критерии:**
- программы покрывают: literals, functions, conditionals, loops, structs, method calls
- хотя бы одна программа с self-hosting round-trip
- эталонный вывод берётся от Ruby bootstrap — он источник истины

**MAX_ITER смысл:**
- после 20 итераций агент пишет structured status report в трек и останавливается
- явный сигнал что нужна ручная помощь, не молчаливое зависание

**specs/index.md смысл:**
- docs уже разбиты по темам (LANGUAGE_DESIGN, CPP_PARSER_DESIGN, MEMORY_MODEL и др.)
- index.md добавляет теги: какой документ грузить при каком типе трека
- снижает токен-стоимость, меньше шума в контексте
