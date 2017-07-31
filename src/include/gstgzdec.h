// Hi Emacs, this is -*- coding: utf-8; mode: c; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef __GST_GZDEC_H__
#define __GST_GZDEC_H__

#include <gst/gst.h>
#include <stdint.h>
#include <zlib.h>


G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_GZDEC \
  (gst_gz_dec_get_type())
#define GST_GZDEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GZDEC,GstGzDec))
#define GST_GZDEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GZDEC,GstGzDecClass))
#define GST_IS_GZDEC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GZDEC))
#define GST_IS_GZDEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GZDEC))

typedef struct _GstGzDec      GstGzDec;
typedef struct _GstGzDecClass GstGzDecClass;

struct _GstGzDec
{
  GstElement element;

  GstPad* sinkpad;
  GstPad* srcpad;
  gboolean silent;
  z_stream strm;
};

struct _GstGzDecClass 
{
  GstElementClass parent_class;
};

GType gst_gz_dec_get_type (void);

G_END_DECLS

#endif /* __GST_GZDEC_H__ */
