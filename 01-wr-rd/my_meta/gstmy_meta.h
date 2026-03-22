#ifndef GST_MY_META_H
#define GST_MY_META_H

#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include <gst/gstmeta.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstMyMeta {
    GstMeta meta;
    guint64 frame_id;
    guint64 timestamp;
    guint32 version;
} GstMyMeta;

GType gst_my_meta_api_get_type(void);
#define GST_MY_META_API_TYPE (gst_my_meta_api_get_type())

const GstMetaInfo *gst_my_meta_get_info(void);
#define GST_MY_META_INFO (gst_my_meta_get_info())

G_END_DECLS

#endif
