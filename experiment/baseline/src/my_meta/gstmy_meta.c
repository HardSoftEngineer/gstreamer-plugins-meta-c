#include "gstmy_meta.h"

static gboolean gst_my_meta_init(GstMeta *meta, gpointer params, GstBuffer *buffer) {
    GstMyMeta *m = (GstMyMeta *) meta;
    m->frame_id = 0;
    m->pts = 0;
    m->timestamp = 0;
    m->version = 1;
    return TRUE;
}

GType gst_my_meta_api_get_type(void) {
    static GType type = 0;

    if (g_once_init_enter(&type)) {
        GType existing = g_type_from_name("GstMyMetaAPI");
        if (existing != 0) {
            g_once_init_leave(&type, existing);
            return existing;
        }

        static const gchar *tags[] = { "my_meta", NULL };
        GType _type = gst_meta_api_type_register("GstMyMetaAPI", tags);
        g_once_init_leave(&type, _type);
    }

    return type;
}

static gboolean gst_my_meta_transform(
    GstBuffer *dest_buf,
    GstMeta *meta,
    GstBuffer *src_buf,
    GQuark type,
    gpointer data)
{
    GstMyMeta *src_meta = (GstMyMeta *) meta;

    g_print("gst_my_meta_transform");

    // new meta в dest buffer
    GstMyMeta *dst_meta = (GstMyMeta *) gst_buffer_add_meta(
        dest_buf,
        GST_MY_META_INFO,
        NULL
    );

    if (!dst_meta)
        return FALSE;

    // copy
    dst_meta->frame_id = src_meta->frame_id;
    dst_meta->pts = src_meta->pts;
    dst_meta->timestamp = src_meta->timestamp;
    dst_meta->version = src_meta->version;

    return TRUE;
}

const GstMetaInfo *gst_my_meta_get_info(void) {
    static const GstMetaInfo *meta_info = NULL;

    if (g_once_init_enter((GstMetaInfo **)&meta_info)) {
        const GstMetaInfo *mi = gst_meta_register(
            GST_MY_META_API_TYPE,
            "GstMyMeta",
            sizeof(GstMyMeta),
            gst_my_meta_init,
            NULL,
            gst_my_meta_transform
        );
        g_once_init_leave((GstMetaInfo **)&meta_info, (GstMetaInfo *)mi);
    }

    return meta_info;
}
