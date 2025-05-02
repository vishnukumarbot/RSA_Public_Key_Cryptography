RSA Public Key Encryption Algorithm

Developed by Vishnu Gelle

This is an implementation of a public-key encryption system that supports key generation, encryption, and decryption of messages. The system is based on a RSA algorithm where a public-private key pair is used for encryption and decryption operations.

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
