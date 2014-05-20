AM_CFLAGS = @GCCWARN@

AM_CPPFLAGS = -I../liblsd -I../libproc -I../liblcmt 

sbin_PROGRAMS = lcmtmetric lcmtstats
bin_SCRIPTS = lctop

common_ldadd = \
	$(top_builddir)/liblcmt/liblcmt.la \
	$(top_builddir)/libproc/libproc.la \
	$(top_builddir)/liblsd/liblsd.la

lcmtstats_SOURCES = lcmtstats.c
lcmtstats_LDADD = $(common_ldadd)

lcmtmetric_SOURCES = lcmtmetric.c
lcmtmetric_LDADD = $(common_ldadd) 

