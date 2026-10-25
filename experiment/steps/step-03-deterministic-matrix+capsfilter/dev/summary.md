# Baseline summary (original code)

- Runs per pipeline: 10 (NUM_BUFFERS=300)
- GST_DEBUG: GST_META:5,mymetawriter:4,mymetareader:4

| pipeline | runs | meta_present_pct | frames_with_meta/reader_total | no_meta | transform_calls | ages_avg_us | ages_p50 | ages_p90 | ages_p95 | ages_min-max |
|---|---|---|---|---|---|---|---|---|---|---|
| det_identity_dropalloc | 10 | 100.0 | 3000/3000 | 0 | 0 | 43.6 | 34 | 68 | 93 | 7-1076 |
| det_ref | 10 | 100.0 | 3000/3000 | 0 | 0 | 42.1 | 34 | 61 | 77 | 6-1707 |
| det_videoconvert | 10 | 100.0 | 3000/3000 | 0 | 0 | 35.93 | 26 | 53 | 79 | 7-8315 |
| det_videorate | 10 | 100.0 | 3000/3000 | 0 | 3000 | 543.43 | 482 | 775 | 923 | 60-6869 |
| det_videoscale_videoconvert | 10 | 100.0 | 3000/3000 | 0 | 0 | 37.13 | 29 | 59 | 80 | 6-1409 |

## Notes
- meta_age_mono_us = g_get_monotonic_time() - meta->timestamp (end-to-end from writer attach to reader)
- transform_calls counted by string "gst_my_meta_transform" in logs (GST_META debug)
- queues increase age significantly; identity drop-allocation often avoids transform; videorate behavior varies; jpegenc!jpegdec loses meta
