/*
 * Copyright 1995-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "../include/internal/cryptlib_int.h"
#include "../../include/internal/err.h"
#include "../include/internal/err_int.h"
#include "../../include/openssl/err.h"
#include "../../include/openssl/crypto.h"
#include "../../include/openssl/buffer.h"
#include "../../include/openssl/bio.h"
#include "../../include/openssl/opensslconf.h"
#include "../../include/internal/thread_once.h"
#include "../include/internal/ctype.h"
#include "../../include/internal/constant_time_locl.h"
#include "../../e_os.h"

int load_crypto_strings_inited = 0;

static int err_load_strings(const ERR_STRING_DATA *str);

void ERR_STATE_free(err_state_st *s);
#ifndef OPENSSL_NO_ERR
static ERR_STRING_DATA ERR_str_libraries[] = {
    {ERR_PACK(ERR_LIB_NONE, 0, 0), "unknown library"},
    {ERR_PACK(ERR_LIB_SYS, 0, 0), "system library"},
    {ERR_PACK(ERR_LIB_BN, 0, 0), "bignum routines"},
    {ERR_PACK(ERR_LIB_RSA, 0, 0), "rsa routines"},
    {ERR_PACK(ERR_LIB_DH, 0, 0), "Diffie-Hellman routines"},
    {ERR_PACK(ERR_LIB_EVP, 0, 0), "digital envelope routines"},
    {ERR_PACK(ERR_LIB_BUF, 0, 0), "memory buffer routines"},
    {ERR_PACK(ERR_LIB_OBJ, 0, 0), "object identifier routines"},
    {ERR_PACK(ERR_LIB_PEM, 0, 0), "PEM routines"},
    {ERR_PACK(ERR_LIB_DSA, 0, 0), "dsa routines"},
    {ERR_PACK(ERR_LIB_X509, 0, 0), "x509 certificate routines"},
    {ERR_PACK(ERR_LIB_ASN1, 0, 0), "asn1 encoding routines"},
    {ERR_PACK(ERR_LIB_CONF, 0, 0), "configuration file routines"},
    {ERR_PACK(ERR_LIB_CRYPTO, 0, 0), "common libcrypto routines"},
    {ERR_PACK(ERR_LIB_EC, 0, 0), "elliptic curve routines"},
    {ERR_PACK(ERR_LIB_ECDSA, 0, 0), "ECDSA routines"},
    {ERR_PACK(ERR_LIB_ECDH, 0, 0), "ECDH routines"},
    {ERR_PACK(ERR_LIB_SSL, 0, 0), "SSL routines"},
    {ERR_PACK(ERR_LIB_BIO, 0, 0), "BIO routines"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, 0), "PKCS7 routines"},
    {ERR_PACK(ERR_LIB_X509V3, 0, 0), "X509 V3 routines"},
    {ERR_PACK(ERR_LIB_PKCS12, 0, 0), "PKCS12 routines"},
    {ERR_PACK(ERR_LIB_RAND, 0, 0), "random number generator"},
    {ERR_PACK(ERR_LIB_DSO, 0, 0), "DSO support routines"},
    {ERR_PACK(ERR_LIB_TS, 0, 0), "time stamp routines"},
    {ERR_PACK(ERR_LIB_ENGINE, 0, 0), "engine routines"},
    {ERR_PACK(ERR_LIB_OCSP, 0, 0), "OCSP routines"},
    {ERR_PACK(ERR_LIB_UI, 0, 0), "UI routines"},
    {ERR_PACK(ERR_LIB_FIPS, 0, 0), "FIPS routines"},
    {ERR_PACK(ERR_LIB_CMS, 0, 0), "CMS routines"},
    {ERR_PACK(ERR_LIB_CRMF, 0, 0), "CRMF routines"},
    {ERR_PACK(ERR_LIB_HMAC, 0, 0), "HMAC routines"},
    {ERR_PACK(ERR_LIB_CT, 0, 0), "CT routines"},
    {ERR_PACK(ERR_LIB_ASYNC, 0, 0), "ASYNC routines"},
    {ERR_PACK(ERR_LIB_KDF, 0, 0), "KDF routines"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, 0), "STORE routines"},
    {ERR_PACK(ERR_LIB_SM2, 0, 0), "SM2 routines"},
    {ERR_PACK(ERR_LIB_ESS, 0, 0), "ESS routines"},
    {0, NULL},
};

static ERR_STRING_DATA ERR_str_functs[] = {
    {ERR_PACK(0, SYS_F_FOPEN, 0), "fopen"},
    {ERR_PACK(0, SYS_F_CONNECT, 0), "connect"},
    {ERR_PACK(0, SYS_F_GETSERVBYNAME, 0), "getservbyname"},
    {ERR_PACK(0, SYS_F_SOCKET, 0), "socket"},
    {ERR_PACK(0, SYS_F_IOCTLSOCKET, 0), "ioctlsocket"},
    {ERR_PACK(0, SYS_F_BIND, 0), "bind"},
    {ERR_PACK(0, SYS_F_LISTEN, 0), "listen"},
    {ERR_PACK(0, SYS_F_ACCEPT, 0), "accept"},
# ifdef OPENSSL_SYS_WINDOWS
    {ERR_PACK(0, SYS_F_WSASTARTUP, 0), "WSAstartup"},
# endif
    {ERR_PACK(0, SYS_F_OPENDIR, 0), "opendir"},
    {ERR_PACK(0, SYS_F_FREAD, 0), "fread"},
    {ERR_PACK(0, SYS_F_GETADDRINFO, 0), "getaddrinfo"},
    {ERR_PACK(0, SYS_F_GETNAMEINFO, 0), "getnameinfo"},
    {ERR_PACK(0, SYS_F_SETSOCKOPT, 0), "setsockopt"},
    {ERR_PACK(0, SYS_F_GETSOCKOPT, 0), "getsockopt"},
    {ERR_PACK(0, SYS_F_GETSOCKNAME, 0), "getsockname"},
    {ERR_PACK(0, SYS_F_GETHOSTBYNAME, 0), "gethostbyname"},
    {ERR_PACK(0, SYS_F_FFLUSH, 0), "fflush"},
    {ERR_PACK(0, SYS_F_OPEN, 0), "open"},
    {ERR_PACK(0, SYS_F_CLOSE, 0), "close"},
    {ERR_PACK(0, SYS_F_IOCTL, 0), "ioctl"},
    {ERR_PACK(0, SYS_F_STAT, 0), "stat"},
    {ERR_PACK(0, SYS_F_FCNTL, 0), "fcntl"},
    {ERR_PACK(0, SYS_F_FSTAT, 0), "fstat"},
    {0, NULL},
};

static ERR_STRING_DATA ERR_str_reasons[] = {
    {ERR_R_SYS_LIB, "system lib"},
    {ERR_R_BN_LIB, "BN lib"},
    {ERR_R_RSA_LIB, "RSA lib"},
    {ERR_R_DH_LIB, "DH lib"},
    {ERR_R_EVP_LIB, "EVP lib"},
    {ERR_R_BUF_LIB, "BUF lib"},
    {ERR_R_OBJ_LIB, "OBJ lib"},
    {ERR_R_PEM_LIB, "PEM lib"},
    {ERR_R_DSA_LIB, "DSA lib"},
    {ERR_R_X509_LIB, "X509 lib"},
    {ERR_R_ASN1_LIB, "ASN1 lib"},
    {ERR_R_EC_LIB, "EC lib"},
    {ERR_R_BIO_LIB, "BIO lib"},
    {ERR_R_PKCS7_LIB, "PKCS7 lib"},
    {ERR_R_X509V3_LIB, "X509V3 lib"},
    {ERR_R_ENGINE_LIB, "ENGINE lib"},
    {ERR_R_UI_LIB, "UI lib"},
    {ERR_R_OSSL_STORE_LIB, "STORE lib"},
    {ERR_R_ECDSA_LIB, "ECDSA lib"},

    {ERR_R_NESTED_ASN1_ERROR, "nested asn1 error"},
    {ERR_R_MISSING_ASN1_EOS, "missing asn1 eos"},

    {ERR_R_FATAL, "fatal"},
    {ERR_R_MALLOC_FAILURE, "malloc failure"},
    {ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED,
     "called a function you should not call"},
    {ERR_R_PASSED_NULL_PARAMETER, "passed a null parameter"},
    {ERR_R_INTERNAL_ERROR, "internal error"},
    {ERR_R_DISABLED, "called a function that was disabled at compile-time"},
    {ERR_R_INIT_FAIL, "init fail"},
    {ERR_R_OPERATION_FAIL, "operation fail"},

    {0, NULL},
};
#endif

static CRYPTO_ONCE err_init = CRYPTO_ONCE_STATIC_INIT;
static int set_err_thread_local;
static CRYPTO_THREAD_LOCAL err_thread_local;

static CRYPTO_ONCE err_string_init = CRYPTO_ONCE_STATIC_INIT;
static CRYPTO_RWLOCK *err_string_lock;

static ERR_STRING_DATA *int_err_get_item(const ERR_STRING_DATA *);

/*
 * The internal state
 */

