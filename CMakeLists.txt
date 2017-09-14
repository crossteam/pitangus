cmake_minimum_required(VERSION 3.0)
project(pitangus)

set(CMAKE_CXX_STANDARD 11)


find_package(LibXml2 REQUIRED)
if (LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})
else ()
    message(FATAL_ERROR "LibXml2's libraries not found")
endif ()

find_package(LibXslt REQUIRED)
if (LIBXSLT_FOUND)
    include_directories(${LIBXSLT_INCLUDE_DIR})
    set(LIBS ${LIBS} ${LIBXSLT_LIBRARIES})
else ()
    message(FATAL_ERROR "LibXslt's libraries not found")
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${SSL_LIBRARIES})
else ()
    message(FATAL_ERROR "Não foi possível localizar biblioteca OpenSSL e seus arquivos de desenvolvimento")
endif ()

find_package(CURL REQUIRED)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${CURL_LIBRARIES})
else ()
    message(FATAL_ERROR "Não foi possível localizar biblioteca cURL e seus arquivos de desenvolvimento")
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
if (GTK3_FOUND)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories($(GTK3_LIBRARY_DIRS))
    add_definitions(${GTK3_CFLAGS_OTHER})
    set(LIBS ${LIBS} ${GTK3_LIBRARIES})
else ()
    message(FATAL_ERROR "Não foi possível localizar biblioteca GTK3 e seus arquivos de desenvolvimento")
endif ()

add_definitions(-I/usr/include/libxml2)
add_definitions(-I/usr/include/libxslt)
add_definitions(-I/usr/local/include/xmlsec1)
add_definitions(-I/usr/local/include/openssl)
add_definitions(-I/usr/local/include/gtk-3.0)
add_definitions(-DXMLSEC_OPENSSL_100=1)
add_definitions(-DXMLSEC_CRYPTO_OPENSSL=1)
add_definitions(-DXMLSEC_CRYPTO=openssl)

include_directories(.)
include_directories(include)

set(SOURCE_FILES
        include/pitangus/genxml.h
        include/pitangus/libsped.h
        include/pitangus/sped.h
        include/pitangus/sefaz.h
        include/pitangus/utils.h
        include/pitangus/errno.h
        include/pitangus/pitangus.h
        src/tool_crypto_interface.c
        src/tool_crypto_interface.h
        src/tool_db.c
        src/tool_db.h
        src/tool_db_create.c
        src/tool_db_init.c
        src/tool_db_interface.h
        src/tool_db_nfe.c
        src/tool_db_prefs.c
        src/tool_emitente_manager.c
        src/tool_emitente_manager.h
        src/tool_gtk_common.c
        src/tool_gtk_common.h
        src/tool_item_manager.c
        src/tool_item_manager.h
        src/tool_pitangus.c
        src/tool_pitangus.h
        src/tool_window.c
        src/tool_window.h
        src/tool_main.c
        src/tool_nfe_manager.c
        src/tool_nfe_manager.h
        src/tool_prefs.c
        src/tool_prefs.h
        src/tool_prefs_dialog.c
        src/tool_prefs_dialog.h
        src/resources.c
        src/tool_sefaz_response.c
        src/tool_sefaz_response.h
        src/genxml.c
        src/libsped.c
        src/sped.c
        src/sefaz.c
        src/send.c
        src/send.h
        src/sign.c
        src/sign.h
        src/xml.c
        src/xml.h
        src/utils.c)

add_executable(pitangus ${SOURCE_FILES})

target_link_libraries(pitangus ${GTK3_LIBRARIES} -lxmlsec1-openssl -lssl -lcrypto ${SSL_LIBRARIES} -lpthread ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} -lxmlsec1 -lp11 -lsqlite3)
