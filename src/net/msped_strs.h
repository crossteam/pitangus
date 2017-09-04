/*
 * libmicrosped - Micro biblioteca multiplataforma para comunicação com web services
 * de SPED da SEFAZ.
 *
 * Copyright (c) 2017 Silvio Clecio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MSPED_STRS_H
#define MSPED_STRS_H

#define S_MSPED_NENHUM _("Nenhum")
#define S_MSPED_ARG_INV _("Argumento inválido: %s\n")
#define S_MSPED_ARG_REQ _("Argumento requerido: %s\n")
#define S_MSPED_ARQ_NAO_ENC _("Arquivo não encontrado: %s\n")
#define S_MSPED_OBJ_ERR _("Não foi possível criar objeto: %s\n")
#define S_MSPED_VAR_ERR _("Não foi possível instanciar variável: %s\n")
#define S_MSPED_CFG_INV _("Configuração inválida\n")
#define S_MSPED_CHV_INV _("Chave inválida de NF: %s\n")
#define S_MSPED_SRV_IND _("Serviço indisponível para %s: %s\n")
#define S_MSPED_SRV_MOD_IND _("Serviço indisponível para modelo %s no %s: %s\n")
#define S_MSPED_LOTE_MAX_ERR _("Quantidade máxima de itens permitida por lote: %d\n")
#define S_MSPED_XML_ELEM_ERR _("Elemento não encontrado no XML: %s\n")
#define S_MSPED_XML_ATRI_ERR _("Atributo não encontrado no XML: %s\n")
#define S_MSPED_QRCODE_ERR _("Não foi possível gerar QR Code para documento: %s\n")
#define S_MSPED_SEM_NFE_ERR _("Nenhuma nota a ser enviada\n")
#define S_MSPED_LIM_TAM_LOTE_ERR _("Tamanho de lote acima do permitido: %zu %s\n")
#define S_MSPED_EV_NUM_SEQ_ERR _("O número sequencial do evento deve ser entre 1 e 20\n")
#define S_MSPED_EV_COR_ERR _("A correção deve ter entre 15 e 1000 caracteres\n")
#define S_MSPED_EV_JUS_ERR _("A justificativa deve ter pelo menos 15 digitos e no máximo 255\n")
#define S_MSPED_EV_ASS_MSG_ERR _("Não foi possível assinar mensagem: %s\n")
#define S_MSPED_SMTP_CFG_INV _("Configuração inválida de SMTP\n")

#endif