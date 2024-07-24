#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
//idk if this is needed
#include "randstate.h"
#include "numtheory.h"

//find the greatest common divisor of a and b, and sets the value of g to the answer
void gcd(mpz_t g, const mpz_t a, const mpz_t b) {

    //temp var
    mpz_t temp;
    mpz_init(temp);

    //a and b are constants and they cannot be altered
    //must instead copy the values elsewhere, to be modified by the function
    mpz_t A;
    mpz_init(A);
    mpz_t B;
    mpz_init(B);

    //copy over vals of a  and b
    mpz_set(B, b);
    mpz_set(A, a);

    while (mpz_cmp_ui(B, 0) != 0) {

        mpz_set(temp, B);

        mpz_mod(B, A, B);

        mpz_set(A, temp);
    }

    //set the value of g to the answer
    mpz_set(g, A);

    //clear mpz types
    mpz_clear(temp);
    mpz_clear(A);
    mpz_clear(B);
}

//mod inverse of a and n
void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {

    //variables
    mpz_t temp;
    mpz_init(temp);

    mpz_t q;
    mpz_t val;
    mpz_t r;
    mpz_t rp;
    mpz_t t;
    mpz_t tp;
    mpz_init(r);
    mpz_init(rp);
    mpz_init(t);
    mpz_init(tp);
    mpz_init(q);
    mpz_init(val);

    //set values of t and t' and r and r'
    mpz_set(r, n);
    mpz_set(rp, a);

    mpz_set_ui(t, 0);
    mpz_set_ui(tp, 1);

    //while r is not 0
    while (mpz_cmp_ui(rp, 0) != 0) {

        //q = r/rp
        mpz_fdiv_q(q, r, rp);

        //set temp to r
        mpz_set(temp, r);

        //set r to rp
        mpz_set(r, rp);

        //set val to q*rp
        mpz_mul(val, q, rp);

        //val = r - q*rp
        mpz_sub(val, temp, val);

        //rp = val
        mpz_set(rp, val);

        //set temp to r
        mpz_set(temp, t);

        //set r to rp
        mpz_set(t, tp);

        //set val to q*rp
        mpz_mul(val, q, tp);

        //val = r - q*rp
        mpz_sub(val, temp, val);

        //rp = val
        mpz_set(tp, val);
    }
    //default
    mpz_set(o, t);

    //if t < 0
    if (mpz_sgn(t) == -1) {

        mpz_add(t, t, n);

        mpz_set(o, t);
    }

    //else set to 0
    if (mpz_cmp_ui(r, 1) > 0) {

        //gmp_printf("no inverse\n");

        mpz_set_ui(o, 0);
    }

    //clear vars
    mpz_clear(r);
    mpz_clear(rp);
    mpz_clear(t);
    mpz_clear(tp);
    mpz_clear(temp);
    mpz_clear(q);
    mpz_clear(val);
}

//power mod
void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {

    //vars
    mpz_t v;
    mpz_init(v);
    mpz_t p;
    mpz_init(p);

    //copy d value
    mpz_t D;
    mpz_init(D);
    mpz_set(D, d);

    //set values of v and a
    mpz_set_ui(v, 1);
    mpz_set(p, a);

    //set some value to 2
    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    //while D is > 0
    while (mpz_sgn(D) == 1) {

        //if D is odd
        if (mpz_odd_p(D) > 0) {

            // v = v*p mod n
            mpz_mul(v, v, p);

            mpz_mod(v, v, n);
        }

        // p = p*p mod n
        mpz_mul(p, p, p);

        mpz_mod(p, p, n);

        //division
        mpz_fdiv_q(D, D, two);
    }

    mpz_set(o, v);

    //clear all mpz type variables
    mpz_clear(v);
    mpz_clear(p);
    mpz_clear(D);
    mpz_clear(two);
}

