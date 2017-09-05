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

#ifndef __returns_nonnull
#  define __returns_nonnull __attribute__((returns_nonnull))
#endif

#ifndef __variadic
# ifdef _WIN32
#  define __variadic
# else
#  define __variadic __attribute__((__sentinel__))
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

#include <stdint.h>

#endif

/* utilidades */

/**
 * \brief Representa um tamanho inteiro em uma unidade de medida.
 * \param tam Tamanho em bytes.
 * \return Unidade de medida.
 */
PTG_EXTERN const char *ptg_tamanho(uint64_t tam);

/**
 * \brief Retorna data e hora atual no formato RFC 2822.
 * \return Data e hora atual no formato RFC 2822.
 * \warning O chamador é responsável em liberar a string alocada.
 */
PTG_EXTERN char *ptg_agora_rfc2822() __malloc __returns_nonnull;

#ifdef __cplusplus
}
#endif

#endif