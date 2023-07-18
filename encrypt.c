#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTION "i:o:n:vh"
//encrypt.c

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
        "   -i: specifies the input file to encrypt (default: stdin).\n"
        "   -o: specifies the output file to encrypt (default: stdout).\n"
        "   -n: specifies the file containing the public key (default: rsa.pub).\n"
        "   -v: enables verbose output.\n"
        "   -h: displays program synopsis and usage.\n",
        exec);
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    FILE *output = stdout;
    FILE *pbfile = fopen("rsa.pub", "r");
    bool verbos = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTION)) != -1) {
        switch (opt) {
        case 'i': input = fopen(optarg, "r"); break;
        case 'o': output = fopen(optarg, "w"); break;
        case 'n': pbfile = fopen(optarg, "r"); break;
        case 'v': verbos = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        }
    }
    if (pbfile == NULL) {
        printf("pbfile is invalid.\n");
        return 1;
    }
    mpz_t n, e, s, m;
    mpz_inits(n, e, s, m, NULL);
    char username[100];
    rsa_read_pub(n, e, s, username, pbfile);

    if (verbos) {
        printf("user = %s\n", username);
        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    mpz_set_str(m, username, 62);

    if (!rsa_verify(m, s, e, n)) {
        printf("username is incorrect.\n");
        return 1;
    }

    rsa_encrypt_file(input, output, n, e);

    fclose(pbfile);
    fclose(input);
    fclose(output);
    mpz_clears(n, e, s, m, NULL);

    return 0;
}
