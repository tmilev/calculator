/*
 * Copyright 1995-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "../../include/internal/cryptlib.h"
#include "../../include/openssl/evp.h"
#include "../include/internal/objects.h"
#include "../../include/openssl/x509.h"
#include "../include/internal/evp_int.h"

int EVP_add_cipher(const EVP_CIPHER *c)
{
    int r;

    if (c == NULL)
        return 0;

    r = OBJ_NAME_add(OBJ_nid2sn(c->nid), OBJ_NAME_TYPE_CIPHER_METH,
                     (const char *)c);
    if (r == 0)
        return 0;
    r = OBJ_NAME_add(OBJ_nid2ln(c->nid), OBJ_NAME_TYPE_CIPHER_METH,
                     (const char *)c);
    return r;
}

int EVP_add_digest(const EVP_MD *md)
{
    int r;
    const char *name;

    name = OBJ_nid2sn(md->type);
    r = OBJ_NAME_add(name, OBJ_NAME_TYPE_MD_METH, (const char *)md);
    if (r == 0)
        return 0;
    r = OBJ_NAME_add(OBJ_nid2ln(md->type), OBJ_NAME_TYPE_MD_METH,
                     (const char *)md);
    if (r == 0)
        return 0;

    if (md->pkey_type && md->type != md->pkey_type) {
        r = OBJ_NAME_add(OBJ_nid2sn(md->pkey_type),
                         OBJ_NAME_TYPE_MD_METH | OBJ_NAME_ALIAS, name);
        if (r == 0)
            return 0;
        r = OBJ_NAME_add(OBJ_nid2ln(md->pkey_type),
                         OBJ_NAME_TYPE_MD_METH | OBJ_NAME_ALIAS, name);
    }
    return r;
}

int EVP_add_mac(const evp_mac_st *m)
{
    int r;

    if (m == NULL)
        return 0;

    r = OBJ_NAME_add(OBJ_nid2sn(m->type), OBJ_NAME_TYPE_MAC_METH,
                     (const char *)m);
    if (r == 0)
        return 0;
    r = OBJ_NAME_add(OBJ_nid2ln(m->type), OBJ_NAME_TYPE_MAC_METH,
                     (const char *)m);
    return r;
}

const EVP_MD* EVP_get_digestbynid(int nid, std::stringstream* commentsOnError) {
  return EVP_get_digestbyname(OBJ_nid2sn(nid), commentsOnError);
}

const EVP_MD* EVP_get_digestbyobj(const ASN1_OBJECT* obj, std::stringstream *commentsOnError) {
  return EVP_get_digestbynid(OBJ_obj2nid(obj), commentsOnError);
}

const EVP_CIPHER* EVP_get_cipherbyobj(ASN1_OBJECT * obj, std::stringstream* commentsOnError) {
  return EVP_get_cipherbynid(OBJ_obj2nid(obj), commentsOnError);
}

const EVP_CIPHER* EVP_get_cipherbynid(int nid, std::stringstream* commentsOnError) {
  return EVP_get_cipherbyname(OBJ_nid2sn(nid), commentsOnError);
}

const EVP_CIPHER *EVP_get_cipherbyname(const char *name, std::stringstream* commentsOnError) {
  const EVP_CIPHER *cp;
  if (!OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL, commentsOnError))
        return NULL;

    cp = (const EVP_CIPHER *)OBJ_NAME_get(name, OBJ_NAME_TYPE_CIPHER_METH);
    return cp;
}

const EVP_MD *EVP_get_digestbyname(const char *name, std::stringstream* commentsOnError) {
    const EVP_MD *cp;

    if (!OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_DIGESTS, NULL, commentsOnError))
        return NULL;

    cp = (const EVP_MD *)OBJ_NAME_get(name, OBJ_NAME_TYPE_MD_METH);
    return cp;
}

const evp_mac_st *EVP_get_macbyname(const char *name)
{
    const evp_mac_st *mp;

    if (!OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_MACS, NULL, 0))
        return NULL;

    mp = (const evp_mac_st *) OBJ_NAME_get(name, OBJ_NAME_TYPE_MAC_METH);
    return mp;
}

void evp_cleanup_int(void)
{
    OBJ_NAME_cleanup(OBJ_NAME_TYPE_MAC_METH);
    OBJ_NAME_cleanup(OBJ_NAME_TYPE_CIPHER_METH);
    OBJ_NAME_cleanup(OBJ_NAME_TYPE_MD_METH);
    /*
     * The above calls will only clean out the contents of the name hash
     * table, but not the hash table itself.  The following line does that
     * part.  -- Richard Levitte
     */
    OBJ_NAME_cleanup(-1);

    EVP_PBE_cleanup();
    OBJ_sigid_free();

    evp_app_cleanup_int();
}

