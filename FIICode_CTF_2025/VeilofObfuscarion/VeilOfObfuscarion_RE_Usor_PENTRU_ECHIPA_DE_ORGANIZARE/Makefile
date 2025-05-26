CC = gcc
CFLAGS = -O2 -fno-plt -fomit-frame-pointer -fvisibility=hidden -s
LDFLAGS = -lcrypto

all: encrypt decrypt

encrypt: encrypt.c
	$(CC) $(CFLAGS) encrypt.c -o encrypt $(LDFLAGS)

decrypt: decrypt.c
	$(CC) $(CFLAGS) decrypt.c -o decrypt $(LDFLAGS)

strip: encrypt decrypt
	strip --strip-all encrypt decrypt

obfuscate: encrypt decrypt
	upx --best encrypt decrypt

clean:
	rm -f encrypt decrypt
