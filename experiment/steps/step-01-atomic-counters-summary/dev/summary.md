# Baseline summary (original code)

- Runs per pipeline: 10 (NUM_BUFFERS=300)
- GST_DEBUG: GST_META:5,mymetawriter:4,mymetareader:4

| pipeline | runs | meta_present_pct | frames_with_meta/reader_total | no_meta | transform_calls | ages_avg_us | ages_p50 | ages_p90 | ages_p95 | ages_min-max |
|---|---|---|---|---|---|---|---|---|---|---|
| base_writer_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 55.53 | 42 | 75 | 101 | 11-15702 |
| identity_dropalloc | 10 | 100.0 | 3000/3000 | 0 | 0 | 48.74 | 40 | 68 | 89 | 14-2667 |
| jpegenc_jpegdec | 10 | 0.0 | 0/3000 | 3000 | 0 | None | None | None | None | None-None |
| with_queues | 10 | 100.0 | 3000/3000 | 0 | 0 | 290.01 | 143 | 510 | 1157 | 21-10210 |
| writer_videoconvert_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 44.32 | 37 | 67 | 84 | 8-870 |
| writer_videorate_reader | 10 | 100.0 | 3000/3000 | 0 | 3000 | 1497.88 | 1299 | 2313 | 2710 | 69-14405 |
| writer_videoscale_videoconvert_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 52.58 | 44 | 79 | 104 | 12-1157 |

## Notes
- meta_age_mono_us = g_get_monotonic_time() - meta->timestamp (end-to-end from writer attach to reader)
- transform_calls counted by string "gst_my_meta_transform" in logs (GST_META debug)
- queues increase age significantly; identity drop-allocation often avoids transform; videorate behavior varies; jpegenc!jpegdec loses meta
