lib_LTLIBRARIES = libpitangussped.la

libpitangussped_la_SOURCES = gen_xml.c nfe.c sefaz.c send.c \
		    sign.c xml.c libnfe.c

libpitangussped_la_LDFLAGS = -L/usr/local/lib -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs xmlsec1-openssl`\
       	`curl-config --libs` -version-info 0:0:0

 
libpitangussped_la_CFLAGS = -I../include -I$(top_srcdir)/include \
    	`xml2-config --cflags` `pkg-config --cflags xmlsec1-openssl`\
       	`curl-config --cflags`
