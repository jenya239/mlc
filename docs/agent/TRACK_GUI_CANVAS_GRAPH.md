# Track: Retained scene-graph фундамент (classic UI + game UI + Flash-rich + Figma/blueprint canvas)

Parent: [../GUI.md](../GUI.md), [../archive/tracks/TRACK_GUI_FRAMEWORK.md](../archive/tracks/TRACK_GUI_FRAMEWORK.md),
[../TEXT_RENDERING.md](../TEXT_RENDERING.md) §5.3.
Trigger: пользователь 2026-07-11 — хочет ультрабыстрое масштабируемое поле
со связями (Figma/blueprint), **и** классические интерфейсы, **и**
игровые, **и** вычурные как во Flash — **одним** фреймворком, не
отдельными системами.

## Status: **closed** (2026-07-16) — Phase A–D Critic OK

Phase A: [archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md).
Phase B: [archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md)
(**closed** 2026-07-14, Critic OK).
Phase C: [archive/tracks/TRACK_GUI_SCENE_PHASE_C.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_C.md)
(**closed** 2026-07-15, Critic OK).
Phase D: [archive/tracks/TRACK_GUI_SCENE_PHASE_D.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_D.md)
(**closed** 2026-07-16, Critic OK).
§33b LIVE_POLISH: [archive/tracks/TRACK_EDITOR_LIVE_POLISH.md](../archive/tracks/TRACK_EDITOR_LIVE_POLISH.md)
(**closed** 2026-07-15, Critic OK).

## Next step

**closed** — epic §10c/§29 foundation delivered (A–D). §34 Path-match + §35 form_live
archived Critic OK. Queue → Planner.

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

## Phase A: фундамент (retained tree + affine transform + hit-test + batched draw)

Active work + STEP status:
[archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md)
(**closed** 2026-07-13, Critic OK).

| Step | Item | Status |
|------|------|--------|
| 1 | Design decision: tree indices, Affine2×3, camera-relative f32 | **done** |
| 2 | `misc/gui/scene.mlc`: `SceneNode` + flat `Scene` | **done** |
| 3 | World-transform pass | **done** |
| 4 | Hit-testing through tree | **done** |
| 5 | Batched draw (rect-fill) | **done** |
| 6 | Migration smoke vs `gui_button_demo` | **done** |
| 7 | Verify | **done** |

## Phase B: виджеты — **closed** (2026-07-14)

Label / checkbox / slider / text-field as `SceneNode` kinds.
Track: [archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md)
(**closed** Critic OK; `c28c620b`…`31d9299a`).
Text-input reuses keyboard inject from `TRACK_GUI_INPUT_ROBUSTNESS` (no dup).

## Phase C: dirty-tracking + spatial index — **closed** (2026-07-15)

Пересчёт world-transform только для dirty-узлов; AABB + optional quadtree
(порог 64). Track: [archive/tracks/TRACK_GUI_SCENE_PHASE_C.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_C.md)
(**closed** Critic OK; `d0a3996f`…`64014467`).

## Phase D: camera + vector path + blueprint canvas — **closed** (2026-07-16)

Camera pan/zoom = `Scene.camera`. Vector-path bezier stroke; node/wire MVP.
Track: [archive/tracks/TRACK_GUI_SCENE_PHASE_D.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_D.md)
(**closed** Critic OK; `d9cc60de`…`9e3eaebf`).

## Out of scope

- Полноценный дизайн-инструмент (Figma-класса продукт) — только
  фундамент + минимальный MVP демо в Phase D, не продукт.
- v0 (`misc/gui/`) не удаляется до explicit Phase A Step 6 миграционного
  smoke — работает параллельно.
