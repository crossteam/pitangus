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

unit libmicroxml;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

interface

{$IF NOT DEFINED(FPC) AND DEFINED(POSIX)}
uses
  SysTypes;
{$ENDIF}

const
  LIB_NAME =
{$IFDEF MSWINDOWS}
    'libmicroxml-3'
{$ELSE}
 {$IFDEF FPC}
    'microxml'
 {$ELSE}
    'libmicroxml'
 {$ENDIF}
{$ENDIF};

type
{$IF NOT DEFINED(FPC) AND DEFINED(NEXTGEN) AND DEFINED(POSIX)}
  PAnsiChar = PUTF8Char;
  PPAnsiChar = ^PUTF8Char;
{$ENDIF}
  pcvoid = Pointer;
  pcchar = PAnsiChar;
  ppcchar = PPAnsiChar;
  cbool = Boolean;
  cint = LongInt;
  cenum = cint;

const
  MXML_ERR_SIZE = 256;

type
  MXML_SEC_CB_TYPE = cenum;

const
  MXML_SEC_CB_TYPE_BEFORE = 0;
  MXML_SEC_CB_TYPE_SIGNING = 1;
  MXML_SEC_CB_TYPE_AFTER = 2;

type
  Pmxml_xml_cfg = ^mxml_xml_cfg;
  mxml_xml_cfg = record
  end;

  Pmxml_xmlsec_cfg = ^mxml_xmlsec_cfg;
  mxml_xmlsec_cfg = record
  end;

  mxml_sec_cb = function(cls: pcvoid; doc: pcvoid; cb_type: MXML_SEC_CB_TYPE): cbool; cdecl;

  mxml_err_cb = procedure(cls: pcvoid; const fmt: pcchar); cdecl;

function mxml_version: pcchar; cdecl; external LIB_NAME;

procedure mxml_xml_init; cdecl; external LIB_NAME;

procedure mxml_xml_finit; cdecl; external LIB_NAME;

function mxml_xml_cfg_new(const xsd_uri: pcchar; error_cb: mxml_err_cb; warn_cb: mxml_err_cb; error_warn_cls: pcvoid): Pmxml_xml_cfg; cdecl; external LIB_NAME;

procedure mxml_xml_cfg_free(cfg: Pmxml_xml_cfg); cdecl; external LIB_NAME;

function mxml_xml_cfg_ok(cfg: Pmxml_xml_cfg): cbool; cdecl; external LIB_NAME;

