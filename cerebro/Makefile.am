AM_CFLAGS = @GCCWARN@

AM_CPPFLAGS = \
	-I$(top_builddir)/liblcmt \
	-I$(top_builddir)/libproc \
	-I$(top_builddir)/liblsd

common_libadd = \
        $(top_builddir)/liblcmt/liblcmt.la \
        $(top_builddir)/libproc/libproc.la \
        $(top_builddir)/liblsd/liblsd.la

module_ldflags = -module -avoid-version -Wl,--version-script=metric.map
modulelibdir = $(libdir)/cerebro

EXTRA_DIST = metric.map

modulelib_LTLIBRARIES = \
	cerebro_metric_lmt_client.la

cerebro_metric_lmt_client_la_SOURCES = client.c
cerebro_metric_lmt_client_la_LDFLAGS = $(module_ldflags)
cerebro_metric_lmt_client_la_LIBADD = $(common_libadd)
