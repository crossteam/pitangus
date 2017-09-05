/*
 * Pitangus - Biblioteca multiplataforma para geração e envio de documentos SPED da SEFAZ.
 *
 * Copyright (c) 2017 Pablo G. Gallardo, Silvio Clecio, Waldir Paim
 *
 * Pitangus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pitangus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Pitangus.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PITANGUS_H
#define PITANGUS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PTG_EXTERN
# ifdef PTG_SHARED_LIBRARY
#  ifdef _WIN32
#   define PTG_EXTERN __declspec(dllexport)
#  else
#   define PTG_EXTERN extern
#  endif
# else
#  ifdef PTG_STATIC_LIBRARY
#   define PTG_EXTERN
#  else
#   define PTG_EXTERN extern
#  endif
# endif
#endif

#ifdef __GNUC__

#ifndef __format
# define __format(args...) __attribute__((__format__(__printf__, args)))
#endif

#ifndef __nonnull
# define __nonnull(params)
#endif

#ifndef __null_term
# ifdef _WIN32
#  define __null_term
# else
#  define __null_term __attribute__((__sentinel__))
# endif
#endif

#ifndef __malloc
# ifdef _WIN32
#  define __malloc
# else
#  define __malloc __attribute_malloc__
# endif
#endif

#endif

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#endif

#ifdef __cplusplus
}
#endif

#endif