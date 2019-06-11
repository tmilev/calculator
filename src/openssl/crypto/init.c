/*
 * Copyright 2016-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "../e_os.h"
#include "include/internal/cryptlib_int.h"
#include "../include/openssl/err.h"
#include "include/internal/rand_int.h"
#include "../include/internal/bio.h"
#include "../include/openssl/evp.h"
#include "include/internal/evp_int.h"
#include "../include/internal/conf.h"
#include "include/internal/async.h"
#include "include/internal/engine.h"
#include "../include/internal/comp.h"
#include "../include/internal/err.h"
#include "include/internal/err_int.h"
#include "include/internal/objects.h"
#include <stdlib.h>
#include <assert.h>
#include "../include/internal/thread_once.h"
#include "include/internal/dso_conf.h"
#include "../include/internal/dso.h"
#include "include/internal/store.h"
#include "../include/openssl/trace.h"

static int stopped = 0;
extern int load_crypto_strings_inited;

/*
 * Since per-thread-specific-data destructors are not universally
 * available, i.e. not on Windows, only below CRYPTO_THREAD_LOCAL key
 * is assumed to have destructor associated. And then an effort is made
 * to call this single destructor on non-pthread platform[s].
 *
 * Initial value is "impossible". It is used as guard value to shortcut
 * destructor for threads terminating before libcrypto is initialized or
 * after it's de-initialized. Access to the key doesn't have to be
 * serialized for the said threads, because they didn't use libcrypto
 * and it doesn't matter if they pick "impossible" or derefernce real
 * key value and pull NULL past initialization in the first thread that
 * intends to use libcrypto.
 */
static union {
    long sane;
    CRYPTO_THREAD_LOCAL value;
} destructor_key = { -1 };

static void ossl_init_thread_stop(struct thread_local_inits_st *locals);

static void ossl_init_thread_destructor(void *local)
{
    ossl_init_thread_stop((struct thread_local_inits_st *)local);
}

static struct thread_local_inits_st *ossl_init_get_thread_local(int alloc)
{
    struct thread_local_inits_st *local = (thread_local_inits_st *)
        CRYPTO_THREAD_get_local(&destructor_key.value);

    if (alloc) {
        if (local == NULL
            && (local = (thread_local_inits_st *) OPENSSL_zalloc(sizeof(*local))) != NULL
            && !CRYPTO_THREAD_set_local(&destructor_key.value, local)) {
            OPENSSL_free(local);
            return NULL;
        }
    } else {
        CRYPTO_THREAD_set_local(&destructor_key.value, NULL);
    }

    return local;
}

typedef struct ossl_init_stop_st OPENSSL_INIT_STOP;
struct ossl_init_stop_st {
    void (*handler)(void);
    OPENSSL_INIT_STOP *next;
};

static OPENSSL_INIT_STOP *stop_handlers = NULL;

int ossl_init_base(std::stringstream* commentsOnError) {
  static bool alreadyRan = false;
  static int resultFirstRun = 0;
  if (alreadyRan) {
    return resultFirstRun;
  }
  alreadyRan = true;
  CRYPTO_THREAD_LOCAL key;
  ossl_malloc_setup_failures();
  destructor_key.value = key;
  resultFirstRun = 1;
  return 1;
}

static CRYPTO_ONCE register_atexit = CRYPTO_ONCE_STATIC_INIT;
#if !defined(OPENSSL_SYS_UEFI) && defined(_WIN32)
static int win32atexit(void)
{
    OPENSSL_cleanup();
    return 0;
}
#endif

DEFINE_RUN_ONCE_STATIC(ossl_init_register_atexit)
{
#ifdef OPENSSL_INIT_DEBUG
    fprintf(stderr, "OPENSSL_INIT: ossl_init_register_atexit()\n");
#endif
#ifndef OPENSSL_SYS_UEFI
# ifdef _WIN32
    /* We use _onexit() in preference because it gets called on DLL unload */
    if (_onexit(win32atexit) == NULL)
        return 0;
# else
    if (atexit(OPENSSL_cleanup) != 0)
        return 0;
# endif
#endif

    return 1;
}