static LHASH_OF(ERR_STRING_DATA) *int_error_hash = NULL;
static int int_err_library_number = ERR_LIB_USER;

static unsigned long get_error_values(
  int inc,
  int top,
  std::string* outputFile,
  int* outputLine,
  std::string* outputData,
  int* outputFlags
);

static unsigned long err_string_data_hash(const ERR_STRING_DATA *a)
{
    unsigned long ret, l;

    l = a->error;
    ret = l ^ ERR_GET_LIB(l) ^ ERR_GET_FUNC(l);
    return (ret ^ ret % 19 * 13);
}

static int err_string_data_cmp(const ERR_STRING_DATA *a,
                               const ERR_STRING_DATA *b)
{
    if (a->error == b->error)
        return 0;
    return a->error > b->error ? 1 : -1;
}

static ERR_STRING_DATA *int_err_get_item(const ERR_STRING_DATA *d)
{
    ERR_STRING_DATA *p = NULL;

    p = lh_ERR_STRING_DATA_retrieve(int_error_hash, d);

    return p;
}

#ifndef OPENSSL_NO_ERR
/* A measurement on Linux 2018-11-21 showed about 3.5kib */
# define SPACE_SYS_STR_REASONS 4 * 1024
# define NUM_SYS_STR_REASONS 127

