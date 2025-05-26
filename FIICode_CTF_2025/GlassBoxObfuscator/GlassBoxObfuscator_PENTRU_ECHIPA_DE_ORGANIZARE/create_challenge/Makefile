CC      = gcc
CFLAGS  = -O2 -static -fno-stack-protector -fPIE -ffunction-sections \
          -Wl,-z,relro,-z,now
LDFLAGS = -static -lcrypto -lseccomp -ldl -pthread

all: challenge

loader: loader.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	strip $@

challenge: loader packer.py
	python3 packer.py loader $@

clean:
	rm -f loader challenge

.PHONY: all clean
