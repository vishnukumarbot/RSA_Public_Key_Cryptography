#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Modular exponentiation
uint64_t modexp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

// Extended Euclidean Algorithm for modular inverse
uint64_t modinv(uint64_t a, uint64_t m) {
    int64_t m0 = m, t, q;
    int64_t x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        q = a / m;
        t = m;

        m = a % m;
        a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;
    return x1;
}

// Miller-Rabin primality test
bool miller_rabin(uint64_t n, int iterations) {
    if (n < 2 || n % 2 == 0)
        return n == 2;

    uint64_t d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }

    for (int i = 0; i < iterations; i++) {
        uint64_t a = 2 + rand() % (n - 3);
        uint64_t x = modexp(a, d, n);
        if (x == 1 || x == n - 1) continue;

        bool continue_loop = false;
        for (int r = 1; r < s; r++) {
            x = modexp(x, 2, n);
            if (x == n - 1) {
                continue_loop = true;
                break;
            }
        }
        if (continue_loop) continue;
        return false;
    }
    return true;
}

// Generate a 32-bit safe prime p = 2q + 1, where q ≡ 5 mod 12
uint64_t generate_safe_prime() {
    while (1) {
        uint64_t q = ((uint64_t)rand() << 16 | rand()) | 1;
        q |= (1ULL << 30);
        if (q % 12 != 10) continue;
        if (!miller_rabin(q, 10)) continue;

        uint64_t p = 2 * q + 1;
        if (miller_rabin(p, 10))
            return p;
    }
}

// Encrypts text from plaintext.txt using pubkey.txt to cipher.txt
void encrypt_file(const char *input_path, const char *pubkey_path, const char *output_path) {
    uint64_t g, e, p;
    FILE *pub = fopen(pubkey_path, "r");
    if (!pub || fscanf(pub, "%lu %lu %lu", &g, &e, &p) != 3) {
        perror("Error reading pubkey.txt");
        return;
    }
    fclose(pub);

    FILE *in = fopen(input_path, "r");
    if (!in) {
        perror("Error opening ptxt.txt");
        return;
    }

    FILE *out = fopen(output_path, "w");
    if (!out) {
        perror("Error opening cipher.txt");
        fclose(in);
        return;
    }

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        if (!isprint(ch)) continue;

        uint64_t m = (uint64_t)ch;
        uint64_t k = 2 + rand() % (p - 3);
        uint64_t c1 = modexp(g, k, p);
        uint64_t c2 = (m * modexp(e, k, p)) % p;
        fprintf(out, "%lu %lu\n", c1, c2);
    }

    fclose(in);
    fclose(out);
    printf("Encryption complete. Output saved to ctxt.txt\n");
}

// Decrypts cipher.txt using prikey.txt to decrypted.txt
void decrypt_file(const char *cipher_path, const char *prikey_path, const char *output_path) {
    uint64_t d, g, p;
    FILE *pri = fopen(prikey_path, "r");
    if (!pri || fscanf(pri, "%lu %lu %lu", &d, &g, &p) != 3) {
        perror("Error reading prikey.txt");
        return;
    }
    fclose(pri);

    FILE *in = fopen(cipher_path, "r");
    if (!in) {
        perror("Error opening cipher.txt");
        return;
    }

    FILE *out = fopen(output_path, "w");
    if (!out) {
        perror("Error opening decrypted.txt");
        fclose(in);
        return;
    }

    uint64_t c1, c2;
    while (fscanf(in, "%lu %lu", &c1, &c2) == 2) {
        uint64_t s = modexp(c1, d, p);
        uint64_t s_inv = modinv(s, p);
        uint64_t m = (c2 * s_inv) % p;
        fputc((char)m, out);
    }

    fclose(in);
    fclose(out);
    printf("Decryption complete. Output saved to decrypted.txt\n");
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s --genkey\n", argv[0]);
        printf("  %s -e -k pubkey.txt -in plaintext.txt -out ciphertext.txt\n", argv[0]);
        printf("  %s -d -k prikey.txt -in ciphertext.txt -out decrypted.txt\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--genkey") == 0) {
        uint64_t p = generate_safe_prime();
        uint64_t g = 2;
        uint64_t d = 2 + rand() % (p - 3);
        uint64_t e = modexp(g, d, p);

        FILE *pub = fopen("pubkey.txt", "w");
        FILE *pri = fopen("prikey.txt", "w");
        if (!pub || !pri) {
            perror("Error opening key files");
            return 1;
        }

        fprintf(pub, "%lu %lu %lu\n", g, e, p);
        fprintf(pri, "%lu %lu\n", d, p);
        fclose(pub);
        fclose(pri);

        printf("Keys generated successfully.\n");
        return 0;
    }

    if (argc != 8) {
        printf(" %d \n", argc);
        fprintf(stderr, "Invalid argument count.\n");
        return 1;
    }

    char *mode = argv[1]; // -e or -d
    char *key_file = NULL;
    char *input_file = NULL;
    char *output_file = NULL;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 && i + 1 < argc)
            key_file = argv[++i];
        else if (strcmp(argv[i], "-in") == 0 && i + 1 < argc)
            input_file = argv[++i];
        else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc)
            output_file = argv[++i];
        else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (!key_file || !input_file || !output_file) {
        fprintf(stderr, "Missing required arguments.\n");
        return 1;
    }

    if (strcmp(mode, "-e") == 0) {
        encrypt_file(input_file, key_file, output_file);
    } else if (strcmp(mode, "-d") == 0) {
        decrypt_file(input_file, key_file, output_file);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", mode);
        return 1;
    }

    return 0;
}