static ERR_STRING_DATA SYS_str_reasons[NUM_SYS_STR_REASONS + 1];
/*
 * SYS_str_reasons is filled with copies of strerror() results at
 * initialization. 'errno' values up to 127 should cover all usual errors,
 * others will be displayed numerically by ERR_error_string. It is crucial
 * that we have something for each reason code that occurs in
 * ERR_str_reasons, or bogus reason strings will be returned for SYSerr(),
 * which always gets an errno value and never one of those 'standard' reason
 * codes.
 */

static void build_SYS_str_reasons(void)
{
    /* OPENSSL_malloc cannot be used here, use static storage instead */
    static char strerror_pool[SPACE_SYS_STR_REASONS];
    char *cur = strerror_pool;
    size_t cnt = 0;
    static int init = 1;
    int i;
    int saveerrno = get_last_sys_error();

    if (!init) {
        return;
    }

    for (i = 1; i <= NUM_SYS_STR_REASONS; i++) {
        ERR_STRING_DATA *str = &SYS_str_reasons[i - 1];

        str->error = ERR_PACK(ERR_LIB_SYS, 0, i);
        if (str->string == NULL) {
            if (openssl_strerror_r(i, cur, sizeof(strerror_pool) - cnt)) {
                size_t l = strlen(cur);

                str->string = cur;
                cnt += l;
                if (cnt > sizeof(strerror_pool))
                    cnt = sizeof(strerror_pool);
                cur += l;

                /*
                 * VMS has an unusual quirk of adding spaces at the end of
                 * some (most? all?) messages.  Lets trim them off.
                 */
                while (ossl_isspace(cur[-1])) {
                    cur--;
                    cnt--;
                }
                *cur++ = '\0';
                cnt++;
            }
        }
        if (str->string == NULL)
            str->string = "unknown";
    }

    /*
     * Now we still have SYS_str_reasons[NUM_SYS_STR_REASONS] = {0, NULL}, as
     * required by ERR_load_strings.
     */

    init = 0;

    CRYPTO_THREAD_unlock(err_string_lock);
    /* openssl_strerror_r could change errno, but we want to preserve it */
    set_sys_error(saveerrno);
    err_load_strings(SYS_str_reasons);
}
#endif

