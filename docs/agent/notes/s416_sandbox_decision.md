# Шаг 416 — sandbox entry без правки compiler

Решение: продолжать. Новая точка входа `script_host/tools/sandbox_main.mlc` собирается уже существующим `compiler/out/mlcc -o <dir> <entry>` и `compiler/build_bin.sh`. Frontend компилятора и `lib/mlc/` не нужны. Остановки очереди нет.

Транспорт: сначала loopback (карточки 421–422), затем child + launcher (423–424). SVGUI-24 в этой очереди не открывается.

## Почему compiler/** не требуется

`mlcc` принимает любой `.mlc` с `fn main() -> i32`. Так уже собираются unit-тесты: `scripts/run_script_vm_unit.rb` вызывает `compiler/out/mlcc -o tmp/script_vm_<имя> <entry>` и линкует `compiler/build_bin.sh`.

Линк выбирает сгенерированный `main` через `MLCC_ENTRY_BASENAME` (`compiler/build_bin.sh`, по умолчанию `main`). Для песочницы: `MLCC_ENTRY_BASENAME=sandbox_main`. Список `mlcc_only_skip_source` касается `tests_main.cpp` / `test_*.cpp` при сборке самого `mlcc`, не `sandbox_main.cpp`.

Отдельный CLI-флаг или правка `compiler/frontend` для «ещё одного entry» не нужны: entry — файл, переданный `-o`.

Первая версия sandbox принимает compiled bytecode, не source. Полный frontend внутрь runtime не тащится (`docs/MLC_SCRIPT_VM_GUI_IMPLEMENTATION_RU.md` §10.1).

## Файлы вне compiler/** и lib/mlc/**

Создавать по карточкам 417–424, не в этом шаге:

- `script_host/protocol.mlc`
- `script_host/tests/ipc_codec_unit.mlc`
- `script_host/tests/transport_loopback_unit.mlc`
- `runtime/include/mlc/script_host/transport.hpp`
- `runtime/src/script_host/transport_loopback.cpp`
- `script_host/tools/sandbox_main.mlc`
- `runtime/include/mlc/script_host/launcher.hpp`
- `runtime/src/script_host/launcher.cpp`
- `script_host/tests/sandbox_policy_unit.mlc`
- при необходимости Ruby-скрипт сборки по образцу `scripts/run_script_vm_unit.rb` (`MLCC_ENTRY_BASENAME=sandbox_main`)

Native transport/launcher линкуются как остальной `runtime/`, не как модули `compiler/`.

## Не блокер

Карточка требует остановку только если нужен compiler frontend для нового entry. Этого нет. `tests_main.mlc` в подкаталоге `compiler/tests/` — чужой pre-existing import-path; `script_host/tools/sandbox_main.mlc` этому пути не следует.
