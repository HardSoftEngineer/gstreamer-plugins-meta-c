#ifndef GST_MY_META_H
#define GST_MY_META_H

#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include <gst/gstmeta.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstMyMeta {
    GstMeta meta;         // Base GstMeta structure, required for all custom metadata

    guint64 frame_id;     // Sequential frame number assigned by the writer

    guint64 pts;          // Presentation Timestamp (PTS) from GStreamer pipeline
                          // Indicates when this buffer/frame should be presented
                          // Measured in nanoseconds relative to stream start

    guint64 timestamp;    // System timestamp at moment of metadata creation
                          // Measured in microseconds from monotonic clock (g_get_monotonic_time)
                          // Useful for measuring pipeline latency

    guint32 version;      // Version of this metadata structure, can be used for backward compatibility
} GstMyMeta;

GType gst_my_meta_api_get_type(void);
#define GST_MY_META_API_TYPE (gst_my_meta_api_get_type())

const GstMetaInfo *gst_my_meta_get_info(void);
#define GST_MY_META_INFO (gst_my_meta_get_info())

G_END_DECLS

#endif