err_state_st& err_state_st::GetLastError() {
  static err_state_st globalLastError;
  return globalLastError;
}

err_state_st::err_state_st() {
  this->err_buffer = 0;
  this->err_data = "";
  this->err_data_flags = 0;
  this->err_file = "";
  this->err_flags = 0;
  this->err_line = 0;
}

err_state_st::~err_state_st() {
  this->Free();
}

void err_state_st::Free() {
  this->err_data = "";
  this->err_data_flags = 0;
  this->err_flags = 0;
  this->err_buffer = 0;
  this->err_file = "";
  this->err_line = - 1;
}

void ERR_STATE_free(err_state_st *s) {
  s->Free();
}

DEFINE_RUN_ONCE_STATIC(do_err_strings_init)
{
    if (!OPENSSL_init_crypto(0, NULL, (std::stringstream*) commentsOnError))
        return 0;
    err_string_lock = CRYPTO_THREAD_lock_new();
    if (err_string_lock == NULL)
        return 0;
    int_error_hash = lh_ERR_STRING_DATA_new(err_string_data_hash,
                                            err_string_data_cmp);
    if (int_error_hash == NULL) {
        CRYPTO_THREAD_lock_free(err_string_lock);
        err_string_lock = NULL;
        return 0;
    }
    return 1;
}

void err_cleanup(void)
{
    if (set_err_thread_local != 0)
        CRYPTO_THREAD_cleanup_local(&err_thread_local);
    CRYPTO_THREAD_lock_free(err_string_lock);
    err_string_lock = NULL;
    lh_ERR_STRING_DATA_free(int_error_hash);
    int_error_hash = NULL;
}

/*
 * Legacy; pack in the library.
 */
static void err_patch(int lib, ERR_STRING_DATA *str)
{
    unsigned long plib = ERR_PACK(lib, 0, 0);

    for (; str->error != 0; str++)
        str->error |= plib;
}

/*
 * Hash in |str| error strings. Assumes the URN_ONCE was done.
 */
static int err_load_strings(const ERR_STRING_DATA *str)
{
    CRYPTO_THREAD_write_lock(err_string_lock);
    for (; str->error; str++)
        (void)lh_ERR_STRING_DATA_insert(int_error_hash,
                                       (ERR_STRING_DATA *)str);
    CRYPTO_THREAD_unlock(err_string_lock);
    return 1;
}

int ERR_load_ERR_strings(std::stringstream *commentsOnError) {
    if (!RUN_ONCE(&err_string_init, do_err_strings_init, commentsOnError)) {
        return 0;
    }

    err_load_strings(ERR_str_libraries);
    err_load_strings(ERR_str_reasons);
    err_patch(ERR_LIB_SYS, ERR_str_functs);
    err_load_strings(ERR_str_functs);
    build_SYS_str_reasons();
    return 1;
}

int ERR_load_strings(int lib, ERR_STRING_DATA *str)
{
    if (ERR_load_ERR_strings(0) == 0)
        return 0;

    err_patch(lib, str);
    err_load_strings(str);
    return 1;
}

