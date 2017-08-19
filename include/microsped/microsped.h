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

#ifndef MICROSPED_H
#define MICROSPED_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSPED_EXTERN
#define MSPED_EXTERN extern
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <microxml.h>
#include <microutils.h>

// web service

/**
 * \brief Tamanho máximo de uma mensagens de erro.
 */
#define MSPED_ERR_TAM 256

/**
 * \brief Timeout padrão para requisições HTTP.
 */
#define MSPED_HTTP_TIMEOUT_PADRAO 5000

/**
 * \brief Timeout padrão para requisições SMTP.
 */
#define MSPED_SMTP_TIMEOUT_PADRAO 5000

/**
 * \brief Quantidade máxima de itens em um lote de envio.
 */
#define MSPED_LOTE_QTD_MAX_ITENS 49

/**
 * \brief Tamanho máximo para dados em um lote: 500 KB.
 */
#define MSPED_LOTE_TAMANHO_MAX 500 * 1024

/**
 * \brief Tipo de documento fiscal.
 */
enum MSPED_TIPO {
    /// Nenhum.
    MSPED_TIPO_NENHUM = 0,
    /// Tipo NF-e.
    MSPED_TIPO_NFE = 1,
    /// Tipo CT-e.
    MSPED_TIPO_CTE = 2,
    /// Tipo MFD-e.
    MSPED_TIPO_MDFE = 3
};

/**
 * \brief Tipo de documento do contribuinte.
 */
enum MSPED_DOC_TIPO {
    /// Nenhum.
    MSPED_DOC_TIPO_NENHUM = 0,
    /// Tipo CPF (Cadastro de Pessoas Físicas).
    MSPED_DOC_TIPO_CPF = 1,
    /// Tipo CNPJ (Cadastro Nacional da Pessoa Jurídica).
    MSPED_DOC_TIPO_CNPJ = 2,
    /// Tipo IE (Inscrição Estadual).
    MSPED_DOC_TIPO_IE = 3
};

/**
 * \brief Enumeração de cUFs disponíveis para definir web service.
 */
enum MSPED_CUF {
    /// Nenhum.
    MSPED_CUF_NENHUM = 0,
    /// AC.
    MSPED_CUF_AC = 12,
    /// AL.
    MSPED_CUF_AL = 27,
    /// AP.
    MSPED_CUF_AP = 16,
    /// AM.
    MSPED_CUF_AM = 13,
    /// AN (Ambiente Nacional)
    MSPED_CUF_AN = 91,
    /// BA.
    MSPED_CUF_BA = 29,
    /// CE.
    MSPED_CUF_CE = 23,
    /// DF.
    MSPED_CUF_DF = 53,
    /// ES.
    MSPED_CUF_ES = 32,
    /// GO.
    MSPED_CUF_GO = 52,
    /// MA.
    MSPED_CUF_MA = 21,
    /// MT.
    MSPED_CUF_MT = 51,
    /// MS.
    MSPED_CUF_MS = 50,
    /// MG.
    MSPED_CUF_MG = 31,
    /// PA.
    MSPED_CUF_PA = 15,
    /// PB.
    MSPED_CUF_PB = 25,
    /// PR.
    MSPED_CUF_PR = 41,
    /// PE.
    MSPED_CUF_PE = 26,
    /// PI.
    MSPED_CUF_PI = 22,
    /// RJ.
    MSPED_CUF_RJ = 33,
    /// RN.
    MSPED_CUF_RN = 24,
    /// RS.
    MSPED_CUF_RS = 43,
    /// RO.
    MSPED_CUF_RO = 11,
    /// RR.
    MSPED_CUF_RR = 14,
    /// SC.
    MSPED_CUF_SC = 42,
    /// SP.
    MSPED_CUF_SP = 35,
    /// SE.
    MSPED_CUF_SE = 28,
    /// TO.
    MSPED_CUF_TO = 17
};

/**
 * \brief Enumeração de ambiente para definir web service.
 */
