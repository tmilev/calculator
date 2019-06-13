/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "../include/openssl/objects.h"
#include "ssl_locl.h"

/*-
 * TLS/SSLv3 methods
 */

ssl_method_st* ssl_method_st::initializeTLS(
  const std::string& inputName,
  int inputVersion,
  unsigned inputFlags,
  unsigned inputMask,
  const std::string& acceptName,
  int (s_accept)(SSL *s, std::stringstream* commentsOnError),
  const std::string& connectName,
  int (s_connect)(SSL *s, std::stringstream* commentsOnError),
  const ssl3_enc_method* inputEncData
) {
  this->initializeCommon(
    inputName,
    inputVersion,
    inputFlags,
    inputMask,
    acceptName,
    s_accept,
    connectName,
    s_connect,
    inputEncData
  );
  this->initializeTLSCommon();
  return this;
}

ssl_method_st* ssl_method_st::initialize_D_TLS(
  const std::string& inputName,
  int inputVersion,
  unsigned inputFlags,
  unsigned inputMask,
  const std::string& acceptName,
  int (s_accept)(SSL *s, std::stringstream* commentsOnError),
  const std::string& connectName,
  int (s_connect)(SSL *s, std::stringstream* commentsOnError),
  const ssl3_enc_method* inputEncData
) {
  this->initializeCommon(
    inputName,
    inputVersion,
    inputFlags,
    inputMask,
    acceptName,
    s_accept,
    connectName,
    s_connect,
    inputEncData
  );
  this->initialize_D_TLSCommon();
  return this;
}

ssl_method_st* ssl_method_st::initializeCommon(
  const std::string& inputName,
  int inputVersion,
  unsigned inputFlags,
  unsigned inputMask,
  const std::string& acceptName,
  int (s_accept)(SSL *s, std::stringstream* commentsOnError),
  const std::string& connectName,
  int (s_connect)(SSL *s, std::stringstream* commentsOnError),
  const ssl3_enc_method* inputEncData
) {
  this->name = inputName;
  this->version = inputVersion;
  this->flags = inputFlags;
  this->mask = inputMask;
  this->sslAccept.initialize(acceptName, s_accept);
  this->sslConnect.initialize(connectName, s_connect);
  this->ssl3_enc = inputEncData;
  return this;
}

long ssl3_ctx_callback_ctrl(SSL_CTX *ctx, int cmd, void (*fp) (void));
long ssl3_callback_ctrl(SSL *s, int cmd, void (*fp) (void));

void ssl_method_st::initialize_D_TLSCommon() {
  this->ssl_new               = dtls1_new;
  this->ssl_clear             = dtls1_clear;
  this->ssl_free              = dtls1_free;
  this->ssl_read              = ssl3_read;
  this->ssl_peek              = ssl3_peek;
  this->ssl_write             = ssl3_write;
  this->ssl_shutdown          = dtls1_shutdown;
  this->ssl_renegotiate       = ssl3_renegotiate;
  this->ssl_renegotiate_check = ssl3_renegotiate_check;
  this->ssl_read_bytes        = dtls1_read_bytes;
  this->ssl_write_bytes       = dtls1_write_app_data_bytes;
  this->ssl_dispatch_alert    = dtls1_dispatch_alert;
  this->ssl_ctrl              = dtls1_ctrl;
  this->ssl_ctx_ctrl          = ssl3_ctx_ctrl;
  this->get_cipher_by_char    = ssl3_get_cipher_by_char;
  this->put_cipher_by_char    = ssl3_put_cipher_by_char;
  this->ssl_pending           = ssl3_pending;
  this->num_ciphers           = ssl3_num_ciphers;
  this->get_cipher            = ssl3_get_cipher;
  this->get_timeout           = dtls1_default_timeout;
  this->ssl_version           = ssl_undefined_void_function;
  this->ssl_callback_ctrl     = ssl3_callback_ctrl;
  this->ssl_ctx_callback_ctrl = ssl3_ctx_callback_ctrl;
}

void ssl_method_st::initializeTLSCommon() {
  this->ssl_new               = tls1_new;
  this->ssl_clear             = tls1_clear;
  this->ssl_free              = tls1_free;
  this->ssl_read              = ssl3_read;
  this->ssl_peek              = ssl3_peek;
  this->ssl_write             = ssl3_write;
  this->ssl_shutdown          = ssl3_shutdown;
  this->ssl_renegotiate       = ssl3_renegotiate;
  this->ssl_renegotiate_check = ssl3_renegotiate_check;
  this->ssl_read_bytes        = ssl3_read_bytes;
  this->ssl_write_bytes       = ssl3_write_bytes;
  this->ssl_dispatch_alert    = ssl3_dispatch_alert;
  this->ssl_ctrl              = ssl3_ctrl;
  this->ssl_ctx_ctrl          = ssl3_ctx_ctrl;
  this->get_cipher_by_char    = ssl3_get_cipher_by_char;
  this->put_cipher_by_char    = ssl3_put_cipher_by_char;
  this->ssl_pending           = ssl3_pending;
  this->num_ciphers           = ssl3_num_ciphers;
  this->get_cipher            = ssl3_get_cipher;
  this->get_timeout           = tls1_default_timeout;
  this->ssl_version           = ssl_undefined_void_function;
  this->ssl_callback_ctrl     = ssl3_callback_ctrl;
  this->ssl_ctx_callback_ctrl = ssl3_ctx_callback_ctrl;
}

