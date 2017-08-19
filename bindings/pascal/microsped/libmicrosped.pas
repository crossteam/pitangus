(*
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
 *)

unit libmicrosped;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

interface

{$IFDEF FPC}
 {$IFDEF UNIX}
uses
  BaseUnix;
 {$ENDIF}
{$ELSE}
uses
 {$IFDEF MSWINDOWS}
  Windows
 {$ELSE}
  SysTypes
 {$ENDIF};
{$ENDIF}

const
  LIB_NAME =
{$IFDEF MSWINDOWS}
    'libmicrosped-1'
{$ELSE}
 {$IFDEF FPC}
    'microsped'
 {$ELSE}
    'libmicrosped'
 {$ENDIF}
{$ENDIF};

type
{$IF NOT DEFINED(FPC) DEFINED(NEXTGEN) AND DEFINED(POSIX)}
  AnsiChar = UTF8Char;
  AnsiString = UTF8String;
  PAnsiChar = PUTF8Char;
  PPAnsiChar = ^PUTF8Char;
{$ENDIF}
  pcvoid = Pointer;
  cchar = AnsiChar;
  pcchar = PAnsiChar;
  ppcchar = PPAnsiChar;
  cbool = Boolean;
  pcbool = PBoolean;
  cint = LongInt;
  clong = {$IFDEF CPU64}Int64{$ELSE}LongInt{$ENDIF};
  culong = LongWord;
  cint8_t = ShortInt;
  cuint8_t = Byte;
  cuint64_t = UInt64;
  csize =
{$IFDEF FPC}
{$IFDEF UNIX}
    BaseUnix
{$ELSE}
    System
{$ENDIF}
{$ELSE}
 {$IFDEF MSWINDOWS}
    Windows
 {$ELSE}
    SysTypes
 {$ENDIF}
{$ENDIF}.size_t;
  cenum = cint;
  cuseconds_t = Cardinal;

const
  MSPED_ERRO_MAX_TAM = 256;
  MSPED_HTTP_TIMEOUT_PADRAO = 5000;
  MSPED_SMTP_TIMEOUT_PADRAO = 5000;
  MSPED_LOTE_QTD_MAX_ITENS = 49;
  MSPED_LOTE_TAMANHO_MAX = 500 * 1024;

type
  MSPED_TIPO = cenum;

const
  MSPED_TIPO_NENHUM = 0;
  MSPED_TIPO_NFE = 1;
  MSPED_TIPO_CTE = 2;
  MSPED_TIPO_MDFE = 3;

type
  MSPED_DOC_TIPO = cenum;

const
  MSPED_DOC_TIPO_NENHUM = 0;
  MSPED_DOC_TIPO_CPF = 1;
  MSPED_DOC_TIPO_CNPJ = 2;
  MSPED_DOC_TIPO_IE = 3;

type
  PMSPED_CUF = ^MSPED_CUF;
  MSPED_CUF = cenum;

const
  MSPED_CUF_NENHUM = 0;
  MSPED_CUF_AC = 12;
  MSPED_CUF_AL = 27;
  MSPED_CUF_AP = 16;
  MSPED_CUF_AM = 13;
  MSPED_CUF_AN = 91;
  MSPED_CUF_BA = 29;
  MSPED_CUF_CE = 23;
  MSPED_CUF_DF = 53;
  MSPED_CUF_ES = 32;
  MSPED_CUF_GO = 52;
  MSPED_CUF_MA = 21;
  MSPED_CUF_MT = 51;
  MSPED_CUF_MS = 50;
  MSPED_CUF_MG = 31;
  MSPED_CUF_PA = 15;
  MSPED_CUF_PB = 25;
  MSPED_CUF_PR = 41;
  MSPED_CUF_PE = 26;
  MSPED_CUF_PI = 22;
  MSPED_CUF_RJ = 33;
  MSPED_CUF_RN = 24;
  MSPED_CUF_RS = 43;
  MSPED_CUF_RO = 11;
  MSPED_CUF_RR = 14;
  MSPED_CUF_SC = 42;
  MSPED_CUF_SP = 35;
  MSPED_CUF_SE = 28;
  MSPED_CUF_TO = 17;

type
  MSPED_AMBIENTE = cenum;

const
  MSPED_AMB_NENHUM = 0;
  MSPED_AMB_PRODUCAO = 1;
  MSPED_AMB_HOMOLOGACAO = 2;

type
  PMSPED_NF_MODELO = ^MSPED_NF_MODELO;
  MSPED_NF_MODELO = cenum;

const
  MSPED_NF_NENHUM = 0;
  MSPED_NF_MODELO_55 = 55;
  MSPED_NF_MODELO_65 = 65;

type
  MSPED_SV = cenum;

