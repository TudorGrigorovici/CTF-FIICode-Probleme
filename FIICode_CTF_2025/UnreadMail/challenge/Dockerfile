FROM python:3.11-slim

# Install necessary tools
RUN apt-get update && apt-get install -y \
    wget \
    curl \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create user and home directory
RUN useradd -m -s /bin/bash ctf

# Create working directory
WORKDIR /home/ctf/app

# Copy source code
COPY app/main.py .  

COPY flag.txt /var/spool/mail/flag.txt

# Set ownership to the ctf user
RUN chown -R ctf:ctf /home/ctf

# Install Flask
RUN pip install flask

# Drop privileges to 'ctf'
USER ctf

# Expose port
EXPOSE 80

# Run the Flask app
CMD ["python", "main.py"]