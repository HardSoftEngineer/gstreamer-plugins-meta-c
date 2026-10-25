#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

#include "gstmy_meta.h"

static gint _reader_with_meta = 0;
static gint _reader_no_meta_count = 0;

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
        g_atomic_int_inc(&_reader_no_meta_count);
        g_print("Reader: no meta\n");
        return GST_FLOW_OK;
    }

    g_atomic_int_inc(&_reader_with_meta);

    if (meta->version != 1) {
        g_print("Reader: unsupported meta version %u\n", meta->version);
        return GST_FLOW_OK;
    }

    guint64 meta_pts = meta->pts;              // PTS из буфера
    guint64 meta_timestamp = meta->timestamp;  // timestamp из буфера 
    guint64 now_us = g_get_monotonic_time();   // текущее системное время в микросекундах

    g_print("Reader: frame=%" G_GUINT64_FORMAT 
            " meta_pts=%" G_GUINT64_FORMAT 
            " meta_time=%" G_GUINT64_FORMAT 
            " sys_time=%" G_GUINT64_FORMAT
            " delta meta-sys=%" G_GUINT64_FORMAT "\n",
            meta->frame_id,
            meta_pts,
            meta_timestamp,
            now_us,
            now_us > meta_timestamp ? now_us - meta_timestamp : 0);

    return GST_FLOW_OK;
}

// ===== CLASS INIT =====

static void gst_my_reader_finalize(GObject *object) {
    gint tf = 0;
    gst_my_meta_get_transform_count(&tf);
    g_print("SUMMARY: reader_with_meta=%d reader_no_meta=%d transform_calls=%d\n", g_atomic_int_get(&_reader_with_meta), g_atomic_int_get(&_reader_no_meta_count), tf);
    G_OBJECT_CLASS(gst_my_reader_parent_class)->finalize(object);
}

static void gst_my_reader_class_init(GstMyReaderClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

    gobject_class->finalize = gst_my_reader_finalize;

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