DEFINE_RUN_ONCE_STATIC_ALT(ossl_init_no_register_atexit,
                           ossl_init_register_atexit)
{
#ifdef OPENSSL_INIT_DEBUG
    fprintf(stderr, "OPENSSL_INIT: ossl_init_no_register_atexit ok!\n");
#endif
    /* Do nothing in this case */
    return 1;
}

static CRYPTO_ONCE add_all_ciphers = CRYPTO_ONCE_STATIC_INIT;

int ossl_init_add_all_ciphers(void* commentsOnError);

static int ossl_init_add_all_ciphers_ossl_ret_ = 0;

void ossl_init_add_all_ciphers_ossl_(void* commentsOnError) {
  ossl_init_add_all_ciphers_ossl_ret_ = ossl_init_add_all_ciphers(commentsOnError);
}

int ossl_init_add_all_ciphers(void* commentsOnError) {
  openssl_add_all_ciphers_int((std::stringstream*) commentsOnError);
  return 1;
}

static CRYPTO_ONCE add_all_digests = CRYPTO_ONCE_STATIC_INIT;
DEFINE_RUN_ONCE_STATIC(ossl_init_add_all_digests)
{
    /*
     * OPENSSL_NO_AUTOALGINIT is provided here to prevent at compile time
     * pulling in all the ciphers during static linking
     */
#ifndef OPENSSL_NO_AUTOALGINIT
    OSSL_TRACE(INIT, "openssl_add_all_digests()\n");
    openssl_add_all_digests_int();
#endif
    return 1;
}

static CRYPTO_ONCE add_all_macs = CRYPTO_ONCE_STATIC_INIT;
DEFINE_RUN_ONCE_STATIC(ossl_init_add_all_macs)
{
    /*
     * OPENSSL_NO_AUTOALGINIT is provided here to prevent at compile time
     * pulling in all the macs during static linking
     */
#ifndef OPENSSL_NO_AUTOALGINIT
    OSSL_TRACE(INIT, "openssl_add_all_macs_int()\n");
    openssl_add_all_macs_int();
#endif
    return 1;
}

static int config_inited = 0;
static const OPENSSL_INIT_SETTINGS *conf_settings = NULL;

//DEFINE_RUN_ONCE_STATIC(init = ossl_init_config
static int ossl_init_config(std::stringstream *commentsOnError);



static int ossl_init_config(std::stringstream* commentsOnError) {
    std::cout << "DEBUG: OH no this piece of code is being called!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    int ret = openssl_config_int(conf_settings, commentsOnError);
    config_inited = 1;
    return ret;
}

#ifndef OPENSSL_NO_ENGINE
static CRYPTO_ONCE engine_openssl = CRYPTO_ONCE_STATIC_INIT;
DEFINE_RUN_ONCE_STATIC(ossl_init_engine_openssl)
{
    OSSL_TRACE(INIT, "engine_load_openssl_int()\n");
    engine_load_openssl_int();
    return 1;
}
# ifndef OPENSSL_NO_RDRAND
static CRYPTO_ONCE engine_rdrand = CRYPTO_ONCE_STATIC_INIT;
DEFINE_RUN_ONCE_STATIC(ossl_init_engine_rdrand)
{
    OSSL_TRACE(INIT, "engine_load_rdrand_int()\n");
    engine_load_rdrand_int();
    return 1;
}
# endif
static CRYPTO_ONCE engine_dynamic = CRYPTO_ONCE_STATIC_INIT;
DEFINE_RUN_ONCE_STATIC(ossl_init_engine_dynamic)
{
    OSSL_TRACE(INIT, "engine_load_dynamic_int()\n");
    engine_load_dynamic_int();
    return 1;
}
#endif

#ifndef OPENSSL_NO_COMP
static CRYPTO_ONCE zlib = CRYPTO_ONCE_STATIC_INIT;

static int zlib_inited = 0;
DEFINE_RUN_ONCE_STATIC(ossl_init_zlib)
{
    /* Do nothing - we need to know about this for the later cleanup */
    zlib_inited = 1;
    return 1;
}
#endif

