lib_LTLIBRARIES = libpitangus.la

libpitangus_la_SOURCES = genxml.c sped.c sefaz.c send.c \
		    sign.c xml.c libsped.c utils.c

libpitangus_la_LDFLAGS = -L/usr/local/lib -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs xmlsec1-openssl`\
       	`curl-config --libs` -version-info 0:0:0

 
libpitangus_la_CFLAGS = -I../include -I$(top_srcdir)/include \
    	`xml2-config --cflags` `pkg-config --cflags xmlsec1-openssl`\
       	`curl-config --cflags`

SUBDIRS = icons

AM_CFLAGS = -I../include -I$(top_srcdir)/include `pkg-config --cflags gtk+-3.0`\
    	`xml2-config --cflags` `pkg-config --cflags xmlsec1-openssl`\
       	`curl-config --cflags`
AM_LDFLAGS = -L/usr/local/lib -lsqlite3 -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs gtk+-3.0` \
	`pkg-config --libs xmlsec1-openssl` `curl-config --libs`
bin_PROGRAMS = pitangus 
pitangus_SOURCES = tool_main.c tool_db.c tool_db_create.c tool_db_init.c \
		tool_pitangus.c tool_window.c resources.c  tool_db_nfe.c \
		tool_nfe_manager.c tool_item_manager.c tool_crypto_interface.c \
		tool_db_prefs.c tool_emitente_manager.c tool_sefaz_response.c \
		tool_prefs.c tool_prefs_dialog.c tool_gtk_common.c

pitangus_LDADD = libpitangus.la


resources.c: tool_pitangus.gresource.xml tool_window.ui tool_nfe_manager.ui \
		tool_item_manager.ui tool_emitente_manager.ui \
		tool_sefaz_response.ui icons/pitangus.png tool_prefs_dialog.ui
	glib-compile-resources --target=$@ --generate-source $<

clean-local:
	rm -f resources.c

