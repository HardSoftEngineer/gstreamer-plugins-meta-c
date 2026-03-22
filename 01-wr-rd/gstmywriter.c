#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

typedef struct _GstMyWriter {
    GstBaseTransform parent;
    guint64 counter;
} GstMyWriter;

typedef struct _GstMyWriterClass {
    GstBaseTransformClass parent_class;
} GstMyWriterClass;

G_DEFINE_TYPE(GstMyWriter, gst_my_writer, GST_TYPE_BASE_TRANSFORM);

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

GType gst_my_meta_api_get_type(void) {
    static GType type = 0;

    static const gchar *tags[] = { "my_meta", NULL };

    if (g_once_init_enter(&type)) {
        GType _type = gst_meta_api_type_register("GstMyMetaAPI", tags);
        g_once_init_leave(&type, _type);
    }

    return type;
}

static gboolean gst_my_meta_init(GstMeta *meta, gpointer params, GstBuffer *buffer) {
    GstMyMeta *m = (GstMyMeta *) meta;

    m->frame_id = 0;
    m->timestamp = 0;
    m->version = 1;

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
            NULL
        );

        g_once_init_leave((GstMetaInfo **)&meta_info, (GstMetaInfo *)mi);
    }

    return meta_info;
}

// transform_ip — обработка буфера "на месте"
static GstFlowReturn gst_my_writer_transform_ip(GstBaseTransform *base, GstBuffer *buf) {
    GstMyWriter *self = (GstMyWriter *)base;

    self->counter++;

    // 
    GstMyMeta *meta = (GstMyMeta *) gst_buffer_add_meta(
        buf,
        GST_MY_META_INFO,
        NULL
    );

    if (meta) {
        meta->frame_id = self->counter;
        meta->timestamp = GST_BUFFER_PTS(buf);
        meta->version = 1;
    }

    g_print("Writer: frame %" G_GUINT64_FORMAT "\n", self->counter);

    return GST_FLOW_OK;
}


static void gst_my_writer_class_init(GstMyWriterClass *klass) {
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

    base_transform_class->transform_ip = gst_my_writer_transform_ip;

    // 
    GstCaps *caps = gst_caps_new_any();

    GstPadTemplate *sink_templ = gst_pad_template_new(
        "sink",
        GST_PAD_SINK,
        GST_PAD_ALWAYS,
        caps
    );

    GstPadTemplate *src_templ = gst_pad_template_new(
        "src",
        GST_PAD_SRC,
        GST_PAD_ALWAYS,
        caps
    );

    gst_element_class_add_pad_template(element_class, sink_templ);
    gst_element_class_add_pad_template(element_class, src_templ);

    gst_element_class_set_static_metadata(
        element_class,
        "My Writer",
        "Filter/Effect/Video",
        "Counts frames and prints",
        "You"
    );
}

static void gst_my_writer_init(GstMyWriter *self) {
    self->counter = 0;
}

// регистрация плагина
static gboolean plugin_init(GstPlugin *plugin) {
    return gst_element_register(
        plugin,
        "mymetawriter",
        GST_RANK_NONE,
        gst_my_writer_get_type()
    );
}

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mymetawriter,
    "My meta writer plugin",
    plugin_init,
    "1.0",
    "LGPL",
    "myplugin",
    "https://example.com"
)