static void ossl_init_thread_stop(struct thread_local_inits_st *locals)
{
    /* Can't do much about this */
    if (locals == NULL)
        return;

    if (locals->async) {
        OSSL_TRACE(INIT, "async_delete_thread_state()\n");
        async_delete_thread_state();
    }

    if (locals->err_state) {
        OSSL_TRACE(INIT, "err_delete_thread_state()\n");
        err_delete_thread_state();
    }

    if (locals->rand) {
        OSSL_TRACE(INIT, "drbg_delete_thread_state()\n");
        drbg_delete_thread_state();
    }

    OPENSSL_free(locals);
}

void OPENSSL_thread_stop(void)
{
    if (destructor_key.sane != -1)
        ossl_init_thread_stop(ossl_init_get_thread_local(0));
}

int ossl_init_thread_start(uint64_t opts)
{
    struct thread_local_inits_st *locals;

    if (!OPENSSL_init_crypto(0, NULL, 0))
        return 0;

    locals = ossl_init_get_thread_local(1);

    if (locals == NULL)
        return 0;

    if (opts & OPENSSL_INIT_THREAD_ASYNC) {
        OSSL_TRACE(INIT,
                   "ossl_init_thread_start: "
                   "marking thread for async\n");
        locals->async = 1;
    }

    if (opts & OPENSSL_INIT_THREAD_ERR_STATE) {
        OSSL_TRACE(INIT,
                   "ossl_init_thread_start: "
                   "marking thread for err_state\n");
        locals->err_state = 1;
    }

    if (opts & OPENSSL_INIT_THREAD_RAND) {
        OSSL_TRACE(INIT,
                   "ossl_init_thread_start: "
                   "marking thread for rand\n");
        locals->rand = 1;
    }

    return 1;
}

void OPENSSL_cleanup(void)
{
    OPENSSL_INIT_STOP *currhandler, *lasthandler;
    CRYPTO_THREAD_LOCAL key;

    /* If we've not been inited then no need to deinit */

    /* Might be explicitly called and also by atexit */
    if (stopped)
        return;
    stopped = 1;

    /*
     * Thread stop may not get automatically called by the thread library for
     * the very last thread in some situations, so call it directly.
     */
    ossl_init_thread_stop(ossl_init_get_thread_local(0));

    currhandler = stop_handlers;
    while (currhandler != NULL) {
        currhandler->handler();
        lasthandler = currhandler;
        currhandler = currhandler->next;
        OPENSSL_free(lasthandler);
    }
    stop_handlers = NULL;


    /*
     * We assume we are single-threaded for this function, i.e. no race
     * conditions for the various "*_inited" vars below.
     */

#ifndef OPENSSL_NO_COMP
    if (zlib_inited) {
        OSSL_TRACE(INIT, "OPENSSL_cleanup: comp_zlib_cleanup_int()\n");
        comp_zlib_cleanup_int();
    }
#endif

    //if (async_initialiazed) {
    //    OSSL_TRACE(INIT, "OPENSSL_cleanup: async_deinit()\n");
    //    async_deinit();
    //}

    if (load_crypto_strings_inited) {
        OSSL_TRACE(INIT, "OPENSSL_cleanup: err_free_strings_int()\n");
        err_free_strings_int();
    }

    key = destructor_key.value;
    destructor_key.sane = -1;
    CRYPTO_THREAD_cleanup_local(&key);

    /*
     * Note that cleanup order is important:
     * - rand_cleanup_int could call an ENGINE's RAND cleanup function so
     * must be called before engine_cleanup_int()
     * - ENGINEs use CRYPTO_EX_DATA and therefore, must be cleaned up
     * before the ex data handlers are wiped in CRYPTO_cleanup_all_ex_data().
     * - conf_modules_free_int() can end up in ENGINE code so must be called
     * before engine_cleanup_int()
     * - ENGINEs and additional EVP algorithms might use added OIDs names so
     * obj_cleanup_int() must be called last
     */
    OSSL_TRACE(INIT, "OPENSSL_cleanup: rand_cleanup_int()\n");
    rand_cleanup_int();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: rand_drbg_cleanup_int()\n");
    rand_drbg_cleanup_int();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: conf_modules_free_int()\n");
    conf_modules_free_int();
#ifndef OPENSSL_NO_ENGINE
    OSSL_TRACE(INIT, "OPENSSL_cleanup: engine_cleanup_int()\n");
    engine_cleanup_int();
#endif
    OSSL_TRACE(INIT, "OPENSSL_cleanup: ossl_store_cleanup_int()\n");
    ossl_store_cleanup_int();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: crypto_cleanup_all_ex_data_int()\n");
    crypto_cleanup_all_ex_data_int();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: bio_cleanup()\n");
    bio_cleanup();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: evp_cleanup_int()\n");
    evp_cleanup_int();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: obj_cleanup_int()\n");
    obj_cleanup_int();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: err_int()\n");
    err_cleanup();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: CRYPTO_secure_malloc_done()\n");
    CRYPTO_secure_malloc_done();

    OSSL_TRACE(INIT, "OPENSSL_cleanup: ossl_trace_cleanup()\n");
    ossl_trace_cleanup();

}

