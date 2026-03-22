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

const GstMetaInfo *gst_my_meta_get_info(void) {
    static const GstMetaInfo *meta_info = NULL;

    if (g_once_init_enter((GstMetaInfo **)&meta_info)) {
        const GstMetaInfo *mi = gst_meta_register(
            GST_MY_META_API_TYPE,
            "GstMyMeta",
            sizeof(GstMyMeta),
            gst_my_meta_init,
            NULL,
            NULL
        );
        g_once_init_leave((GstMetaInfo **)&meta_info, (GstMetaInfo *)mi);
    }

    return meta_info;
}
