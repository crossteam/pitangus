#ifndef PTG_UTILS_H
#define PTG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#endif
#include <pitangus.h>

/* utilidades */

/**
 * \brief Retorna a unidade de medida de um tamanho inteiro.
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