/*
 * If this function is called with a non NULL settings value then it must be
 * called prior to any threads making calls to any OpenSSL functions,
 * i.e. passing a non-null settings value is assumed to be single-threaded.
 */
int OPENSSL_init_crypto(uint64_t opts, const ossl_init_settings_st *settings, std::stringstream* commentsOnError) {
  if (stopped) {
    if (!(opts & OPENSSL_INIT_BASE_ONLY)) {
      if (commentsOnError != 0) {
        *commentsOnError << "Cryptography is marked as stopped.\n";
      }
    }
    return 0;
  }
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: initializing crypto.\n";
  }

  /*
   * When the caller specifies OPENSSL_INIT_BASE_ONLY, that should be the
   * *only* option specified.  With that option we return immediately after
   * doing the requested limited initialization.  Note that
   * err_shelve_state() called by us via ossl_init_load_crypto_nodelete()
   * re-enters OPENSSL_init_crypto() with OPENSSL_INIT_BASE_ONLY, but with
   * base already initialized this is a harmless NOOP.
   *
   * If we remain the only caller of err_shelve_state() the recursion should
   * perhaps be removed, but if in doubt, it can be left in place.
   */
  if (!ossl_init_base(commentsOnError)) {
    std::cout << "DEBUG: Initializing base. " << std::endl;
    return 0;
  }

  if (opts & OPENSSL_INIT_BASE_ONLY) {
    if (commentsOnError != 0) {
      *commentsOnError << "DEBUG: Initializing base only, the rest is postponed.\n";
    }
    return 1;
  }
  static bool alreadyRan = false;
  if (alreadyRan) {
    return 1;
  }
  alreadyRan = true;

  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: Initializing crypto fully.\n";
  }
    /*
     * Now we don't always set up exit handlers, the INIT_BASE_ONLY calls
     * should not have the side-effect of setting up exit handlers, and
     * therefore, this code block is below the INIT_BASE_ONLY-conditioned early
     * return above.
     */
  if ((opts & OPENSSL_INIT_NO_ATEXIT) != 0) {
    std::cout << "DEBUG: Initializing once alt. " << std::endl;
    if (!RUN_ONCE_ALT(&register_atexit, ossl_init_no_register_atexit, ossl_init_register_atexit)) {
      return 0;
    }
  } else if (!RUN_ONCE(&register_atexit, ossl_init_register_atexit, 0)) {
    return 0;
  }
  if (!err_load_crypto_strings_int(commentsOnError)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to load crypto error strings.\n";
    }
    return 0;
  }
  std::cout << "DEBUG: about to load ciphers. " << std::endl;
  if (!RUN_ONCE(&add_all_ciphers, ossl_init_add_all_ciphers, commentsOnError)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to add all ciphers.\n";
    }
    std::cout << "DEBUG: failed to add all ciphers!!!!!\n";
    return 0;
  }
  std::cout << "DEBUG: Ciphers loaded. " << std::endl;
  if (!RUN_ONCE(&add_all_digests, ossl_init_add_all_digests, commentsOnError)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to add all digests.\n";
    }
    return 0;
  }
  if (!RUN_ONCE(&add_all_macs, ossl_init_add_all_macs, 0)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to add all macs.\n";
    }
    return 0;
  }
  std::cout << "DEBUG: about to init config. " << std::endl;
  conf_settings = settings;
  if (ossl_init_config(commentsOnError) <= 0) {
    conf_settings = NULL;
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to initialize configuration.\n";
    }
    return 0;
  }
  conf_settings = NULL;
  //if (!async_init()) {
  //  if (commentsOnError != 0) {
  //    *commentsOnError << "Failed to initialize async.\n";
  //  }
  //  return 0;
  //}

  if (!RUN_ONCE(&engine_openssl, ossl_init_engine_openssl, commentsOnError)) {
    return 0;
  }
  if (!RUN_ONCE(&engine_rdrand, ossl_init_engine_rdrand, commentsOnError)) {
    return 0;
  }
  if (!RUN_ONCE(&engine_dynamic, ossl_init_engine_dynamic, commentsOnError)) {
    return 0;
  }
  ENGINE_register_all_complete(commentsOnError);
  std::cout << "DEBUG: about to no comp. " << std::endl;
  if (!RUN_ONCE(&zlib, ossl_init_zlib, commentsOnError)) {
    return 0;
  }
  std::cout << "DEBUG: no comp done!!!!. " << std::endl;
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: crypto initialization went till the end.\n";
  }
  return 1;
}

