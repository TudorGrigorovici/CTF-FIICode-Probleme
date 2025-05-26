#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>  
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <openssl/evp.h>
#include <openssl/err.h>

/* ----- embedded AES blob + 1-byte VM XOR key ----- */
#define AES_BLOB_LEN (8 + 32 + 16 + 1)          /* "MAGICAES" + key + iv + vmkey */
__attribute__((section(".rodata")))
static const unsigned char aes_blob[AES_BLOB_LEN] =
    "MAGICAES"                                  /* 8-byte tag              */
    /* 32-byte key (patched by packer) */        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                                                 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    /* 16-byte IV  (patched by packer) */        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    /* 1-byte VM XOR key (patched) */            "\0";

#include <sys/prctl.h>
#include <seccomp.h>        /* new */

/* --- extra anti-debug helpers ----------------------------- */
static int looks_suspicious_parent(void)
{
    /* read /proc/PID/comm (just the basename); 16-byte buffer is enough */
    char path[32], name[17] = {0};
    snprintf(path, sizeof path, "/proc/%d/comm", getppid());
    int fd = open(path, O_RDONLY);
    if (fd < 0) return 0;
    read(fd, name, 16);
    close(fd);

    const char *bad[] = { "gdb", "strace", "ltrace", "frida", "lldb", NULL };
    for (int i = 0; bad[i]; ++i)
        if (strstr(name, bad[i])) return 1;
    return 0;
}

static void install_seccomp_kill_ptrace(void)
{
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) _exit(1);

    /* kill the process if it ever calls ptrace(…) */
    seccomp_rule_add(ctx, SCMP_ACT_KILL_PROCESS, SCMP_SYS(ptrace), 0);

    /* …or if it tries to raise SIGTRAP with tgkill(…) */
    seccomp_rule_add(ctx, SCMP_ACT_KILL_PROCESS, SCMP_SYS(tgkill), 1,
                     SCMP_A1(SCMP_CMP_EQ, SIGTRAP));

    /* lock filter */
    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
    seccomp_load(ctx);
    seccomp_release(ctx);
}

/* --- consolidated anti-debug entry point ------------------ */
static void anti_debug(void)
{
    /* 1 – classic “can I ptrace myself?” trick */
    if (ptrace(PTRACE_TRACEME, 0, NULL, 0) == -1)
        _exit(1);

    /* 2 – check TracerPid in /proc/self/status (original code) */
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) _exit(1);
    char line[128];
    while (fgets(line, sizeof line, f))
        if (!strncmp(line, "TracerPid:", 10) && atoi(line + 10) != 0)
            _exit(1);
    fclose(f);

    /* 3 – parent process sanity */
    if (looks_suspicious_parent())
        _exit(1);

    /* 4 – lock things down: make core-dumps impossible */
    prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);

    /* 5 – install seccomp “ptrace-kill” rules */
    install_seccomp_kill_ptrace();
}

/* ----- map our own file read-only ----- */
static uint8_t *map_self(off_t *sz_out)
{
    int fd = open("/proc/self/exe", O_RDONLY);
    if (fd < 0) return NULL;
    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return NULL; }
    uint8_t *p = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return NULL;
    *sz_out = st.st_size;
    return p;
}

/* ----- AES-256-CBC decrypt ----- */
static uint8_t *aes_decrypt(const uint8_t *ct, size_t clen,
                            size_t *outlen)
{
    const unsigned char *key = aes_blob + 8;
    const unsigned char *iv  = aes_blob + 8 + 32;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        EVP_CIPHER_CTX_free(ctx); return NULL;
    }

    uint8_t *pt = mmap(NULL, clen, PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_PRIVATE | MAP_ANON, -1, 0);
    if (pt == MAP_FAILED) { EVP_CIPHER_CTX_free(ctx); return NULL; }

    int l, total = 0;
    if (!EVP_DecryptUpdate(ctx, pt, &l, ct, (int)clen)) goto bad;
    total += l;
    if (!EVP_DecryptFinal_ex(ctx, pt + total, &l)) {
        ERR_print_errors_fp(stderr);
        goto bad;
    }
    total += l;
    *outlen = total;
    mprotect(pt, total, PROT_READ | PROT_EXEC);
    EVP_CIPHER_CTX_free(ctx);
    return pt;
bad:
    EVP_CIPHER_CTX_free(ctx);
    munmap(pt, clen);
    return NULL;
}

/* ----- tiny VM with on-the-fly XOR decode -----
   decoded opcodes:
     0x00                 – NOP / junk
     0x10 reg offset[4]   – MOV   Rreg, offset
     0x20 reg             – PRINT Rreg          (null-terminated)
     0x30 reg1 reg2       – ADD   Rreg1 += Rreg2
     0xFF                 – HALT
*/
static void vm_run(uint8_t *mem, size_t size, uint8_t k)
{
    uint64_t R[4] = {0};
    uint32_t pc   = 0;
    while (pc < size) {

        /* lightweight helper: fetch & decode one byte */
        #define FETCH()  ((pc < size) ? (mem[pc++] ^ k) : 0)

        uint8_t op = FETCH();

        switch (op)
        {
        case 0x00:                                /* NOP / junk */
            break;

        case 0x10: {                              /* MOV */
            uint8_t reg = FETCH();
            uint32_t off = 0;
            off |= (uint32_t)FETCH()       <<  0;
            off |= (uint32_t)FETCH()       <<  8;
            off |= (uint32_t)FETCH()       << 16;
            off |= (uint32_t)FETCH()       << 24;
            R[reg & 3] = off;
        } break;

        case 0x20: {                              /* PRINT (XOR-decode) */
            uint8_t reg = FETCH();
            uint64_t off = R[reg & 3];
            if (off >= size) return;
            /* decode one byte at a time until  (byte ^ k) == 0 */
            for (uint64_t p = off; p < size; ++p) {
                uint8_t ch = mem[p] ^ k;
                if (!ch) break;
                putchar(ch);
            }
            putchar('\n');
            fflush(stdout);
        } break;

        case 0x30: {                              /* ADD */
            uint8_t dst = FETCH();
            uint8_t src = FETCH();
            R[dst & 3] += R[src & 3];
        } break;

        case 0xFF:                                /* HALT */
            return;

        default:                                  /* unrecognised ? NOP */
            break;
        }
    }

    #undef FETCH
}

/* ----- main ----- */
int main(void)
{
    anti_debug();
    off_t binsz;
    unsigned char c_vm_key;
    uint8_t *self = map_self(&binsz);
    if (!self || binsz < 4) return 1;

    /* ciphertext length is stored little-endian in the last 4 bytes */
    uint32_t clen;
    memcpy(&clen, self + binsz - 4, 4);
    if (clen > binsz) return 1;
    const uint8_t *cipher = self + binsz - 4 - clen;

    size_t plen;
    uint8_t *plain = aes_decrypt(cipher, clen, &plen);
    if (!plain) return 1;

    vm_run(plain, plen, aes_blob[AES_BLOB_LEN-1]);
    printf("Try harder and you will be victorious! Here is a hint: %ld\n", strlen(plain));

    /* anti-dump clean-up */
    mprotect(plain, plen, PROT_READ | PROT_WRITE);
    memset(plain, 0, plen);
    mprotect(plain, plen, PROT_NONE);

    munmap(plain, plen);
    munmap(self, binsz);
    return 0;
}