enum MSPED_AMBIENTE {
    /// Nenhum.
    MSPED_AMB_NENHUM = 0,
    /// Ambiente de produção.
    MSPED_AMB_PRODUCAO = 1,
    /// Ambiente de homologação (testes).
    MSPED_AMB_HOMOLOGACAO = 2
};

/**
 * \brief Enumeração de modelos de NF para definir web service.
 */
enum MSPED_NF_MODELO {
    /// Nenhum.
    MSPED_NF_NENHUM = 0,
    /// Modelo 55
    MSPED_NF_MODELO_55 = 55,
    /// Modelo 65
    MSPED_NF_MODELO_65 = 65
};

/**
 * \brief Enumeração de Sefaz Virtual.
 */
enum MSPED_SV {
    /// Nenhum Sefaz Virtual.
    MSPED_SV_NENHUM = 0,
    /// SVAN
    MSPED_SVAN = 91,
    /// SVRS
    MSPED_SVRS = 1,
    /// SVCAN
    MSPED_SVCAN = 2,
    /// SVCRS
    MSPED_SVCRS = 3
};

/**
 * \brief Tipo de processo usado nas requisições.
 */
enum MSPED_PROCESSO_TIPO {
    /// Nenhum.
    MSPED_PROC_TIPO_NENHUM = 0,
    // Processo síncrono.
    MSPED_PROC_TIPO_ASSINCRONO = 1,
    // Processo assíncrono.
    MSPED_PROC_TIPO_SINCRONO = 2
};

/**
 * \brief Tipo de conteúdo usado no envio de e-mails.
 */
enum MSPED_SMTP_TIPO_CONTEUDO {
    /// Nenhum.
    MSPED_SMTP_TIPO_CONT_NENHUM = 0,
    /// Texto puro.
    MSPED_SMTP_TIPO_CONT_TEXTO = 1,
    /// Texto HTML.
    MSPED_SMTP_TIPO_CONT_HTML = 2
};

/**
 * \brief Objeto principal para configuração da bilioteca.
 */
struct msped_cfg;

/**
 * \brief Objeto lista para representar lote de NFs.
 */
struct msped_lote;

/**
 * \brief Objeto para configuração de SMTP.
 */
struct msped_smtp;

/**
 * \brief Callback para tratamento de erros.
 * \param cls Classe a ser passada para o callback.
 * \param msg Mensagem de erro. (máximo definido em #MSPED_ERRO_MAX_TAM)
 */
typedef void (*msped_erro_callback)(void *cls, const char *msg);

/**
 * \brief Callback para tratamento de tentativas de requisições HTTP.
 * \param cls Classe a ser passada para o callback.
 * \param http_client Instância do objeto client de HTTP.
 * \param url URL da requisição.
 * \return **false** para cancelar as tentativas.
 */
typedef bool (*msped_http_tentativa_callback)(void *cls, const void *http_client, const char *url);

/**
 * \brief Callback para escrita da dados retornados pelo client HTTP.
 * \param dados Dados para escrita.
 * \param tam Tamanho dos dados para escrita.
 * \param qtd Quantidade de dados para escrita.
 * \param cls Classe a ser passada para o callback.
 */
typedef size_t (*msped_http_escrita_callback)(void *dados, size_t tam, size_t qtd, void *cls);

/**
 * \brief Callback usado na geração de ID de lote.
 * \param cls Classe a ser passada para o callback.
 * \return Número gerado de lote.
 */
typedef uint64_t (*msped_lote_geraracao_id_callback)(void *cls);

/**
 * \brief Callback usado na assinatura da mensagem no envio de evento.
 * \param cls Classe a ser passada para o callback.
 * \param msg Mensagem a ser assinada.
 * \param id_tag ID da tag anterior a da assinatura.
 * \param msg_ass Mensagem assinada.
 * \return **true** se assinado e **false** em caso contrário.
 */
typedef bool (*msped_evento_assinatura_msg_callback)(void *cls, const char *msg, const char *id_tag,
                                                     const char **msg_ass);

/**
 * \brief Callback usado na atribuição de protocolo nas mensagens de eventos.
 * \param cls Classe a ser passada para o callback.
 * \param doc_retornado Instância XML do documento retornado pelo evento.
 */