ssl_method_st* ssl_method_st::initializeSSL(
  const std::string& inputName,
  const std::string& acceptName,
  int (s_accept)(SSL *s, std::stringstream* commentsOnError),
  const std::string& connectName,
  int (s_connect)(SSL *s, std::stringstream* commentsOnError)
) {
  this->initializeCommon(
    inputName,
    SSL3_VERSION,
    SSL_METHOD_NO_FIPS | SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_SSLv3,
    acceptName,
    s_accept,
    connectName,
    s_connect,
    &SSLv3_enc_data
  );
  this->initializeSSLCommon();
  return this;
}

void ssl_method_st::initializeSSLCommon() {
  this->ssl_new               =  tls1_new;
  this->ssl_clear             =  tls1_clear;
  this->ssl_free              =  tls1_free;
  this->ssl_read              =  ssl3_read;
  this->ssl_peek              =  ssl3_peek;
  this->ssl_write             =  ssl3_write;
  this->ssl_shutdown          =  ssl3_shutdown;
  this->ssl_renegotiate       =  ssl3_renegotiate;
  this->ssl_renegotiate_check =  ssl3_renegotiate_check;
  this->ssl_read_bytes        =  ssl3_read_bytes;
  this->ssl_write_bytes       =  ssl3_write_bytes;
  this->ssl_dispatch_alert    =  ssl3_dispatch_alert;
  this->ssl_ctrl              =  ssl3_ctrl;
  this->ssl_ctx_ctrl          =  ssl3_ctx_ctrl;
  this->get_cipher_by_char    =  ssl3_get_cipher_by_char;
  this->put_cipher_by_char    =  ssl3_put_cipher_by_char;
  this->ssl_pending           =  ssl3_pending;
  this->num_ciphers           =  ssl3_num_ciphers;
  this->get_cipher            =  ssl3_get_cipher;
  this->get_timeout           =  tls1_default_timeout;
  this->ssl_version           =  ssl_undefined_void_function;
  this->ssl_callback_ctrl     =  ssl3_callback_ctrl;
  this->ssl_ctx_callback_ctrl =  ssl3_ctx_callback_ctrl;
}


const SSL_METHOD *TLS_method() {
  static SSL_METHOD TLS_method_data;

  TLS_method_data.initializeTLS(
    "TLS method",
    TLS_ANY_VERSION,
    0,
    0,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_2_enc_data
  );

  return &TLS_method_data;
}

const ssl_method_st* tlsv1_3_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_3_VERSION",
    TLS1_3_VERSION, 0,
    SSL_OP_NO_TLSv1_3,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_3_enc_data
  );
}

const ssl_method_st* tlsv1_2_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_2_VERSION",
    TLS1_2_VERSION,
    0,
    SSL_OP_NO_TLSv1_2,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_2_enc_data
  );
}

const ssl_method_st* tlsv1_1_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_1_VERSION",
    TLS1_1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_TLSv1_1,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_1_enc_data
  );
}

const ssl_method_st* tlsv1_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_VERSION",
    TLS1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_TLSv1,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_enc_data
  );
}

const ssl_method_st* TLS_server_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS_ANY_VERSION",
    TLS_ANY_VERSION,
    0,
    0,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &TLSv1_2_enc_data
  );
}

const ssl_method_st* tlsv1_3_server_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_3_VERSION",
    TLS1_3_VERSION,
    0,
    SSL_OP_NO_TLSv1_3,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &TLSv1_3_enc_data
  );
}

const ssl_method_st* tlsv1_2_server_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_2_VERSION",
    TLS1_2_VERSION,
    0,
    SSL_OP_NO_TLSv1_2,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &TLSv1_2_enc_data
  );
}


const ssl_method_st* tlsv1_1_server_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_1_VERSION",
    TLS1_1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_TLSv1_1,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &TLSv1_1_enc_data
  );
}

const ssl_method_st* tlsv1_server_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_VERSION",
    TLS1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_TLSv1,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &TLSv1_enc_data
  );
}

const ssl_method_st* sslv3_server_method() {
  static ssl_method_st result;
  return result.initializeSSL(
    "sslv3_server_method",
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function
  );
}


const ssl_method_st* sslv3_method() {
  static ssl_method_st result;
  return result.initializeSSL(
    "sslv3_method",
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect
  );
}

const ssl_method_st* TLS_client_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS_ANY_VERSION",
    TLS_ANY_VERSION,
    0,
    0,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_2_enc_data
  );
}

