#include "ss.h"
#include "randstate.h"
#include "numtheory.h"

//for random()
#include <stdlib.h>
#include <time.h>

//-- lcm function --
void lcm(mpz_t result, mpz_t x, mpz_t y) {

    mpz_t xy;
    mpz_init(xy);
    mpz_mul(xy, x, y);

    mpz_t divisor;
    mpz_init(divisor);

    gcd(divisor, x, y);

    mpz_div(result, xy, divisor);

    mpz_clears(xy, divisor, NULL);
}

//-- make the public key --
void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {

    srandom(time(NULL));

    uint64_t pb = ((random() % ((nbits / 5) - 1)) + (2 * (nbits / 5)));

    uint64_t qb = (nbits - pb);

    //if pb is divisible by two, the divide
    if (pb % 2 == 0) {

        pb = pb / 2;

    } else {

        pb += 1;
        qb -= 1;
        pb = pb / 2;
    }

    //printf("p bits = %lu\n", pb);
    //printf("q bits = %lu\n", qb);

    //-- make primes p and q, using the bits from above --
    //p
    make_prime(p, pb + 1, iters);
    //gmp_printf("p = %Zd\n", p);

    //q
    make_prime(q, qb + 1, iters);
    //gmp_printf("q = %Zd\n", q);

    mpz_t p1, q1, mp, mq;

    mpz_init(p1);
    mpz_init(q1);
    mpz_init(mp);
    mpz_init(mq);

    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);

    mpz_mod(mp, q1, p);
    mpz_mod(mq, p1, q);

    while (mpz_cmp_ui(mp, 0) == 0 || mpz_cmp_ui(mq, 0) == 0) {

        make_prime(p, pb + 1, iters);
        //gmp_printf("p = %Zd\n", p);

        //q
        make_prime(q, qb + 1, iters);
        //gmp_printf("q = %Zd\n", q);
    }

    //set n = p*p*q
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
}

//-- make private key --
void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {

    //save values for q-1 and p-1, and the lcm
    mpz_t p1;
    mpz_init(p1);
    mpz_t q1;
    mpz_init(q1);
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);

    mpz_t n;
    mpz_init(n);

    //printf("variables set\n");

    //set pq to lcm p-1 q-1
    lcm(pq, p1, q1);

    //printf("lcm calculated\n");

    //calculate n = ppq
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);

    mod_inverse(d, n, pq);

    //printf("mod inverse calculated\n");

    mpz_mul(pq, p, q);

    mpz_clears(p1, q1, n, NULL);
}

//-- write pub key to a file --
void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {

    //writes the public exponent to a file
    //the public exponent is ppq
    gmp_fprintf(pbfile, "%Zx\n", n);

    //username
    fprintf(pbfile, "%s\n", username);
}

//-- read pubkey --
void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {

    gmp_fscanf(pbfile, "%Zx\n", n);

    fscanf(pbfile, "%s\n", username);
}

//-- write privatekey --
void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {

    gmp_fprintf(pvfile, "%Zx\n", pq);

    gmp_fprintf(pvfile, "%Zx\n", d);
}

//-- read privatekey --
void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {

    gmp_fscanf(pvfile, "%Zx\n", pq);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

//-- encrypts a single int --
void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {

    pow_mod(c, m, n, n);
}

//-- decrypts one int --
void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {

    pow_mod(m, c, d, pq);
}

//-- encrypts an entire file using the encrypt function --
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {

    //sqrt n
    mpz_t N;
    mpz_init(N);
    mpz_sqrt(N, n);

    size_t k = ((mpz_sizeinbase(N, 2) - 1) / 8);

    //allocate mem for block
    uint8_t *block = (uint8_t *) malloc(k * sizeof(uint8_t));

    //set index 0 to 0xff
    block[0] = 0xFF;

    mpz_t m;
    mpz_init(m);

    mpz_t c;
    mpz_init(c);

    size_t j;

    //while not end of file
    while (feof(infile) != true) {

        j = fread(block + 1, sizeof(uint8_t), k - 1, infile);

        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);

        ss_encrypt(c, m, n);

        gmp_fprintf(outfile, "%Zx\n", c);

        //j = fread(block + 1, sizeof(uint8_t), k - 1, infile);
    }

    mpz_clears(m, c, NULL);
    free(block);
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {

    mpz_t m;
    mpz_init(m);
    mpz_t c;
    mpz_init(c);

    size_t k = ((mpz_sizeinbase(pq, 2) - 1) / 8);

    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));

    size_t j;

    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {

        ss_decrypt(m, c, d, pq);

        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);

        fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
    }

    mpz_clears(c, m, NULL);
    free(block);
}
