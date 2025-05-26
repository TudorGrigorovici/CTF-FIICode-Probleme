# Stage 1 --------------------------------------------------------- build
FROM alpine:3.19 AS builder
RUN apk add --no-cache \
        build-base musl-dev linux-headers \
        libseccomp-dev libseccomp-static \
        socat

WORKDIR /src
COPY vuln.c .

RUN gcc vuln.c -o vuln \
      -O2 -static -fPIE -pie \
      -fstack-protector-all -z relro -z now \
      -lseccomp -static -ldl -pthread

# Stage 2 ------------------------------------------------------- runtime
FROM alpine:3.19 AS runtime
RUN apk add --no-cache socat
COPY --from=builder /src/vuln /home/ctf/vuln
COPY flag.txt /flag.txt
RUN chown root:root /home/ctf/vuln && chmod 4755 /home/ctf/vuln && \
    adduser -D -u 1337 ctf
USER ctf

EXPOSE 1337
ENTRYPOINT ["socat","TCP-LISTEN:1337,reuseaddr,fork","EXEC:/home/ctf/vuln,stderr"]
