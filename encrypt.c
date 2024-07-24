#include <stdio.h>
#include <gmp.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"

#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define OPTIONS "hvi:o:n:"

uint64_t bitcount(mpz_t x) {

    //x, as an mpz_t type
    mpz_t num;
    mpz_init_set(num, x);

    uint64_t bits = 0;

    while (mpz_cmp_ui(num, 0) > 0) {

        bits++;

        mpz_fdiv_q_ui(num, num, 2);
    }

    mpz_clear(num);

    return bits;
}

int output = 0;
char *name;

int main(int argc, char **argv) {

    char *pub = "ss.pub";
    FILE *infile = stdin;
    FILE *outfile = stdout;
    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

        case 'h': {

            printf("SYNOPSIS\n   Encrypts a file using the SS algorithm.\nUSAGE\n   ./encrypt "
                   "[OPTIONS]\n\nOPTIONS\n   -h              Display program help and usage.\n   "
                   "-v              Display verbose program output.\n   -i infile         File to "
                   "encrypt (default: stdin).\n   -o outfile   file to write encryption to "
                   "(default: stdout).");
            return 1;
        }

        case 'v': {

            output = 1;
            break;
        }

        case 'i': {

            name = optarg;
            infile = fopen(name, "r");

            break;
        }

        case 'o': {

            name = optarg;
            outfile = fopen(name, "w");
            break;
        }

        case 'n': {

            pub = optarg;
            break;
        }
        }
    }

    //open files
    FILE *pubkey = fopen(pub, "r");

    //if any files are bad, error
    if (pubkey == NULL || infile == NULL || outfile == NULL) {

        printf("bad file\n");
        return 1;
    }

    //variables
    mpz_t n;
    mpz_init(n);
    char *username = NULL;

    //read public key from file
    ss_read_pub(n, username, pubkey);

    //output
    if (output == 1) {

        username = getenv("USER");

        printf("user = %s\n", username);

        gmp_printf("n (%lu bits): %Zd\n", bitcount(n), n);
    }

    //encrypt
    ss_encrypt_file(infile, outfile, n);

    //close
    mpz_clear(n);
    fclose(pubkey);
    fclose(infile);
    fclose(outfile);

    return 0;
}