int ERR_load_strings_const(const ERR_STRING_DATA *str)
{
    if (ERR_load_ERR_strings(0) == 0)
        return 0;
    err_load_strings(str);
    return 1;
}

int ERR_unload_strings(int lib, ERR_STRING_DATA *str)
{
    if (!RUN_ONCE(&err_string_init, do_err_strings_init, 0))
        return 0;

    CRYPTO_THREAD_write_lock(err_string_lock);
    /*
     * We don't need to ERR_PACK the lib, since that was done (to
     * the table) when it was loaded.
     */
    for (; str->error; str++)
        (void)lh_ERR_STRING_DATA_delete(int_error_hash, str);
    CRYPTO_THREAD_unlock(err_string_lock);

    return 1;
}

void err_free_strings_int(void)
{
  if (load_crypto_strings_inited) {
    load_crypto_strings_inited = 0;
  }
    if (!RUN_ONCE(&err_string_init, do_err_strings_init, 0))
        return;
}


int SetError(int lib, int func) {
  return 0;
}
/********************************************************/

void err_state_st::SetError(int lib, int func, int reason, const char *file, int line) {
  this->err_flags = 0;
  this->err_buffer = ERR_PACK(lib, func, reason);
  this->err_file = file;
  this->err_line = line;
}

void ERR_put_error(int lib, int func, int reason, const char *file, int line) {
  err_state_st& globalError = err_state_st::GetLastError();
  globalError.Free();
  globalError.SetError(lib, func, reason, file, line);
}

void ERR_clear_error() {
  err_state_st::GetLastError().Free();
}

unsigned long ERR_get_error(void)
{
    return get_error_values(1, 0, NULL, NULL, NULL, NULL);
}

unsigned long ERR_get_error_line(std::string* outputFile, int* outputLine) {
  return get_error_values(1, 0, outputFile, outputLine, NULL, NULL);
}

unsigned long ERR_get_error_line_data(
  std::string *outputFile, int *line, std::string *outputData, int *flags
) {
  return get_error_values(1, 0, outputFile, line, outputData, flags);
}

unsigned long ERR_peek_error(void)
{
    return get_error_values(0, 0, NULL, NULL, NULL, NULL);
}

unsigned long ERR_peek_error_line(std::string *outputFile, int *outputLine) {
  return get_error_values(0, 0, outputFile, outputLine, NULL, NULL);
}

unsigned long ERR_peek_error_line_data(
  std::string *outputFile, int *outputLine, std::string *outputData, int *flags
) {
  return get_error_values(0, 0, outputFile, outputLine, outputData, flags);
}

unsigned long ERR_peek_last_error(void)
{
    return get_error_values(0, 1, NULL, NULL, NULL, NULL);
}

unsigned long ERR_peek_last_error_line(std::string *outputFile, int *outputLine) {
  return get_error_values(0, 1, outputFile, outputLine, NULL, NULL);
}

unsigned long ERR_peek_last_error_line_data(
  std::string *outputFile,
  int *line,
  std::string *outputData,
  int *flags
) {
    return get_error_values(0, 1, outputFile, line, outputData, flags);
}

static unsigned long get_error_values(
  int inc,
  int top,
  std::string* outputFile,
  int* outputLine,
  std::string* outputData,
  int* outputFlags
) {
  (void) top;
  err_state_st *es;
  es = ERR_get_state();
  return es->getErrorValues(inc, outputFile, outputLine, outputData, outputFlags);
}


unsigned long err_state_st::getErrorValues(
  int inc,
  std::string* outputFile,
  int* outputLine,
  std::string* outputData,
  int* outputFlags
) {
  if (this->err_buffer == 0) {
    return 0;
  }
  unsigned long result;
  result = this->err_buffer;
  if (outputFile != 0) {
    *outputFile = this->err_file;
  }
  if (outputLine != 0) {
    *outputLine = this->err_line;
  }
  if (outputData != 0) {
    *outputData = this->err_data;
  }
  if (outputFlags != 0) {
    *outputFlags = this->err_flags;
  }
  if (inc != 0) {
    this->Free();
  }
  return result;
}

