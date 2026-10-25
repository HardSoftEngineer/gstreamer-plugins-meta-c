# step-01-atomic-counters-summary

Изменения:
- my_meta: добавлен атомарный счётчик transform_calls, функция gst_my_meta_get_transform_count(), g_print("gst_my_meta_transform\n")
- gstmywriter: счётчик writer_frames, SUMMARY в finalize
- gstmyreader: счётчики with_meta/no_meta, SUMMARY в finalize

Цель: машинно-читаемая сводка, надёжнее подсчёта по grep.
