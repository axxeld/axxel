
/**
 *
 * Axxel - Acl memory caching
 *
 * Copyright (c) 2013 Axxel Team
 *
 * Use and distribution licensed under the MIT license.
 * See the LICENSE file for full text.
 *
 * Authors: Andres Gutierrez <andres@phalconphp.com>
 */

#include "stdio.h"

int primes() {

    int max = 4096;
    int primes[max];
    int n_primes = 0;
    int i;

    for (i = 2; n_primes < max; i++) {
        int j;

        for (j = 0; j < n_primes; j++) {
            if (i % primes[j] == 0) {
                goto not_prime;
            }
        }
        primes[n_primes] = i;
        n_primes++;
        not_prime:
            ;
    }

    /* Now display the list. */

    //printf ("The first %d primes are:\n", max);
    for (i = 0; i < n_primes; i++) {
        if ((i + 1) % 5 == 0) {
            printf (" %5d,", primes[i]);
            if ((i + 1) % 16 == 0) {
                printf ("\n");
            }
        }
    }
    printf ("\n");
    return 0;
}

int main(int argc, char **argv) {
    primes();
    return 0;
}