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
