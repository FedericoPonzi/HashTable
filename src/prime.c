//
// Created by fponzi on 20/05/22.
//

#include "prime.h"
#include <math.h>

/*
 * Return whether x is prime or not
 *
 * Returns:
 *   1  - prime
 *   0  - not prime
 *   -1 - undefined (i.e. x < 2)
 */
size_t ht_prime_is_prime(const size_t x) {
    if (x < 2) { return -1; }
    if (x < 4) { return 1; }
    if ((x % 2) == 0) { return 0; }
    for (int i = 3; i <= floor(sqrt((double) x)); i += 2) {
        if ((x % i) == 0) {
            return 0;
        }
    }
    return 1;
}
size_t ht_prime_next_prime(size_t x){
    while (!ht_prime_is_prime(x)) {
        x++;
    }
    return x;
}


