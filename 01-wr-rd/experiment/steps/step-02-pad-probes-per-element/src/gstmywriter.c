#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

#include "gstmy_meta.h"

static gint _writer_frames = 0;

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
    g_atomic_int_inc(&_writer_frames);

    // 
    GstMyMeta *meta = (GstMyMeta *) gst_buffer_add_meta(
        buf,
        GST_MY_META_INFO,
        NULL
    );

    guint64 pts = GST_BUFFER_PTS(buf);         // PTS из буфера
    guint64 now_us = g_get_monotonic_time();   // текущее системное время в микросекундах

    if (meta) {
        meta->frame_id = self->counter;
        meta->pts = pts;
        meta->timestamp = now_us;
        meta->version = 1;
    }

    g_print("Writer: frame=%" G_GUINT64_FORMAT 
            " meta_pts=%" G_GUINT64_FORMAT 
            " sys_time=%" G_GUINT64_FORMAT
            " delta_us=%" G_GUINT64_FORMAT "\n",
            self->counter,
            pts,
            now_us,
            now_us > pts ? now_us - pts : 0);

    return GST_FLOW_OK;
}

static void gst_my_writer_finalize(GObject *object) {
    gint tf = 0;
    gst_my_meta_get_transform_count(&tf);
    g_print("SUMMARY: writer_frames=%d transform_calls=%d\n", g_atomic_int_get(&_writer_frames), tf);
    G_OBJECT_CLASS(gst_my_writer_parent_class)->finalize(object);
}


static void gst_my_writer_class_init(GstMyWriterClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

    gobject_class->finalize = gst_my_writer_finalize;

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
