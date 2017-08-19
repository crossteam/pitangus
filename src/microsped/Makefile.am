AM_CPPFLAGS = -I$(top_srcdir)/include
lib_LTLIBRARIES = libmicrosped.la
libmicrosped_la_SOURCES = \
    microsped.c \
    msped_http.c \
    msped_http.h \
    msped_macros.h \
    msped_smtp.c \
    msped_smtp.h \
    msped_strs.h \
    msped_utils.c \
    msped_ws_defs.c \
    msped_ws_defs.h \
    msped_xpath.c \
    msped_xpath.h
libmicrosped_la_CFLAGS = $(CFLAGS)
libmicrosped_la_LDFLAGS = \
  $(LDFLAGS) \
  -version-info @LIB_VERSION_CURRENT@:@LIB_VERSION_REVISION@:@LIB_VERSION_AGE@