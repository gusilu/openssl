/*
 * Copyright 2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL licenses, (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.openssl.org/source/license.html
 * or in the file LICENSE in the source distribution.
 */

#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include "fuzzer.h"

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
extern int rand_predictable;
#endif
#define ENTROPY_NEEDED 32

int FuzzerInitialize(int *argc, char ***argv)
{
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    ERR_get_state();
    CRYPTO_free_ex_index(0, -1);
    RAND_add("", 1, ENTROPY_NEEDED);
    RAND_status();

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    rand_predictable = 1;
#endif
    return 1;
}

int FuzzerTestOneInput(const uint8_t *buf, size_t len)
{
    const unsigned char *p = buf;
    unsigned char *der = NULL;

    X509 *x509 = d2i_X509(NULL, &p, len);
    if (x509 != NULL) {
        BIO *bio = BIO_new(BIO_s_null());
        /* This will load and print the public key as well as extensions */
        X509_print(bio, x509);
        BIO_free(bio);

        i2d_X509(x509, &der);
        OPENSSL_free(der);

        X509_free(x509);
    }
    ERR_clear_error();
    return 0;
}

void FuzzerCleanup(void)
{
}
