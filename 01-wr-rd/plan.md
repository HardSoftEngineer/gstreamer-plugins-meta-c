# План эксперимента: исследование поведения GstMeta (writer/reader + transform)

Цель: применить предложенные улучшения к проекту `01-wr-rd/`, не трогая основную ветку. Сравнить "как было" vs "как стало" по объективным метрикам.

## Принципы
- Работать в отдельной git-ветке `experiment/meta-tests-improved` (сделано)
- Docker оформлен как **отдельная сущность** в `docker/`. Поднимаем **долгоживущий** контейнер один раз. Образ ребилдим **только** при коордиальных изменениях зависимостей (правка `docker/Dockerfile`).
- Исходники монтируются в контейнер томом/ bind-mount, компиляция/тесты идут **внутри** контейнера. С хоста тоже можно смотреть результаты.
- Каждый шаг правок лежит в своей директории `experiment/steps/step-XX-<alias>/` с копией рабочего дерева + `dev/` (скрипты, логи, сырые выводы, заметки). 
- Сложные прогоны делаются скриптами (чтобы можно было воспроизвести руками).
- Много прогонов для усреднения (ориентир 10), считаем `min/avg/p50/p90/max`, долю буферов с метой, наличие/количество вызовов `gst_my_meta_transform`.

## Структура после старта
```
.
├── docker/                # отдельная сущность
│   ├── Dockerfile
│   ├── docker-compose.yml (или только shell-скрипты)
│   ├── .env
│   ├── up.sh
│   ├── exec.sh
│   ├── shell.sh
│   ├── rebuild-if-needed.sh
│   └── README.md
├── experiment/
│   ├── baseline/          # результаты первого прохода по базовой версии
│   ├── steps/             # шаги с улучшениями
│   └── results/           # сводка сравнения (tables + findings)
├── plan.md
└── 01-wr-rd/              # исходный проект (не переписываем в корне)
```

## Шаги

### Шаг 0. Подготовка Docker-среды
- [ ] Создать `docker/Dockerfile` на базе Debian/Ubuntu с `gstreamer1.0-tools`, `libgstreamer1.0-dev`, `libgstreamer-plugins-base1.0-dev`, `gstreamer1.0-plugins-{base,good,bad,ugly}`, `pkg-config`, `cmake`, `build-essential`, `git`, `python3`, `jq`, `coreutils`.
- [ ] Оформить управление контейнером: создать контейнер `gst-meta-test` при первом `up.sh`, просто `exec` при последующих. Не `rm && build` по умолчанию.
- [ ] Смонтировать корень репозитория в `/work` (bind-mount). Рабочая директория `/work`.
- [ ] Проверить `gst-launch-1.0 --version`, `gst-inspect-1.0 fakesink` внутри контейнера.

### Шаг 1. Базовый замер (baseline) по оригинальному коду
- [ ] Скопировать `01-wr-rd/` в `experiment/baseline/src/`
- [ ] Внутри контейнера собрать (`build.sh` или CMake)
- [ ] Написать скрипт `experiment/baseline/dev/run_matrix.sh`: детерминированные прогоны `videotestsrc num-buffers=300` для связок из `run.sh` (минимум: `mymetawriter!mymetareader`, с `queue`, с `videoconvert`, `videoscale`, `identity drop-allocation=true`, `videorate`, `jpegenc!jpegdec`). 
- [ ] Для каждой связки прогнать **10 раз**, собирать stdout/stderr. 
- [ ] Парсер `experiment/baseline/dev/parse_logs.py` (или bash+awk): посчитать `total_frames_seen`, `frames_with_meta`, `meta_present_pct`, `transform_calls` (по `gst_my_meta_transform`), собрать `meta_age_mono_us` -> `min/avg/p50/p90/max`. 
- [ ] Сохранить всё в `experiment/baseline/dev/` (raw, parsed.json/csv, summary.md)
- [ ] Зафиксировать выводы

### Шаг 2. step-01-atomic-counters-summary
Модификация: добавить атомарные счётчики в `gstmywriter.c`/`gstmyreader.c`/`my_meta/gstmy_meta.c` (или только счётчики). При завершении пайплайна (или по таймауту/сигналу) печатать сводку `SUMMARY: ...` в машиночитаемом виде.
- [ ] Копия в `experiment/steps/step-01-atomic-counters-summary/src/`
- [ ] Правки + `dev/CHANGELOG.md`, `dev/patch.diff`, `dev/NOTES.md`
- [ ] Сборка+тест теми же скриптами (адаптировать парсер под `SUMMARY:`)
- [ ] Результаты в `dev/results/`

### Шаг 3. step-02-pad-probes-per-element
Цель: замерять **sojourn time** именно для тестируемого элемента (pad-probes до/после). Добавить временный тестовый элемент-обёртку либо просто править скрипт `gst-launch` с `--gst-debug` + probes через питон/Gst? Проще: сгенерировать пайплайн через маленький C-утилитный биндинг/либо использовать `gst-launch` + `GST_DEBUG_DUMP_DOT_DIR` не нужно. Альтернатива - добавить debug в скрипте и считать разницу по `meta->timestamp` на src/sink pads целевого элемента.
- [ ] Отдельный скрипт `dev/run_probes.sh` + парсер
- [ ] Сравнить с end-to-end из baseline
- [ ] Зафиксировать отличия

### Шаг 4. step-03-deterministic-matrix+capsfilter
Усилить детерминизм: явно `capsfilter caps=video/x-raw,format=I420,width=320,height=240,framerate=30/1`, `num-buffers`. Исключить переговоры капсов там где возможно.
- [ ] Прогнать матрицу
- [ ] Посмотреть влияет ли на `videorate/identity`

### Шаг 5. step-04-meta-integrity
Проверка целостности при копировании: сравнивать `frame_id/pts/version` в райтере-ридере. Логировать несоответствия. Поможет отличить "скопировано ок" от "что-то пошло не так".
- [ ] Добавить проверки
- [ ] Собрать статистику

### Шаг 6. step-05-json-summary-unified
Сделать единый машиночитаемый вывод всех метрик в JSON (одна строка/файл) для автоматического сравнения.
- [ ] Единый парсер для всех шагов
- [ ] Сгенерировать `experiment/results/comparison.csv` и `experiment/results/comparison.md` с таблицей + +/- + нюансы

## Критерии успеха
- Docker поднят как долгоживущая среда, ребилд только при правке Dockerfile
- Baseline прогнан >=10 раз усреднён
- >=4-5 осмысленных шагов с `dev/` (скрипты+логи+заметки)
- Сравнительная таблица + описание нюансов
- Всё воспроизводимо из скриптов

## Риски
- `videorate` поведение плавающее -> усреднять по большему числу или прогонять пачкой
- `gst_meta_transform` может не печататься если `GST_DEBUG` глушит -> включать нужные категории
- pad-probes в `gst-launch` через чистый скрипт ограничены -> если сложно - компенсируем счётчиками+анализом логов
