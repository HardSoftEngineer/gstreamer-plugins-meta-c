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
