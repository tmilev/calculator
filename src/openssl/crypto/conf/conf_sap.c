/*
 * Copyright 2002-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "../../include/openssl/crypto.h"
#include "../../include/internal/cryptlib.h"
#include "../../include/internal/conf.h"
#include "../../include/openssl/x509.h"
#include "../../include/openssl/asn1.h"
#include "../../include/openssl/engine.h"

#ifdef _WIN32
# define strdup _strdup
#endif

/*
 * This is the automatic configuration loader: it is called automatically by
 * OpenSSL when any of a number of standard initialisation functions are
 * called, unless this is overridden by calling OPENSSL_no_config()
 */

static int openssl_configured = 0;

void OPENSSL_config(const char *appname, std::stringstream* commentsOnError) {
  OPENSSL_INIT_SETTINGS settings;
  memset(&settings, 0, sizeof(settings));
  if (appname != NULL) {
    settings.appname = strdup(appname);
  }
  settings.flags = DEFAULT_CONF_MFLAGS;
  OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, &settings, commentsOnError);
}

int openssl_config_int(const OPENSSL_INIT_SETTINGS *settings, std::stringstream* commentsOnError)
{
    int ret;
    const char *filename;
    const char *appname;
    unsigned long flags;

    if (openssl_configured)
        return 1;

    filename = settings ? settings->filename : NULL;
    appname = settings ? settings->appname : NULL;
    flags = settings ? settings->flags : DEFAULT_CONF_MFLAGS;

#ifdef OPENSSL_INIT_DEBUG
    fprintf(stderr, "OPENSSL_INIT: openssl_config_int(%s, %s, %lu)\n",
            filename, appname, flags);
#endif

    OPENSSL_load_builtin_modules(commentsOnError);
#ifndef OPENSSL_NO_ENGINE
    /* Need to load ENGINEs */
    ENGINE_load_builtin_engines(commentsOnError);
#endif
    ERR_clear_error();
#ifndef OPENSSL_SYS_UEFI
    ret = CONF_modules_load_file(filename, appname, flags, commentsOnError);
#endif
    openssl_configured = 1;
    return ret;
}

void openssl_no_config_int(void)
{
    openssl_configured = 1;
}
