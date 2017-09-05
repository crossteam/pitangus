#
# Pitangus - Biblioteca multiplataforma para geração e emissão de XMLs SPED.
#
# Copyright (c) 2017 Pablo G. Gallardo, Silvio Clecio, Waldir Paim
#
# This file is part of Pitangus.
#
# Pitangus is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Pitangus is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Pitangus.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.6)
project(pitangus)

# versao da standard

set(CMAKE_C_STANDARD 99)

# tipo de build

if (WIN32)
    set(CMAKE_C_FLAGS "-static -Wno-nonnull-compare")
else ()
    set(CMAKE_C_FLAGS "")
endif ()

if (MINGW)
    add_definitions(-DMINGW=1)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG 1)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
    # ativar asan/lsan apenas em debugs
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fsanitize=address")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# opcoes

#TODO: definir!

# modulos cmake

#TODO: definir!

# set de variaveis

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/exemplos)
set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/testes)

# set de codigos da lib

set(LIB_FILES
        ${INC_DIR}/pitangus.h
        ${SRC_DIR}/ptg_utils.c)

# set de codigos dos exemplos

set(EXP_FILES
        ${EXP_DIR}/utilidades.c)

# set de codigos dos testes

set(TST_FILES
        ${TST_DIR}/teste_ptg_utils.c)

# include de diretorios

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INC_DIR})
include_directories(${SRC_DIR})

# biliotecas externas

set(LIBS "")
if (UNIX)
    set(LIBS ${LIBS} m)
endif (UNIX)

# biblioteca (shared)

add_library(pitangus SHARED ${LIB_FILES})
target_compile_definitions(pitangus PUBLIC -DPTG_SHARED_LIBRARY=1)
target_link_libraries(pitangus ${LIBS})

# biblioteca (static)

add_library(pitangus_static STATIC ${LIB_FILES})
target_compile_definitions(pitangus_static PUBLIC -DPTG_STATIC_LIBRARY=1)
set_target_properties(pitangus_static PROPERTIES OUTPUT_NAME pitangus)

# biliotecas da lib

set(LIBS ${LIBS} pitangus)

# exemplos

add_executable(utilidades ${EXP_FILES})
target_link_libraries(utilidades ${LIBS})

# testes

add_executable(teste_ptg_utils ${TST_FILES})
target_link_libraries(teste_ptg_utils ${LIBS})

enable_testing()
add_test(NAME "TestePitangusutils"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testes
        COMMAND teste_ptg_utils
        COMMENT "Testando `teste_ptg_utils`..." VERBATIM)

# documentacao

#TODO: definir!
#find_package(Doxygen)
#if (DOXYGEN_FOUND)
#    add_custom_target(doc ALL
#            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen
#            COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
#            COMMENT "Gerando documentacao da API com Doxygen ..." VERBATIM)
#endif (DOXYGEN_FOUND)