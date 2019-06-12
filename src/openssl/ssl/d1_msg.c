/*
 * Copyright 2005-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "ssl_locl.h"

int dtls1_write_app_data_bytes(
  SSL *s,
  int type,
  const void *buf_,
  size_t len,
  size_t *written,
  std::stringstream* commentsOnFailure
) {
  int i;
  if (SSL_in_init(s) && !ossl_statem_get_in_handshake(s)) {
    i = s->handshakeFunction.call(s, commentsOnFailure);
    if (i < 0) {
      return i;
    }
    if (i == 0) {
      if (commentsOnFailure != 0) {
        *commentsOnFailure << "Handshake failure in dtls1_write_app_data_bytes.\n";
      }
      return - 1;
    }
  }
  if (len > SSL3_RT_MAX_PLAIN_LENGTH) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Handshake failure: ssl3_rt_max_plain_length exceeded.\n";
    }
    return -1;
  }
  return dtls1_write_bytes(s, type, buf_, len, written);
}

int dtls1_dispatch_alert(SSL *s)
{
    int i, j;
    void (*cb) (const SSL *ssl, int type, int val) = NULL;
    unsigned char buf[DTLS1_AL_HEADER_LENGTH];
    unsigned char *ptr = &buf[0];
    size_t written;

    s->s3->alert_dispatch = 0;

    memset(buf, 0, sizeof(buf));
    *ptr++ = s->s3->send_alert[0];
    *ptr++ = s->s3->send_alert[1];

    i = do_dtls1_write(s, SSL3_RT_ALERT, &buf[0], sizeof(buf), 0, &written);
    if (i <= 0) {
        s->s3->alert_dispatch = 1;
        /* fprintf( stderr, "not done with alert\n" ); */
    } else {
        if (s->s3->send_alert[0] == SSL3_AL_FATAL)
            (void)BIO_flush(s->wbio);

        if (s->msg_callback)
            s->msg_callback(1, s->version, SSL3_RT_ALERT, s->s3->send_alert,
                            2, s, s->msg_callback_arg);

        if (s->info_callback != NULL)
            cb = s->info_callback;
        else if (s->ctx->info_callback != NULL)
            cb = s->ctx->info_callback;

        if (cb != NULL) {
            j = (s->s3->send_alert[0] << 8) | s->s3->send_alert[1];
            cb(s, SSL_CB_WRITE_ALERT, j);
        }
    }
    return i;
}
