(*
 * libmicroutils - Micro cross-platform C library for handling strings, files,
 * regex, hashs, among others.
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

unit libmicroutils;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ELSE}
 {$IFDEF MSWINDOWS}
  {$DEFINE HAVE_DELAYED}
  {$WARN SYMBOL_PLATFORM OFF}
 {$ENDIF}
{$ENDIF}

{$IFDEF UNIX}
 {$LINKLIB m} // avoids two errors: "undefined reference to `log'"; "undefined reference to `floor'".
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
    'libmicroutils-4'
{$ELSE}
 {$IFDEF FPC}
    'microutils'
 {$ELSE}
    'libmicroutils'
 {$ENDIF}
{$ENDIF};

type
{$IF NOT DEFINED(FPC) AND DEFINED(NEXTGEN) AND DEFINED(POSIX)}
  AnsiChar = UTF8Char;
  AnsiString = UTF8String;
  PAnsiChar = PUTF8Char;
  PPAnsiChar = ^PUTF8Char;
{$ENDIF}
  Pcvoid = Pointer;
  cchar = AnsiChar;
  Pcchar = PAnsiChar;
  PPcchar = PPAnsiChar;
  Pcuchar = PByte;
  cbool = Boolean;
  cint = LongInt;
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
  Pcsize = ^csize;
  cenum = cint;

const
  MU_HEX_CHARS = '0123456789abcdef';
  MU_HEX_UCHARS = '0123456789ABCDEF';
  MU_DEF_MIME = 'application/octet-stream';
  MU_ZBUF_CHUNK = 16384;

function mu_ver: Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_errno: cint; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_err(errnum: cint): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_alloc(size: csize): Pcvoid; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

procedure mu_free(ptr: Pcvoid); cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_exec2(const dir: Pcchar; const name: Pcchar; const args: PPcchar;
  waited: cbool; showed: cbool; term: cbool;
  timeout: cint): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_exec(const name: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_replace(const str: Pcchar; const substr: Pcchar;
  const rep: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_mreplace(const str: Pcchar;
  const rep: Pcchar): Pcchar; cdecl; varargs; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

type
  mu_split_cb = function(cls: Pcvoid; const step: Pcchar;
    const token: cchar): cbool; cdecl;

function mu_split(const str: Pcchar; const delim: Pcchar;
  cb: mu_split_cb; cls: Pcvoid): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_trim(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_ltrim(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_rtrim(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_nums(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_between(const str: Pcchar; const p1: Pcchar;
  const p2: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_ibetween(const str: Pcchar; const p1: Pcchar;
  const p2: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_fmt(const fmt: Pcchar): Pcchar; cdecl; varargs; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_is_empty(const str: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

procedure mu_uitoh(val: cuint8_t; hex: Pcchar); cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

procedure mu_uitouh(val: cuint8_t; hex: Pcchar); cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_stoh(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_stouh(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_size_unit(size: cuint64_t): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_now_rfc2822: Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_remove(const path: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_rename(const old_path: Pcchar;
  const new_path: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_exists(const path: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_change_ext(const filename: Pcchar;
  const ext: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_ext(const filename: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_filename(const filename: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_path(const filename: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_ftos(const filename: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_stofa(const str: Pcchar; const filename: Pcchar;
  append: cbool): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_stof(const str: Pcchar;
  const filename: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_etomime(const ext: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_fntomime(const filename: Pcchar): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

type
  mu_mmatch_cb = function (cls: Pcvoid; const str: Pcchar; start: csize;
    &end: csize): cbool; cdecl;

function mu_mmatch(const pattern: Pcchar; const str: Pcchar; ncase: cbool;
  cb: mu_mmatch_cb; cls: Pcvoid): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_match(const pattern: Pcchar;
  const str: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_match2(const pattern: Pcchar; const str: Pcchar;
  ncase: cbool): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_imatch(const pattern: Pcchar;
  const str: Pcchar): cbool; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

type
  mu_zstream_data_cb = function(cls: Pcvoid; buf: Pcvoid;
    len: csize): csize; cdecl;

  mu_zstream_eof_cb = function(cls: Pcvoid): cbool; cdecl;

function mu_zdeflate(read_cb: mu_zstream_data_cb; read_cls: Pcvoid;
  write_cb: mu_zstream_data_cb; write_cls: Pcvoid;
  eof_cb: mu_zstream_eof_cb; eof_cls: Pcvoid;
  level: cint): cint; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_zfdeflate(const filename_in: Pcchar; const filename_out: Pcchar;
  level: cint): cint; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_zinflate(read_cb: mu_zstream_data_cb; read_cls: Pcvoid;
  write_cb: mu_zstream_data_cb; write_cls: Pcvoid): cint; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_zfinflate(const filename_in: Pcchar;
  const filename_out: Pcchar): cint; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_zerr(zerrnum: cint): Pcchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_b64enc(const src: Pcuchar; len: csize;
  out_len: Pcsize): Pcuchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function mu_b64dec(const src: Pcuchar; len: csize;
  out_len: Pcsize): Pcuchar; cdecl; external LIB_NAME
  {$IFDEF HAVE_DELAYED}delayed{$ENDIF};

implementation

end.