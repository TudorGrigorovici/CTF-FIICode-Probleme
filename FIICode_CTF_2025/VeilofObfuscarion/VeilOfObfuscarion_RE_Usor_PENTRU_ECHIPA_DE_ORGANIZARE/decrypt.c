/* Veil of Obfuscarion — matching decryptor
 * usage: ./decrypt <Base85-ciphertext>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

/* --------- shared helpers ----------------------------- */
static void decode_key(uint8_t *out,
                       const uint8_t *in,
                       size_t len,
                       uint8_t mask)
{
    for (size_t i = 0; i < len; i++) out[i] = in[i] ^ mask;
}

/* BBS PRNG */
static const uint64_t BBS_P = 0xF7E75FDC469067FFULL;
static const uint64_t BBS_Q = 0xE85CED54AF57E53EULL;
static const uint64_t BBS_N = BBS_P * BBS_Q;
static uint64_t       bbs_state = 0xDEADBEEFC0FFEEULL;

static uint8_t bbs_next_byte(void)
{
    bbs_state = (bbs_state * bbs_state) % BBS_N;
    return (uint8_t)(bbs_state & 0xFF);
}
static void bbs_xor(uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) buf[i] ^= bbs_next_byte();
}

/* RC4 */
static const uint8_t obf_rc4_key[16] = {
    'F'^0xAA,'I'^0xAA,'X'^0xAA,'E'^0xAA,
    'D'^0xAA,'R'^0xAA,'C'^0xAA,'4'^0xAA,
    'K'^0xAA,'E'^0xAA,'Y'^0xAA,'1'^0xAA,
    '2'^0xAA,'3'^0xAA,'4'^0xAA,'\0'^0xAA
};
static void rc4_init(uint8_t *S, const uint8_t *K, size_t klen)
{
    uint8_t j = 0;
    for (int i = 0; i < 256; i++) S[i] = i;
    for (int i = 0; i < 256; i++) {
        j = j + S[i] + K[i % klen];
        uint8_t t = S[i]; S[i] = S[j]; S[j] = t;
    }
}
static void rc4_crypt(uint8_t *S, uint8_t *buf, size_t len)
{
    uint8_t i = 0, j = 0;
    for (size_t n = 0; n < len; n++) {
        i++; j += S[i];
        uint8_t t = S[i]; S[i] = S[j]; S[j] = t;
        buf[n] ^= S[(S[i] + S[j]) & 0xFF];
    }
}

/* AES-128-CBC decrypt, standard padding */
static const uint8_t obf_aes_key[16] = {
    'A'^0x5A,'E'^0x5A,'S'^0x5A,'C'^0x5A,
    'H'^0x5A,'A'^0x5A,'L'^0x5A,'L'^0x5A,
    'E'^0x5A,'N'^0x5A,'G'^0x5A,'E'^0x5A,
    'K'^0x5A,'E'^0x5A,'Y'^0x5A,'\0'^0x5A
};
static size_t aes_cbc_decrypt(const uint8_t *in, size_t len,
                              const uint8_t *key,
                              uint8_t **out)
{
    if (len < 32 || (len & 0x0F)) {
        fputs("cipher length invalid\n", stderr);
        exit(1);
    }

    const uint8_t *iv  = in;
    const uint8_t *ct  = in + 16;
    size_t         clen = len - 16;

    AES_KEY aeskey;
    AES_set_decrypt_key(key, 128, &aeskey);

    uint8_t prev[16];
    memcpy(prev, iv, 16);

    *out = malloc(clen);

    for (size_t b = 0; b < clen; b += 16) {
        AES_decrypt(ct + b, *out + b, &aeskey);
        for (int i = 0; i < 16; i++) (*out)[b+i] ^= prev[i];
        memcpy(prev, ct + b, 16);
    }

    /* strip PKCS#7 */
    uint8_t pad = (*out)[clen-1];
    if (pad == 0 || pad > 16) {
        fputs("bad padding\n", stderr);
        exit(1);
    }
    return clen - pad;
}

