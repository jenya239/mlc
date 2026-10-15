# Архитектура MLC VM в нативной OpenGL GUI-среде

**Версия:** 1.0 · **Дата:** 15 октября 2026 года · **Статус:** проект технической спецификации для реализации и ревью.

**Назначение:** определить безопасную, отказоустойчивую и предсказуемую архитектуру собственной MLC VM, которая исполняет прикладную логику, расширения и автоматизацию и управляет собственным нативным GUI через контролируемый Host API.

**Происхождение и границы документа.** Исходный чат «Архитектура VM GUI» был прочитан, но содержимое ответов в нём доступно только как служебные ссылки. Поэтому этот документ систематизирует требования из текущего запроса и предлагает согласованную архитектуру; он не является дословной реконструкцией прежних ответов. MLC здесь — название собственного языка/байткода/VM пользователя; связь с какими-либо одноимёнными внешними проектами не предполагается. Репозиторий, ISA, существующий GC и конкретная GUI-библиотека не предоставлены. Интерфейсы и численные бюджеты ниже — предлагаемый контракт, а не описание уже реализованной системы или результаты измерений.

Слова **ДОЛЖЕН**, **НЕЛЬЗЯ** обозначают обязательные требования данного проекта. **СЛЕДУЕТ** обозначает рекомендуемый выбор, от которого можно отступить с зафиксированным обоснованием. Псевдокод иллюстрирует семантику, не привязан к языку реализации и не заменяет проверку конкурентности и управления памятью.

## Содержание

