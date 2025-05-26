/* Veil of Obfuscarion — fixed encryptor
 * usage: ./encrypt <FLAG>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

/* --------- utility: XOR-mask de-obfuscation ------------ */
static void decode_key(uint8_t *out,
                       const uint8_t *in,
                       size_t len,
                       uint8_t mask)
{
    for (size_t i = 0; i < len; i++) out[i] = in[i] ^ mask;
}

/* --------- 1) Blum-Blum-Shub keystream ---------------- */
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

/* --------- 2) RC4 with fixed key ---------------------- */
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
        uint8_t t = S[i];  S[i] = S[j];  S[j] = t;
    }
}

static void rc4_crypt(uint8_t *S, uint8_t *buf, size_t len)
{
    uint8_t i = 0, j = 0;
    for (size_t n = 0; n < len; n++) {
        i++; j += S[i];
        uint8_t t = S[i];  S[i] = S[j];  S[j] = t;
        buf[n] ^= S[(S[i] + S[j]) & 0xFF];
    }
}

/* --------- 3) AES-128-CBC (standard, PKCS#7) ----------- */
static const uint8_t obf_aes_key[16] = {
    'A'^0x5A,'E'^0x5A,'S'^0x5A,'C'^0x5A,
    'H'^0x5A,'A'^0x5A,'L'^0x5A,'L'^0x5A,
    'E'^0x5A,'N'^0x5A,'G'^0x5A,'E'^0x5A,
    'K'^0x5A,'E'^0x5A,'Y'^0x5A,'\0'^0x5A
};

static size_t aes_cbc_encrypt(const uint8_t *in, size_t len,
                              const uint8_t *key,
                              uint8_t **out)
{
    uint8_t iv[16] = {0};

    /* PKCS#7 pad */
    uint8_t pad = 16 - (len & 0x0F);
    size_t  plen = len + pad;
    uint8_t *pbuf = malloc(plen);
    memcpy(pbuf, in, len);
    memset(pbuf + len, pad, pad);

    AES_KEY aeskey;
    AES_set_encrypt_key(key, 128, &aeskey);

    *out = malloc(16 + plen);          /* prepend IV */
    memcpy(*out, iv, 16);

    uint8_t prev[16];
    memcpy(prev, iv, 16);

    for (size_t b = 0; b < plen; b += 16) {
        uint8_t blk[16];
        for (int i = 0; i < 16; i++) blk[i] = pbuf[b+i] ^ prev[i];
        AES_encrypt(blk, (*out) + 16 + b, &aeskey);
        memcpy(prev, (*out) + 16 + b, 16);
    }

    free(pbuf);
    return 16 + plen;
}

/* --------- 4) HMAC-SHA-256 ? ChaCha20 ------------------ */
static const char LABEL[] = "SALCHACH";          /* 8 bytes */

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

/* --------- 5) Base-85 (Ascii85) encode ----------------- */
static size_t base85_encode(const uint8_t *in, size_t len, char *out)
{
    size_t o = 0;
    for (size_t i = 0; i < len; i += 4) {
        uint32_t v = 0;
        size_t chunk = (len - i >= 4) ? 4 : (len - i);
        for (size_t j = 0; j < chunk; j++) v = (v << 8) | in[i+j];
        v <<= (4 - chunk) * 8;                 /* shift if chunk<4 */

        for (int j = 4; j >= 0; j--) {
            out[o + j] = (char)(v % 85 + 33);
            v /= 85;
        }
        o += 5;
    }
    out[o] = '\0';
    return o;
}

/* Convert binary buffer ? hex string */
static void bin2hex(const uint8_t *in, size_t len, char *out) {
    for (size_t i = 0; i < len; i++) {
        sprintf(out + 2*i, "%02x", in[i]);
    }
    out[2*len] = '\0';
}

/* ------------------- main ----------------------------- */
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <FLAG>\n", argv[0]);
        return 1;
    }

    size_t len = strlen(argv[1]);
    uint8_t *buf = malloc(len + 16);   /* scratch */

    /* 1-2. BBS XOR + RC4 */
    memcpy(buf, argv[1], len);
    bbs_xor(buf, len);

    uint8_t rc4_key[16];  decode_key(rc4_key, obf_rc4_key, 16, 0xAA);
    uint8_t S[256];       rc4_init(S, rc4_key, 16);
    rc4_crypt(S, buf, len);

    /* 3. AES-CBC + pad */
    uint8_t aes_key[16];  decode_key(aes_key, obf_aes_key, 16, 0x5A);
    uint8_t *aout;
    size_t   alen = aes_cbc_encrypt(buf, len, aes_key, &aout);

    /* 4. HMAC-SHA-256 key, ChaCha20 */
    //uint8_t key[32];
    //unsigned int klen;
    //HMAC(EVP_sha256(), LABEL, sizeof(LABEL)-1, aout, alen, key, &klen);

    //uint8_t *sout = malloc(alen);
    //chacha20_crypt(aout, alen, key, sout);

    uint8_t* sout = aout;
    //puts(sout);

    /* 5. Base-85 */
    size_t b85len = (alen + 3) / 4 * 5 + 1;
    char  *b85    = malloc(b85len);
    base85_encode(sout, alen, b85);

    //puts(b85);

    /* Hex-encode Base85 output */
    char *hexout = malloc(b85len * 2 + 1);
    bin2hex((uint8_t*)b85, b85len, hexout);
    //printf("%s\n", hexout);
    if (memcmp(hexout, "21212121212121212121212121212121212121215f3d7444396d40563a4224285b2470385230722a45756566362a57354b6169235630394a2349734c7138266d722c3f5c365a423c44743e712c2663284e59676f38356d6347756e53386469676b44633b00", strlen(hexout)) == 0) {
        puts("Good job! You found the flag!\n");
    } else {
        puts("Try harder!\n");
    }

    //puts(b85);

    free(buf); free(aout); /*free(sout);*/ free(b85); free(hexout);
    return 0;
}
