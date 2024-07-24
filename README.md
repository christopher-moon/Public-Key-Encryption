FILES:

decrypt.c: allows the user to decrypt an encrypted file 

encrypt.c: allows the user to encrypt a file

keygen.c: allows the user to generate a public/private key pair

numtheory.c: mathematics behind SS encryption method

randstate.c: random state initialization functions

ss.c: contains implementation of the SS encryption method


COMPILING

make all: compiles all

make clean: removes all executables

make format: formats C and header files


KEYGEN.C options:

HOW TO RUN: ./keygen [options]

OPTIONS:

    -i: infile

    -o: outfile

    -b: bits

    -v: display bits

    -h: help message


ENCRYPT.C options:

HOW TO RUN: ./keygen [options]

OPTIONS:

    -i: infile

    -o: outfile

    -n: public key file

    -v: display bits (CURRENTLY NON_FUNCTIONING)

    -h: help message


DECRYPT.C options:

HOW TO RUN: ./keygen [options]

OPTIONS:

    -i: infile

    -o: outfile

    -n: private key file

    -v: display bits (CURRENTLY NON_FUNCTIONING)

    -h: help message
