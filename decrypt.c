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

    char *priv = "ss.priv";
    FILE *infile = stdin;
    FILE *outfile = stdout;
    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

        case 'h': {

            printf("SYNOPSIS\n   Decrypts a file using the SS algorithm.\nUSAGE\n   ./decrypt "
                   "[OPTIONS]\n\nOPTIONS\n   -h              Display program help and usage.\n   "
                   "-v              Display verbose program output.\n   -i infile         File to "
                   "decrypt (default: stdin).\n   -o outfile   file to write decryption to "
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

            priv = optarg;
            break;
        }
        }
    }

    //open files
    FILE *privkey = fopen(priv, "r");

    //if any files are bad, error
    if (privkey == NULL || infile == NULL || outfile == NULL) {

        printf("bad file\n");
        return 1;
    }

    //variables
    mpz_t d;
    mpz_init(d);

    mpz_t pq;
    mpz_init(pq);

    //read priv key from file
    ss_read_priv(pq, d, privkey);

    //output
    if (output == 1) {

        gmp_printf("pq (%lu bits): %Zd\n", bitcount(pq), pq);

        gmp_printf("d (%lu bits): %Zd\n", bitcount(d), d);
    }

    //decrypt
    ss_decrypt_file(infile, outfile, d, pq);

    //close
    mpz_clear(d);
    mpz_clear(pq);
    fclose(privkey);
    fclose(infile);
    fclose(outfile);

    return 0;
}
