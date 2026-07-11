# Track: Retained scene-graph фундамент (classic UI + game UI + Flash-rich + Figma/blueprint canvas) — design-only

Parent: [../GUI.md](../GUI.md), [../archive/tracks/TRACK_GUI_FRAMEWORK.md](../archive/tracks/TRACK_GUI_FRAMEWORK.md),
[../TEXT_RENDERING.md](../TEXT_RENDERING.md) §5.3.
Trigger: пользователь 2026-07-11 — хочет ультрабыстрое масштабируемое поле
со связями (Figma/blueprint), **и** классические интерфейсы, **и**
игровые, **и** вычурные как во Flash — **одним** фреймворком, не
отдельными системами.

## Status: open, design-only, низкий приоритет

## Ключевая коррекция (2026-07-11)

Первая версия этого трека ошибочно называла canvas-со-связями «другой
архитектурой, не расширением v0». Это неверно: classic/game/Flash-rich/
Figma-canvas — **один фундамент**, разный контент поверх него:

- **Camera/zoom** = трансформация корневого узла дерева, не отдельный
  механизм — если у каждого узла есть affine-трансформация (position/
  scale/rotation) относительно родителя (как `_x/_y/_xscale/_yscale/
  _rotation` у Flash `DisplayObject`), pan/zoom всей сцены — тот же
  примитив на верхнем узле.
- **Вложенные классические виджеты** и **вложенные Flash `MovieClip`** —
  одно и то же дерево узлов с локальной трансформацией.
- **Игровой UI** — тот же tree+transform+batched draw, отличается только
  контентом (текстуры вместо форм).
- **Связи-кривые blueprint** и **вычурные Flash-формы** — один примитив
  (vector path: bezier fill+stroke), не два разных.
- **Spatial culling/instancing** — не отдельная архитектура, а опциональный
  проход (quadtree по bounding box узлов) над тем же деревом; включается
  когда узлов много, no-op на 20 узлах классической формы.

Вывод: нужен **один** retained affine-transform scene graph + vector-
primitive рендерер с самого начала. v0 (`misc/gui/`, screen-space `Rect`,
чистый immediate-mode) — прототип одной кнопки, не основа: в нём нет
трансформации на узле и нет retained-дерева. Не патчить v0 → нужен новый
фундамент, на котором classic/game/Flash-rich/canvas — конфигурации, не
отдельные ветки кода.

## Требования к фундаменту (по значимости)

| # | Требование | Сегодня | Нужно |
|---|-----------|---------|-------|
| 1 | Узел сцены = affine-трансформация (position/scale/rotation) относительно родителя | Нет — `layout.mlc` `Rect`/`Point` screen-space `i32`, без трансформации/иерархии | Retained tree, трансформация накапливается по цепочке родителей (как Flash `DisplayObject` matrix) |
| 2 | Vector-примитив (bezier path fill+stroke) | Нет — только rect/circle/line helper в `gl_renderer.mlc` | Path-примитив нужен и для blueprint-связей, и для вычурных форм |
| 3 | Retained-дерево с dirty-tracking | Нет — пересчёт каждый кадр (ок для одной кнопки) | Перекладывать/перетесселлировать только изменившееся — критично при анимациях (Flash-style tweens) и большом дереве |
| 4 | Batched/instanced draw | Нет — per-shape triangle helper | GPU instancing для тысяч узлов (canvas/игра); для 20 классических виджетов не критично, но не мешает |
| 5 | Опциональный spatial index (culling) | Нет | Проход над тем же деревом, включается по порогу числа узлов, не отдельная структура данных для canvas |
| 6 | Hit-testing/событийная модель через то же дерево | Нет (только `point_in_rect` для одной кнопки) | Event bubbling через transform-цепочку — общее для click в классике, hover в игре, drag узла в canvas |
| 7 | Текст при произвольном масштабе | **Готово** — MSDF `RenderMode=MsdfRgb8`, single atlas entry per glyph | — |

## Decision — отложено

Не фиксировать API/структуры до решения строить: нужен отдельный design-
STEP с разбором конкретной библиотеки-референса (структура matrix-стека,
формат retained-tree — не переизобретать, посмотреть на существующие
scene-graph 2D движки: Flash `DisplayObjectContainer`, Skia `SkCanvas`
save/restore stack, Godot `CanvasItem`). Когда трек активируется — STEP=1
должен закрыть: формат дерева (запись MLC vs указатели/индексы для
кеш-локальности), формат matrix (2×3 affine достаточно, без перспективы),
координатная точность на больших полях (camera-relative rendering —
вычитать позицию камеры перед отправкой в GPU).

## Out of scope (пока)

- Реализация — трек фиксирует требование, не авторизует код.
- v0 (`misc/gui/`) не трогать до решения строить — параллельный
  прототип, не основа будущего фундамента.

## Steps

| Step | Item | Status |
|------|------|--------|
| — | Design-only, не активирован. Активировать явной командой пользователя. | n/a |
