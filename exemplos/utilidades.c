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

/*
 * Exemplo de uso:
 *
 * ./utilidades
 */

#include <stdio.h>
#include <stdlib.h>
#include <pitangus.h>

int main() {
    /* unidade de medida para tamanho. */
#define TAM 1000000
    printf("Formatação de tamanho: %d %s\n", TAM, ptg_tamanho(TAM));
#undef TAM
    char *agora_rfc2822;
    /* data/hora atual (formato RFC 2822). */
    if ((agora_rfc2822 = ptg_agora_rfc2822())) {
        printf("Data/hora atual no formato RFC 2822: %s\n", agora_rfc2822);
        free(agora_rfc2822);
    }
    return EXIT_SUCCESS;
}