#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTION "i:o:n:vh"
// decrypt.c

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   The keygen program will be in charge of key generation, producing RSA public and "
        "private key.\n"
        "\n"
        "USAGE\n"
        "   %s [i:o:n:vh] [-i input] [-o output] [-n pbfile]\n"
        "\n"
        "OPENTIONS\n"
        "   -i: specifies the input file to decrypt (default: stdin).\n"
        "   -o: specifies the output file to decrypt (default: stdout).\n"
        "   -n: specifies the file containing the private key (default: rsa.priv).\n"
        "   -v: enables verbose output.\n"
        "   -h: displays program synopsis and usage.\n",
        exec);
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    FILE *output = stdout;
    FILE *pvfile = fopen("rsa.priv", "r");
    bool verbos = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTION)) != -1) {
        switch (opt) {
        case 'i': input = fopen(optarg, "r"); break;
        case 'o': output = fopen(optarg, "w"); break;
        case 'n': pvfile = fopen(optarg, "r"); break;
        case 'v': verbos = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        }
    }

    if (pvfile == NULL) {
        printf("pvfile is invalid.\n");
        return 1;
    }

    mpz_t n, d;
    mpz_inits(n, d, NULL);
    rsa_read_priv(n, d, pvfile);

    if (verbos) {
        gmp_printf("n (%zu bits) %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%zu bits) %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(input, output, n, d);

    fclose(pvfile);
    fclose(input);
    fclose(output);
    mpz_clears(n, d, NULL);
    return 0;
}