bool is_prime(const mpz_t n, uint64_t iters) {

    //-- special cases --
    //two is the only even prime, 3 is also a weird case
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        return true;
    }

    //even and not 2, then not a prime, also if negative or 1
    if (mpz_even_p(n) != 0 || mpz_cmp_ui(n, 1) <= 0) {
        return false;
    }

    //-- solve for s and r --
    mpz_t s;
    mpz_init(s);
    mpz_set_ui(s, 0);
    mpz_t r;
    mpz_init(r);
    mpz_sub_ui(r, n, 1);

    //-- variables --
    //a is used to store the result of urandomm
    mpz_t a;
    mpz_init(a);
    //n3 is used to store n-3
    mpz_t n3;
    mpz_init(n3);
    mpz_sub_ui(n3, n, 3);
    //y is used to store power mod
    mpz_t y;
    mpz_init(y);
    //n1 stores n-1
    mpz_t n1;
    mpz_init(n1);
    mpz_sub_ui(n1, n, 1);
    //j
    mpz_t j;
    mpz_init(j);
    //two = 2
    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);
    //n2 = n-2
    mpz_t n2;
    mpz_init(n2);
    mpz_sub_ui(n2, n, 2);

    //keep looping until r is odd
    while (mpz_even_p(r) != 0) {

        mpz_div_ui(r, r, 2);

        mpz_add_ui(s, s, 1);
    }

    //set s1 to s-1
    mpz_t s1;
    mpz_init(s1);
    mpz_sub_ui(s1, s, 1);

    mpz_t temp;
    mpz_init(temp);
    mpz_set(temp, n);

    //-- loop from 1 to iterations --
    for (uint64_t i = 1; i < iters; i++) {

        //-- select a random number from 2 to n-2 --
        //initialize the random state using the time
        //randstate_init(time(NULL));

        //generate a number from 0 to (n-3) -1 using urandomm
        //mpz_urandomm(a, state, n3);

        //add 2 to a, making the resulting number in the range 2 to n- 1
        //mpz_add_ui(a, a, 2);

        //get number from a really big range
        mpz_urandomb(a, state, 1000);

        //a % (n - 2) + 2 to fix bounds
        mpz_sub_ui(temp, temp, 2);

        mpz_mod(a, a, temp);

        mpz_add_ui(a, a, 2);

        //-- set y to power mod --
        pow_mod(y, a, r, n);

        //-- if y is not 1 and y is not n - 1 --
        //0 means x and y are equal
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n1) != 0) {

            //set j to 1
            mpz_set_ui(j, 1);

            //-- while j less than or equal to s-1 and y is not n-1 --
            while (mpz_cmp(j, s1) <= 0 && mpz_cmp(y, n1) != 0) {

                //set y to pow mod(y,2,n)
                pow_mod(y, y, two, n);

                //-- if the new y is 1, return false
                if (mpz_cmp_ui(y, 1) == 0) {

                    //clear mpz types
                    mpz_clears(temp, s, a, y, r, j, n3, n1, s1, NULL);

                    //printf("y=1\n");
                    return false;
                }

                //j = j+1
                mpz_add_ui(j, j, 1);
            }

            //-- if y is not n-1, return false --
            if (mpz_cmp(y, n1) != 0) {

                //clear mpz types
                mpz_clears(temp, s, a, y, r, j, n3, n1, s1, two, NULL);
                //printf("y not n-1\n");
                return false;
            }
        }
    }

    mpz_clears(temp, s, a, y, r, j, n3, n1, s1, two, NULL);
    return true;
}

//make prime
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    //variables
    mpz_t lower;
    mpz_t guess;
    mpz_t two;
    mpz_inits(lower, guess, two, NULL);

    mpz_set_ui(two, 2);

    //lower = 2 ^ (bits -1)
    mpz_pow_ui(lower, two, bits - 1);

    //initialize state
    //randstate_init(time(NULL));

    //get random value from 0 to 2^bits - 1
    mpz_urandomb(guess, state, bits);

    //if num is less than the lower bound, then add the lower bound
    if (mpz_cmp(guess, lower) < 0) {
        //add 2^(bits-1)
        mpz_add(guess, lower, guess);
    }

    //gmp_printf("end guess is %Zd\n", guess);

    bool prime = false;

    //while number is not prime, add 1
    while (prime == false) {

        prime = is_prime(guess, iters);

        mpz_add_ui(guess, guess, 1);

        //gmp_printf("new guess is %Zd\n", guess);
    }

    mpz_sub_ui(p, guess, 1);

    mpz_clears(lower, guess, two, NULL);

    //randstate_clear();
}