1. [Цели, область применения и базовые решения](#s01)
2. [Модель угроз и границы доверия](#s02)
3. [Ключевые инварианты](#s03)
4. [Компоненты, процессы и потоки](#s04)
5. [Байткод, verifier и безопасная память](#s05)
6. [Host API и capability security](#s06)
7. [Идентичность: handles, generation, revision и epochs](#s07)
8. [Actors, async continuations и планировщик](#s08)
9. [Safepoints, fuel, отмена и квоты](#s09)
10. [UI thread, события и transactional GUI](#s10)
11. [Render pipeline и OpenGL](#s11)
12. [Worker pool и нативные вычисления](#s12)
13. [Sandbox-процессы и ограничения Linux/X11](#s13)
14. [IPC, backpressure и большие данные](#s14)
15. [Reactive subscriptions](#s15)
16. [Persistent actor state и внешние эффекты](#s16)
17. [Checkpoint/resume](#s17)
18. [Supervision и восстановление](#s18)
19. [Hot reload и миграция](#s19)
20. [Обновление native host](#s20)
21. [Agent/MCP gateway](#s21)
22. [FFI и расширение доверенной базы](#s22)
23. [Производительность и управление перегрузкой](#s23)
24. [Observability и диагностика](#s24)
25. [Тестирование и критерии приёмки](#s25)
26. [Этапы реализации](#s26)
27. [Реестр решений и открытые вопросы](#s27)
28. [Сквозные сценарии и матрица отказов](#s28)
29. [Источники и терминология](#s29)

<a id="s01"></a>
## 1. Цели, область применения и базовые решения

### 1.1. Что строится

Нативный host владеет окнами, event loop, деревом GUI, OpenGL, системными интеграциями, хранилищем и политикой доступа. MLC VM реализует поведение приложения через типизированные команды и события. Один и тот же логический Host API используется встроенными доверенными модулями, изолированными расширениями и агентной автоматизацией, но каждому субъекту выдаётся свой набор прав.

Основная единица конкурентности — **actor** с единственным активным исполнителем и собственным состоянием. Основная единица полномочий — **capability** на ресурс или ограниченный класс действий. Основная единица изменения GUI — **атомарный batch**. Основная граница защиты от нативного сбоя и компрометации VM — **отдельный процесс**.

### 1.2. Базовые решения

| Область | Решение по умолчанию | Причина |
|---|---|---|
| Исполнение | Интерпретатор проверенного типизированного байткода | Небольшая доверенная база и контролируемые точки прерывания |
| Недоверенный код | VM в sandbox-процессе без доступа к display/GPU | Сбой runtime не должен уничтожать host |
| Доверенный код | Та же VM; допустим встроенный режим | Меньше задержек, но общая судьба с native host |
| GUI | Единственный владелец mutable GUI state — UI thread | Упрощение порядка, событий и транзакций |
| Rendering | Сначала UI + GL на одном потоке; выделение render thread по измерениям | Меньше синхронизации на первом этапе |
| Конкурентность | Actors, сообщения, async I/O, ограниченный worker pool | Отсутствие разделяемого изменяемого VM heap |
| Доступ к системе | Broker/Host API с повторной проверкой прав | VM не получает ambient authority |
| Сохранение | Версионированное логическое состояние и журнал эффектов | Перезапуск без сохранения нативных указателей |
| Reload | На границе actor turn; новая code epoch и миграция | Старые callbacks не исполняются в новом коде случайно |
| Агент | Внешний клиент предметного API через отдельный gateway | Модель не получает скрытых административных привилегий |

### 1.3. Что не обещается

Это не hard real-time система. Fuel не ограничивает время зависшего драйвера, blocking FFI или системного вызова. Процессная изоляция не устраняет уязвимости ядра и GPU-драйвера. Встроенная VM не защищает host от ошибки в интерпретаторе или unsafe native-коде. Общая exactly-once семантика для сети и внешних сервисов не обещается.

Не входят в первую версию: произвольный native plugin ABI внутри GUI-процесса, произвольные шейдеры от расширений, общий изменяемый heap между actors, распределённый консенсус, перенос выполняющегося нативного стека между версиями host и JIT недоверенного кода.

<a id="s02"></a>
## 2. Модель угроз и границы доверия

### 2.1. Защищаемые активы

- Целостность и доступность GUI, документов и пользовательских действий.
- Файлы, токены, учётные данные, clipboard и данные других расширений.
- Правильность capability policy, истории подтверждений и обновлений.
- Ограниченный расход CPU, RAM, GPU, файловых дескрипторов, диска и очередей.
- Согласованность persistent state после сбоев, reload и перезапуска.

### 2.2. Противники и сбои

Рассматриваются: злонамеренный модуль; повреждённый байткод; actor с бесконечным циклом; компрометированный sandbox; malformed IPC; prompt injection во внешнем контенте; ошибочный native helper; отказ диска; рестарт процесса; задержанные и повторные ответы; переполнение очередей; изменение разрешений во время операции.

Скомпрометированные root/kernel и пользователь, непосредственно изменяющий доверенные бинарники, находятся вне гарантируемой границы защиты. Побочные каналы общего CPU/cache не устраняются. Если требуется защита между враждебными арендаторами с сильной конфиденциальностью, отдельного локального процесса может быть недостаточно: нужен отдельный OS account, VM или иной более сильный deployment boundary.

### 2.3. Профили доверия

| Профиль | Размещение | Полномочия | Последствие native crash |
|---|---|---|---|
| Core | Native host и строго контролируемые встроенные модули | Минимально необходимые системные права | Может завершить приложение |
| Trusted application logic | Встроенная VM или VM-service | Явные app capabilities | Во встроенном режиме может упасть host |
| Untrusted extension | Отдельный sandbox на principal/trust domain | Ограниченная GUI subtree и brokered services | Теряется sandbox и его actors |
| Native integration | Отдельный helper с узким протоколом | Только интеграционные права | Перезапускается helper |
| Agent | Отдельный gateway и клиент | Предметные tools, scopes, approvals | Завершается сессия/операция |

Несколько actors в одном процессе изолированы логически, но не являются независимыми security boundaries при компрометации runtime. Разные недоверенные principals по умолчанию не размещаются в одном sandbox. Подпись пакета устанавливает происхождение, а не безвредность.

### 2.4. Trusted Computing Base

В TCB входят verifier, interpreter/GC, capability broker, IPC decoder, GUI transaction engine, persistence/recovery, launcher sandbox, update verifier и небольшая часть OS. Встроенные native libraries, шрифтовые и графические декодеры тоже входят в область риска; сложные декодеры следует выносить в ограниченные helpers.

Каждая граница требует своей проверки:

```text
Недоверенные байты -> bounded decoder -> bytecode verifier -> interpreter
Недоверенное IPC   -> bounded decoder -> identity/policy -> semantic validation
                                              |
                                      UI transaction commit
                                              |
                                      trusted render snapshot
```

Верификация байткода не заменяет проверку параметров Host API, а sandbox не заменяет ограничения ресурсов.

<a id="s03"></a>
## 3. Ключевые инварианты

| ID | Инвариант | Где обеспечивается |
|---|---|---|
| I01 | VM не создаёт и не разыменовывает native pointers | ISA, verifier, runtime, FFI boundary |
| I02 | Исполняются только immutable bytes, прошедшие verifier | Loader, verified-module object |
| I03 | На actor одновременно исполняется не более одного turn/continuation | Scheduler ownership token |
| I04 | Mutable heap одного actor недоступен другому напрямую | Heap API, message copy/ownership transfer |
| I05 | Mutable GUI state изменяется только UI thread | Thread assertions, command ingress |
| I06 | OpenGL вызывается только владельцем current context | Render owner, resource queue |
| I07 | Каждый эффект проверяется от имени исходного principal | Broker, authenticated channel |
| I08 | Handle не является доказательством права доступа | Separate capability validation |
| I09 | GUI batch применяется целиком или не применяется | Stage/validate/publish, bounded commit |
| I10 | Await не удерживает lock, GUI transaction или native borrow | Verifier/effect rules, runtime assertions |
| I11 | Очереди и ресурсы ограничены в количестве и байтах | Admission control и quotas |
| I12 | Старое поколение/epoch не может изменить новое воплощение объекта | Handle lookup, fencing, correlation |
| I13 | Отмена/timeout не означают, что внешний эффект не произошёл | Operation ledger, explicit unknown outcome |
| I14 | Restart не увеличивает права и не обнуляет глобальный бюджет | Supervisor policy, principal quotas |
| I15 | Checkpoint не содержит указателей, GL names, OS fd и bearer secrets | State schema и serializer |
| I16 | ACK durable commit выдаётся после требуемого durable barrier | Storage transaction и flush policy |
| I17 | Нет синхронного ожидания VM/worker/IPC на UI thread | API design, blocking-call instrumentation |
| I18 | Проверка и применение GUI команды используют согласованную политику | Serialized authorization/commit boundary |
| I19 | Capability нельзя неявно сериализовать в сообщение/снимок | Explicit transfer/rehydration broker |
| I20 | После потери непрерывности событий клиент получает resync | Subscription epoch/sequence/revision |

Инварианты — предмет автоматических тестов, debug assertions и код-ревью. Нарушение I01–I09 или I18 считается блокером релиза.

<a id="s04"></a>
## 4. Компоненты, процессы и потоки

### 4.1. Рекомендуемая топология

```text
                         Native host process
 OS input ------> [UI thread / GUI authority] ---> [render snapshot]
                         ^       |                         |
                         |       v                         v
                   [Host API broker]              [GL owner thread]*
                    /      |       \                       |
          [I/O reactor] [storage] [trusted workers]         GPU
                 ^         ^
                 |         |
       bounded, authenticated IPC
          /                |                  \
 [Sandbox principal A] [Sandbox principal B] [Native helper]
 [actors + scheduler]  [actors + scheduler]  [codec/FFI]
          ^
          |
 [Agent/MCP gateway] -> тот же предметный broker, свой principal

 * На первом этапе GL owner = UI thread.
```

Agent gateway не обязан проходить через VM: он может обращаться к broker напрямую, сохраняя ту же проверку политики и семантику операций.

### 4.2. Разделение ответственности

| Компонент | Владеет | Не выполняет |
|---|---|---|
| UI thread | Дерево GUI, focus, selection, layout publication | Сетевое I/O, произвольный VM turn, длительная запись на диск |
| Render owner | GL context, GPU allocations, fences | Вызовы MLC и изменение бизнес-состояния |
| VM scheduler | Actor execution ownership, continuations | Native UI calls |
| I/O reactor | Async sockets/timers, cancellation registration | Исполнение actor-кода в callback |
| Worker pool | Ограниченные CPU jobs | Мутацию GUI/actor state |
| Broker | Identity, capability policy, routing, accounting | Доверие к principal из payload |
| Storage service | Durable transactions, snapshots, outbox | Сохранение живых native pointers |
| Supervisor | Process lifecycle, restart policy, health | Самовольное расширение прав при восстановлении |

### 4.3. Очереди и порядок

UI ingress — bounded MPSC; на UI thread один consumer. Render snapshots — очередь малой глубины с явной политикой замены. Actor mailboxes — ограниченные очереди с serial dequeue. Control plane для отмены, отзыва прав и shutdown имеет зарезервированную ёмкость и не должен блокироваться потоком обычных команд.

FIFO гарантируется только для принятого потока от конкретного отправителя в рамках session epoch. Глобального порядка между отправителями нет. Зависимость выражается через request ID, ожидаемую revision или явную причинную связь. Приоритет control plane может нарушать порядок обычных сообщений намеренно; это должно отражаться в контракте.

Нельзя держать native mutex при вызове callback, передаче управления VM или ожидании IPC. Для многокомпонентных операций предпочтительнее actor/message ownership, чем распределённая система locks.

<a id="s05"></a>
## 5. Байткод, verifier и безопасная память

### 5.1. Контейнер модуля

```text
Module {
  format_version: u32
  vm_semantics_version: u32
  module_id: UUID
  content_hash: Hash256
  required_host_api: VersionRange
  feature_flags: Set<KnownFeature>
  type_table: BoundedArray<Type>
  imports: BoundedArray<ImportSignature>
  constants: BoundedArray<TypedConstant>
  functions: BoundedArray<FunctionBody>
  state_schema_id: SchemaId
  debug_info: Optional<BoundedDebugMap>
}
```

Хеш вычисляется загрузчиком по исполняемому содержимому, а не принимается из заголовка на веру. Подписи и манифест прав проверяются отдельно. Неизвестная обязательная feature, opcode или версия формата приводят к отказу загрузки.

Decoder ограничивает общий размер, количество секций, длину строк, глубину типов, число basic blocks и объём работы verifier. Арифметика длины и смещения — checked, включая `offset + length`, умножение размеров и преобразования integer widths. Запрещены пересекающиеся и выходящие за границы секции. Модуль читается в принадлежащий runtime immutable buffer: проверка изменяемого mmap с последующим исполнением недопустима.

### 5.2. ISA и типы

Базовая ISA поддерживает числа фиксированной ширины, bool, строки/bytes, безопасные массивы и records, tagged unions, функции с явной сигнатурой, actor references и opaque resource references. Нет инструкций `int_to_ptr`, произвольного dereference, native jump и вызова по адресу из данных.

Выбранная семантика integer overflow должна быть фиксирована: например, арифметические операции trap, а wrapping доступны отдельными opcodes. Bounds checks и runtime type tags остаются там, где статическое доказательство отсутствует. Для deterministic replay отдельно фиксируются float/NaN semantics, порядок обхода map и источники времени/случайности.

Capability — специальное opaque значение runtime, которое нельзя создать из обычных bytes. В IPC его представление всё равно рассматривается как недоверенный идентификатор: broker проверяет запись, владельца и scope.

### 5.3. Алгоритм верификации

```text
verify(bytes, limits, host_schema):
  module = decode_with_limits(bytes)
  reject_unknown_required_features(module)
  check_all_indices_and_section_bounds(module)
  check_types_and_import_signatures(module, host_schema)

  for function in module.functions:
    cfg = build_cfg_from_instruction_boundaries(function)
    reject_invalid_targets_and_exception_edges(cfg)
    state = bounded_dataflow_fixpoint(cfg):
      check_operand_types_and_stack_height()
      check_definite_initialization()
      check_call_arguments_and_return_types()
      check_resource_and_borrow_rules()
      merge_only_compatible_states()
    verify_bounded_interrupt_distance(cfg)
    verify_continuation_and_gc_metadata(function)

  return VerifiedModule(owned_immutable_bytes, verifier_version, policy_hash)
```

Все переходы, включая exception handlers, switch и indirect calls, учитываются в CFG. Косвенный вызов проверяет сигнатуру цели. На каждом цикле должен быть metering/safepoint; дополнительно ограничивается длина прямолинейного участка. Для простого интерпретатора надёжнее fuel на каждую инструкцию; оптимизированный учёт по basic blocks требует доказательства покрытия всех путей.

Verifier доказывает структурную корректность в пределах своей модели, а не завершение программы и не безопасность бизнес-логики. Корректность GC, bounds checks, host marshalling и интерпретатора остаётся обязательной. Кеш результата верификации ключуется по полным bytes hash, версии verifier, ISA и политике features; неизвестные/старые кеши не считаются доверенными.

### 5.4. Heap и GC

Рекомендуемый старт: отдельный heap на actor, tracing GC с полным учётом roots в frames, continuations и временных runtime values. Это проще для aliasing и изоляции. Для большого числа мелких actors можно рассмотреть heap на isolate, но тогда пауза и лимит памяти становятся общими для группы.

- Между actors передаются immutable values, копии или явно переданное владение буфером.
- Cycles в heap обрабатываются GC; лимит учитывает metadata, queues и native buffers, а не только payload.
- Allocation проверяет quota **до** резервирования. Экстренный резерв позволяет сформировать OOM/завершить actor без нового большого allocation.
- Finalizers не исполняют произвольный MLC-код и не служат гарантией своевременного закрытия ресурсов. Закрытие явное; broker также закрывает ресурсы по смерти owner/lease.
- Moving GC запрещает сохранение borrowed указателя на heap через safepoint. Native bridges используют copies или ограниченные pin scopes.
- GC и heap scanning должны быть incremental/bounded для заявленного latency target либо выполняться в отдельном sandbox с признанной возможностью задержки.

### 5.5. JIT как отдельное расширение

JIT не нужен для первой версии. Его появление добавляет code generator, executable memory и stack maps в TCB. Обязательны W^X, metering всех путей, корректные safepoints, differential tests с интерпретатором и sandbox policy, рассчитанная на JIT. Включение JIT не должно автоматически расширять права других sandbox-профилей.

<a id="s06"></a>
## 6. Host API и capability security

### 6.1. Форма API

API должен описывать предметные действия: `document.open`, `document.apply_patch`, `ui.commit`, `task.start`, `subscription.open`. Нельзя предоставлять универсальные обходы вроде `invoke_native(address)`, `eval_host_code` или бесконтрольного `run_shell`.

```text
Request<T> {
  request_id: UUID
  session_epoch: u64
  actor_incarnation: u64
  code_epoch: u64
  capability_ref: OpaqueCapRef
  deadline_budget_ms: u32
  idempotency_key: Optional<UUID>
  body: T
}

Reply<T> = Ok(T) | Error {
  code: ErrorCode
  retry: Never | AfterBackoff | AfterRefresh | QueryStatus
  operation_id: Optional<UUID>
  safe_details: BoundedMap
}
```

Principal и session binding берутся из доверенного connection context; payload не может их переопределить. Внешние deadlines переводятся в локальное monotonic время при ingress и ограничиваются серверным максимумом. Повторный hop получает оставшийся бюджет, а не полный первоначальный timeout.

Ошибки различаются: `InvalidArgument`, `PermissionDenied`, `Revoked`, `Expired`, `StaleHandle`, `RevisionConflict`, `QuotaExceeded`, `Busy`, `Cancelled`, `DeadlineExceeded`, `OutcomeUnknown`, `Unavailable`, `IncompatibleVersion`, `InternalError`. `DeadlineExceeded` с внешним эффектом обязан сопровождаться статусом эффекта/operation ID; нельзя трактовать его как доказательство отсутствия изменения.

### 6.2. Синхронность и reentrancy

Синхронные импорты допустимы только для коротких локальных pure операций с доказанным верхним пределом затрат. Доступ к GUI, файлам, сети и долгим native calculations — async. Host не вызывает MLC callback рекурсивно внутри Host API call: completion становится сообщением actor scheduler.

```text
host_call(import_id, args):
  validate_types_sizes_and_budget(args)
  req = broker.submit(current_principal, import_id, args)
  save_continuation(req.id)
  suspend_current_task()  // OS thread возвращается scheduler
```

### 6.3. Capability record

```text
CapabilityRecord {
  cap_id: Random128OrMore
  owner_principal: PrincipalId
  bound_session: Optional<SessionId>
  resource_scope: ResourceSelector
  allowed_operations: Set<Operation>
  constraints: {max_bytes, max_rate, allowed_destinations, ...}
  expires_at: Optional<HostMonotonicDeadline>
  policy_epoch: u64
  parent_cap: Optional<CapId>
  delegation: Denied | AttenuationOnly
  revoked: bool
}
```

Случайный ID затрудняет угадывание, но сам по себе не даёт защиты: broker проверяет owner, session, lifetime, ancestry и scope. Передача capability другому actor/principal осуществляется явным `delegate`, который создаёт более узкую запись. Проверяются ограничения всей родительской цепочки; отзыв родителя делает потомков недействительными. Долгоживущий абсолютный monotonic timestamp не переносится через reboot — lease пересоздаётся по политике.

Примеры scopes: право изменять только конкретную GUI subtree; читать выбранный пользователем документ; посылать HTTPS-запросы только заданному сервису; создавать jobs в пределах CPU budget. UI capability не должна позволять расширению рисовать поверх доверенного approval dialog или перехватывать системные shortcut без отдельного права.

### 6.4. Проверка и отзыв

```text
authorize(ctx, cap_ref, operation, target, cost):
  cap = lookup_cap_for_authenticated_session(ctx, cap_ref)
  require cap.owner == ctx.principal
  require cap_and_all_parents_live(cap)
  require operation in cap.allowed_operations
  require target_within_scope(target, cap.resource_scope)
  reserve_principal_and_cap_budget(cost)
  return AuthorizationTicket(cap.id, current_policy_epoch, bounded_scope)
```

Предварительная авторизация не разрешает безусловно выполнить запрос позднее. Для GUI policy update и commit сериализуются в одной authority domain: непосредственно перед публикацией проверяется действительность ticket, и между проверкой и commit отзыв не может вклиниться. Если broker и UI разделены, нужен versioned policy snapshot и протокол commit fence, а не просто два несвязанных lookup.

Семантика отзыва: операции, чей commit линеаризован **после** отзыва, запрещены. Уже committed действие не отменяется автоматически. Долгая streaming-операция повторно проверяет lease на ограниченных шагах. Для удалённого эффекта есть момент отправки, после которого отзыв может остановить следующие действия, но не отозвать уже отправленный запрос.

Для файлов broker использует безопасное разрешение относительно разрешённого directory handle и ограничения symlink traversal; строкового prefix check недостаточно. Для сети проверяются redirect, DNS resolution и фактический адрес назначения, иначе domain allowlist может обходиться через SSRF/DNS rebinding. Credentials остаются в broker, а VM получает результаты разрешённых операций.

<a id="s07"></a>
## 7. Идентичность: handles, generation, revision и epochs

```text
ResourceHandle {
  namespace: HostInstanceId
  kind: ResourceKind
  slot: u32
  generation: u64
}
ResourceEntry {
  generation: u64
  owner: PrincipalId
  revision: u64
  lifecycle: Live | Closing | Dead
  value: HostOwnedObject
}
```

| Поле | Что различает | Когда меняется |
|---|---|---|
| Handle slot | Место в runtime table | При allocation/reuse |
| Generation | Воплощение объекта в этом slot | При освобождении/повторном использовании |
| Revision | Версию содержимого живого объекта | При успешном изменении |
| Host instance/session epoch | Жизнь host/соединения | После restart/reconnect |
| Actor incarnation | Жизнь actor | После restart actor |
| Code epoch | Версию исполняемого модуля | После reload |
| Persistent logical ID | Предметную сущность между перезапусками | Не зависит от runtime handle |

Generation предотвращает ABA: освобождённый widget не становится тем же объектом после reuse slot. Revision предотвращает lost update: команда с устаревшим `expected_revision` отклоняется. Epoch отбрасывает ответы от старых сессий и версий кода. Ни одно из этих полей не заменяет capability.

```text
resolve(handle, expected_kind, ctx):
  require handle.namespace == host.instance_id
  require handle.kind == expected_kind
  entry = bounded_lookup(handle.slot)
  require entry.generation == handle.generation
  require entry.lifecycle == Live
  authorize_resource_access(ctx, entry)
  return entry  // только краткий host-side borrow
```

Счётчики не должны оборачиваться: при исчерпании generation slot выводится из использования; при исчерпании revision/epoch создаётся новый namespace. Runtime handles не сохраняются в persistent state. После resume logical IDs заново разрешаются и получают новые handles и capabilities.

Длинная задача фиксирует **какую именно** версию данных она читала. Её результат применяется с `expected_revision`; при конфликте пересчитывается или становится предложением пользователю. Проверка актуальности только при старте недостаточна.

<a id="s08"></a>
## 8. Actors, async continuations и планировщик

### 8.1. Модель actor

```text
Actor {
  logical_id: ActorId
  incarnation: u64
  principal: PrincipalId
  module_hash: Hash256
  code_epoch: u64
  state_schema: SchemaId
  state: ActorOwnedHeap
  mailbox: BoundedQueue<Message>
  tasks: BoundedMap<TaskId, Continuation>
  run_state: Idle | Runnable | Running | Suspended | Stopping | Dead
  execution_token: Atomic<Unowned | WorkerId>
  quotas: ActorBudget
}

Continuation {
  task_id: TaskId
  code_epoch: u64
  function_id: FunctionId
  resume_label: LabelId
  live_values: TypedRootSet
  waiting_for: RequestId
  cancellation_scope: ScopeId
  expected_state_revision: Optional<u64>
}
```

Actor — не OS thread. Scheduler выдаёт короткие кванты выполнения на ограниченном числе runtime threads. Mailbox, suspended tasks и VM heap входят в quota actor/principal.

### 8.2. Await и логическая reentrancy

По умолчанию `await` освобождает actor execution token. Пока задача ждёт, actor может обработать другие сообщения. Одновременной мутации нет, но логическое состояние между началом операции и продолжением могло измениться.

```text
on RefreshPreview:
  source_revision = state.document_revision
  source = immutable_copy(state.document)
  result = await host.compute_preview(source)
  if state.document_revision != source_revision:
    discard(result)
    schedule_latest_refresh_if_needed()
    return
  state.preview = result
```

Ссылки на mutable состояние не удерживаются как неизменные предположения через await. Для простоты можно вводить actor с non-reentrant mailbox, который во время ожидания принимает только completion/cancel/control messages. Но такой actor легко создаёт head-of-line blocking; нельзя ждать ответ от операции, для выполнения которой нужно обычное сообщение этому же actor. Режим определяется явно на типе actor, а не меняется незаметно.

Actor turn — участок от получения сообщения/возобновления до await, yield, завершения или ошибки. Исчерпание fuel может приостановить текущий turn без допуска другого turn, если нужны более строгие последовательные семантики; конкретно здесь переключение на другое сообщение допускается только в объявленной reentrancy boundary, а не в произвольной середине изменения state.

### 8.3. Планировщик

```text
worker_loop():
  actor = runnable_queue.take_fairly()
  if not actor.execution_token.try_acquire(worker_id): continue
  try:
    task = actor.select_resumable_task_or_message()
    outcome = interpreter.run(task, quantum_fuel, quantum_deadline)
    match outcome:
      Completed: finish_turn_and_release_roots()
      Awaiting(req): register_wait_atomically(req, task)
      FuelYield: save_task_without_opening_new_turn()
      Trap(error): supervisor.report(actor, error)
  finally:
    actor.execution_token.release()
  reschedule_if_runnable(actor)
```

Регистрация ожидания и доставка completion согласуются так, чтобы мгновенный ответ не потерялся между `submit` и `suspend`. Runtime использует state machine future (`Pending/Ready/Consumed/Cancelled`) и единый атомарный переход/actor-owned inbox, а не произвольный callback.

Fairness обеспечивается между principals, затем между actors; создание тысяч actors не должно давать расширению большую долю CPU. Work stealing допускается для runnable actors, но один actor нельзя исполнять на двух workers одновременно. Actor-local GC следует тому же ownership token.

### 8.4. Сообщения и ошибки

Сообщения содержат correlation ID, incarnation, code epoch, ограниченный payload и trace context. Sender identity задаётся runtime. Передача mutable references запрещена. Запросы должны иметь timeout, cancellation scope и политику orphaned result.

Необработанное исключение завершает текущую задачу. Если целостность actor state после частичной мутации не гарантируется, actor помечается failed и восстанавливается целиком; продолжение обработки на потенциально испорченном state запрещено. Для восстановления отдельного turn нужен явный transactional state механизм, а не предположение, что exception откатил присваивания.

Structured concurrency связывает дочерние задачи со scope: выход из scope отменяет или ожидает их по объявленной политике. Detached jobs требуют явного долгоживущего owner и квоты.

<a id="s09"></a>
## 9. Safepoints, fuel, отмена и квоты

### 9.1. Два независимых ограничения

**Fuel** — детерминированный счётчик стоимости VM-работы. **Deadline** — ограничение wall-clock ожидания/исполнения, проверяемое в точках, где runtime способен вернуть управление. Fuel помогает против бесконечных циклов; deadline выявляет медленный прогресс. Ни одно из них не может безопасно прервать произвольную native инструкцию.

Safepoints необходимы на циклах, вызовах/возвратах, allocation, больших copy/hash/string операциях и периодически в длинном прямолинейном коде. Между safepoints не должно быть неограниченной работы. Цена `sort(array)` не равна цене одной дешёвой инструкции: операция разбивается на шаги или уходит в ограниченный worker/helper.

```text
safepoint(task, next_cost):
  require next_cost <= remaining_total_operation_budget
  if cancellation_requested(task): return CancelledAtSafeBoundary
  if monotonic_now() >= task.deadline: return DeadlineAtSafeBoundary
  if next_cost > task.quantum_fuel: return Yield
  task.quantum_fuel -= next_cost
  task.total_fuel += next_cost
  if gc_step_needed(): perform_bounded_gc_step()
```

Квант восстанавливается при перепланировании, но общий лимит задачи и principal за окно времени — нет. Иначе бесконечная работа обходит ограничение простым `yield`.

### 9.2. Что ограничивается

| Ресурс | Уровень учёта | Действие на пределе |
|---|---|---|
| Fuel/CPU | Task, actor, principal, process | Yield, throttle, terminate task/process |
| Heap и native buffers | Actor и process | Reject allocation, fail actor |
| Mailbox/IPC | Сообщения + bytes + age | Backpressure, bounded drop/resync |
| GUI mutations | Ops, bytes, depth, layout work | Reject/разбить запрос до commit |
| Outstanding requests | Actor/principal | Busy, admission отказ |
| Storage | Bytes, write rate, journal retention | QuotaExceeded, read-only degraded mode |
| GPU | Textures, geometry, upload bytes/frame | Deferred upload, reject resource |
| Logs/metrics | Bytes/sec и cardinality | Sample/drop с отдельным счётчиком |
| Processes/threads/fd | Principal и sandbox | Отказ в создании, restart policy |

### 9.3. Семантика отмены

`cancel(request_id)` идемпотентен. Ответ разделяет `CancelledBeforeEffect`, `AlreadyCommitted`, `CancellationRequested` и `OutcomeUnknown`. Нельзя выдавать успех отмены только потому, что локальная continuation удалена.

Внутренний автомат операции:

```text
Created -> Admitted -> Running -> Committed -> Replied
    \          \          \
     CancelledBeforeEffect  Failed | OutcomeUnknown
```

Гонка cancellation/commit решается одним линеаризуемым переходом внутри владельца операции. После commit допускается только новая компенсационная операция. Late completion проверяет request ID, incarnation и code epoch, затем либо применяется, либо сохраняется в operation ledger, либо отбрасывается с освобождением ресурсов.

Для зависшего native helper: cooperative cancel → ограниченный grace period → terminate process → restart с новой incarnation. Для зависшего native вызова внутри host безопасного общего `kill thread` нет; это причина выноса такого вызова в helper.

<a id="s10"></a>
## 10. UI thread, события и transactional GUI

### 10.1. Владение и взаимодействие

UI thread владеет widget tree, focus, selection и публикуемым layout. VM видит handles и immutable snapshots. Она не держит `Widget*` и не вызывает OpenGL.

Путь ввода:

```text
OS event -> native event normalization -> hit testing на актуальном UI state
         -> host default behavior -> bounded actor event queue
         -> MLC turn -> GUI batch -> UI validate/commit -> frame
```

Необходимое немедленное поведение — focus, drag feedback, простая анимация — выполняется native контролами или ограниченными декларативными state machines. UI не ждёт решения sandbox для каждого mouse move. `preventDefault` требует заранее зарегистрированной политики либо отдельной протокольной фазы с жёстким deadline и fallback, а не синхронного callback через IPC.

Доверенные системные элементы, запросы полномочий и идентификатор владельца расширения рисуются вне доступной расширению subtree. Расширение не может подменить источник approval или заслонить его своим окном.

### 10.2. Контракт batch

```text
GuiBatch {
  transaction_id: UUID
  owner_subtree: ResourceHandle
  expected_subtree_revision: u64
  operations: BoundedArray<GuiOp>
  estimated_cost_hint: u32  // только hint, host считает сам
}
GuiOp = Create {temp_id, parent, kind, props}
      | SetProperty {target, property, typed_value}
      | InsertChild {parent, child, index}
      | MoveChild {child, new_parent, index}
      | Remove {target}
      | BindEvent {target, event_kind, subscription_id}

CommitResult {
  transaction_id: UUID
  new_subtree_revision: u64
  created: Map<TempId, ResourceHandle>
  render_revision: u64
}
```

`temp_id` существует только внутри batch и позволяет ссылаться на созданные в нём widgets. Он не является runtime handle. Ревизия subtree даёт простой первый вариант conflict detection. Позднее допустим read/write set с per-object revisions, но только при корректной проверке структурных зависимостей и родителей.

### 10.3. Алгоритм применения

```text
apply_gui_batch(authenticated_context, batch):
  bounded_decode_and_syntax_check(batch)
  plan = stage_against_immutable_snapshot(batch)
  compute_actual_cost_and_reserve_memory(plan)

  on_ui_thread:
    enter_commit_authority_boundary()
    require policy_still_allows(authenticated_context, plan)
    require live_handles_and_expected_revisions_match(plan)
    require valid_tree_after_all_operations(plan)
    require all_publish_allocations_prepared(plan)
    next_tree = finalize_staged_delta(plan)
    publish_atomically(next_tree, incremented_revision)
    record_session_dedup_result(batch.transaction_id)
    leave_commit_authority_boundary()

  emit_events_and_schedule_layout_after_commit()
  return CommitResult(...)
```

Staging не делает наблюдаемых изменений. Проверяются циклы дерева, допустимые parent/child types, диапазоны свойств, ownership, размеры текста и resources. Все fallible allocations для атомарного publish готовятся заранее; «применять команды по одной и надеяться на rollback» недостаточно.

Между validation и publish UI state не может измениться; если staging выполнен на worker, окончательная revision/policy validation обязательна на UI thread. Ошибка любой операции отклоняет весь batch. Никаких MLC callbacks и внешних I/O внутри commit.

### 10.4. Три разных уровня завершения

- **Accepted:** запрос принят в bounded очередь; изменение ещё не гарантировано.
- **Committed:** логическое GUI state изменено атомарно, revision опубликована.
- **Presented:** кадр с этой или более новой revision передан в механизм presentation по определённому backend contract.

`Committed` не гарантирует успешный GPU draw. `Presented` не всегда доказывает физическое отображение пикселей пользователю: compositor/driver могут добавлять задержки. API должен определить точный смысл своего presentation acknowledgment.

GUI-транзакция не распространяется на файлы, сеть, GPU execution и persistent actor state. Долговечный документ изменяется в предметной storage transaction, а GUI является проекцией committed состояния. Это устраняет необходимость обещать атомарный commit между базой данных и окном.

### 10.5. Batching, coalescing и event order

Соседние `SetProperty` можно объединять до отправки, если промежуточные значения не имеют семантических событий. Нельзя безусловно схлопывать текстовый ввод, команды сохранения, clicks и undo actions. `pointermove`, progress и preview допускают latest-value policy.

Событие содержит target handle, subtree revision и последовательный event sequence. Если объект удалён до обработки события, actor получает устаревший target и обязан не модифицировать новый объект в том же slot. Дублирующий `transaction_id` с тем же payload возвращает прежний результат в пределах dedup window; с другим payload — protocol error. После host restart runtime GUI создаётся заново, и старая transaction ID не обещает постоянную идемпотентность.

Большие обновления разбиваются на ограниченные batches либо строятся как скрытая новая subtree с bounded atomic swap. Последний вариант требует квоты на одновременное существование старого и нового дерева. «Атомарная замена» не разрешает неограниченный layout на UI thread.

<a id="s11"></a>
## 11. Render pipeline и OpenGL

### 11.1. Минимальный вариант

На первом этапе event loop, commit, ограниченный layout, построение draw list и GL submission выполняются на UI thread. VM и I/O работают отдельно. Это самый простой ownership model, но тяжёлый GL driver call может задержать input. Если измерения показывают такое влияние, выделяется render thread.

Один OpenGL context может быть current только на одном потоке одновременно. Перед переносом context его нужно снять с прежнего потока. Конкретные ограничения оконных операций зависят от platform backend; например, GLFW отдельно определяет thread-safety своего API. См. [GLFW Context guide](https://www.glfw.org/docs/latest/context.html) и [GLFW Introduction](https://www.glfw.org/docs/latest/intro).

### 11.2. Вариант с отдельным render thread

```text
UI state revision N
   -> layout result N
   -> immutable RenderSnapshot(N, resource references)
   -> bounded snapshot queue
   -> render thread consumes latest eligible snapshot
   -> uploads within budget -> GL draw -> fence -> present
```

Render snapshot содержит подготовленные primitives, clipping, transforms и ресурсные handles, но не указатели на mutable widgets. Если layout рассчитывается параллельно, результат помечается исходной revision и отбрасывается при устаревании.

Очередь snapshots обычно имеет глубину 2–3. Устаревший ещё не обработанный snapshot можно заменить новым, но ресурсные upload/delete операции должны иметь отдельный корректный lifetime protocol. При пропуске revision completion для ожидающих клиентов должен явно означать `Superseded` либо «представлена более новая revision», а не зависать навсегда.

### 11.3. GPU resource lifetime

```text
GpuResource {
  logical_id: ResourceId
  generation: u64
  gl_name: NativeOnly
  cpu_descriptor: Optional<RebuildDescriptor>
  last_used_frame: FrameId
  state: UploadPending | Ready | Retiring | Lost
}
```

GPU deletion происходит у владельца GL context после того, как ресурс не нужен CPU snapshots и завершено использующее его GPU work. Одного CPU reference count недостаточно; требуется fence или другая доказуемая стратегия deferred reclamation. Новое поколение не переиспользует незавершённые GPU resources случайно.

По умолчанию один GL context и централизованная загрузка. Shared contexts добавляют synchronization и platform-specific риски; их следует вводить только после profiling. Runtime handles, GL names и GPU memory не попадают в checkpoints.

### 11.4. Ограничения и отказ

Недоверенная VM создаёт ограниченные высокоуровневые primitives; не передаёт произвольные GL команды или shaders. Ограничиваются число draws, размеры texture, глубина clip stack, geometry и upload bandwidth. Компиляция trusted shader variants по возможности выполняется вне интерактивной критической фазы, с кешем и fallback.

При потере context host помечает ресурсы Lost, прекращает использование старых GL names, пересоздаёт renderer из CPU descriptors и публикует новый render epoch. При необратимом driver hang render thread не обеспечивает crash isolation; отдельный renderer process — возможный следующий этап, но он усложняет surfaces, sharing и IPC. Ни один вариант не гарантирует защиту всей машины от ошибки kernel GPU driver.

<a id="s12"></a>
## 12. Worker pool и нативные вычисления

Worker pool предназначен для чистых CPU-задач: parsing, geometry, подготовка layout, encoding небольших проверенных данных. Блокирующие I/O идут через reactor или отдельный bounded blocking pool; недоверенные codecs/FFI — в helpers.

```text
Job {
  job_id: UUID
  principal: PrincipalId  // устанавливается broker
  owner_actor: (ActorId, Incarnation)
  code_epoch: u64
  input_revision: u64
  input: OwnedImmutableData
  deadline: MonotonicTime
  cancellation: CancelToken
  cost_class: Small | Medium | Large
}
```

Worker возвращает owned result через очередь. Он не меняет actor heap, GUI или resource table. UI/actor применяет результат только после revision/epoch validation. Native function не получает runtime pointer, который может переместить GC.

Размер pool выбирается по hardware и profiling с резервом для UI/render/I/O. Фиксированное «по thread на actor/job» запрещено. Очередь bounded; большие jobs не должны вытеснять все малые. Cancellation проверяется по шагам. Неотменяемая job либо имеет доказанную верхнюю границу времени, либо вынесена в процесс.

<a id="s13"></a>
## 13. Sandbox-процессы и ограничения Linux/X11

### 13.1. Defense in depth

Sandbox-процесс ограничивает последствия ошибки VM/FFI, но его наличие само по себе недостаточно. Рекомендуется сочетать:

1. Minimal environment, закрытие лишних fd, отсутствие секретов и прямого display connection.
2. Отдельные namespaces там, где они доступны и нужны: mount, PID, IPC, network, user.
3. `no_new_privs`, снятие capabilities и syscall allowlist с проверкой architecture/ABI.
4. Read-only runtime filesystem и небольшой quota-limited writable temp.
5. LSM/Landlock policy как дополнительный запрет доступа к ресурсам.
6. Resource limits и доступные cgroup v2 controls для process group.
7. Единственный broker IPC channel с ограниченным протоколом.

Seccomp уменьшает доступную поверхность системных вызовов, но не является полноценной sandbox-политикой и не умеет разыменовывать pathname pointer для проверки строк. Фильтр должен учитывать syscall ABI. См. [Linux Seccomp BPF](https://docs.kernel.org/userspace-api/seccomp_filter.html).

Landlock добавляет ограничения поверх существующих прав; доступность функций зависит от runtime ABI и конфигурации ядра. Унаследованные открытые дескрипторы требуют отдельного аудита, а ограничения должны вводиться до запуска недоверенных потоков. Проверять фактическую поддержку важнее, чем только номер kernel version. См. [Linux Landlock userspace API](https://cdn.kernel.org/doc/html/latest/userspace-api/landlock.html).

### 13.2. Последовательность запуска

```text
launcher:
  create_authenticated_ipc_pair()
  establish_principal_resource_group_if_available()
  spawn_minimal_helper_with_explicit_fd_allowlist()

child, before loading untrusted module:
  clear_environment_and_sensitive_runtime_state()
  close_all_non_allowlisted_descriptors()
  configure_namespaces_and_minimal_filesystem()
  drop_privileges_and_set_no_new_privs()
  install_resource_limits_and_supported_LSM_policy()
  install_seccomp_profile_for_runtime_and_selected_ABI()
  self_test_required_denials()
  report_sandbox_profile_and_feature_results()
  negotiate_protocol()
  accept_and_verify_module()
  run()
```

Это логическая последовательность, а не готовая shell-рецептура: некоторые setup syscalls должны произойти до final seccomp, а bootstrap loading — до запрета соответствующих filesystem operations. Launcher должен быть реализован и протестирован для каждого поддерживаемого профиля. Нельзя загружать недоверенный модуль до установки обязательных ограничений.

Передавать cgroup limits обычный непривилегированный процесс может только при соответствующей delegation. Если они недоступны, профиль должен явно определить достаточную альтернативу или отказаться от запуска недоверенного кода. RLIMIT_AS не тождественен точному учёту RSS, а process-wide OOM не является graceful actor exception. Cgroup v2 предоставляет отдельные CPU, memory и process controls при наличии настроенных controllers. См. [Control Group v2](https://docs.kernel.org/admin-guide/cgroup-v2.html).

### 13.3. Почему X11 — особая проблема

Доступ к общему desktop X server — существенный канал взаимодействия с другими клиентами. Нельзя считать процесс безопасно изолированным от рабочего стола, если ему переданы обычные X11 credentials/socket. X SECURITY extension различает trusted/untrusted clients и вводит ограничения, но не должен служить универсальной заменой системной изоляции. См. [X Security Extension Specification](https://www.x.org/releases/X11R7.7-RC1/doc/xextproto/security.pdf).

**В выбранной архитектуре sandbox вообще не подключается к X11 или Wayland.** Окна рисует native host, а VM передаёт высокоуровневые GUI batches. Поэтому для VM не требуются `DISPLAY`, `XAUTHORITY`, `WAYLAND_DISPLAY`, desktop session bus, GPU device и display sockets.

Очистки переменных окружения недостаточно. Нужно закрыть унаследованные соединения и исключить доступ к filesystem и abstract sockets, device nodes, доступному `/proc`, ptrace/process memory, desktop D-Bus и другим посредникам. X11 socket может быть доступен несколькими transport mechanisms; политика тестируется на фактической desktop конфигурации. Внешний уже скомпрометированный X11 client всё ещё может угрожать самому GUI host — это отдельное ограничение окружения, которое VM sandbox не устраняет.

Wayland меняет модель доступа клиентов, но простая смена display server не является доказательством sandbox isolation. В проекте сохраняется правило отсутствия display connection у VM. Если native third-party GUI действительно нужен, требуется отдельный изолированный display/compositor и отдельная модель угроз.

### 13.4. Fail closed и диагностика

Определяются named profiles: `trusted-in-process`, `isolated-required`, `development-relaxed`. Для `isolated-required` отсутствие обязательного механизма или провал self-test означает отказ запуска. Дополнительные ограничения можно применять best-effort, но нельзя молча объявлять урезанный профиль эквивалентным полному. Development mode явно помечается в UI/логах и не выбирается автоматически после ошибки запуска.

<a id="s14"></a>
## 14. IPC, backpressure и большие данные

### 14.1. Transport и envelope

Для локального первого варианта подходит Unix domain socket с явным framing и схемой сообщений. Конкретный codec выбирается после оценки library quality; использование Protobuf/CBOR/другого формата не отменяет limits и semantic validation.

```text
FrameHeader {
  magic: u32
  wire_major: u16
  wire_minor: u16
  kind: u16
  flags: u16
  payload_length: u32
  session_epoch: u64
  sequence: u64
  request_id: UUID
}
```

Длины проверяются до allocation; числа имеют заданный byte order, а optional fields — определённую семантику. Неизвестное обязательное сообщение закрывает соединение/отклоняется по протоколу. Negotiation выбирает пересечение версий и features, но не понижает обязательные security requirements.

Peer credentials полезны, но одного UID недостаточно для разделения нескольких расширений одного пользователя. Launcher связывает созданный канал с principal/session; broker не принимает чужую identity из payload. Для присоединяемых каналов используется явная session authentication. PID сам по себе не является устойчивой identity из-за reuse.

### 14.2. Admission и backpressure

Отправитель получает credits на bytes и число outstanding messages. Receiver проверяет лимиты независимо от соблюдения credits отправителем. Control traffic имеет отдельный небольшой reserve. Очереди внутри broker также bounded: ограничить только socket buffer недостаточно.

| Тип данных | Политика перегрузки |
|---|---|
| Команда изменения документа/GUI | Явный Busy/QuotaExceeded до принятия |
| Mouse move / progress | Coalesce latest с sequence gap semantics |
| Надёжное domain event | Durable log + cursor либо producer backpressure |
| Preview frame | Drop stale, не drop resource lifetime obligations |
| Cancel/revoke/shutdown | Зарезервированный control path |
| Logs | Drop/sample; счётчик потерь |

Для accepted request должен существовать eventual completion, status query или определённый сбой сессии; молчаливое исчезновение команды запрещено. На transport disconnect pending операции переходят в known failed либо unknown outcome в зависимости от стадии.

### 14.3. Большие buffers

Первый вариант копирует bounded payload. Shared memory вводится только по измеренной необходимости. Недоверенный sender не должен менять bytes после проверки receiver: read-only mapping у receiver сам по себе не мешает writable mapping у sender.

Безопасные варианты: copy into owned memory либо OS-enforced immutable/sealed backing с проверкой seals и отсутствия возможности mutation. Для mutable shared ring требуется строгий ownership protocol, атомики с определённым memory order, индексы с проверкой wrap и отсутствие использования проверенных данных после их возможного изменения.

Передаваемый fd проверяется как ресурс: тип, права, размер, seals, количество дескрипторов и соответствие заявленному object. FD не разрешает обход capability policy. Незнакомые дескрипторы закрываются. Каждый buffer имеет owner/lease и accounting; disconnect освобождает неиспользуемые ресурсы.

### 14.4. Idempotency

Повторный request ID не означает автоматически exactly-once. Для недолговечного GUI достаточно session-scoped dedup. Для durable операции нужен persisted idempotency record с payload hash и результатом в той же transaction, что изменение состояния. Срок dedup retention входит в контракт; повтор после истечения срока не должен обещать старую гарантию.

<a id="s15"></a>
## 15. Reactive subscriptions

### 15.1. Модель подписки

Подписки связывают domain state с GUI и VM без постоянного polling. Они возвращают immutable projection, а не живую ссылку на host object.

```text
Subscription {
  id: UUID
  owner: (PrincipalId, ActorId, Incarnation)
  subscription_epoch: u64
  query: ValidatedProjection
  authorization_scope: CapId
  last_sequence: u64
  current_revision: u64
  mode: LatestValue | OrderedDelta | DurableEvents
  buffer_limit: {messages, bytes}
  lease: Lease
}
Update = Snapshot {epoch, sequence, revision, value}
       | Delta {epoch, sequence, base_revision, new_revision, patch}
       | ResetRequired {epoch, reason}
       | Closed {reason}
```

Query должен быть ограниченным DSL или именованным projection, а не произвольным native expression. Ограничиваются fan-out, частота, размер результата и стоимость пересчёта.

### 15.2. Отсутствие окна потери данных

Операция subscribe должна атомарно определить начальную revision и зарегистрировать наблюдение за следующими изменениями. Возможны snapshot + log cursor под одной storage boundary или подписка с буферизацией до snapshot. Наивная последовательность «прочитать state, затем подписаться» теряет изменения между шагами.

```text
subscribe(query):
  within_authority_boundary:
    register_change_cursor()
    capture_snapshot_revision_and_value()
  send_snapshot()
  send_changes_after_snapshot_revision()
```

Delta применяется, только если `base_revision == local_revision` и sequence непрерывен. Иначе клиент запрашивает новый snapshot. Для LatestValue можно пропускать промежуточные значения, для OrderedDelta нужен reset при overflow, а для DurableEvents — persisted cursor, retention и политика lagging consumer.

### 15.3. Lifecycle и защита от циклов

Подписка закрывается при смерти actor, отзыве capability, истечении lease или уничтожении target. После reconnect создаётся новая subscription epoch; старые сообщения не применяются. Данные фильтруются до передачи, чтобы сам факт подписки не раскрывал запрещённые поля.

Изменения, сделанные самим subscriber, могут снова вызвать update. Поэтому применяются equality checks, causal transaction IDs, debounce и bounded recomputation. Нельзя «исправлять» бесконечный feedback loop только увеличением очереди. Listener/callback никогда не вызывается в середине GUI/storage commit.

<a id="s16"></a>
## 16. Persistent actor state и внешние эффекты

### 16.1. Что является долговечным

Persistent state — сериализуемое предметное состояние, а не дамп VM heap. Оно содержит logical IDs, schema version, task progress и ссылки на эффекты. GUI, временные caches, handles, capabilities, sockets и GPU resources восстанавливаются как проекции.

```text
PersistentActorRecord {
  actor_id: UUID
  state_schema: SchemaId
  state_revision: u64
  module_hash: Hash256
  state: CanonicalTypedValue
  inbox_cursor: Optional<Cursor>
  completed_inputs: BoundedDedupIndex
  pending_effects: Array<EffectIntent>
  checksum: Hash256
}
EffectIntent {
  effect_id: UUID
  kind: NamedOperation
  args: VersionedValue
  idempotency_key: UUID
  status: Pending | Dispatched | Succeeded | Failed | OutcomeUnknown
  result_ref: Optional<LogicalResultRef>
}
```

Секреты хранятся в host credential store; state содержит только логическую ссылку, которая заново авторизуется. Данные из checkpoint тоже считаются входом parser и проверяются по размеру, schema и integrity. Если целостность хранилища находится под угрозой, checksum недостаточен для аутентичности — нужен MAC/signature и защищённый ключ.

### 16.2. Durable actor turn

Для durable actor обработка входа формирует новое state и **намерения** внешних действий. До commit она не делает внешнего эффекта.

```text
handle_durable_input(actor_id, input):
  old = storage.load(actor_id)
  if storage.has_completed_input(input.id): return stored_result
  (next, intents, reply) = reduce_without_external_effects(old.state, input)
  storage.transaction:
    require current_revision(actor_id) == old.revision
    write_state(next, old.revision + 1)
    append_outbox(intents)
    mark_input_completed(input.id, reply)
  acknowledge_after_durable_commit(reply)
```

Outbox dispatcher отправляет эффекты после storage commit и пишет outcomes. На restart pending/dispatched записи подлежат reconciliation. Crash между отправкой и сохранением результата может привести к повторной отправке: получатель должен поддерживать тот же idempotency key либо выполняется status query/ручное разрешение `OutcomeUnknown`.

Для изменения собственного документа можно обеспечить однократный логический commit в одной локальной storage transaction. Для удалённого сервиса без idempotency/status API exactly-once недостижим одной только локальной записью. Компенсация — новое действие с собственными правами и возможным отказом, а не магический rollback внешнего мира.

### 16.3. Storage durability и владение

Рекомендуется зрелое transactional storage с журналированием и проверенной crash recovery. ACK обозначает выбранный durability mode: например, durable commit после требуемой синхронизации накопителя. Atomic rename сам по себе не гарантирует сохранность после power loss; snapshot replacement требует корректного flush файла и directory metadata для выбранной ОС/FS.

Только один активный владелец durable actor допускается к записи. Storage revision CAS защищает от lost update; supervisor fencing token защищает от запоздалого старого процесса. Внешние эффекты старой incarnation также блокируются broker. Если helper имеет прямой внешний канал, полностью отозвать уже выданные внешние credentials сложнее — поэтому эффекты лучше проводить через broker.

### 16.4. Журнал и snapshots

Журнал хранит inputs, state transitions или effect intents согласно выбранной модели. Необязательно хранить каждую VM instruction. Snapshot содержит last included log sequence; compaction начинается после durable сохранения и проверки snapshot. Retention ограничивается квотой, но не удаляет записи, необходимые для pending outcomes и dedup contract.

Event sourcing полезен для replay и аудита, но требует versioned event schemas и миграции reducers. Для первой версии достаточно snapshots + transactional outbox + bounded dedup, если полный replay не является продуктовым требованием.

<a id="s17"></a>
## 17. Checkpoint/resume

### 17.1. Три уровня сохранения

| Уровень | Содержимое | Переносимость | Рекомендация |
|---|---|---|---|
| Логическое состояние | State, workflow step, effect ledger | Между совместимыми версиями через миграцию | Базовый механизм |
| Continuation snapshot | PC/labels, frames, live values | Только при точной совместимости VM/code/schema | Опциональный fast resume |
| Process memory image | Native stacks, mappings и runtime internals | Сильно зависит от ОС и ресурсов | Не использовать как контракт продукта |

Safepoint не обязательно является durable checkpoint point. На safepoint можно безопасно остановить interpreter, но там могут существовать несериализуемые ресурсы, незавершённый turn или незафиксированные намерения. Для долгоживущих workflows предпочтительны явные durable await points, представленные как state machine.

```text
WorkflowState = WaitingForInput {document_id}
              | WaitingForEffect {effect_id, next_step}
              | ProcessingChunk {source_id, offset, partial_result}
              | Completed {result_id}
```

### 17.2. Протокол checkpoint

1. Supervisor останавливает admission новых входов для выбранного actor, сохраняя bounded ingress или возвращая Busy.
2. Actor достигает объявленной checkpoint boundary. Незавершённый mutable turn либо завершается, либо checkpoint откладывается до deadline.
3. Входные сообщения распределяются по cut: consumed подтверждены в state/inbox cursor, остальные остаются в durable inbox либо гарантированно доставляются повторно отправителем. Ephemeral сообщения явно могут быть потеряны.
4. Регистрируются pending effects и их operation IDs. Неизвестный outcome не превращается в «не было эффекта».
5. Создаётся immutable snapshot состояния с module/schema/version, sequence, policy metadata и integrity data.
6. Storage атомарно сохраняет snapshot и manifest с нужной durability. При ошибке остаётся предыдущий валидный snapshot.
7. Actor возобновляется, либо процесс завершается для upgrade.

Checkpoint timeout не разрешает принудительно сериализовать середину native call. Для неотвечающего sandbox используется последний durable checkpoint и recovery policy.

### 17.3. Resume

```text
resume(actor_id):
  snapshot = load_latest_valid_committed_snapshot(actor_id)
  validate_schema_integrity_and_limits(snapshot)
  choose_compatible_module_or_run_migration(snapshot)
  acquire_new_incarnation_and_fencing_token()
  reacquire_capabilities_under_current_policy()
  rebuild_runtime_handles_and_subscriptions()
  reconcile_pending_effects()
  reconstruct_GUI_projection()
  replay_eligible_inputs_after_snapshot_cursor()
  mark_ready_and_open_admission()
```

Права никогда не восстанавливаются только по старому snapshot. Удалённый файл, отозванный аккаунт и отсутствующий сервис дают typed recovery error, а не автоматическое расширение доступа. Таймеры сохраняются как предметный deadline/remaining policy; monotonic timestamp между reboot не переносится напрямую. Для overdue таймера задаётся политика: fire once, skip или bounded catch-up.

Глобально согласованный checkpoint нескольких actors требует отдельного протокола consistent cut или общего transactional store с inbox/outbox boundaries. Набор независимо снятых snapshots не следует называть атомарным снимком всей системы.

<a id="s18"></a>
## 18. Supervision и восстановление

### 18.1. Иерархия

```text
Native supervisor
  ├── VM service supervisor
  │     ├── principal A sandbox -> actor tree A
  │     └── principal B sandbox -> actor tree B
  ├── helper supervisor -> codec/FFI helpers
  ├── agent gateway supervisor
  └── storage/renderer health monitors
```

Supervisor вне sandbox отвечает за process lifetime. Actor supervisors внутри sandbox управляют логическими задачами, но не могут надёжно сообщить о собственном segfault — его видит parent process.

### 18.2. Политики

- `one-for-one`: перезапуск сбойного actor, если зависимости действительно независимы.
- `rest-for-one`/группа: если downstream actors зависят от инвариантов предыдущих.
- Restart budget: максимум N попыток за окно, exponential backoff + jitter, затем quarantine.
- `never-restart`: для неидемпотентных операций, требующих reconciliation или user action.
- Poison message после повторяемого падения помещается в ограниченную quarantine/dead-letter очередь, а не воспроизводится бесконечно.

Конкретные N/окно задаются профилем. Actor restart меняет incarnation, освобождает leases/subscriptions, отзывает runtime capabilities и восстанавливает state. Квоты principal и restart budget сохраняются вне перезапускаемого процесса.

### 18.3. Health не равен heartbeat

Process heartbeat показывает, что какой-то код исполняется, но не доказывает прогресс конкретного actor. Нужны last safepoint, scheduler lag, oldest pending message, last successful commit, storage health и renderer progress. Watchdog не должен зависеть от той же очереди, зависание которой он диагностирует.

Падение VM сохраняет последний committed GUI state с явным индикатором недоступности; действия расширения отключаются. Нельзя оставлять активную кнопку, которая бесконечно посылает команды мёртвому actor. Native host продолжает обслуживать базовую навигацию, сохранение доступных данных и отключение расширения.

Native heap corruption внутри host не лечится catch/exception. Если повреждена доверенная база, допустим controlled shutdown с восстановлением при следующем запуске. Отдельный process помогает только там, где действительно проведена граница процесса.

<a id="s19"></a>
## 19. Hot reload и миграция

### 19.1. Scope reload

Hot reload заменяет MLC module и его логическую state schema. Он не означает безопасную замену native host, GC ABI, live C++ objects или драйвера.

Базовая политика: новые turns исполняются новой code epoch; старые continuations не продолжаются под новым байткодом. Они либо завершаются до reload, либо отменяются и реконструируются из durable workflow state. Сохранение старых continuations допускается только как отдельный режим side-by-side old module, с ограниченным сроком и явным ownership.

### 19.2. Протокол

```text
prepare:
  load_and_verify_new_module()
  check_host_API_and_schema_compatibility()
  allocate_candidate_runtime_with_bounded_budget()
  preflight_migration_on_snapshot_copy()

quiesce:
  stop_new_actor_turns()
  drain_or_cancel_old_tasks_by_policy()
  fence_old_callbacks_and_effect_dispatch()
  persist_checkpoint_and_pending_effect_ledger()

migrate:
  candidate_state = migrate(old_schema, new_schema, old_state)
  validate_new_state_invariants(candidate_state)
  persist_candidate_state_and_module_manifest_atomically()

switch:
  activate_new_code_epoch_and_actor_incarnation()
  rebuild_handles_and_subscriptions()
  reconcile_pending_effects_without_blind_resend()
  publish_new_GUI_projection()
  reopen_admission()
```

Миграция выполняется как чистое bounded преобразование без сети, GUI mutation и неограниченного filesystem access. Ей выдаются отдельные fuel/memory budgets. Ошибка до commit оставляет старый module/state активным. Старый runtime и candidate требуют двойной памяти — она резервируется до quiesce.

### 19.3. Compatibility и rollback

Миграция задаётся для поддерживаемых пар схем или через цепочку явно протестированных переходов. Проверяются defaults, removed fields, enum evolution, числовые диапазоны и неизвестные payload в durable inbox/outbox.

После успешного переключения новая версия может записать state, который старый код не понимает, или совершить внешний эффект. Поэтому rollback к старому snapshot не является автоматическим универсальным откатом: он может потерять новые данные и повторить старые действия. Возможные стратегии: backward-compatible schema, reverse migration, forward fix либо явный recovery mode. Code rollback и data rollback оформляются отдельно.

GUI continuity поддерживается logical widget IDs: host может переиспользовать соответствующие элементы после проверки, но реальные handles и generations контролирует host. Ввод во время quiesce буферизуется с лимитом или получает Busy; просроченный ввод не применяется к новой семантике молча.

### 19.4. Поздние ответы

Ответ старой code epoch не доставляется в новую continuation. Если это только preview, он отбрасывается. Если это подтверждение внешнего эффекта, outcome сохраняется в ledger и передаётся новому workflow через стабильный effect ID. Потеря callback не должна означать потерю знания о выполненной оплате/записи/отправке.

<a id="s20"></a>
## 20. Обновление native host

### 20.1. Выбранная стратегия

Native host обновляется контролируемым restart/handoff, а не заменой кода внутри выполняющегося процесса. Причины: ABI, allocator ownership, существующие threads, native globals, драйверные ресурсы и callbacks.

Updater проверяет источник пакета, подпись доверенным ключом, integrity и совместимость состояния. Политика downgrade и ротации ключей должна быть явной. Загрузка пакета не даёт updater произвольного права исполнять его до проверки.

### 20.2. Последовательность

1. Скачать и проверить пакет в staging area, не затрагивая текущий executable.
2. Проверить свободное место, поддержку текущей state schema и необходимость миграции.
3. Закрыть admission опасных новых операций; завершить или записать pending effects.
4. Сохранить durable state, документы, logical layout и workflow checkpoints.
5. Сформировать handoff manifest без bearer secrets; отметить pending upgrade.
6. Завершить actors/helpers по политике, корректно закрыть GL/окна, остановить host.
7. Запустить новую версию, получить exclusive ownership хранилища и новый host instance ID.
8. Восстановить GUI/resources, заново выдать capabilities, reconcile effects.
9. Подтвердить health новой версии и лишь затем считать upgrade завершённым.

Если используется overlapping handoff процессов, требуется жёсткий storage/broker fencing: старая версия после передачи lease не может писать state и отправлять эффекты. Нельзя позволить двум hosts одновременно стать владельцем одного durable actor.

### 20.3. Совместимость и recovery

Wire major/minor, Host API version, VM semantics version и persistent schema — независимые версии. New host может временно поддерживать предыдущий wire/API контракт, но нельзя автоматически загружать несовместимый module.

Recovery при failed upgrade выбирается по состоянию миграции: до изменения storage возможно вернуться к старому бинарнику; после необратимой миграции нужен совместимый recovery binary/forward repair. Backup проверяется восстановлением, а не только наличием файла. Для критичных данных updater не удаляет последнюю пригодную копию до подтверждённой миграции.

<a id="s21"></a>
## 21. Agent/MCP gateway

### 21.1. Роль gateway

Gateway — адаптер между внешним агентом/MCP-клиентом и предметным Host API. Он не является обходом sandbox и не исполняет произвольный MLC с полномочиями приложения.

```text
Agent tool call
  -> transport authentication / session binding
  -> schema + size validation
  -> principal/capability policy
  -> optional approval bound to exact operation
  -> subject API command
  -> operation ledger
  -> bounded typed result + audit event
```

Примеры tools: `get_document_snapshot`, `propose_document_patch`, `apply_document_patch`, `get_operation_status`, `start_analysis_job`, `cancel_job`. Предпочтительны logical IDs, expected revisions и typed patches. Произвольные координатные клики менее устойчивы и хуже выражают полномочия; если они нужны, это отдельное ограниченное право.

### 21.2. Авторизация и approvals

MCP не делает действие безопасным только потому, что оно описано tool schema. Политика host определяет, какие действия разрешены автоматически, какие требуют одобрения и какие запрещены. Gateway не доверяет описаниям tools от внешнего сервера как правилам своей безопасности.

Подтверждение связывается с principal, operation, target IDs, нормализованными аргументами/payload hash, expected revision, сроком действия и nonce. Изменение параметров или существенное изменение target после подтверждения требует нового решения. Approval dialog отображается trusted host UI, недоступным расширению для подделки. Одобрение чтения не означает одобрение отправки полученных данных наружу.

Для HTTP transport применяются проверка адресата токена, audience/scopes, session binding и безопасная OAuth-конфигурация. Token passthrough между сервисами не используется. Для local transport применяется отдельная модель identity; loopback сам по себе не считается аутентификацией. Подробности зависят от выбранной фиксированной версии MCP. Основание: [MCP Security Best Practices](https://modelcontextprotocol.io/docs/2025-11-25/tutorials/security/security_best_practices).

### 21.3. Prompt injection и данные

Текст документа, web response, tool result и описание ресурса — данные, а не полномочия. Встроенная в них инструкция «отправь секрет» не изменяет capability policy. Проверки исполняются вне модели и не могут быть отключены её ответом.

Ограничиваются egress destinations, доступные fields и объём выдачи. Credentials не попадают в prompts, tool results и logs. Большие ответы выдаются paging/handles с scope, а не неограниченным dump приложения. Передача документа внешнему AI provider — отдельный эффект с отдельными правами.

У каждого tool call есть request ID, deadline, idempotency policy и status query. Длительные операции возвращают job/operation ID. Изменяющий tool не должен повторяться автоматически после transport timeout, пока не определён outcome. Remote MCP servers, если host выступает MCP client, получают свои principals и не наследуют полномочия текущего пользователя целиком.

<a id="s22"></a>
## 22. FFI и расширение доверенной базы

### 22.1. Категории

| Тип native функции | Размещение | Контракт |
|---|---|---|
| Маленькая bounded pure primitive | Runtime/host | Typed values, checked bounds, no blocking |
| Длительный проверенный CPU algorithm | Worker | Immutable input, owned output, cancellation |
| Codec/third-party library с риском crash | Sandbox helper | IPC, quotas, process kill |
| Системная привилегированная интеграция | Broker/service | Narrow capability, audit, no raw pointers |
| Произвольная native библиотека расширения | Не загружается в host | Только отдельный согласованный профиль helper |

### 22.2. Минимальный ABI

```text
NativeResult execute_v1(
  FunctionId id,
  ReadOnlyBytes encoded_input,
  BudgetContext budget,
  CancellationToken cancel
)
```

Это концептуальная форма. Реальный ABI фиксирует widths, alignment, encoding, ownership и способ освобождения result. Исключения не пересекают ABI boundary. Ни одна сторона не освобождает память чужим allocator без установленного контракта. `panic`/exception от доверенной функции переводится в typed error только если процесс и состояние остаются корректными; segfault/heap corruption таким способом не восстанавливаются.

Callbacks доставляются как queued events, а не reentrant MLC invocation. Borrowed pointer действителен только в текущем вызове, не переживает yield/await/GC. Работа, которая может превысить квант, должна иметь собственные cancellation checkpoints или исполняться в killable helper. Unloading библиотеки запрещён, пока живы callbacks, objects, threads или deleters из неё.

Memory-safe bytecode не делает unsafe FFI memory-safe. Каждая добавленная native функция проходит отдельный review input bounds, side effects, allocation, cancellation и authority.

<a id="s23"></a>
## 23. Производительность и управление перегрузкой

### 23.1. Приоритеты оптимизации

Сначала измеряются очереди, latency и allocations; затем уменьшаются количество Host API round trips, размер patches и повторная работа. Batch на 100 изменений обычно предпочтительнее 100 отдельных IPC requests, но не должен превращаться в неограниченную транзакцию.

Порядок оптимизаций: bounded batching → coalescing разрешённых событий → immutable snapshots → кеширование по revision → устранение лишних copies → shared buffers → оптимизация interpreter/JIT. Уменьшать копирование ценой TOCTOU и неясного ownership нельзя.

### 23.2. Начальный performance contract

Числа ниже — **предлагаемые цели для первого performance gate**, а не результаты benchmark. Перед утверждением продукта их нужно прогнать на зафиксированном минимальном hardware; если профиль меняется, цифры пересматриваются явно. Бюджеты не складываются бездумно: CPU, GPU и I/O частично перекрываются, но end-to-end latency измеряется отдельно.

| Метрика | Начальная цель | Условия измерения |
|---|---|---|
| Frame cadence | 60 Hz, бюджет 16,7 ms | Reference desktop scene, обычный режим |
| Frame interval | p95 ≤ 16,7 ms; p99 ≤ 33,4 ms | 10 минут steady load после warm-up |
| UI CPU work | p99 ≤ 4 ms/frame | Commit + ограниченный layout + snapshot build |
| Нормализация input → GUI commit | p95 ≤ 50 ms, p99 ≤ 100 ms | Разрешённая нагрузка, без remote I/O |
| VM quantum | Начальный target около 1–2 ms | Калиброванный fuel + safepoints; не hard deadline |
| Реакция на cancel в VM | p99 ≤ 10 ms | Interpreter-only workload, без blocking native call |
| Один GUI batch | Начальный limit 256 ops / 128 KiB | Также лимит стоимости layout и отдельных properties |
| Обычный IPC frame | Начальный limit 1 MiB | Крупные blobs отдельным протоколом |
| Outstanding actor requests | Начальный limit 64 | На principal дополнительно общий limit |
| Render snapshot queue | 2–3 | Latest eligible snapshot policy |
| Неответивший helper | Grace 500 ms до termination | После cancel, для killable профиля |
| Восстановление малого actor | p95 ≤ 2 s | Snapshot ≤ 1 MiB, доступное storage, без remote reconciliation |

Limits конфигурируются профилем и не являются публичной гарантией до измерений. Опасные элементы payload, например гигантский текст в одной operation, имеют собственные caps. Короткий batch может вызвать дорогой reflow — количество операций не является достаточной моделью затрат.

### 23.3. Benchmark workload

Минимальный reference suite:

- Сцена с 10 000 logical nodes и 1 000 видимых элементов, virtualized lists.
- 20 активных actors, 2 sandbox principals, фоновые CPU jobs и streaming progress.
- 100 GUI property updates за frame с batching; отдельный стресс мелких requests.
- 500 input events/sec с допустимым coalescing pointer movement, но без потери clicks/text.
- Background file parsing, snapshots и периодические GC cycles.
- Один злонамеренный actor с infinite loop, один flooding sender, один crash-loop helper.

Фиксируются CPU model/core count, RAM, GPU/driver, kernel, display server, refresh rate, build mode, power mode и dataset. Измеряются warm/cold runs, p50/p95/p99/max, RSS/heap/native memory, queue age и dropped events. Производительность нельзя обосновывать только throughput без tail latency.

### 23.4. Overload policy

При росте нагрузки сначала сокращаются необязательные previews, частота progress и background jobs. Затем broker прекращает admission новых дорогих операций. Уже принятые транзакции сохраняют свой контракт; нельзя начать частично применять batch из-за нехватки бюджета.

UI/render и control plane имеют зарезервированную долю ресурсов. Actor fairness считается по principal; CPU time native jobs и broker work также атрибутируется инициатору, иначе дорогой Host API обходит VM fuel. Priority inheritance или приоритетный completion path нужен там, где UI-critical операция ждёт низкоприоритетный shared service; главное — не допускать такого ожидания на UI thread.

### 23.5. Memory accounting

```text
principal_memory = VM heaps
                 + continuations and mailbox payloads
                 + in-flight IPC buffers
                 + staged GUI trees and snapshots
                 + worker inputs/results
                 + broker resource allocations
                 + attributed GPU allocations
```

Shared buffers учитываются один раз физически и отдельно как logical quota у владельцев согласно объявленной политике. Временный пик reload/checkpoint/staging учитывается до операции. Linux RSS и внутренний accounting сопоставляются, но не считаются идентичными.

<a id="s24"></a>
## 24. Observability и диагностика

### 24.1. Correlation

Каждый запрос несёт trace ID и operation/request ID; broker добавляет principal, actor incarnation, code epoch, host instance, transaction ID и relevant revision. Идентификаторы высококардинальны и подходят для logs/traces, но не как безграничные labels каждого metric.

```text
Trace:
  input.received
    -> actor.mailbox_wait
    -> actor.turn
    -> host.request_admission
    -> ipc.transfer
    -> gui.stage
    -> gui.commit(revision=N)
    -> render.submit(revision=N)
    -> presentation.ack(contract=backend_defined)
```

### 24.2. Обязательные метрики

| Область | Метрики |
|---|---|
| VM | Fuel consumption, instructions, safepoint gap, turn time, traps |
| GC | Heap live bytes, allocated bytes, pause/step duration, pinned bytes |
| Scheduler | Runnable count, wait time, fairness, suspended tasks |
| IPC | Bytes, message rate, queue age, rejects, reconnects, malformed frames |
| GUI | Batch size/cost, commit duration, revision conflicts, layout time |
| Render | Frame intervals, upload budget, resource count, fence wait, lost context |
| Persistence | Commit latency, snapshot age, outbox lag, unknown outcomes |
| Supervision | Restart count, quarantine, heartbeat/progress age |
| Security | Denied/revoked operations, sandbox profile failures, approval mismatch |

Audit log безопасности отделяется от debug logs. В него попадают решения по полномочиям и эффекты, но не секретные payload. Audit write failure для действий, где обязательный аудит является частью политики, приводит к отказу admission; для обычной телеметрии допускается bounded drop. Конкретная политика должна быть явно задана, чтобы переполненный диск логов не неожиданно отключал всё приложение.

### 24.3. Диагностические снимки

Host предоставляет bounded debug snapshot: actors и состояния задач, возраст очередей, resource counts, последние безопасные error summaries, active module hashes и sandbox profiles. Для недоверенного клиента snapshot фильтруется по scope. Native addresses, токены, содержимое документов и чужие principals не раскрываются автоматически.

Crash report содержит версии, fault class, последние trace IDs и минимальные данные. Полные heap/core dumps могут содержать секреты и требуют отдельной политики хранения/экспорта. Symbolication хранит связь bytecode offset с исходником без необходимости публиковать весь исходный текст.

### 24.4. Replay

Для воспроизводимости можно записывать inputs, порядок сообщений, результаты времени/random/Host API и scheduling decisions. Replay запускается в режиме без реальных внешних эффектов. Записанный результат не является новым разрешением выполнить эффект.

Детерминированность VM не означает детерминированность всей системы: GPU, OS events и external services недетерминированы. Scope replay фиксируется: например, actor reducer по заданной последовательности входов. Более широкий deterministic scheduler — отдельная дорогая функция.

<a id="s25"></a>
## 25. Тестирование и критерии приёмки

### 25.1. Стратегия

Тестирование охватывает смысл контрактов и границы отказа, а не только happy path. Для каждого инварианта есть positive/negative tests; для каждого persisted transition — crash injection до и после durability boundary.

| Уровень | Что проверяется |
|---|---|
| Unit/property | Handle reuse, type checks, capability attenuation, revision monotonicity |
| Verifier fuzzing | Malformed CFG, invalid stack/type joins, exception edges, size/complexity bombs |
| Interpreter differential | Одинаковая семантика reference interpreter и optimized backend |
| IPC fuzzing | Truncated/oversized frames, nesting, unknown tags, duplicate IDs, fd abuse |
| Model/state-machine tests | Cancel vs commit, revoke vs commit, completion vs restart, subscription gaps |
| Concurrency stress | Actor single ownership, race detection, queue shutdown, buffer lifetime |
| Crash/power-loss simulation | State/outbox atomicity, snapshots, dedup, upgrade recovery |
| Sandbox integration | Фактические запреты FS/network/display/ptrace и limits на поддерживаемых ОС |
| GUI/render integration | Atomic batches, stale handles, frame lifetimes, context recreation |
| Security review | Confused deputy, SSRF, permission escalation, prompt injection, update chain |
| Performance/soak | Tail latency, bounded memory, restart storm, GC, queue saturation |

Sanitizers и race detectors применяются к поддерживаемым native builds; отсутствие находок не является доказательством безопасности. Fuzzing verifier и IPC выполняется с ограничением ресурсов, corpus regression и coverage tracking. При использовании unsafe блоков они перечисляются и отдельно ревьюятся.

### 25.2. Приёмка безопасности и корректности

| ID | Проверка | Условие прохождения |
|---|---|---|
| A01 | Некорректный байткод и parser fuzz corpus | Нет crash/UB host; модуль отвергается bounded способом |
| A02 | Out-of-bounds, forged types и forged capability | Trap/denial без доступа к чужой памяти/ресурсу |
| A03 | Бесконечный цикл VM | Scheduler сохраняет fairness; cancel проходит в заданном профиле |
| A04 | Actor ownership under stress | Ни одного одновременного executor у actor |
| A05 | Handle ABA и cross-session replay | Старый handle/epoch никогда не меняет новый объект |
| A06 | Batch с ошибкой в последней операции | Наблюдаемое GUI state полностью равно исходному |
| A07 | Конкурирующие batches одной revision | Один commit; второй RevisionConflict либо иной явно заданный serial результат |
| A08 | Revoke/commit race | После точки отзыва нет нового разрешённого commit по отозванному праву |
| A09 | Await + изменение state | Устаревший результат отвергается/пересчитывается по контракту |
| A10 | Late completion после restart/reload | Нет исполнения старой continuation; effects ledger не теряет outcome |
| A11 | Flood mailbox/IPC/logs | Память остаётся в квоте; control plane и UI сохраняют работоспособность |
| A12 | Sandbox compromise simulation | Нет доступа к host files/secrets/display вне профиля; broker отвергает чужие scopes |
| A13 | Наследованные fd/env | Неожиданные descriptors и секреты отсутствуют; запреты проверены реально |
| A14 | Native helper segfault/hang | Host жив; helper ограниченно завершается/перезапускается |
| A15 | Crash в state/outbox transaction | Видны либо все связанные записи, либо ни одной |
| A16 | Crash после отправки внешнего эффекта | Reconcile/idempotency; нет необоснованного blind retry |
| A17 | Subscription overflow/reconnect | Reset/snapshot; клиент не продолжает применять несовместимые deltas |
| A18 | Migration failure | Старый module/state доступны; новые эффекты не выполнены |
| A19 | Host upgrade failure | Проверенный recovery path без двух активных владельцев storage |
| A20 | MCP injection/approval substitution | Текст данных не выдаёт права; изменение args делает approval недействительным |
| A21 | Shared buffer mutation race | Receiver использует immutable/copy или доказанный ownership; нет TOCTOU |
| A22 | Snapshot с повреждением/неизвестной schema | Безопасный отказ или предыдущий валидный snapshot, без исполнения данных |

A12 не доказывает отсутствие kernel exploit: это проверка заявленной policy boundary. Безопасность профиля дополнительно требует review и контроля обновлений зависимостей.

### 25.3. Приёмка производительности и устойчивости

- На зафиксированном reference workload выполняются цели раздела 23; публикуются исходные measurements и метод percentile calculation.
- После 8-часового soak с повторными reload/restart/subscribe/unsubscribe память возвращается к установленному steady-state диапазону; количество живых handles, fd и GPU resources не растёт монотонно без причины.
- При нагрузке 2× от admission limits происходит предсказуемый Busy/coalescing/throttling, а не неограниченный рост очередей.
- Один flood/crash-loop principal не блокирует ввод и действия другого principal.
- UI thread не содержит синхронных ожиданий VM, storage, network и workers; blocking instrumentation не находит нарушений в suite.
- Checkpoint/recovery тестируется с disk full, permission error, повреждённым последним snapshot и отсутствующим внешним ресурсом.
- Инъекция crash в каждую критическую фазу reload/upgrade не приводит к смешению schema, двойной записи и повторному неконтролируемому эффекту.

### 25.4. Минимальные negative scenarios

```text
1. Создать widget H(g=7), удалить, переиспользовать slot с g=8.
   Отправить SetProperty на H(g=7). Ожидание: StaleHandle.

2. В batch [Set A, Create B, Invalid Move C] последняя операция неверна.
   Ожидание: нет A change, нет B, та же revision.

3. Запустить await task в code_epoch=4, reload до 5, доставить старый ответ.
   Ожидание: старый callback не исполняется; effect outcome учтён отдельно.

4. Commit state+outbox; отправить запрос; crash до записи ответа.
   Ожидание: тот же idempotency key или OutcomeUnknown/reconciliation.

5. Отозвать parent capability при queued GUI command от delegated child.
   Ожидание: command после revoke fence отклонён.

6. Изменить shared-memory payload после preliminary validation.
   Ожидание: mutation невозможна либо host использует собственную копию.
```

<a id="s26"></a>
## 26. Этапы реализации

Этапы описывают зависимости и exit gates; календарные оценки зависят от команды, языка host и текущего состояния VM.

### Этап 0. Контракты и threat model

**Результат:** выбранные ISA/type semantics, trust profiles, versioning, capability model, error taxonomy и определения commit/cancel/durability. Оформляются ADR по actor reentrancy и GUI transaction boundary.

**Gate:** инварианты I01–I20 имеют owners и тестовые сценарии; установлено, какой код допустим внутри host. Без этого нельзя обещать поддержку недоверенных расширений.

### Этап 1. Минимальная безопасная VM

**Результат:** bounded decoder, verifier, interpreter, typed heap, fuel/safepoints, trap model и module loader. Начальная интеграция с небольшим fake Host API.

**Gate:** A01–A04, fuzz corpus, limits на allocation и verifier complexity; бесконечный цикл не захватывает весь scheduler. Встроенный режим пока только для доверенного кода.

### Этап 2. Host API и GUI boundary

**Результат:** capability table, handles/generation/revision, async requests, atomic GUI batches, event queue и единый UI/GL owner.

**Gate:** A05–A10; невозможно получить `Widget*`/GL name из MLC; batch rollback и revocation race проверены. API не блокирует UI.

### Этап 3. Actors и bounded concurrency

**Результат:** mailboxes, continuations, structured cancellation, fair scheduler, worker/I/O separation, subscriptions с resync.

**Gate:** A04, A09–A11, A17; soak/stress без двойного исполнения actor и lost wakeup. Политика reentrancy документирована и отражена в SDK.

### Этап 4. Process isolation и IPC

**Результат:** launcher, sandbox profiles, authenticated framed IPC, credits, quotas, supervisor и native helpers.

**Gate:** A12–A14, A21; фактические security tests на поддерживаемых Linux distributions и X11/Wayland environments. Только после этого появляется заявленная поддержка недоверенного кода.

### Этап 5. Persistence и recovery

**Результат:** schemas, durable state, inbox/outbox, operation ledger, checkpoint/resume, leases и fencing.

**Gate:** A15–A16, A22; fault injection на storage boundaries; тест восстановления без сети и с неизвестным outcome. Гарантии потери ephemeral state явно отделены от durable state.

### Этап 6. Reload и host upgrade

**Результат:** pure migration runner, code epochs, quiesce protocol, staged module switch, update verification и controlled restart.

**Gate:** A18–A19; проверенный rollback/recovery для поддерживаемых версий, отсутствие повторения внешних эффектов и смешения handles.

### Этап 7. Agent/MCP gateway

**Результат:** предметные tools, authentication, scopes, review/approval binding, job/status/cancel и audit.

**Gate:** A20 и повторное прохождение capability/effect tests через gateway. Текст модели не влияет на policy enforcement. MCP version закреплена и compatibility проверена.

### Этап 8. Performance hardening

**Результат:** полный reference benchmark, профили allocations/queues, bounded layout, resource caches. Отдельный render thread/shared buffers/JIT вводятся только если решают измеренную проблему.

**Gate:** performance/soak criteria, regression dashboard и документированная совместимость backend/driver. Любая оптимизация повторно проходит относящиеся к ней safety tests.

<a id="s27"></a>
## 27. Реестр решений и открытые вопросы

### 27.1. Tradeoffs

| Решение | Преимущество | Цена/ограничение | Когда пересматривать |
|---|---|---|---|
| VM внутри host | Низкий call overhead | Native crash общей судьбы | Для доверенной core logic после profiling |
| VM отдельным процессом | Crash/security boundary | IPC, startup, serialization | Базовый выбор для недоверенного кода |
| Actor-local heap | Простое ownership и локальные паузы | Copy и overhead множества heaps | При тысячах малых actors |
| Reentrant await | Прогресс actor во время I/O | Нужно проверять revisions после await | Для строгих workflow можно serial actor |
| Coarse subtree revision | Простой serializable GUI commit | Ложные конфликты независимых edits | После появления реального contention |
| Immutable render snapshots | Ясный ownership между потоками | Память/copy или persistent structures | Оптимизировать структурное sharing |
| One GL context | Простая синхронизация | Ограничение параллельной submission | Только при доказанном bottleneck |
| Logical checkpoint | Совместимость и простое recovery | Нужны явные workflow states | Continuation snapshot как optional fast path |
| Brokered effects | Политика и ledger в одном месте | Broker входит в TCB и может быть bottleneck | Шардирование без потери identity/fencing |
| Copy IPC | Простая безопасность | Bandwidth и allocations | Shared buffers для крупных данных |
| Interpreter first | Меньше TCB, проще metering | Меньшая compute throughput | JIT после стабилизации семантики |
| GUI как projection | Не нужен distributed commit с окном | Eventual update после storage commit | Обычно сохранять эту модель |

### 27.2. Что нужно определить до production

1. Язык native host, GUI/window library и целевые OS/architectures.
2. Точная ISA, GC strategy, exception semantics и допустимый memory model.
3. Какие модули считаются trusted и кто может менять этот статус.
4. Минимальный Linux sandbox profile и политика при недоступности namespaces/cgroups/LSM.
5. Каталог Host API, resource kinds и операции, требующие отдельного approval.
6. Storage engine, durability mode, retention, encryption/key management и recovery ownership.
7. Какие workflows требуют durable resume, а какие могут начинаться заново.
8. Какие внешние сервисы поддерживают idempotency/status query и как обрабатывается unknown outcome.
9. Fixed MCP version/transports и правила egress для agent integration.
10. Минимальное hardware, максимальный документ, node count и latency SLO.
11. Требуется ли isolation renderer process из-за модели угроз или стабильности драйвера.
12. Политика пакетных подписей, update trust roots, совместимости и emergency recovery.

Эти вопросы не препятствуют разработке reference implementation, но не должны оставаться неявными при объявлении security и durability guarantees.

<a id="s28"></a>
## 28. Сквозные сценарии и матрица отказов

### 28.1. Пользователь редактирует документ, VM обновляет preview

1. Native UI принимает ввод и посылает предметную команду с revision документа.
2. Broker проверяет principal/capability и admission.
3. Durable actor формирует новую версию документа и сохраняет её transactionally.
4. Subscription публикует committed document revision.
5. Actor запускает worker preview job с immutable source и этой revision.
6. UI обновляет текстовую проекцию независимо от завершения preview.
7. Worker возвращает preview; actor сверяет document revision.
8. Актуальный preview устанавливается GUI batch, устаревший освобождается.
9. Render snapshot передаёт ресурсы GL owner; удаление старого preview ждёт корректного lifetime boundary.

Если preview job упала, документ остаётся сохранённым, а GUI показывает ошибку/предыдущий preview. Вспомогательная визуализация не определяет durability документа.

### 28.2. Недоверенный модуль зависает и перезапускается

1. VM fuel заканчивается; task уступает execution.
2. Principal quota не позволяет обходить fairness новыми actors.
3. Supervisor запрашивает отмену; native hang в sandbox может не ответить.
4. После grace sandbox завершается как процесс.
5. Broker отзывает leases и fencing старой incarnation, отмечает pending outcomes.
6. GUI остаётся на последней committed projection, действия недоступного модуля отключены.
7. По restart policy новый sandbox получает новый session/actor epoch.
8. Восстанавливается durable state; полномочия выдаются заново по текущей policy.
9. Выполняется reconciliation, затем возобновляются подписки и admission.

Если рестарты повторяются, principal переводится в quarantine. Цикл рестартов не сбрасывает квоты и не повторяет неконтролируемые эффекты.

### 28.3. Hot reload во время внешнего запроса

1. Старая версия записала effect intent E и отправила его через broker.
2. Reload останавливает новые turns и сохраняет E как pending/dispatched.
3. Миграция переносит workflow state, сохраняя E и idempotency key.
4. Активируется новая code epoch; старая continuation недействительна.
5. Поздний ответ на E попадает в ledger, а не в старый стек.
6. Новый workflow получает outcome по E и выполняет следующий шаг.
7. Если ответ потерян, используется status query/идемпотентный retry; иначе E становится OutcomeUnknown.

### 28.4. Агент предлагает изменение

1. Агент получает ограниченный document snapshot с revision R.
2. Формирует typed patch и отправляет tool call с `expected_revision=R`.
3. Gateway проверяет scope и связывает необходимое approval с точным patch.
4. Пока пользователь/политика принимает решение, документ может измениться.
5. Commit повторно проверяет revision: конфликт возвращается агенту без частичного применения.
6. Агент получает новый snapshot и формирует новое предложение; старое approval не переносится на другой patch.
7. Успешный durable commit публикуется в GUI обычной subscription.

### 28.5. Матрица отказов

| Отказ | Наблюдаемое состояние | Реакция | Что нельзя делать |
|---|---|---|---|
| Invalid bytecode | Module load rejected | Ошибка модуля, bounded diagnostic | Запускать «в permissive mode» |
| VM trap/OOM | Task/actor failed | Supervision, последний durable state | Продолжать с неизвестно повреждённым state |
| Infinite loop | Нет логического прогресса | Fuel, throttle, cancel | Полагаться только на watchdog UI |
| Native helper hang | Request pending | Cancel grace, process termination | Убивать произвольный thread host |
| Sandbox segfault | IPC disconnect | Fence/revoke, restart/quarantine | Считать все pending effects невыполненными |
| IPC malformed/flood | Protocol error/overload | Deny/close/throttle, audit | Выделять память по непроверенной длине |
| Stale handle | Resource не соответствует generation | StaleHandle | Подставлять объект по одному slot |
| Revision conflict | State изменилось | Refresh/recompute | Применять stale patch автоматически |
| Capability revoked | Право недействительно | Deny after fence, cancel future steps | Восстанавливать право из snapshot |
| Subscription gap | Неизвестная непрерывность | Reset + snapshot/cursor recovery | Применять delta к другой base revision |
| Storage full | Durable commit не подтверждён | Fail write, preserve last committed state | Выдавать durable ACK |
| Snapshot corrupted | Integrity/schema error | Previous valid snapshot/recovery | Слепо десериализовать executable state |
| Crash after external send | Outcome неизвестен | Reconcile/idempotency/manual resolution | Blind retry non-idempotent operation |
| GPU context lost | Renderer resources invalid | Новый render epoch, rebuild | Повторно использовать старые GL names |
| GPU driver hang | Возможен UI/process stall | Recovery по backend, возможно restart | Обещать, что fuel остановит драйвер |
| Migration failed | Candidate не принят | Сохранить прежний module/state | Смешивать старую schema с новым кодом |
| Upgrade failed after schema migration | Старый binary может быть несовместим | Compatible recovery/forward fix | Безусловно запускать старую версию |
| Agent prompt injection | Враждебные данные | Policy вне модели, scope limits | Считать текст разрешением |

<a id="s29"></a>
## 29. Источники и терминология

### 29.1. Первичные источники

Проверены при подготовке документа 15 октября 2026 года. Они обосновывают свойства внешних платформ; конкретная архитектура MLC VM, интерфейсы и budgets являются проектными предложениями этого документа. Страницы `latest` изменяются; перед реализацией нужно закрепить поддерживаемые версии и сохранить результаты compatibility tests.

1. [GLFW — Context guide](https://www.glfw.org/docs/latest/context.html): current context и передача между потоками.
2. [GLFW — Introduction to the API](https://www.glfw.org/docs/latest/intro): требования к потокам и thread safety оконного API.
3. [Linux kernel — Seccomp BPF](https://docs.kernel.org/userspace-api/seccomp_filter.html): syscall filtering, ABI checks и границы механизма.
4. [Linux kernel — Landlock userspace API](https://cdn.kernel.org/doc/html/latest/userspace-api/landlock.html): ограничения доступа, runtime ABI, наследование и caveats.
5. [Linux kernel — Control Group v2](https://docs.kernel.org/admin-guide/cgroup-v2.html): CPU/memory/process controls и delegation.
6. [X.Org — Security Extension Specification](https://www.x.org/releases/X11R7.7-RC1/doc/xextproto/security.pdf): доверие к X clients и ограничения SECURITY extension.
7. [Model Context Protocol — Security Best Practices](https://modelcontextprotocol.io/docs/2025-11-25/tutorials/security/security_best_practices): confused deputy, token handling и transport/security concerns. Это versioned reference, не заявление о самой новой версии MCP.

### 29.2. Краткий словарь

| Термин | Значение в документе |
|---|---|
| Actor | Состояние и mailbox с единственным активным исполнителем |
| Turn | Участок обработки сообщения/continuation до объявленной границы |
| Continuation | Сохранённое продолжение VM-задачи, не OS thread |
| Capability | Проверяемое ограниченное полномочие на ресурс/операцию |
| Principal | Субъект security policy и общего resource accounting |
| Handle | Runtime identity ресурса, не само право доступа |
| Generation | Версия жизни объекта в переиспользуемом slot |
| Revision | Версия содержимого живой сущности |
| Epoch/incarnation | Версия сессии, кода или процесса исполнения |
| Safepoint | Точка, где runtime может безопасно обработать остановку/GC/yield |
| Fuel | Счётчик вычислительной работы, не точное wall-clock время |
| Broker | Доверенный посредник, проверяющий полномочия и исполняющий эффекты |
| Fencing | Запрет старому владельцу изменять состояние после смены владельца |
| Outbox | Durable намерения эффектов, записанные вместе с state transaction |
| Reconciliation | Установление фактического результата незавершённой операции |
| Backpressure | Ограничение admission/скорости при насыщении потребителя |
| Quiesce | Контролируемое прекращение новых turns и достижение безопасной границы |
| TCB | Компоненты, корректности которых требует модель безопасности |

**Архитектурная позиция:** native host сохраняет владение GUI, OpenGL и системными эффектами; MLC VM выражает поведение через ограниченные асинхронные контракты. Actors дают управляемую конкурентность, verifier и runtime обеспечивают безопасность модели исполнения, а sandbox-процессы ограничивают последствия ошибок native реализации. Durability, reload и агентная автоматизация строятся поверх тех же identity, authority и effect semantics, а не вводят обходные пути.
