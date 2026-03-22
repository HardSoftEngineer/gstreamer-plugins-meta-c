#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

#include "gstmy_meta.h"

// ===== READER =====

typedef struct _GstMyReader {
    GstBaseTransform parent;
} GstMyReader;

typedef struct _GstMyReaderClass {
    GstBaseTransformClass parent_class;
} GstMyReaderClass;

G_DEFINE_TYPE(GstMyReader, gst_my_reader, GST_TYPE_BASE_TRANSFORM);

// ===== LOGIC =====

static GstFlowReturn gst_my_reader_transform_ip(GstBaseTransform *base, GstBuffer *buf) {

    GstMyMeta *meta = (GstMyMeta *) gst_buffer_get_meta(
        buf,
        GST_MY_META_API_TYPE
    );

    if (!meta) {
        g_print("Reader: no meta\n");
        return GST_FLOW_OK;
    }

    if (meta->version != 1) {
        g_print("Reader: unsupported meta version %u\n", meta->version);
        return GST_FLOW_OK;
    }

    g_print("Reader: frame=%" G_GUINT64_FORMAT
            " ts=%" G_GUINT64_FORMAT "\n",
            meta->frame_id,
            meta->timestamp);

    return GST_FLOW_OK;
}

// ===== CLASS INIT =====

static void gst_my_reader_class_init(GstMyReaderClass *klass) {
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

    base_transform_class->transform_ip = gst_my_reader_transform_ip;

    GstCaps *caps = gst_caps_new_any();

    GstPadTemplate *sink_templ = gst_pad_template_new(
        "sink", GST_PAD_SINK, GST_PAD_ALWAYS, caps);

    GstPadTemplate *src_templ = gst_pad_template_new(
        "src", GST_PAD_SRC, GST_PAD_ALWAYS, caps);

    gst_element_class_add_pad_template(element_class, sink_templ);
    gst_element_class_add_pad_template(element_class, src_templ);

    gst_element_class_set_static_metadata(
        element_class,
        "My Meta Reader",
        "Filter/Effect/Video",
        "Reads custom GstMeta",
        "You"
    );
}

static void gst_my_reader_init(GstMyReader *self) {}

// ===== PLUGIN INIT =====

static gboolean plugin_init(GstPlugin *plugin) {
    return gst_element_register(
        plugin,
        "mymetareader",
        GST_RANK_NONE,
        gst_my_reader_get_type()
    );
}

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mymetareader,
    "My meta reader plugin",
    plugin_init,
    "1.0",
    "LGPL",
    "myplugin",
    "https://example.com"
)