const ssl_method_st* tlsv1_3_client_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_3_VERSION",
    TLS1_3_VERSION,
    0,
    SSL_OP_NO_TLSv1_3,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_3_enc_data
  );
}

const ssl_method_st* tlsv1_2_client_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_2_VERSION",
    TLS1_2_VERSION,
    0,
    SSL_OP_NO_TLSv1_2,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_2_enc_data
  );
}

const ssl_method_st* tlsv1_1_client_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_1_VERSION",
    TLS1_1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_TLSv1_1,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_1_enc_data
  );
}

const ssl_method_st* tlsv1_client_method() {
  static ssl_method_st result;
  return result.initializeTLS(
    "TLS1_VERSION",
    TLS1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_TLSv1,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &TLSv1_enc_data
  );
}

const ssl_method_st* sslv3_client_method() {
  static ssl_method_st result;
  return result.initializeSSL(
    "sslv3_client_method",
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect
  );
}

const ssl_method_st* dtlsv1_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_VERSION",
    DTLS1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_DTLSv1,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_enc_data
  );
}

const ssl_method_st* dtlsv1_2_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_2_VERSION",
    DTLS1_2_VERSION,
    0,
    SSL_OP_NO_DTLSv1_2,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_2_enc_data
  );
}

const ssl_method_st* DTLS_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS_ANY_VERSION",
    DTLS_ANY_VERSION,
    0,
    0,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_2_enc_data
  );
}

const ssl_method_st* dtlsv1_server_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_VERSION",
    DTLS1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_DTLSv1,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &DTLSv1_enc_data
  );
}

const ssl_method_st* dtlsv1_2_server_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_2_VERSION",
    DTLS1_2_VERSION,
    0,
    SSL_OP_NO_DTLSv1_2,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &DTLSv1_2_enc_data
  );
}

const ssl_method_st* DTLS_server_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS_ANY_VERSION",
    DTLS_ANY_VERSION,
    0,
    0,
    "ossl_statem_accept",
    ossl_statem_accept,
    "ssl_undefined_function",
    ssl_undefined_function,
    &DTLSv1_2_enc_data
  );
}

const ssl_method_st* dtlsv1_client_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_VERSION",
    DTLS1_VERSION,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_DTLSv1,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_enc_data
  );
}

const ssl_method_st* dtls_bad_ver_client_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_BAD_VER",
    DTLS1_BAD_VER,
    SSL_METHOD_NO_SUITEB,
    SSL_OP_NO_DTLSv1,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_enc_data
  );
}

const ssl_method_st* dtlsv1_2_client_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS1_2_VERSION",
    DTLS1_2_VERSION,
    0,
    SSL_OP_NO_DTLSv1_2,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_2_enc_data
  );
}

const ssl_method_st* DTLS_client_method() {
  static ssl_method_st result;
  return result.initialize_D_TLS(
    "DTLS_ANY_VERSION",
    DTLS_ANY_VERSION,
    0,
    0,
    "ssl_undefined_function",
    ssl_undefined_function,
    "ossl_statem_connect",
    ossl_statem_connect,
    &DTLSv1_2_enc_data
  );
}

const SSL_METHOD *TLSv1_2_method(void)
{
    return tlsv1_2_method();
}

const SSL_METHOD *TLSv1_2_server_method(void)
{
    return tlsv1_2_server_method();
}

const SSL_METHOD *TLSv1_2_client_method(void)
{
    return tlsv1_2_client_method();
}

const SSL_METHOD *TLSv1_1_method(void)
{
    return tlsv1_1_method();
}

const SSL_METHOD *TLSv1_1_server_method(void)
{
    return tlsv1_1_server_method();
}

const SSL_METHOD *TLSv1_1_client_method(void)
{
    return tlsv1_1_client_method();
}

const SSL_METHOD *TLSv1_method(void)
{
    return tlsv1_method();
}

const SSL_METHOD *TLSv1_server_method(void)
{
    return tlsv1_server_method();
}

const SSL_METHOD *TLSv1_client_method(void)
{
    return tlsv1_client_method();
}

const SSL_METHOD *SSLv3_method(void)
{
    return sslv3_method();
}

const SSL_METHOD *SSLv3_server_method(void)
{
    return sslv3_server_method();
}

const SSL_METHOD *SSLv3_client_method(void)
{
    return sslv3_client_method();
}

const SSL_METHOD *DTLSv1_2_method(void)
{
    return dtlsv1_2_method();
}

const SSL_METHOD *DTLSv1_2_server_method(void)
{
    return dtlsv1_2_server_method();
}

const SSL_METHOD *DTLSv1_2_client_method(void)
{
    return dtlsv1_2_client_method();
}

const SSL_METHOD *DTLSv1_method(void)
{
    return dtlsv1_method();
}

const SSL_METHOD *DTLSv1_server_method(void)
{
    return dtlsv1_server_method();
}

const SSL_METHOD *DTLSv1_client_method(void)
{
    return dtlsv1_client_method();
}
