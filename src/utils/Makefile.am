lib_LTLIBRARIES = libpitangusutils.la

libpitangusutils_la_SOURCES = utils.c

libpitangusutils_la_LDFLAGS = -L/usr/local/lib -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs xmlsec1-openssl`\
       	`curl-config --libs` -version-info 0:0:0

 
libpitangusutils_la_CFLAGS = -I../include -I$(top_srcdir)/include \
    	`xml2-config --cflags` `pkg-config --cflags xmlsec1-openssl`\
       	`curl-config --cflags`
