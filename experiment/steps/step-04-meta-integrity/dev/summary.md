# Baseline summary (original code)

- Runs per pipeline: 10 (NUM_BUFFERS=300)
- GST_DEBUG: GST_META:5,mymetawriter:4,mymetareader:4

| pipeline | runs | meta_present_pct | frames_with_meta/reader_total | no_meta | transform_calls | ages_avg_us | ages_p50 | ages_p90 | ages_p95 | ages_min-max |
|---|---|---|---|---|---|---|---|---|---|---|
| base_writer_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 54.9 | 43 | 82 | 119 | 13-1466 |
| identity_dropalloc | 10 | 100.0 | 3000/3000 | 0 | 0 | 60.38 | 47 | 86 | 114 | 12-5202 |
| jpegenc_jpegdec | 10 | 0.0 | 0/3000 | 3000 | 0 | None | None | None | None | None-None |
| with_queues | 10 | 100.0 | 3000/3000 | 0 | 0 | 290.92 | 160 | 529 | 1059 | 22-15377 |
| writer_videoconvert_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 49.84 | 41 | 72 | 95 | 14-1253 |
| writer_videorate_reader | 10 | 100.0 | 3000/3000 | 0 | 3000 | 1657.65 | 1438 | 2547 | 3174 | 63-10873 |
| writer_videoscale_videoconvert_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 51.25 | 41 | 74 | 103 | 8-1295 |

## Notes
- meta_age_mono_us = g_get_monotonic_time() - meta->timestamp (end-to-end from writer attach to reader)
- transform_calls counted by string "gst_my_meta_transform" in logs (GST_META debug)
- queues increase age significantly; identity drop-allocation often avoids transform; videorate behavior varies; jpegenc!jpegdec loses meta