const
  MSPED_SV_NENHUM = 0;
  MSPED_SVAN = 91;
  MSPED_SVRS = 1;
  MSPED_SVCAN = 2;
  MSPED_SVCRS = 3;

type
  MSPED_PROCESSO_TIPO = cenum;

const
  MSPED_PROC_TIPO_NENHUM = 0;
  MSPED_PROC_TIPO_ASSINCRONO = 1;
  MSPED_PROC_TIPO_SINCRONO = 2;

type
  MSPED_SMTP_TIPO_CONTEUDO = cenum;

const
  MSPED_SMTP_TIPO_CONT_NENHUM = 0;
  MSPED_SMTP_TIPO_CONT_TEXTO = 1;
  MSPED_SMTP_TIPO_CONT_HTML = 2;

type
  Pmsped_cfg = ^msped_cfg;
  msped_cfg = record
  end;

  Pmsped_lote = ^msped_lote;
  msped_lote = record
  end;

  Pmsped_smtp = ^msped_smtp;
  msped_smtp = record
  end;

  msped_erro_callback = procedure(cls: pcvoid; const msg: pcchar); cdecl;

  msped_http_tentativa_callback = function(cls: pcvoid; const http_client: pcvoid; const url: pcchar): cbool; cdecl;

  msped_http_escrita_callback = function(dados: pcvoid; tam: csize; qtd: csize; cls: pcvoid): csize; cdecl;

  msped_lote_geraracao_id_callback = function(cls: pcvoid): cuint64_t; cdecl;

  msped_evento_assinatura_msg_callback = function(cls: pcvoid; const msg: pcchar; const id_tag: pcchar; const msg_ass: ppcchar): cbool; cdecl;

  msped_atribuir_prot_msg_callback = function(cls: pcvoid; doc_retornado: pcvoid): cbool; cdecl;

procedure msped_inicializar; cdecl; external LIB_NAME;

procedure msped_finalizar; cdecl; external LIB_NAME;

function msped_cfg_arquivo_novo(tipo: MSPED_TIPO; cuf: MSPED_CUF; ambiente: MSPED_AMBIENTE; sv: MSPED_SV; modelo: MSPED_NF_MODELO; const arquivo_modelo: pcchar; const pfx: pcchar; const pfx_senha: pcchar; const n_versao: pcchar; const token: pcchar; const id_token: pcchar; http_timeout: culong; depuravel: cbool; erro_cb: msped_erro_callback; erro_cls: pcvoid): Pmsped_cfg; cdecl; external LIB_NAME;

procedure msped_cfg_liberar(cfg: Pmsped_cfg); cdecl; external LIB_NAME;

function msped_cfg_ok(cfg: Pmsped_cfg): cbool; cdecl; external LIB_NAME;

function msped_cfg_ws_info(cfg: Pmsped_cfg; const servico: pcchar; metodo: ppcchar; operacao: ppcchar; versao: ppcchar; sv_str: ppcchar; url: ppcchar; erro_cb: msped_erro_callback; erro_cls: pointer): cbool; cdecl; external LIB_NAME;

