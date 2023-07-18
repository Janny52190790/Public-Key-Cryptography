#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <math.h>
#include "numtheory.h"
#include "randstate.h"
//rsa.c

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t m, d, p1, q1, l, mul, base, bitsm;
    mpz_init_set_ui(base, 2);
    mpz_inits(m, d, p1, q1, l, mul, bitsm, NULL);
    uint64_t bits = (random() % (nbits / 2)) + (nbits / 4);
    make_prime(p, bits, iters);
    make_prime(q, (nbits - bits), iters);
    mpz_mul(n, p, q);
    mpz_sub_ui(p1, p, 1); // p1 = p - 1
    mpz_sub_ui(q1, q, 1); // q1 = q - 1
    gcd(m, p1, q1);
    mpz_mul(mul, p1, q1); //(p-1)*(q-1)
    mpz_abs(mul, mul);
    mpz_tdiv_q(l, mul, m); //mpz_t l = abs((p1)*(q1))/m;
    uint64_t bits1 = nbits - 1;
    mpz_pow_ui(bitsm, base, bits1);
    while (true) {
        mpz_urandomb(e, state, nbits);
        mpz_add(e, e, bitsm);
        gcd(d, e, l);
        if (mpz_cmp_ui(d, 1) == 0) { //(d == 1)
            mpz_clears(m, d, p1, q1, l, mul, bitsm, NULL);
            return;
        }
    }
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    if (pbfile != NULL) {
        gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n", n, e, s);
        fputs(username, pbfile);
        return;
    }
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    if (pbfile != NULL) {
        gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n", n, e, s);
        fscanf(pbfile, "%s", username);
        return;
    }
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t m, p1, q1, l, mul;
    mpz_inits(m, p1, q1, l, mul, NULL);
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    gcd(m, p1, q1);
    mpz_mul(mul, p1, q1);
    mpz_abs(mul, mul);
    mpz_tdiv_q(l, mul, m);
    mod_inverse(d, e, l); // inverse of op1 modulo op2
    mpz_clears(m, p1, q1, l, mul, NULL);
    return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    if (pvfile != NULL) {
        gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
        return;
    }
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    if (pvfile != NULL) {
        gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);
        return;
    }
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);
    size_t x = mpz_sizeinbase(n, 2);
    size_t k = (x - 1) / 8; // block size
    uint8_t *arr;
    arr = (uint8_t *) malloc(k * sizeof(uint8_t)); // allocate memory for k
    assert(arr);
    arr[0] = 0xff;
    k = k - 1;
    while (!feof(infile)) {
        size_t j = fread(arr + 1, sizeof(uint8_t), k, infile);
        mpz_import(m, j + 1, 1, 1, 1, 0, arr);
        rsa_encrypt(c, m, e, n);
        //write the encrypted number to outfile as a hexstring followed by a trailing newline.
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    mpz_clears(m, c, NULL);
    return;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    //k = (mpz_sizeinbase(n, 2) - 1) /  8;
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    size_t x = mpz_sizeinbase(n, 2);
    uint8_t k = (x - 1) / 8; // block size
    uint8_t *arr;
    arr = (uint8_t *) malloc(k * sizeof(uint8_t));
    size_t b;
    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
        rsa_decrypt(m, c, d, n); // need decrypt first?
        mpz_export(arr, &b, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(arr + 1, sizeof(uint8_t), b - 1, outfile);
    }
    mpz_clears(c, m, NULL);
    return;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) { // t == m
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}
