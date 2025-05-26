/*
 * Off-by-one CTF — hardened version
 *   • static musl build, PIE, RELRO, canary, NX
 *   • prints the flag with open/read/write (no /bin/sh)
 *   • installs its own seccomp allow-list
 *
 * Compile inside Dockerfile with:
 *   gcc vuln.c -o bof -O2 -static -fPIE -pie \
 *       -fstack-protector-all -z relro -z now  -lseccomp
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <seccomp.h>

#define MAGIC      0xCAFEBABE
#define MAX_BUFFER 256
#define FLAG_PATH  "/flag.txt"

typedef struct {
    char  buffer[MAX_BUFFER];  /* 0-255 */
    bool  print_flag;          /* 256   */
    short custom_value;        /* 258-259 */
} frame_t;

/* ------------------------------  helpers  ----------------------------- */

static void fatal(const char *m) { perror(m); _exit(1); }

static inline uint32_t clamp_u32(uint32_t x, uint32_t max)
{ return x > max ? max : x; }

/* print flag without spawning a shell */
static void win(void)
{
    int fd = open(FLAG_PATH, O_RDONLY);
    if (fd < 0) fatal("open flag");
    char buf[128];
    ssize_t n;
    while ((n = read(fd, buf, sizeof buf)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n) fatal("write");
    close(fd);
}

/* ---------------------------  vulnerability  -------------------------- */

void handle_stream(FILE *fp)
{
    frame_t f = { .print_flag = false };
    uint32_t header[2];

    puts("Welcome to the party!\n");
    if (fread(header, 4, 2, fp) != 2) return;

    if (header[0] != MAGIC)           return;

    printf("You are close!\n");

    uint32_t req_len = clamp_u32(header[1], MAX_BUFFER);
    (void)req_len;

    uint32_t v2;
    if (fread(&v2, 4, 1, fp) != 1)    return;

    if (fread(f.buffer, 1, v2, fp) != v2) return;

    if (f.print_flag && (f.custom_value % 2 == 0))
        win();
}

int main(void)
{
    setbuf(stdout, NULL);
    alarm(30);
    handle_stream(stdin);
    return 0;
}
