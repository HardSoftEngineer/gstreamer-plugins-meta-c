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

// transform_ip — обработка буфера "на месте"
static GstFlowReturn gst_my_writer_transform_ip(GstBaseTransform *base, GstBuffer *buf) {
    GstMyWriter *self = (GstMyWriter *)base;

    self->counter++;

    g_print("Writer: frame %lu\n", self->counter);

    return GST_FLOW_OK;
}

static void gst_my_writer_class_init(GstMyWriterClass *klass) {
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

    base_transform_class->transform_ip = gst_my_writer_transform_ip;

    gst_element_class_set_static_metadata(
        GST_ELEMENT_CLASS(klass),
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