function msped_consultar_disponibilidade(cfg: Pmsped_cfg; const servico: pcchar; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_consultar_situacao(cfg: Pmsped_cfg; const chave: pcchar; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_consultar_status_servico(cfg: Pmsped_cfg; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_consultar_cadastro(cfg: Pmsped_cfg; tipo_doc: MSPED_DOC_TIPO; const doc: pcchar; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_lote_novo(erro_cb: msped_erro_callback; erro_cls: pcvoid): Pmsped_lote; cdecl; external LIB_NAME;

procedure msped_lote_liberar(lote: Pmsped_lote); cdecl; external LIB_NAME;

function msped_lote_ok(lote: Pmsped_lote): cbool; cdecl; external LIB_NAME;

function msped_lote_arquivo_adicionar(lote: Pmsped_lote; const arquivo_xml: pcchar; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_lote_arquivo_obter_por_indice(lote: Pmsped_lote; indice: cuint8_t; erro_cb: msped_erro_callback; erro_cls: pcvoid): pcchar; cdecl; external LIB_NAME;

function msped_lote_contar(lote: Pmsped_lote): cint8_t; cdecl; external LIB_NAME;

function msped_doc_obter_chave(doc: pcvoid; const tag: pcchar): pcchar; cdecl; external LIB_NAME;

function msped_qrcode_acrescentar(cfg: Pmsped_cfg; xml_doc: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_enviar_lote(cfg: Pmsped_cfg; lote: Pmsped_lote; processo: MSPED_PROCESSO_TIPO; gerar_id_cb: msped_lote_geraracao_id_callback; lote_gera_id_cls: pcvoid; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_consultar_recibo(cfg: Pmsped_cfg; recibo: cuint64_t; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_atribuir_prot_msg(const tagproc: pcchar; const tagmsg: pcchar; const xmlmsg: pcchar; const tagretorno: pcchar; const xmlretorno: pcchar; atribuido: pcbool; atrib_cb: msped_atribuir_prot_msg_callback; atrib_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): pcchar; cdecl; external LIB_NAME;

function msped_enviar_evento_cce(cfg: Pmsped_cfg; tipo_doc: MSPED_DOC_TIPO; const doc: pcchar; const chave: pcchar; const x_correcao: pcchar; n_seq_evento: cuint8_t; assin_msg_cb: msped_evento_assinatura_msg_callback; assin_msg_cls: pcvoid; lote_gera_id_cb: msped_lote_geraracao_id_callback; lote_gera_id_cls: pcvoid; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_enviar_evento_canc(cfg: Pmsped_cfg; tipo_doc: MSPED_DOC_TIPO; const doc: pcchar; const chave: pcchar; const x_just: pcchar; const n_prot: pcchar; assin_msg_cb: msped_evento_assinatura_msg_callback; assin_msg_cls: pcvoid; lote_gera_id_cb: msped_lote_geraracao_id_callback; lote_gera_id_cls: pcvoid; http_escrita_cb: msped_http_escrita_callback; http_escrita_cls: pcvoid; http_tentativa_cb: msped_http_tentativa_callback; http_tentativa_cls: pcvoid; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

procedure msped_smtp_inicializar; cdecl; external LIB_NAME;

procedure msped_smtp_finalizar; cdecl; external LIB_NAME;

function msped_smtp_novo(const servidor: pcchar; porta: culong; ssl: cbool; const usuario: pcchar; const senha: pcchar; timeout: culong; depuravel: cbool; erro_cb: msped_erro_callback; erro_cls: pcvoid): Pmsped_smtp; cdecl; external LIB_NAME;

procedure msped_smtp_liberar(smtp: Pmsped_smtp); cdecl; external LIB_NAME;

function msped_smtp_ok(smtp: Pmsped_smtp): cbool; cdecl; external LIB_NAME;

function msped_smtp_enviar(smtp: Pmsped_smtp; tipo_conteudo: MSPED_SMTP_TIPO_CONTEUDO; const de: pcchar; const para: pcchar; const cc: pcchar; const cco: pcchar; const titulo: pcchar; const msg: pcchar; const anexos: pcchar; erro_cb: msped_erro_callback; erro_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function msped_sha1_criar_hash(const str: pcchar): pcchar; cdecl; external LIB_NAME;

function msped_extrair_tipo_doc(const doc: pcchar): MSPED_DOC_TIPO; cdecl; external LIB_NAME;

function msped_extrair_cuf_modelo(const chave: pcchar; cuf: PMSPED_CUF; modelo: PMSPED_NF_MODELO): cbool; cdecl; external LIB_NAME;

function msped_cuf_para_uf(cuf: MSPED_CUF): pcchar; cdecl; external LIB_NAME;

function msped_uf_para_cuf(const uf: pcchar): MSPED_CUF; cdecl; external LIB_NAME;

function msped_validar_tipo(tipo: MSPED_TIPO): cbool; cdecl; external LIB_NAME;

function msped_validar_cuf(cuf: MSPED_CUF): cbool; cdecl; external LIB_NAME;

function msped_validar_ambiente(ambiente: MSPED_AMBIENTE): cbool; cdecl; external LIB_NAME;

function msped_ambiente_para_str(ambiente: MSPED_AMBIENTE): pcchar; cdecl; external LIB_NAME;

function msped_validar_modelo_nf(modelo: MSPED_NF_MODELO): cbool; cdecl; external LIB_NAME;

function msped_modelo_para_str(modelo: MSPED_NF_MODELO): pcchar; cdecl; external LIB_NAME;

function msped_str_para_modelo(const str: pcchar): MSPED_NF_MODELO; cdecl; external LIB_NAME;

function msped_validar_chave(const chave: Pcchar): cbool; cdecl; external LIB_NAME;

procedure msped_gerar_id_lote_sleep(const id: pcchar; usegs: cuseconds_t); cdecl; external LIB_NAME;

procedure msped_gerar_id_lote(const id: pcchar); cdecl; external LIB_NAME;

function msped_data_hora_utc: pcchar; cdecl; external LIB_NAME;

function msped_limpar_mensagem(const msg: pcchar): pcchar; cdecl; external LIB_NAME;

function msped_limpar_prot(const proc_xml: pcchar): pcchar; cdecl; external LIB_NAME;

function msped_limpar_xml(const xml: pcchar; rem_enc: cbool): pcchar; cdecl; external LIB_NAME;

function msped_static_str_to_long(const str: pcchar): clong; external LIB_NAME;

function msped_str_to_long(str: pcchar): clong; external LIB_NAME;

implementation

end.