void ERR_error_string_n(unsigned long e, char *buf, size_t len)
{
    char lsbuf[64], fsbuf[64], rsbuf[64];
    const char *ls, *fs, *rs;
    unsigned long l, f, r;

    if (len == 0)
        return;

    l = ERR_GET_LIB(e);
    ls = ERR_lib_error_string(e);
    if (ls == NULL) {
        BIO_snprintf(lsbuf, sizeof(lsbuf), "lib(%lu)", l);
        ls = lsbuf;
    }

    fs = ERR_func_error_string(e);
    f = ERR_GET_FUNC(e);
    if (fs == NULL) {
        BIO_snprintf(fsbuf, sizeof(fsbuf), "func(%lu)", f);
        fs = fsbuf;
    }

    rs = ERR_reason_error_string(e);
    r = ERR_GET_REASON(e);
    if (rs == NULL) {
        BIO_snprintf(rsbuf, sizeof(rsbuf), "reason(%lu)", r);
        rs = rsbuf;
    }

    BIO_snprintf(buf, len, "error:%08lX:%s:%s:%s", e, ls, fs, rs);
    if (strlen(buf) == len - 1) {
        /* Didn't fit; use a minimal format. */
        BIO_snprintf(buf, len, "err:%lx:%lx:%lx:%lx", e, l, f, r);
    }
}

/*
 * ERR_error_string_n should be used instead for ret != NULL as
 * ERR_error_string cannot know how large the buffer is
 */
char *ERR_error_string(unsigned long e, char *ret)
{
    static char buf[256];

    if (ret == NULL)
        ret = buf;
    ERR_error_string_n(e, ret, (int)sizeof(buf));
    return ret;
}

const char *ERR_lib_error_string(unsigned long e)
{
    ERR_STRING_DATA d, *p;
    unsigned long l;

    if (!RUN_ONCE(&err_string_init, do_err_strings_init, 0)) {
        return NULL;
    }

    l = ERR_GET_LIB(e);
    d.error = ERR_PACK(l, 0, 0);
    p = int_err_get_item(&d);
    return ((p == NULL) ? NULL : p->string);
}

const char *ERR_func_error_string(unsigned long e)
{
    ERR_STRING_DATA d, *p;
    unsigned long l, f;

    if (!RUN_ONCE(&err_string_init, do_err_strings_init, 0)) {
        return NULL;
    }

    l = ERR_GET_LIB(e);
    f = ERR_GET_FUNC(e);
    d.error = ERR_PACK(l, f, 0);
    p = int_err_get_item(&d);
    return ((p == NULL) ? NULL : p->string);
}

const char *ERR_reason_error_string(unsigned long e)
{
    ERR_STRING_DATA d, *p = NULL;
    unsigned long l, r;

    if (!RUN_ONCE(&err_string_init, do_err_strings_init, 0)) {
        return NULL;
    }

    l = ERR_GET_LIB(e);
    r = ERR_GET_REASON(e);
    d.error = ERR_PACK(l, 0, r);
    p = int_err_get_item(&d);
    if (!p) {
        d.error = ERR_PACK(0, 0, r);
        p = int_err_get_item(&d);
    }
    return ((p == NULL) ? NULL : p->string);
}

void err_delete_thread_state(void) {
  err_state_st *state = (err_state_st *) CRYPTO_THREAD_get_local(&err_thread_local);
    if (state == NULL)
        return;

    CRYPTO_THREAD_set_local(&err_thread_local, NULL);
    ERR_STATE_free(state);
}

#if !OPENSSL_API_1_1_0
void ERR_remove_thread_state(void *dummy)
{
}
#endif