typedef bool (*msped_atribuir_prot_msg_callback)(void *cls, void *doc_retornado);

// web service

/**
 * \brief Inicializa bilioteca e suas dependências.
 */
MSPED_EXTERN void msped_inicializar();

/**
 * \brief Finaliza bilioteca e suas dependências.
 */
MSPED_EXTERN void msped_finalizar();

/**
 * \brief Cria objeto de configuração.
 * \param tipo Tipo de documento para definir web service.
 * \param cuf Enum **cUF** para definir web service.
 * \param ambiente Ambiente de web service.
 * \param sv Enum **SV** para Sefaz Virtual. (opcional)
 * \param modelo Enum do arquivo modelo de documento.
 * \param arquivo_modelo Nome do arquivo modelo de documento.
 * \param pfx Arquivo PFX.
 * \param pfx_senha Senha do PFX.
 * \param http_timeout Timeout em milisegundos para as requisições HTTP. (padrão definido em #MSPED_HTTP_TIMEOUT_PADRAO)
 * \param depuravel Permite depurar biblioteca. (usa _stdout_)
 * \param erro_cb Callback para tratamento de erro.
 * \param erro_cls Classe a ser passada para o callback de erros.
 * \return Retorna objeto de configuração com as configurações definitas nos parâmetros.
 * \note Se **sv** receber #MSPED_SV_NENHUM a função escolherá o _SV_ automaticamente de acordo o _cUF_ informado em **cuf**.
 */
