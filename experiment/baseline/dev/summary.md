# Baseline summary (original code)

- Runs per pipeline: 10 (NUM_BUFFERS=300)
- GST_DEBUG: GST_META:5,mymetawriter:4,mymetareader:4

| pipeline | runs | meta_present_pct | frames_with_meta/reader_total | no_meta | transform_calls | ages_avg_us | ages_p50 | ages_p90 | ages_p95 | ages_min-max |
|---|---|---|---|---|---|---|---|---|---|---|
| base_writer_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 53.83 | 39 | 73 | 100 | 8-13137 |
| identity_dropalloc | 10 | 100.0 | 3000/3000 | 0 | 0 | 60.74 | 46 | 80 | 110 | 13-16109 |
| jpegenc_jpegdec | 10 | 0.0 | 0/3000 | 3000 | 0 | None | None | None | None | None-None |
| with_queues | 10 | 100.0 | 3000/3000 | 0 | 0 | 285.64 | 125 | 475 | 1195 | 16-20037 |
| writer_videoconvert_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 53.33 | 42 | 75 | 100 | 7-10013 |
| writer_videorate_reader | 10 | 100.0 | 3000/3000 | 0 | 3000 | 1690.62 | 1544 | 2477 | 2775 | 61-10916 |
| writer_videoscale_videoconvert_reader | 10 | 100.0 | 3000/3000 | 0 | 0 | 52.3 | 44 | 74 | 96 | 14-3146 |

## Notes
- meta_age_mono_us = g_get_monotonic_time() - meta->timestamp (end-to-end from writer attach to reader)
- transform_calls counted by string "gst_my_meta_transform" in logs (GST_META debug)
- queues increase age significantly; identity drop-allocation often avoids transform; videorate behavior varies; jpegenc!jpegdec loses meta