#if !OPENSSL_API_1_0_0
void ERR_remove_state(unsigned long pid)
{
}
#endif

DEFINE_RUN_ONCE_STATIC(err_do_init)
{
    set_err_thread_local = 1;
    return CRYPTO_THREAD_init_local(&err_thread_local, NULL);
}

err_state_st* ERR_get_state() {
  static err_state_st state;
  return &state;
}

/*
 * err_shelve_state returns the current thread local error state
 * and freezes the error module until err_unshelve_state is called.
 */
int err_shelve_state(void **state)
{
    int saveerrno = get_last_sys_error();


    if (!RUN_ONCE(&err_init, err_do_init, 0))
        return 0;

    *state = CRYPTO_THREAD_get_local(&err_thread_local);
  if (!CRYPTO_THREAD_set_local(&err_thread_local, (err_state_st*) - 1)) {
    return 0;
  }

    set_sys_error(saveerrno);
    return 1;
}

/*
 * err_unshelve_state restores the error state that was returned
 * by err_shelve_state previously.
 */
void err_unshelve_state(void* state) {
  if (state != (void*) - 1) {
    CRYPTO_THREAD_set_local(&err_thread_local, (err_state_st*) state);
  }
}

int ERR_get_next_error_library()
{
    int ret;

    if (!RUN_ONCE(&err_string_init, do_err_strings_init, 0))
        return 0;

    CRYPTO_THREAD_write_lock(err_string_lock);
    ret = int_err_library_number++;
    CRYPTO_THREAD_unlock(err_string_lock);
    return ret;
}

static int err_set_error_data_int(char *data, int flags) {
  err_state_st *es;
  es = ERR_get_state();
  if (es == NULL) {
    return 0;
  }
  es->Free();
  es->err_data.assign(data);
  es->err_data_flags = flags;
  return 1;
}

void ERR_set_error_data(char *data, int flags)
{
    /*
     * This function is void so we cannot propagate the error return. Since it
     * is also in the public API we can't change the return type.
     */
    err_set_error_data_int(data, flags);
}

void ERR_add_error_data(int num, ...)
{
    va_list args;
    va_start(args, num);
    ERR_add_error_vdata(num, args);
    va_end(args);
}

void ERR_add_error_vdata(int num, va_list args)
{
    int i, n, s;
    char *str, *p, *a;

    s = 80;
    if ((str = (char*) OPENSSL_malloc(s + 1)) == NULL) {
        /* ERRerr(ERR_F_ERR_ADD_ERROR_VDATA, ERR_R_MALLOC_FAILURE); */
        return;
    }
    str[0] = '\0';

    n = 0;
    for (i = 0; i < num; i++) {
        a = va_arg(args, char *);
        if (a == NULL)
            a = "<NULL>";
        n += strlen(a);
        if (n > s) {
            s = n + 20;
            p = (char*) OPENSSL_realloc(str, s + 1);
            if (p == NULL) {
                OPENSSL_free(str);
                return;
            }
            str = p;
        }
        OPENSSL_strlcat(str, a, (size_t)s + 1);
    }
    if (!err_set_error_data_int(str, ERR_TXT_MALLOCED | ERR_TXT_STRING))
        OPENSSL_free(str);
}

int ERR_set_mark() {
  err_state_st *es = ERR_get_state();
  es->err_flags |= ERR_FLAG_MARK;
  return 1;
}

int ERR_pop_to_mark() {
  err_state_st *es = ERR_get_state();
  es->err_flags &= ~ERR_FLAG_MARK;
  return 1;
}

int ERR_clear_last_mark(void) {
  err_state_st *es = ERR_get_state();
  es->err_flags &= ~ERR_FLAG_MARK;
  return 1;
}

void err_clear_last_constant_time(int clear) {
  err_state_st *es = ERR_get_state();
  /*
   * Flag error as cleared but remove it elsewhere to avoid two errors
   * accessing the same error stack location, revealing timing information.
   */
  es->err_flags |= clear;
}