MSPED_EXTERN struct msped_cfg *msped_cfg_arquivo_novo(enum MSPED_TIPO tipo, enum MSPED_CUF cuf,
                                                      enum MSPED_AMBIENTE ambiente, enum MSPED_SV sv,
                                                      enum MSPED_NF_MODELO modelo,
                                                      const char *arquivo_modelo,
                                                      const char *pfx, const char *pfx_senha,
                                                      const char *n_versao, const char *token, const char *id_token,
                                                      long http_timeout, bool depuravel,
                                                      msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Libera objeto de configuração da memória.
 * \param cfg Objeto a ser liberado.
 */
MSPED_EXTERN void msped_cfg_liberar(struct msped_cfg *cfg);

/**
 * \brief Verifica se objeto de configuração está instanciado e configurado.
 * \param cfg Objeto de configuração a ser testado.
 * \return **true** se instanciado e configurado e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_cfg_ok(struct msped_cfg *cfg);

/**
 * \brief Retorna parâmetros de web service informando objeto de configuração e serviço.
 * \param cfg Objeto de configuração.
 * \param servico Serviço informado. (ex: NfeConsultaProtocolo)
 * \param metodo Método retornado. (ex: nfeConsultaNF2)
 * \param operacao Operação retornada. (ex: NfeConsulta2)
 * \param versao Versão retornada. (ex: 3.10)
 * \param sv_str SV retornada. (ex: PE, MT, SVAN etc.)
 * \param url URL retornada. (ex: https://nfe.sefaz.mt.gov.br/nfews/v2/services/NfeConsulta2)
 * \param erro_cb Callback para tratamento de erro.
 * \param erro_cls Classe a ser passada para o callback de erros.
 * \return **true** se serviço existir e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_cfg_ws_info(struct msped_cfg *cfg, const char *servico,
                                    char **metodo, char **operacao, char **versao, char **sv_str, char **url,
                                    msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Consulta se serviço está disponível.
 * \param cfg Objeto de configuração.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se disponível e **false** em caso contrário.
 * \note Esta chamada não consta no manual do contribuinte, pois é um recurso próprio da _libmicrosped_.
 */
MSPED_EXTERN bool msped_consultar_disponibilidade(struct msped_cfg *cfg, const char *servico,
                                                  msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Consulta situação de documento fiscal pela sua cahve.
 * \param cfg Objeto de configuração.
 * \param chave Chave do documento a ser consultado.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se disponível e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_consultar_situacao(struct msped_cfg *cfg, const char *chave,
                                           msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                           msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                           msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Consulta status do serviço de determinado estado.
 * \param cfg Objeto de configuração.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se disponível e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_consultar_status_servico(struct msped_cfg *cfg,
                                                 msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                                 msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                                 msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Consulta cadastro de contribuinte pelo seu documento.
 * \param cfg Objeto de configuração.
 * \param tipo_doc Tipo de documento para consulta.
 * \param doc Número do documento para consulta.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se disponível e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_consultar_cadastro(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc,
                                           msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                           msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                           msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Cria um novo objeto de lote.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return Objeto de lote instanciado ou **NULL** em caso de falha.
 */
MSPED_EXTERN struct msped_lote *msped_lote_novo(msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Libera objeto de lote da memória.
 * \param lote Objeto a ser liberado.
 */
MSPED_EXTERN void msped_lote_liberar(struct msped_lote *lote);

/**
 * \brief Verifica se um objeto de lote está instanciado e se contém itens.
 * \param lote Instância de objeto lote.
 * \return **true** se objeto estiver OK e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_lote_ok(struct msped_lote *lote);

/**
 * \brief Adiciona arquivo como item de lote.
 * \param lote Objeto de tipo lote.
 * \param arquivo_xml Arquivo a ser adicionado.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se arquivo adicionado com sucesso e **false** caso atingido limite máximo de itens no lote.
 */
MSPED_EXTERN bool msped_lote_arquivo_adicionar(struct msped_lote *lote, const char *arquivo_xml,
                                               msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Obtém arquivo por indice de item de lote.
 * \param lote Objeto de tipo lote.
 * \param indice Indice do item.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return Arquivo retornado caso exista para indice informado.
 */
MSPED_EXTERN const char *msped_lote_arquivo_obter_por_indice(struct msped_lote *lote, uint8_t indice,
                                                             msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Conta quantos itens estão adicionados em um lote.
 * \param lote Instância de objeto lote.
 * \return Quantidade de itens.
 */
MSPED_EXTERN int8_t msped_lote_contar(struct msped_lote *lote);

/**
 * \brief Extrai chave da nota informando instância do documento _XML_.
 * \param doc Instância do documento _XML_.
 * \param tag Nome da _tag_ que contém a propriedade _Id_ com a chave da nota. (padrão: infNFe)
 * \warning O chamador da função **deve** liberar a string retornada.
 * \return Nota extraída.
 */
MSPED_EXTERN char *msped_doc_obter_chave(void *doc, const char *tag);

/**
 * \brief Acrescenta QR Code numa instância de objeto _XML_ com o conteúdo do documento fiscal.
 * \param cfg Objeto de configuração.
 * \param xml_doc Instância de objeto _XML_ com o conteúdo do documento fiscal.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se atribuído e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_qrcode_acrescentar(struct msped_cfg *cfg, void *xml_doc,
                                           msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Envia lote de notas fiscais.
 * \param cfg Objeto de configuração.
 * \param lote Objeto lote.
 * \param processo Tipo de processo (síncrono/assíncrono).
 * \param lote_gera_id_cb Callback para gerar ID do lote.
 * \param lote_gera_id_cls Classe para ser passado ao callback de geração de ID do lote.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se enviado com sucesso e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_enviar_lote(struct msped_cfg *cfg, struct msped_lote *lote, enum MSPED_PROCESSO_TIPO processo,
                                    msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                                    msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                    msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                    msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Consulta recibo de lote de notas fiscais.
 * \param cfg Objeto de configuração.
 * \param recibo Código do recibo.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se a consulta ocorrer com sucesso e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_consultar_recibo(struct msped_cfg *cfg, uint64_t recibo,
                                         msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                         msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                         msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Atribui protocolo em mensagem de retorno de evento.
 * \param tagproc Tag do protocolo.
 * \param tagmsg Tag da mensagem.
 * \param xmlmsg XML da mensagem.
 * \param tagretorno Tag do retorno.
 * \param xmlretorno XML do retorno.
 * \param atribuido Indicador de atribuição de protocolo.
 * \param atrib_cb Callback para atribuição de protocolo.
 * \param atrib_cls Classe para ser passada ao callback para atribuição de protocolo.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return XML protocolado.
 */
MSPED_EXTERN char *msped_atribuir_prot_msg(const char *tagproc, const char *tagmsg, const char *xmlmsg,
                                           const char *tagretorno, const char *xmlretorno, bool *atribuido,
                                           msped_atribuir_prot_msg_callback atrib_cb, void *atrib_cls,
                                           msped_erro_callback erro_cb, void *erro_cls);
/**
 * \brief Envia evento Carta de Correção Eletrônica.
 * \param cfg Objeto de configuração.
 * \param tipo_doc Tipo de documento para consulta.
 * \param doc Número do documento para consulta.
 * \param chave Chave do documento a ser consultado.
 * \param x_correcao Correção a ser considerada, texto livre.
 * \param n_seq_evento Sequencial do evento para o mesmo tipo de evento.
 * \param assin_msg_cb Callback para assinatura de mensagem.
 * \param assin_msg_cls Classe para ser passada ao callback de assinatura de mensagem.
 * \param lote_gera_id_cb Callback para geração de ID de lote.
 * \param lote_gera_id_cls Classe a ser passada para o callback de geração de ID de lote.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se a mensagemn for enviada com sucesso e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_enviar_evento_cce(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc,
                                          const char *chave, const char *x_correcao, uint8_t n_seq_evento,
                                          msped_evento_assinatura_msg_callback assin_msg_cb, void *assin_msg_cls,
                                          msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                                          msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                          msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                          msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Envia evento Cancelamento de documento fiscal.
 * \param cfg Objeto de configuração.
 * \param tipo_doc Tipo de documento para consulta.
 * \param doc Número do documento para consulta.
 * \param chave Chave do documento a ser consultado.
 * \param x_just Justificativa do cancelamento.
 * \param n_prot Número do Protocolo de Autorização do Documento a ser Cancelado.
 * \param assin_msg_cb Callback para assinatura de mensagem.
 * \param assin_msg_cls Classe para ser passada ao callback de assinatura de mensagem.
 * \param lote_gera_id_cb Callback para geração de ID de lote.
 * \param lote_gera_id_cls Classe a ser passada para o callback de geração de ID de lote.
 * \param http_escrita_cb Callback para escrita de dados HTTP retornados pelo servidor de web service.
 * \param http_escrita_cls Classe a ser passada para o callback de escrita HTTP.
 * \param http_tentativa_cb Callback para tratamento de tentativas de requisições HTTP.
 * \param http_tentativa_cls Classe a ser passada ao callback de tentativas de requisições HTTP.
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se a mensagemn for enviada com sucesso e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_enviar_evento_canc(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc,
                                           const char *chave, const char *x_just, const char *n_prot,
                                           msped_evento_assinatura_msg_callback assin_msg_cb, void *assin_msg_cls,
                                           msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                                           msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                           msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                           msped_erro_callback erro_cb, void *erro_cls);

// e-mail

/**
 * \brief Inicializa bibliotecas SMTP.
 */
MSPED_EXTERN void msped_smtp_inicializar();

/**
 * \brief Finaliza bibliotecas SMTP.
 */
MSPED_EXTERN void msped_smtp_finalizar();

/**
 * \brief Cria objeto SMTP.
 * \param servidor URL do servidor de SMTP.
 * \param porta Porta do servidor de SMTP.
 * \param ssl **true** para servidores SMTP com suporte a SSL.
 * \param usuario Nome de usuário para acesso ao servidor de SMTP.
 * \param senha Senha para acesso ao servidor de SMTP.
 * \param timeout Timeout para requisições SMTP. (padrão: 5 segundos)
 * \param depuravel Permite depurar requisições SMTP. (usa _stdout_)
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return Instância de objeto para configuração de SMTP.
 */
MSPED_EXTERN struct msped_smtp *msped_smtp_novo(const char *servidor, long porta, bool ssl, const char *usuario,
                                                const char *senha, long timeout, bool depuravel,
                                                msped_erro_callback erro_cb, void *erro_cls);

/**
 * \brief Libera objeto SMTP da memória.
 * \param lote Objeto a ser liberado.
 */
MSPED_EXTERN void msped_smtp_liberar(struct msped_smtp *smtp);

/**
 * \brief Verifica se objeto SMTP está instanciado e configurado.
 * \param smtp Objeto SMTP a ser testado.
 * \return **true** se instanciado e configurado e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_smtp_ok(struct msped_smtp *smtp);

/**
 * \brief Envia e-mail usando protocolo SMTP.
 * \param smtp Instância de objeto SMTP.
 * \param tipo_conteudo Tipo de conteúdo da mensagem. (texto ou HTML)
 * \param de Remetente do e-mail. Exemplos válidos:
 * <pre>
 * meunome@@host.com
 * Meu Nome <meunome@@host.com>
 * </pre>
 * \param para Destinatário(s) do e-mail. Exemplos válidos:
 * <pre>
 * contato@@host.com
 * Contato <contato@@host.com>
 * Contato 1 <contato1@@host.com>, Contato 2 <contato2@@host.com>
 * </pre>
 * \param cc E-mail Com Cópia para outros destinatários (permite mesma síntaxe do parâmetro \p para).
 * \param cco E-mail Com Cópia Oculta para outros destinatários (permite mesma síntaxe do parâmetro \p para).
 * \param titulo Título do e-mail.
 * \param msg Mensagem do e-mail.
 * \param anexos Anexos do e-mail (separados por vírgula).
 * \param erro_cb Callback para tratar erros.
 * \param erro_cls Classe para ser passada ao callback de erros.
 * \return **true** se e-mail enviado com sucesso e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_smtp_enviar(struct msped_smtp *smtp, enum MSPED_SMTP_TIPO_CONTEUDO tipo_conteudo,
                                    const char *de, const char *para, const char *cc, const char *cco,
                                    const char *titulo, const char *msg, const char *anexos,
                                    msped_erro_callback erro_cb, void *erro_cls);

// utilidades

/**
 * \brief Cria hash SHA-1 de uma string.
 * \param str String para criar hash SHA-1.
 * \return Hash SHA-1.
 * \warning O chamador da função **deve** liberar a string retornada.
 */
MSPED_EXTERN char *msped_sha1_criar_hash(const char *str);

/**
 * \brief Extrai enumerador de tipo de documento.
 * \param doc Documento passado como string.
 * \return Enumerador de tipo de documento.
 */
MSPED_EXTERN enum MSPED_DOC_TIPO msped_extrair_tipo_doc(const char *doc);

/**
 * \brief Extrai cUF e modelo passando uma chave de documento.
 * \param chave Chave do documento.
 * \param cuf cUF extraída.
 * \param modelo Modelo extraído.
 * \return **true** se a extração ocorrer com sucesso e **false** em caso contrário.
 */
MSPED_EXTERN bool msped_extrair_cuf_modelo(const char *chave, enum MSPED_CUF *cuf, enum MSPED_NF_MODELO *modelo);

/**
 * \brief Converte enum #MSPED_CUF para UF.
 * \param cuf Enum a ser convertido para UF.
 * \return **cuf** convertido para UF.
 */
MSPED_EXTERN const char *msped_cuf_para_uf(enum MSPED_CUF cuf);

/**
 * \brief Converte UF para enum #MSPED_CUF.
 * \param cuf UF a ser convertida para #MSPED_CUF.
 * \return UF convertida para #MSPED_CUF.
 */
MSPED_EXTERN enum MSPED_CUF msped_uf_para_cuf(const char *uf);

/**
 * \brief Validação de enumerador de tipo de documento fiscal.
 * \param tipo Enumerador a ser validado.
 * \return **true** se válido e **false** se inválido.
 */
MSPED_EXTERN bool msped_validar_tipo(enum MSPED_TIPO tipo);

/**
 * \brief Validação de enumerador de cUF.
 * \param cuf Enumerador a ser validado.
 * \return **true** se válido e **false** se inválido.
 */
MSPED_EXTERN bool msped_validar_cuf(enum MSPED_CUF cuf);

/**
 * \brief Validação de enumerador de ambiente.
 * \param ambiente Enumerador a ser validado.
 * \return **true** se válido e **false** se inválido.
 */
MSPED_EXTERN bool msped_validar_ambiente(enum MSPED_AMBIENTE ambiente);

/**
 * \brief Converte enum #MSPED_AMBIENTE para string.
 * \param ambiente Enum a ser convertido.
 * \return **ambiente** convertido para string.
 */
MSPED_EXTERN const char *msped_ambiente_para_str(enum MSPED_AMBIENTE ambiente);

/**
 * \brief Validação de enumerador de modelo de NF.
 * \param modelo Enumerador a ser validado.
 * \return **true** se válido e **false** se inválido.
 */
MSPED_EXTERN bool msped_validar_modelo_nf(enum MSPED_NF_MODELO modelo);

/**
 * \brief Converte enum #MSPED_NF_MODELO para string.
 * \param ambiente Enum a ser convertido.
 * \return **modelo** convertido para string.
 */
MSPED_EXTERN const char *msped_modelo_para_str(enum MSPED_NF_MODELO modelo);

/**
 * \brief Converte de string para enum #MSPED_NF_MODELO.
 * \param ambiente String a ser convertida.
 * \return Enum #MSPED_NF_MODELO convertido de string.
 */
MSPED_EXTERN enum MSPED_NF_MODELO msped_str_para_modelo(const char *str);

/**
 * \brief Validação de chave usando regra _[0-9]{44}_ estraída do arquivo _XSD_ da _SEFAZ_.
 * \param chave Chave em questão.
 * \return **true** se válida e **false** se inválida.
 */
MSPED_EXTERN bool msped_validar_chave(const char *chave);

/**
 * \brief Gera ID de lote com suporte a atraso informado em microsegundos.
 * \param id ID sequencial com 15 caracteres.
 * \param usegs Atraso em microsegundos.
 * \note Esta função é thread-safe.
 */
MSPED_EXTERN void msped_gerar_id_lote_sleep(char *id, useconds_t usegs);

/**
 * \brief Gera ID de lote com atraso de 100000 microsegundos.
 * \param id ID sequencial com 15 caracteres.
 * \note Esta função é thread-safe.
 */
MSPED_EXTERN void msped_gerar_id_lote(char *id);

/**
 * \brief Gera data e hora no formato AAAA-MM-DDThh:mm:ssTZD (UTC).
 * \return data e hora no formato UTC.
 * \warning O chamador da função **deve** liberar a string retornada.
 */
MSPED_EXTERN char *msped_data_hora_utc();

/**
 * \brief Limpa mensagem de web service.
 * \param msg Mensagem em string.
 * \return Mensagem limpa.
 * \warning O chamador da função **deve** liberar a string retornada.
 */
MSPED_EXTERN char *msped_limpar_mensagem(const char *msg);

/**
 * \brief Limpa o XML após adição do protocolo.
 * \param proc_xml String de entrada.
 * \return XML limpo.
 * \warning O chamador da função **deve** liberar a string retornada.
 */
MSPED_EXTERN char *msped_limpar_prot(const char *proc_xml);

/**
 * \brief Limpa encoding e strings vazias de um XML.
 * \param xml XML de entrada.
 * \param rem_enc **true** para remover encoding.
 * \return XML sem encoding.
 * \warning O chamador da função **deve** liberar a string retornada.
 */
MSPED_EXTERN char *msped_limpar_xml(const char *xml, bool rem_enc);

/**
 * \brief Converte string para inteiro.
 * \param str String de entrada.
 * \return Inteiro convertido.
 */
MSPED_EXTERN long msped_static_str_to_long(const char *str);

/**
 * \brief Converte string para inteiro liberando automaticamente a string passada por parâmetro.
 * \param str String de entrada.
 * \return Inteiro convertido.
 */
MSPED_EXTERN long msped_str_to_long(char *str);

#ifdef __cplusplus
}
#endif

#endif