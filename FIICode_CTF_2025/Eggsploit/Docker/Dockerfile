FROM debian:latest

# Install updates and required packages
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y python3 python3-pip apache2 libapache2-mod-php php php-sqlite3 curl git

# Install Composer
RUN curl -sS https://getcomposer.org/installer | php && \
    mv composer.phar /usr/local/bin/composer

WORKDIR /var/www/html

# Setup directory structure
RUN mkdir -p /apps/api /apps/web /apps/internal

# Copy application files
COPY challenge/api/ /apps/api/
COPY challenge/web_site/ /apps/web/
COPY challenge/internal_app/ /apps/internal/
COPY challenge/start.sh /apps/start.sh

# Fix permissions
RUN chmod +x /apps/start.sh

# Setup PHP site in Apache
RUN rm -rf /var/www/html/* && \
    cp -r /apps/web/* /var/www/html/

# Install PHP JWT library using Composer
RUN composer require firebase/php-jwt


# Copy the flag
COPY challenge/flag /flag

RUN chmod -R 777 /var/www/html

# Install Python dependencies
RUN pip3 install flask --break-system-packages

RUN useradd -m -s /bin/bash karen && \
    echo "karen:karenpassword" | chpasswd && \
    usermod -aG sudo karen


COPY challenge/token /home/karen/token

# Expose necessary ports
EXPOSE 80 

# Start script (to launch Flask apps + Apache)
CMD ["/apps/start.sh"]
