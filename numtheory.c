#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
// numtheory.h

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t p, temp, temp1, exponent1;
    mpz_init_set(p, base);
    mpz_init_set(exponent1, exponent);
    mpz_inits(temp, temp1, NULL);
    mpz_init_set_ui(out, 1);
    while (mpz_sgn(exponent) == 1) { // exponent > 0
        if (mpz_odd_p(exponent)) {
            mpz_mul(temp, p, out);
            mpz_mod(out, temp, modulus); //(p * out) mod modulus;
        }
        mpz_mul(temp1, p, p);
        mpz_mod(p, temp1, modulus); //(p * p) mod modulus;
        mpz_fdiv_q_ui(exponent, exponent, 2); //exponent >>= 1;
    }
    mpz_init_set(exponent, exponent1);
    mpz_clears(p, temp, temp1, exponent1, NULL);
    return;
}

bool is_prime(mpz_t n, uint64_t iters) {
    uint64_t s = 1;
    mpz_t r, a1, a, y, ep, n1, n3;
    mpz_inits(r, a1, a, y, ep, n1, n3, NULL);
    mpz_sub_ui(n1, n, 1); // n1 = n - 1;
    mpz_fdiv_q_ui(r, n1, 2); // r = (n - 1) // 2
    while (mpz_even_p(r)) { //(r % 2) == 0
        s = s + 1;
        mpz_fdiv_q_ui(r, r, 2);
    }
    for (uint64_t i = 1; i < iters; i = i + 1) {
        mpz_sub_ui(n3, n, 3);
        mpz_urandomm(a1, state, n3);
        mpz_add_ui(a, a1, 2); // 2 + rand() % (n - 3)
        pow_mod(y, a, r, n);
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n1) != 0)) { // (y != 1 && y != (n - 1))
            uint64_t j = 1;
            while (j <= (s - 1) && (mpz_cmp(y, n1) != 0)) {
                mpz_set_ui(ep, 2);
                pow_mod(y, y, ep, n);
                if (mpz_cmp_ui(y, 1) == 0) { // (y == 1)
                    mpz_clears(r, a1, a, y, ep, n1, n3, NULL);
                    return false;
                }
                j = j + 1;
            }
            if (mpz_cmp(y, n1) != 0) {
                mpz_clears(r, a1, a, y, ep, n1, n3, NULL);
                return false;
            }
        }
    }
    mpz_clears(r, a1, a, y, ep, n1, n3, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t p1, bitsm, base;
    mpz_init_set_ui(base, 2);
    mpz_inits(p1, bitsm, NULL);
    uint64_t bits1 = bits - 1;
    mpz_pow_ui(bitsm, base, bits1);
    do {
        mpz_urandomb(p1, state, bits);
        mpz_add(p, p1, bitsm);
    } while (!is_prime(p, iters));

    mpz_clears(p1, bitsm, base, NULL);
    return;
}

void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t t, a1, b1;
    mpz_init(t);
    mpz_init_set(a1, a);
    mpz_init_set(b1, b);
    while (mpz_cmp_ui(b, 0) != 0) { // (b != 0)
        mpz_init_set(t, b);
        mpz_mod(b, a, b); //b = a mod b;
        mpz_init_set(a, t);
    }
    mpz_init_set(d, a);
    mpz_set(a, a1);
    mpz_set(b, b1);
    mpz_clears(t, a1, b1, NULL);
    return;
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t tempm, r, r1, q, tempu, i1;
    mpz_inits(tempm, q, tempu, NULL);
    mpz_init_set(r, n);
    mpz_init_set(r1, a);
    mpz_set_ui(i, 0);
    mpz_init_set_ui(i1, 1);
    while (mpz_cmp_ui(r1, 0) != 0) { //r1 != 0
        mpz_fdiv_q(q, r, r1); //mpz_t q = r / r1;
        mpz_set(tempm, r);
        mpz_set(r, r1);
        mpz_mul(r1, q, r1);
        mpz_sub(r1, tempm, r1);
        //swap using tmep
        mpz_set(tempu, i);
        mpz_set(i, i1);
        mpz_mul(q, q, i1);
        mpz_sub(i1, tempu, q);
    }
    if (mpz_cmp_ui(r, 1) > 0) { //(r > 1)
        mpz_clears(tempm, r, r1, q, tempu, i1, NULL);
        printf("no inverse\n");
        return;
    }
    if (mpz_cmp_ui(i, 0) < 0) { // i < 0
        mpz_add(i, n, i); //t = t + n;
    }
    mpz_clears(tempm, r, r1, q, tempu, i1, NULL);
    return;
}
