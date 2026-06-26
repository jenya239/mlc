# BLOG.md — инструкция для роли Blogger

Блог: https://izkaregn.com/mlc-blog/
Репо: /home/jenya/workspaces/web/izkaregn-website-2025/mlc-blog/

## Стратегия

Новый пост создавать редко — раз в день максимум, и только при значимом событии (закрылась фаза).
В остальных случаях — **дополнять последний существующий пост** новыми примерами «было → стало»
из свежих коммитов.

## Алгоритм

```bash
# 1. Найти последний пост
ls -t /home/jenya/workspaces/web/izkaregn-website-2025/mlc-blog/posts/*.html | head -1

# 2. Найти коммиты, которых ещё нет в посте
cd /home/jenya/workspaces/current/mlc
git log --oneline | head -40

# 3. Для каждого интересного коммита посмотреть diff
git show <hash> | grep "^[-+]" | grep -v "^---\|^+++" | head -60

# 4. Выбрать 1-3 примера, которых ещё нет в посте, добавить новые <h2>-секции
```

## Что считается интересным примером

- Было много кода с проверками → стало удалено целиком (стало ненужным)
- Была конкатенация строк → стал структурный объект
- Было невнятное имя `res`/`result` → стало точное имя
- Был один большой блок → стали отдельные маленькие функции
- Добавился новый узел дерева вместо текстового фрагмента

Пропускать: переименования `export`, изменения `import`, правки документации.

## Как дополнить существующий пост

Читать HTML файл поста, найти закрывающий тег `</div>` перед `</article>`,
вставить перед ним новые секции:

```html
<h2>Заголовок нового примера</h2>
<p>Одно предложение — почему старый вариант хуже.</p>
<pre><code>- старый код
+ новый код</code></pre>
```

Сохранить через Python с `encoding='utf-8'`, задеплоить.

## Когда создавать новый пост

Только если:
- Завершилась целая фаза (Phase 2, Phase 2.5 и т.д.)
- Последний пост старше 2 недель и накопилось много нового

## Формат нового поста

Имя файла: `YYYY-MM-DD-slug.html`
Папка: `/home/jenya/workspaces/web/izkaregn-website-2025/mlc-blog/posts/`

```html
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Заголовок поста</title>
<meta name="description" content="Одна строка.">
<link rel="stylesheet" href="/mlc-blog/style.css">
</head>
<body>
<div class="container">
  <header class="site-header"><a href="/mlc-blog/">← MLC Dev Blog</a></header>
  <article>
    <div class="post-header">
      <div class="post-date">YYYY-MM-DD</div>
      <h1>Заголовок</h1>
    </div>
    <div class="post-body">
      <!-- p, h2, pre/code, ul/li -->
    </div>
  </article>
  <nav class="post-nav"><a href="/mlc-blog/">← все посты</a></nav>
</div>
</body>
</html>
```

## Требования к тексту

- Русский, прямой — меньше английских терминов
- Без литературных приёмов и риторических вопросов
- Аудитория: разработчики, не обязательно знакомые с компиляторами
- НЕ использовать слова «трек», «пайплайн», «бридж», «коммит» в тексте

## Деплой

```python
with open('path/to/post.html', 'w', encoding='utf-8') as f:
    f.write(html_content)
```

```bash
scp /home/jenya/workspaces/web/izkaregn-website-2025/mlc-blog/posts/FILENAME.html \
    root@209.209.8.150:/var/www/izkaregn/public/mlc-blog/posts/
curl -s -o /dev/null -w "%{http_code}" https://izkaregn.com/mlc-blog/posts/FILENAME.html
# должно быть 200
```

## После публикации

Enqueue Driver next pending step как обычно.

---

## Reddit draft — self-hosted MLC compiler (STEP=4, 2026-06-26)

Target: r/ProgrammingLanguages, r/rust. Media: [REDDIT_DEMO_MEDIA.md](REDDIT_DEMO_MEDIA.md). Baseline: [reddit_demo_baseline.txt](reddit_demo_baseline.txt).

### Title (pick one)

- MLC: a self-hosted compiler that lowers to C++20 (~23k lines of MLC)
- Self-hosting update: mlcc compiles itself in ~2s, deterministic codegen

### Problem

Most hobby compilers stall before self-hosting, or ship a transpiler without a real checker and test suite. I wanted a small language with algebraic types and pattern matching that compiles to readable C++20, with a compiler written in the same language.

### Solution

MLC is an experimental self-hosted compiler:

- Source in `compiler/` (~23k lines MLC) → C++20 via checker + codegen
- Bootstrap path: Ruby reference compiler builds the first `mlcc`; then `mlcc` drives day-to-day work
- Sum types, inference, `Result`/`?`, traits, `Shared<T>`, COW arrays/maps in the runtime
- Deterministic self-host: two codegen passes of `compiler/main.mlc` produce identical output (`diff_exit=0`)

### Demo (copy/paste)

```bash
git clone <repo-url> && cd mlc

# Build mlcc (Ruby bootstrap → C++)
compiler/build.sh

# Measurements + test summary
scripts/reddit_demo.sh --run

# Record baseline (codegen p1/p2 + diff)
scripts/reddit_demo.sh --record-baseline

# E2E: compile, link, run sample programs
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc
```

### Numbers (baseline 2026-06-26)

| Metric | Value |
|--------|-------|
| Self-host codegen pass 1 | 1.92 s |
| Self-host codegen pass 2 | 2.05 s |
| Codegen determinism (`diff -rq`) | exit 0 |
| Generated C++ modules | 136 |
| Compiler unit tests (`run_tests`) | 1362 passed, 0 failed |
| E2E programs | 6 passed, 0 failed |

### Code snippet (language)

```mlc
type Shape = Circle { radius: i32 } | Rect { width: i32, height: i32 }

fn area(shape: Shared<Shape>) -> i32 =
  match shape {
    Circle { radius } => radius * radius,
    Rect { width, height } => width * height
  }
```

Lowers to `std::variant` + `std::visit` (see root `README.md`).

### Honest limits (do not oversell)

- No LSP, package manager, or language spec site yet
- Minimal stdlib; playground not hosted
- Full `MLCC_BOOTSTRAP=1` g++ link of fresh emit is a known rough edge; day-to-day `mlcc` binary + codegen determinism check works

### Media placeholders

- Screenshot: after `scripts/reddit_demo.sh --run` (checklist in REDDIT_DEMO_MEDIA.md)
- Asciinema: `asciinema rec reddit_demo.cast` — URL: _TBD after manual upload_

### Links

- Repo: _add public URL_
- Demo script: `scripts/reddit_demo.sh`
- Plan / roadmap: `docs/PLAN.md` §Phase 5