int OPENSSL_atexit(void (*handler)(void))
{
    OPENSSL_INIT_STOP *newhand;

#if !defined(OPENSSL_USE_NODELETE)\
    && !defined(OPENSSL_NO_PINSHARED)
    {
        union {
            void *sym;
            void (*func)(void);
        } handlersym;

        handlersym.func = handler;
# if defined(DSO_WIN32) && !defined(_WIN32_WCE)
        {
            HMODULE handle = NULL;
            BOOL ret;

            /*
             * We don't use the DSO route for WIN32 because there is a better
             * way
             */
            ret = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                                    | GET_MODULE_HANDLE_EX_FLAG_PIN,
                                    handlersym.sym, &handle);

            if (!ret)
                return 0;
        }
# elif !defined(DSO_NONE)
        /*
         * Deliberately leak a reference to the handler. This will force the
         * library/code containing the handler to remain loaded until we run the
         * atexit handler. If -znodelete has been used then this is
         * unnecessary.
         */
        {
            DSO *dso = NULL;

            ERR_set_mark();
            dso = DSO_dsobyaddr(handlersym.sym, DSO_FLAG_NO_UNLOAD_ON_FREE);
            /* See same code above in ossl_init_base() for an explanation. */
            OSSL_TRACE1(INIT,
                       "atexit: obtained DSO reference? %s\n",
                       (dso == NULL ? "No!" : "Yes."));
            DSO_free(dso);
            ERR_pop_to_mark();
        }
# endif
    }
#endif

    if ((newhand = (OPENSSL_INIT_STOP *) OPENSSL_malloc(sizeof(*newhand))) == NULL) {
        CRYPTOerr(CRYPTO_F_OPENSSL_ATEXIT, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    newhand->handler = handler;
    newhand->next = stop_handlers;
    stop_handlers = newhand;

    return 1;
}

#ifdef OPENSSL_SYS_UNIX
/*
 * The following three functions are for OpenSSL developers.  This is
 * where we set/reset state across fork (called via pthread_atfork when
 * it exists, or manually by the application when it doesn't).
 *
 * WARNING!  If you put code in either OPENSSL_fork_parent or
 * OPENSSL_fork_child, you MUST MAKE SURE that they are async-signal-
 * safe.  See this link, for example:
 *      http://man7.org/linux/man-pages/man7/signal-safety.7.html
 */

void OPENSSL_fork_prepare(void)
{
}

void OPENSSL_fork_parent(void)
{
}

void OPENSSL_fork_child(void)
{
    rand_fork();
}
#endif
