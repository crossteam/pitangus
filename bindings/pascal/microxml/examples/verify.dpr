(*
 * libmicroxml - Micro cross-platform library for XML validation and signature.
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

program sign;

{$I examples.inc}
{$IFDEF MSWINDOWS}
 {$APPTYPE CONSOLE}
{$ENDIF}

uses
  SysUtils,
  libmicroxml;

const USAGE_FMT = 'Usage: %s <pfx-filename> <pfx-password> <signed-filename>';

  procedure error_cb(cls: pcvoid; const msg: pcchar); cdecl;
  begin
    WriteLn(ErrOutput, string(msg));
  end;

var
  ok: cbool;
  cfg: Pmxml_xmlsec_cfg;
begin
  if ParamCount <> 3 then
  begin
    WriteLn(ErrOutput, Format(USAGE_FMT, [ParamStr(0)]));
    Halt;
  end;
  mxml_xmlsec_init(False, @error_cb, nil);
  cfg := mxml_xmlsec_cfg_pfx_file_new(pcchar(AnsiString(ParamStr(1))),
    pcchar(AnsiString(ParamStr(2))), @error_cb, nil);
  ok := mxml_xmlsec_cfg_ok(cfg);
  if ok then
  begin
     ok := mxml_xmlsec_easy_verify_file(cfg, pcchar(AnsiString(ParamStr(3))),
       nil, nil, @error_cb, nil);
     if ok then
       WriteLn('Verified XML!');
  end;
  mxml_xmlsec_cfg_free(cfg);
  mxml_xmlsec_finit;
end.

