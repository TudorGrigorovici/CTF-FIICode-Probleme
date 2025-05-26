FROM debian:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    socat \
    build-essential \
    iproute2 \
    netcat-openbsd

# Add 32-bit architecture support
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y gcc-multilib libc6:i386 libncurses5:i386 libstdc++6:i386 \
    socat build-essential iproute2 

# Set working directory
WORKDIR /app

# Copy source files
COPY app/vuln.c vuln.c
COPY flag.txt /flag

# Build the binaries inside the container (32bit)
RUN gcc vuln.c -o vuln -fno-stack-protector -z execstack -no-pie -m32 -fpermissive

# Expose the port for netcat to listen
EXPOSE 4444

# Use netcat (nc) to serve the vuln binary over TCP on port 4444
CMD socat TCP-LISTEN:4444,nodelay,reuseaddr,fork EXEC:"stdbuf -i0 -o0 -e0 ./vuln"
