# Project 2 Public Key Encryption Algorithm

**Name:** Vishnu Gelle
**Email address:** d.gelle@wsu.edu

This is an implementation of a public-key encryption system that supports key generation, encryption, and decryption of messages. The system is based on a RSA algorithm where a public-private key pair is used for encryption and decryption operations.

## Files
- `pubkey.c` - Main C source program.
- `pubkey.txt` - Output file containing the generated public key.
- `prikey.txt` - Output file containing the generated private key.
- `ptxt.txt` - Input file containing the plaintext to be encrypted.
- `ctxt.txt` - Output file containing the encrypted ciphertext.
- `decrypted.txt` - Output file contains the decrypted message.
- `README.md` - README which contains project report 
- `make` - makefile

## Compile instructions:
- Ensure we are working in a Linux environment.
- In terminal navigate to the project directory.
- To compile the project use below commands:

gcc pubkey.c -o pubkey

OR

make

OR 

make -f make

## Run instructions:

For key generation:
```sh
./pubkey --genkey
```

To encrypt:
```sh
./pubkey -e -k pubkey.txt -in plaintext.txt -out ciphertext.txt
```

To decrypt:
```sh
./pubkey -d -k prikey.txt -in ciphertext.txt -out decrypted.txt
```