struct doall_cipher {
    void *arg;
    void (*fn) (const EVP_CIPHER *ciph,
                const char *from, const char *to, void *arg);
};

static void do_all_cipher_fn(const OBJ_NAME *nm, void *arg)
{
    struct doall_cipher *dc = (doall_cipher *) arg;
    if (nm->alias)
        dc->fn(NULL, nm->name, nm->data, dc->arg);
    else
        dc->fn((const EVP_CIPHER *)nm->data, nm->name, NULL, dc->arg);
}

void EVP_CIPHER_do_all(void (*fn) (const EVP_CIPHER *ciph,
                                   const char *from, const char *to, void *x),
                       void *arg)
{
    struct doall_cipher dc;

    /* Ignore errors */
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL, 0);

    dc.fn = fn;
    dc.arg = arg;
    OBJ_NAME_do_all(OBJ_NAME_TYPE_CIPHER_METH, do_all_cipher_fn, &dc);
}

void EVP_CIPHER_do_all_sorted(void (*fn) (const EVP_CIPHER *ciph,
                                          const char *from, const char *to,
                                          void *x), void *arg)
{
    struct doall_cipher dc;

    /* Ignore errors */
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL, 0);

    dc.fn = fn;
    dc.arg = arg;
    OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH, do_all_cipher_fn, &dc);
}

struct doall_md {
    void *arg;
    void (*fn) (const EVP_MD *ciph,
                const char *from, const char *to, void *arg);
};

static void do_all_md_fn(const OBJ_NAME *nm, void *arg)
{
    struct doall_md *dc = (doall_md *) arg;
    if (nm->alias)
        dc->fn(NULL, nm->name, nm->data, dc->arg);
    else
        dc->fn((const EVP_MD *)nm->data, nm->name, NULL, dc->arg);
}

void EVP_MD_do_all(void (*fn) (const EVP_MD *md,
                               const char *from, const char *to, void *x),
                   void *arg)
{
    struct doall_md dc;

    /* Ignore errors */
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_DIGESTS, NULL, 0);

    dc.fn = fn;
    dc.arg = arg;
    OBJ_NAME_do_all(OBJ_NAME_TYPE_MD_METH, do_all_md_fn, &dc);
}

void EVP_MD_do_all_sorted(void (*fn) (const EVP_MD *md,
                                      const char *from, const char *to,
                                      void *x), void *arg)
{
    struct doall_md dc;

    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_DIGESTS, NULL, 0);

    dc.fn = fn;
    dc.arg = arg;
    OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_MD_METH, do_all_md_fn, &dc);
}

struct doall_mac {
    void *arg;
    void (*fn) (const evp_mac_st *ciph,
                const char *from, const char *to, void *arg);
};

void do_all_mac_fn(const OBJ_NAME *nm, void *arg)
{
    struct doall_mac *dc = (doall_mac *) arg;

    if (nm->alias)
        dc->fn(NULL, nm->name, nm->data, dc->arg);
    else
        dc->fn((const evp_mac_st *)nm->data, nm->name, NULL, dc->arg);
}

void EVP_MAC_do_all(void (*fn)(const evp_mac_st*, const char *, const char *, void *), void *arg)
{
    struct doall_mac dc;

    /* Ignore errors */
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_MACS, NULL, 0);

    dc.fn = fn;
    dc.arg = arg;
    OBJ_NAME_do_all(OBJ_NAME_TYPE_MAC_METH, do_all_mac_fn, &dc);
}

void EVP_MAC_do_all_sorted(void (*fn)(const evp_mac_st *, const char *, const char *, void *), void *arg)
{
    struct doall_mac dc;

    /* Ignore errors */
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_MACS, NULL, 0);

    dc.fn = fn;
    dc.arg = arg;
    OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_MAC_METH, do_all_mac_fn, &dc);
}
