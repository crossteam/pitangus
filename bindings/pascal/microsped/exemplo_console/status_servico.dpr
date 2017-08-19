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

program status_servico;

{$I exemplos.inc}
{$IFDEF MSWINDOWS}
{$APPTYPE CONSOLE}
{$ENDIF}

uses
  SysUtils,
  Classes,
  libmicrosped;

const
  USO_FMT = Concat('Uso: %s <certificado.pfx> <senha-certificado> <codigo-modelo> <uf>', sLineBreak);
  MOD_PATH = '../../../../config/';
  ARQ_MOD_55 = Concat(MOD_PATH, 'nfe_ws3_mod55.xml');
  ARQ_MOD_65 = Concat(MOD_PATH, 'nfe_ws3_mod65.xml');

  function http_escrita_callback(dados: pcvoid; tam: csize_t; qtd: csize_t;
    cls: pcvoid): csize_t; cdecl;
  begin
    Result := TBytesStream(cls).Write(dados^, tam * qtd);
  end;

  procedure erro_callback(cls: pcvoid; const msg: pcchar); cdecl;
  begin
    WriteLn(ErrOutput, msg);
  end;

var
  ok: cbool;
  arquivo_modelo: pcchar;
  cuf: MSPED_CUF;
  modelo: MSPED_NF_MODELO;
  cfg: Pmsped_cfg;
  xml: TBytesStream;
label
  falha;
begin
  if ParamCount <> 4 then
  begin
    WriteLn(ErrOutput, Format(USO_FMT, [ParamStr(0)]));
    Halt;
  end;
  msped_inicializar;
  modelo := msped_str_para_modelo(pcchar(AnsiString(ParamStr(3))));
  if not msped_validar_modelo_nf(modelo) then
  begin
    WriteLn(ErrOutput, Format('Código inválido de modelo: %d', [modelo]));
    goto falha;
  end;
  cuf := msped_uf_para_cuf(pcchar(AnsiString(ParamStr(4))));
  if not msped_validar_cuf(cuf) then
  begin
    WriteLn(ErrOutput, Format('Código inválido de cUF: %d', [cuf]));
    goto falha;
  end;
  case modelo of
    MSPED_NF_MODELO_55: arquivo_modelo := ARQ_MOD_55;
    MSPED_NF_MODELO_65: arquivo_modelo := ARQ_MOD_65;
  else
    arquivo_modelo := ARQ_MOD_55;
  end;
  xml := TBytesStream.Create;
  cfg := msped_cfg_arquivo_novo(MSPED_TIPO_NFE, cuf, MSPED_AMB_HOMOLOGACAO,
    MSPED_SV_NENHUM, modelo, arquivo_modelo, pcchar(AnsiString(ParamStr(1))),
    pcchar(AnsiString(ParamStr(2))), nil, nil, nil, 0, False,
    @erro_callback, nil);
  try
    ok := msped_cfg_ok(cfg);
    if ok then
    begin
      ok := msped_consultar_status_servico(cfg, @http_escrita_callback, xml,
        nil, nil, @erro_callback, nil);
      if ok then
      begin
        xml.Seek(0, TSeekOrigin.soBeginning);
        WriteLn(TEncoding.UTF8.GetString(xml.Bytes, 0, xml.Size));
      end;
    end;
  finally
    xml.Free;
  end;
falha:
  msped_cfg_liberar(cfg);
  msped_finalizar;
end.
