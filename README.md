# Assignment 5 - Public Key Cryptography
For this assignment, we mainly need to Encrypt and decrypt files using RSA. The keygen program will be in charge of key generation, producing RSA public and private key pairs.The encrypt program will encrypt files using a public key, and the decrypt program will decrypt the encrypted files using the corresponding private key.

## Automate compilation by using Makefile
```
$ make
```

## Cleaning
```
$ make clean
```

## Running

```
$ ./keygen  -b <bits> -i <iters> -n <pbfile> -d <pvfile> -s <seed>
```
```
$ ./encrypt  -i <input file> -o <output file> -n <public key file>
```
```
$ ./decrypt  -i <input file> -o <output file> -n <private key file>
```
## the command-line options your program accepts
## keygen
• -b: specifies the minimum bits needed for the public modulus n.\
• -i: specifies the number of Miller-Rabin iterations for testing primes (default: 50).\
• -n pbfile: specifies the public key file (default: rsa.pub).\
• -d pvfile: specifies the private key file (default: rsa.priv).\
• -s: specifies the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by time(NULL)).\
• -v: enables verbose output.\
• -h: displays program synopsis and usage.
## encrypt
• -i: specifies the input file to encrypt (default: stdin).\
• -o: specifies the output file to encrypt (default: stdout).\
• -n: specifies the file containing the public key (default: rsa.pub).\
• -v: enables verbose output.\
• -h: displays program synopsis and usage.
## decrypt
• -i: specifies the input file to decrypt (default: stdin).\
• -o: specifies the output file to decrypt (default: stdout).\
• -n: specifies the file containing the private key (default: rsa.priv).\
• -v: enables verbose output.\
• -h: displays program synopsis and usage.