/* ChaCha20 */
static void chacha20_crypt(const uint8_t *in, size_t len,
                           const uint8_t *key, uint8_t *out)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    uint8_t iv[12] = {0};

    EVP_EncryptInit_ex(ctx, EVP_chacha20(), NULL, key, iv);
    int outl = 0;
    EVP_EncryptUpdate(ctx, out, &outl, in, len);
    EVP_CIPHER_CTX_free(ctx);
}

/* Base-85 decode */
static size_t base85_decode(const char *in, uint8_t *out)
{
    size_t len = strlen(in);
    if (len % 5) { fputs("bad Base85 length\n", stderr); exit(1); }

    size_t o = 0;
    for (size_t i = 0; i < len; i += 5) {
        uint32_t v = 0;
        for (int j = 0; j < 5; j++) {
            char c = in[i+j];
            if (c < 33 || c > 117) { fputs("bad char\n", stderr); exit(1); }
            v = v*85 + (c - 33);
        }
        for (int j = 3; j >= 0; j--) { out[o+j] = v & 0xFF; v >>= 8; }
        o += 4;
    }
    return o;
}

/* --------- fixed-point loop to recover ChaCha key ------ */
static const char LABEL[] = "SALCHACH";

static void recover_aout(const uint8_t *sout, size_t len, uint8_t *aout)
{
    uint8_t key[32] = {0};               /* start with zero key */

    for (;;) {
        chacha20_crypt(sout, len, key, aout);

        uint8_t newkey[32];
        unsigned int klen;
        HMAC(EVP_sha256(), LABEL, sizeof(LABEL)-1, aout, len, newkey, &klen);

        if (!memcmp(key, newkey, 32)) break;   /* converged */
        memcpy(key, newkey, 32);
    }
}

static size_t hex2bin(const char *hex, uint8_t *out) {
    size_t n = strlen(hex) / 2;
    for (size_t i = 0; i < n; i++) {
        sscanf(hex + 2*i, "%2hhx", &out[i]);
    }
    return n;
}

/* ------------------- main ----------------------------- */
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <ciphertext>\n", argv[0]);
        return 1;
    }

    size_t hexlen = strlen(argv[1]);
    uint8_t *hexbuf = malloc(hexlen / 2);
    memset(hexbuf, 0, hexlen/2);
    size_t b85len = hex2bin(argv[1], hexbuf);

    //puts(hexbuf);

    /* 1. Base-85 ? sout */
    //size_t max = (strlen(argv[1]) / 5) * 4;
    size_t max = (b85len / 5) * 4;
    uint8_t *sout = malloc(max);
    //size_t  slen  = base85_decode(argv[1], sout);
    size_t  slen  = base85_decode(hexbuf, sout);

    //puts(sout);

    /* 2. iterate ChaCha20 ? aout (AES blob) */
    //uint8_t *aout = malloc(slen);
    //recover_aout(sout, slen, aout);

    uint8_t *aout = sout;
    //puts("After aout\n");

    /* 3. AES-CBC decrypt ? buf_rc4 */
    uint8_t aes_key[16]; decode_key(aes_key, obf_aes_key, 16, 0x5A);
    uint8_t *buf_rc4;
    size_t   len_rc4 = aes_cbc_decrypt(aout, slen, aes_key, &buf_rc4);

    /* 4. RC4 decrypt */
    uint8_t rc4_key[16]; decode_key(rc4_key, obf_rc4_key, 16, 0xAA);
    uint8_t S[256]; rc4_init(S, rc4_key, 16);
    rc4_crypt(S, buf_rc4, len_rc4);

    /* 5. BBS XOR */
    bbs_xor(buf_rc4, len_rc4);

    printf("FLAG: %.*s\n", (int)len_rc4, (char*)buf_rc4);

    free(sout); /*free(aout);*/ free(buf_rc4);
    return 0;
}
