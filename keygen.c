#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gmp.h>
#include <time.h>
#include <unistd.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTION "b:i:n:d:s:vh"
// keygen.c

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   The keygen program will be in charge of key generation, producing RSA public and "
        "private key.\n"
        "\n"
        "USAGE\n"
        "   %s [b:i:n:d:s:vh] [-b nbits] [-i iters] [-n pbfile] [-d pbfile]\n"
        "\n"
        "OPENTIONS\n"
        "   -b: specifies the minimum bits needed for the public modulus n.\n"
        "   -i: specifies the number of Miller-Rabin iterations for testing primes.\n"
        "   -n pbfile: specifies the public key file (default: rsa.pub).\n"
        "   -d pvfile: specifies the private key file (default: rsa.pub).\n"
        "   -s: specifies the random seed for the random state initialization.\n"
        "   -v: enables verbose output.\n"
        "   -h: displays program synopsis and usage.\n",
        exec);
}

int main(int argc, char **argv) {
    uint64_t nbits = 256;
    uint64_t iters = 50;
    FILE *pbfile = fopen("rsa.pub", "w");
    FILE *pvfile = fopen("rsa.priv", "w");
    uint64_t seed = time(NULL);
    bool verbos = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTION)) != -1) {
        switch (opt) {
        case 'b': nbits = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'i': iters = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'n': pbfile = fopen(optarg, "w"); break;
        case 'd': pvfile = fopen(optarg, "w"); break;
        case 's': seed = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'v': verbos = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        }
    }
    if (pbfile == NULL) {
        printf("pbfile is invalid");
        return 1;
    }

    if (pvfile == NULL) {
        printf("pvfile is invalid");
        return 1;
    }

    // chomod 0600 pvfile;
    int fd = fileno(pvfile);
    fchmod(fd, 0600);

    mpz_t p, q, n, e, s, d, username;
    mpz_inits(p, q, n, e, s, d, username, NULL);

    randstate_init(seed); // Initialize the random stat
    rsa_make_pub(p, q, n, e, nbits, iters);
    rsa_make_priv(d, e, p, q);

    char *user = getenv("USER");

    mpz_set_str(username, user, 62);

    rsa_sign(s, username, d, n);

    rsa_write_pub(n, e, s, user, pbfile);
    rsa_write_priv(n, d, pvfile);

    if (verbos) {
        printf("user = %s\n", user);
        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%zu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%zu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, n, e, s, d, username, NULL);

    return 0;
}
