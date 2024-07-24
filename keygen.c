#include <stdio.h>
#include <gmp.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"

#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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

uint64_t bits = 256;
uint64_t iters = 50;

int output = 0;
char *name;

#define OPTIONS "hb:i:n:d:s:v"

int main(int argc, char **argv) {

    FILE *pub = fopen("ss.pub", "w");
    FILE *priv = fopen("ss.priv", "w");

    uint64_t seed = time(NULL);

    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

        case 'h': {

            printf("SYNOPSIS\n   Generates an SS public/private key pair.\nUSAGE\n   ./keygen2 "
                   "[OPTIONS]\n\nOPTIONS\n   -h              Display program help and usage.\n   "
                   "-v              Display verbose program output.\n   -b bits         Minimum "
                   "bits needed for public key n (default: 256).\n   -i iterations   Miller-Rabin "
                   "iterations for testing primes (default: 50).\n   -n pbfile       Public key "
                   "file (default: ss.pub).\n   -d pvfile       Private key file (default: "
                   "ss.priv).\n   -s seed         Random seed for testing.");
            return 1;
        }

        case 'b': {

            bits = (uint64_t) strtoul(optarg, NULL, 10);
            break;
        }

        case 'i': {

            iters = (uint64_t) strtoul(optarg, NULL, 10);
            break;
        }

        case 'n': {

            name = optarg;
            pub = fopen(name, "w");
            break;
        }

        case 'd': {

            name = optarg;
            priv = fopen(name, "w");
            break;
        }

        case 's': {

            seed = (uint64_t) strtoul(optarg, NULL, 10);
            break;
        }

        case 'v': {

            output = 1;
            break;
        }
        }
    }

    //if any file is invalid, error
    if (pub == NULL || priv == NULL) {

        printf("bad file\n");
        return 1;
    }
    //printf("public key made\n");

    //initialize state
    randstate_init(seed);

    //make public key
    mpz_t p;
    mpz_t q;
    mpz_t n;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);

    ss_make_pub(p, q, n, bits, iters);

    //printf("public key made\n");

    //make private key
    mpz_t d;
    mpz_t pq;
    mpz_init(pq);
    mpz_init(d);

    ss_make_priv(d, pq, p, q);

    //printf("private key made\n");

    //get username
    char *name = getenv("USER");

    ss_write_pub(n, name, pub);
    ss_write_priv(pq, d, priv);

    //printf("keys written to files\n");

    //output
    if (output == 1) {

        printf("user : %s\n", name);

        gmp_printf("p (%lu bits): %Zd\n", bitcount(p), p);
        gmp_printf("q (%lu bits): %Zd\n", bitcount(q), q);
        gmp_printf("n (%lu bits): %Zd\n", bitcount(n), n);
        gmp_printf("pq (%lu bits): %Zd\n", bitcount(pq), pq);
        gmp_printf("d (%lu bits): %Zd\n", bitcount(d), d);
    }

    fclose(pub);
    fclose(priv);
    randstate_clear();
    mpz_clears(p, q, n, d, pq, NULL);

    return 0;
}
