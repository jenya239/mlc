# Migration Mapping: Old → New Structure

## ЭТАП 1.5: Корневые файлы

### Registries → core/

| Старый путь | Новый путь | Module |
|------------|-----------|--------|
| `lib/mlc/type_registry.rb` | `lib/mlc/core/type_registry.rb` | `MLC::TypeRegistry` → `MLC::Core::TypeRegistry` |
| `lib/mlc/function_registry.rb` | `lib/mlc/core/function_registry.rb` | `MLC::FunctionRegistry` → `MLC::Core::FunctionRegistry` |
| `lib/mlc/function_signature.rb` | `lib/mlc/core/function_signature.rb` | `MLC::FunctionSignature` → `MLC::Core::FunctionSignature` |

### Infrastructure → infrastructure/

| Старый путь | Новый путь | Module |
|------------|-----------|--------|
| `lib/mlc/event_bus.rb` | `lib/mlc/infrastructure/event_bus.rb` | `MLC::EventBus` → `MLC::Infrastructure::EventBus` |
| `lib/mlc/pass_manager.rb` | `lib/mlc/infrastructure/pass_manager.rb` | `MLC::PassManager` → `MLC::Infrastructure::PassManager` |

### Entry point (остаётся)

| Файл | Статус |
|------|--------|
| `lib/mlc/application.rb` | Остаётся в корне |
| `lib/mlc/semantic_gen.rb` | Остаётся в корне (phase loader) |

## ЭТАП 2: Stdlib utilities → compiler/stdlib/

| Старый путь | Новый путь | Module |
|------------|-----------|--------|
| `lib/mlc/services/stdlib_scanner.rb` | `lib/mlc/compiler/stdlib/scanner.rb` | `MLC::StdlibScanner` → `MLC::Compiler::StdlibScanner` |
| `lib/mlc/services/stdlib_resolver.rb` | `lib/mlc/compiler/stdlib/resolver.rb` | `MLC::StdlibResolver` → `MLC::Compiler::StdlibResolver` |
| `lib/mlc/services/stdlib_signature_registry.rb` | `lib/mlc/compiler/stdlib/signature_registry.rb` | `MLC::StdlibSignatureRegistry` → `MLC::Compiler::StdlibSignatureRegistry` |

## ЭТАП 3: SemanticGen services reorganization

### Scope management → semantic_gen/services/scope/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/services/var_type_registry.rb` | `lib/mlc/semantic_gen/services/scope/var_type_registry.rb` |
| `lib/mlc/services/scope_context_service.rb` | `lib/mlc/semantic_gen/services/scope/scope_context.rb` |
| `lib/mlc/services/module_context_service.rb` | `lib/mlc/semantic_gen/services/scope/module_context.rb` |

### Builders → semantic_gen/services/builders/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/semantic_gen/services/ir_builder.rb` | `lib/mlc/semantic_gen/services/builders/ir_builder.rb` |
| `lib/mlc/semantic_gen/services/ast_factory.rb` | `lib/mlc/semantic_gen/services/builders/ast_factory.rb` |
| `lib/mlc/semantic_gen/services/type_builder.rb` | `lib/mlc/semantic_gen/services/builders/type_builder.rb` |
| `lib/mlc/semantic_gen/services/record_literal_builder.rb` | `lib/mlc/semantic_gen/services/builders/record_literal_builder.rb` |
| `lib/mlc/semantic_gen/services/array_literal_builder.rb` | `lib/mlc/semantic_gen/services/builders/array_literal_builder.rb` |

### Checkers → semantic_gen/services/checkers/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/semantic_gen/services/ast_type_checker.rb` | `lib/mlc/semantic_gen/services/checkers/ast_type_checker.rb` |
| `lib/mlc/semantic_gen/services/semantic_ir_classifier.rb` | `lib/mlc/semantic_gen/services/checkers/semantic_ir_classifier.rb` |

