AM_CFLAGS = @GCCWARN@

AM_CPPFLAGS = -I../libproc -I../liblsd

noinst_LTLIBRARIES = liblcmt.la

liblcmt_la_SOURCES = \
	client.c \
	client.h \
	util.c \
	util.h \
	lcmt.h

if CEREBRO
liblcmt_la_SOURCES += \
	lcmtcerebro.c \
	lcmtcerebro.h
endif

liblcmt_la_LIBADD = $(CEREBRO_LIBS) $(LIBLUA)