function mxml_xml_validate_file(cfg: Pmxml_xml_cfg; const filename: pcchar; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xml_doc_create_element(doc: pcvoid; const node_name: pcchar; const node_value: pcchar): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_doc_create_empty_element(doc: pcvoid; const node_name: pcchar): pcvoid; cdecl; external LIB_NAME;

procedure mxml_xml_doc_create_attribute(doc: pcvoid; const attr_name: pcchar); cdecl; external LIB_NAME;

procedure mxml_xml_doc_create_text_node(doc: pcvoid; const content: pcchar); cdecl; external LIB_NAME;

function mxml_xml_node_append_child(parent_node: pcvoid; new_node: pcvoid): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_doc_create_cdata_section(doc: pcvoid; const value: pcchar; value_length: cint): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_node_children_valid(node: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xml_node_is_read_only(node: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xml_node_find_node_by_name(root_node: pcvoid; const node_name: pcchar): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_doc_find_node_by_name(doc: pcvoid; const node_name: pcchar): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_node_try_get_value(node: pcvoid; value: ppcchar): cbool; cdecl; external LIB_NAME;

function mxml_xml_node_get_value_str(root_node: pcvoid; const node_name: pcchar): pcchar; cdecl; external LIB_NAME;

function mxml_xml_doc_get_value_str(doc: pcvoid; const node_name: pcchar): pcchar; cdecl; external LIB_NAME;

function mxml_xml_node_try_get_attr(node: pcvoid; const attr_name: pcchar; value: ppcchar): cbool; cdecl; external LIB_NAME;

function mxml_xml_node_get_attr_str(root_node: pcvoid; const node_name: pcchar; const attr_name: pcchar): pcchar; cdecl; external LIB_NAME;

function mxml_xml_doc_get_attr_str(doc: pcvoid; const node_name: pcchar; const attr_name: pcchar): pcchar; cdecl; external LIB_NAME;

function mxml_xml_node_insert_before(ref_node: pcvoid; new_node: pcvoid): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_node_child_new(parent_node: pcvoid; const node_name: pcchar; const node_value: pcchar): pcvoid; cdecl; external LIB_NAME;

function mxml_xml_node_child_empty_new(parent_node: pcvoid; const node_name: pcchar): pcvoid; cdecl; external LIB_NAME;

procedure mxml_xml_doc_import_node(doc: pcvoid; imported_node: pcvoid; deep: cbool); cdecl; external LIB_NAME;

function mxml_xml_node_dom_element_get_attribute(node: pcvoid; const name: pcchar): pcchar; cdecl; external LIB_NAME;

procedure mxml_xmlsec_init(columned_base64: cbool; error_cb: mxml_err_cb; error_cls: pcvoid); cdecl; external LIB_NAME;

procedure mxml_xmlsec_finit; cdecl; external LIB_NAME;

function mxml_xmlsec_cfg_pfx_file_new(const pfx_filename: pcchar; const pfx_pwd: pcchar; error_cb: mxml_err_cb; error_cls: pcvoid): Pmxml_xmlsec_cfg; cdecl; external LIB_NAME;

procedure mxml_xmlsec_cfg_free(cfg: Pmxml_xmlsec_cfg); cdecl; external LIB_NAME;

function mxml_xmlsec_cfg_ok(cfg: Pmxml_xmlsec_cfg): cbool; cdecl; external LIB_NAME;

function mxml_xmlsec_sign_string(cfg: Pmxml_xmlsec_cfg; const &string: pcchar; signed_string: Ppcchar; const encoding: pcchar; const id_attr: pcchar; const id_name: pcchar; declares_attlist: cbool; formatted: cbool; removes_blanks: cbool; skip_xml_declaration: cbool; compression: cint; auto_uri: cbool; single_cert: cbool; sec_cb: mxml_sec_cb; sec_cls: pcvoid; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xmlsec_easy_sign_string(cfg: Pmxml_xmlsec_cfg; const &string: pcchar; signed_string: Ppcchar; const id_attr: pcchar; const id_name: pcchar; declares_attlist: cbool; sec_cb: mxml_sec_cb; sec_cls: pcvoid; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xmlsec_sign_file(cfg: Pmxml_xmlsec_cfg; const filename: pcchar; const signed_filename: pcchar; const encoding: pcchar; const id_attr: pcchar; const id_name: pcchar; declares_attlist: cbool; formatted: cbool; removes_blanks: cbool; skip_xml_declaration: cbool; compression: cint; auto_uri: cbool; single_cert: cbool; sec_cb: mxml_sec_cb; sec_cls: pcvoid; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xmlsec_easy_sign_file(cfg: Pmxml_xmlsec_cfg; const filename: pcchar; const signed_filename: pcchar; const id_attr: pcchar; const id_name: pcchar; declares_attlist: cbool; sec_cb: mxml_sec_cb; sec_cls: pcvoid; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xmlsec_verify_file(cfg: Pmxml_xmlsec_cfg; const filename: pcchar; removes_dtd: cbool; sec_cb: mxml_sec_cb; sec_cls: pcvoid; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

function mxml_xmlsec_easy_verify_file(cfg: Pmxml_xmlsec_cfg; const filename: pcchar; sec_cb: mxml_sec_cb; sec_cls: pcvoid; error_cb: mxml_err_cb; error_cls: pcvoid): cbool; cdecl; external LIB_NAME;

implementation

end.
