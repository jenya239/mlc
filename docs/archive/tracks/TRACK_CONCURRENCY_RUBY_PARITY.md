# Track: concurrency (`spawn`/`Mutex`/`Channel`/`Task`) отсутствует в Ruby-пайплайне; `Tcp` stdlib отсутствует в self-hosted

Parent: [../../CONCURRENCY_V2.md](../../CONCURRENCY_V2.md),
[TRACK_STDLIB_NET_SERVER.md](TRACK_STDLIB_NET_SERVER.md).
Найдено 2026-07-10 при попытке собрать многопоточный TCP-сервер: два
языковых feature-набора реализованы в двух непересекающихся компиляторах.

## Status: **closed** (2026-07-10) — STEP=1–3 **done**

**Planner 2026-07-10:** выбран после closed SPAWN_DOUBLE_EXEC.

**Driver 2026-07-10:** STEP=1 — Decision **C** (document temporary split).

**Driver 2026-07-10:** STEP=2 — `block_on`/`is_ready` in registry + globals;
spawn gate uses `block_on`.

**Driver 2026-07-10:** STEP=3 — `MLC.md` § «Два пайплайна» + README pointer;
track archived.

**Critic 2026-07-10:** `critique-audit` — commits `02fe9f1f`/`292ec125`/`1136f463`
match STEP 1–3; PLAN §8/§8b/queue/archive synced; `--check-only`+spawn gate
PASS. Residuals (no reopen): new `test_spawn` cases not in stale `run_tests`
(Ruby `build_tests` parse fail pre-existing); option B not opened as TRACK;
`block_on` typed as `Task<_>`→`Unknown`.

## Проблема

1. **`spawn`/`Mutex`/`Channel`/`Task` — только self-hosted.** `rg spawn lib/mlc`
   → 0. Ruby `MLC.build_project` → `Unknown identifier 'spawn'`.

2. **`Tcp` stdlib — только Ruby.** `lib/mlc/common/stdlib/net/tcp.mlc`;
   `rg common/stdlib compiler` → 0. `mlcc` + `import Tcp` fails.

3. **Следствие:** нет одного компилятора с TCP + языковым `spawn`.

4. **`block_on`/`is_ready`** — были только в codegen map; **STEP=2** зарегистрировал
   их в checker (2026-07-10).

## Практический обход (NET_SERVER)

STEP=7: C++ `serve_http_with_thread_pool` — TCP+пул без языкового `spawn`.

## Decision (STEP=1, 2026-07-10) — **C**

**Chosen: C — document temporary pipeline split.**

| Option | Verdict |
|--------|---------|
| **A** Port concurrency into Ruby | **Reject.** |
| **B** Port `common/stdlib/` into `mlcc` | **Defer** (follow-up track). |
| **C** Document split | **Accept.** |

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design A/B/C | **done** (C) |
| 2 | `block_on`/`is_ready` in checker | **done** |
| 3 | Document in `MLC.md`/`README.md`; close | **done** (2026-07-10) |

## Out of scope

- Implementing A or B.
- Full pipeline merge.
- [TRACK_LANG_SPAWN_DOUBLE_EXEC](TRACK_LANG_SPAWN_DOUBLE_EXEC.md) (**closed**).