### Features → semantic_gen/services/features/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/semantic_gen/services/loop_service.rb` | `lib/mlc/semantic_gen/services/features/loop_handler.rb` |
| `lib/mlc/semantic_gen/services/lambda_service.rb` | `lib/mlc/semantic_gen/services/features/lambda_handler.rb` |
| `lib/mlc/semantic_gen/services/match_service.rb` | `lib/mlc/semantic_gen/services/features/match_handler.rb` |
| `lib/mlc/semantic_gen/services/list_comprehension_service.rb` | `lib/mlc/semantic_gen/services/features/comprehension_handler.rb` |
| `lib/mlc/semantic_gen/services/index_access_service.rb` | `lib/mlc/semantic_gen/services/features/index_access_handler.rb` |

### Imports → semantic_gen/services/imports/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/semantic_gen/services/import_service.rb` | `lib/mlc/semantic_gen/services/imports/import_processor.rb` |
| `lib/mlc/semantic_gen/services/module_resolver.rb` | `lib/mlc/semantic_gen/services/imports/module_resolver.rb` |

### Registration → semantic_gen/services/registration/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/services/type_registration_service.rb` | `lib/mlc/semantic_gen/services/registration/type_registrar.rb` |
| `lib/mlc/services/function_registration_service.rb` | `lib/mlc/semantic_gen/services/registration/function_registrar.rb` |
| `lib/mlc/services/sum_type_constructor_service.rb` | `lib/mlc/semantic_gen/services/registration/constructor_registrar.rb` |

### Utils → semantic_gen/services/utils/

| Старый путь | Новый путь |
|------------|-----------|
| `lib/mlc/services/purity_analyzer.rb` | `lib/mlc/semantic_gen/services/utils/purity_analyzer.rb` |
| `lib/mlc/services/type_unification_service.rb` | `lib/mlc/semantic_gen/services/utils/type_unification.rb` |
| `lib/mlc/services/type_resolution_service.rb` | `lib/mlc/semantic_gen/services/utils/type_resolution.rb` |

### Inference → semantic_gen/services/inference/

| Старый путь | Новый путь | Примечание |
|------------|-----------|-----------|
| `lib/mlc/services/type_inference_service.rb` | `lib/mlc/semantic_gen/services/inference/type_inference.rb` | Основной файл |
| `lib/mlc/semantic_gen/services/expression_type_resolver.rb` | **УДАЛИТЬ** | Дублирует TypeInferenceService |
| `lib/mlc/semantic_gen/services/identifier_type_resolver.rb` | **УДАЛИТЬ** | Дублирует TypeInferenceService |
| `lib/mlc/semantic_gen/services/member_access_service.rb` | **УДАЛИТЬ** | Дублирует TypeInferenceService |

## ЭТАП 4: Удаление дублирования

### Файлы для удаления:

- `lib/mlc/semantic_gen/services/expression_type_resolver.rb`
- `lib/mlc/semantic_gen/services/identifier_type_resolver.rb`
- `lib/mlc/semantic_gen/services/member_access_service.rb`

**Заменить uses на:**
- `@type_inference.infer_binary_type(...)`
- `@type_inference.infer_variable_type(...)`
- `@type_inference.infer_member_type(...)`

## ЭТАП 5: Разделение больших классов

### TypeChecker → 3 класса

| Новый файл | Ответственность |
|-----------|----------------|
| `lib/mlc/semantic_gen/services/checkers/type_validator.rb` | Валидация типов |
| `lib/mlc/semantic_gen/services/utils/type_name_resolver.rb` | Именование типов |
| `lib/mlc/semantic_gen/services/utils/error_reporter.rb` | Error reporting |

### TypeInferenceService → 4 класса

| Новый файл | Ответственность |
|-----------|----------------|
| `lib/mlc/semantic_gen/services/inference/type_inference.rb` | Координация |
| `lib/mlc/semantic_gen/services/inference/expression_types.rb` | Binary/unary |
| `lib/mlc/semantic_gen/services/inference/member_types.rb` | Member access |
| `lib/mlc/semantic_gen/services/inference/generic_types.rb` | Generics |

## Статус выполнения

- [x] ЭТАП 1: Создание структуры каталогов
- [ ] ЭТАП 1.5: Реорганизация корневых файлов
- [ ] ЭТАП 2: Перенос stdlib utilities
- [ ] ЭТАП 3: Группировка semantic_gen/services
- [ ] ЭТАП 4: Удаление дублирования
- [ ] ЭТАП 5: Разделение больших классов
- [ ] ЭТАП 6: Финализация
