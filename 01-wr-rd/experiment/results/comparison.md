# Сравнительная таблица (baseline vs шаги)

Условия: 10 прогонов по 300 буферов, videotsrc -> ... -> fakesink. Docker, bind-mount.

| pipeline | baseline meta% | s01 meta%/trans/avg/p50 | s03(det) meta%/trans/avg/p50 | s04 meta%/trans/avg/p50 | s05 meta%/trans/avg/p50 |
|---|---|---|---|---|---|
| base_writer_reader | 100.0 | 100.0/0/55.53/42 | 0/0/None/None | 100.0/0/54.9/43 | 0/0/None/None |
| det_identity_dropalloc | 0 | 0/0/None/None | 100.0/0/43.6/34 | 0/0/None/None | 0/0/None/None |
| det_ref | 0 | 0/0/None/None | 100.0/0/42.1/34 | 0/0/None/None | 0/0/None/None |
| det_videoconvert | 0 | 0/0/None/None | 100.0/0/35.93/26 | 0/0/None/None | 0/0/None/None |
| det_videorate | 0 | 0/0/None/None | 100.0/3000/543.43/482 | 0/0/None/None | 0/0/None/None |
| det_videoscale_videoconvert | 0 | 0/0/None/None | 100.0/0/37.13/29 | 0/0/None/None | 0/0/None/None |
| identity_dropalloc | 100.0 | 100.0/0/48.74/40 | 0/0/None/None | 100.0/0/60.38/47 | 0/0/None/None |
| jpegenc_jpegdec | 0.0 | 0.0/0/None/None | 0/0/None/None | 0.0/0/None/None | 0/0/None/None |
| with_queues | 100.0 | 100.0/0/290.01/143 | 0/0/None/None | 100.0/0/290.92/160 | 0/0/None/None |
| writer_videoconvert_reader | 100.0 | 100.0/0/44.32/37 | 0/0/None/None | 100.0/0/49.84/41 | 0/0/None/None |
| writer_videorate_reader | 100.0 | 100.0/3000/1497.88/1299 | 0/0/None/None | 100.0/3000/1657.65/1438 | 0/0/None/None |
| writer_videoscale_videoconvert_reader | 100.0 | 100.0/0/52.58/44 | 0/0/None/None | 100.0/0/51.25/41 | 0/0/None/None |

## Нюансы (+/-)

### +
- Наличие меты - объективная метрика (meta_present_pct). Качественно переносима.
- Счётчики через SUMMARY (s01) - машиночитаемы.
- Жёсткие capsfilter (s03) - снижают вариативность.
- Проверка целостности (s04).
- Детерминизм: num-buffers + >=10 прогонов.

### -
- delta meta-sys end-to-end сильно зависит от планировщика/очередей, плохо переносим в абсолютных числах между машинами.
- videorate поведение плавающее.
- identity drop-allocation: трансформ может не вызываться - корректно (нет копирования).
- jpegenc!jpegdec - мета теряется при перекодировании, трансформ не спасает.
- pad-probes в чистом gst-launch ограничены.

## Рекомендации
1. Primary метрика - % буферов с метой
2. Sojourn time через pad-probes до/после элемента
3. Фиксированные caps, num-buffers, >=10 прогонов
4. Считать min/avg/p50/p90/p95/max, опираться на медиану/p90
5. GST_DEBUG GST_META:5
6. Разделять: нет копирования/есть копирование/перекодирование
