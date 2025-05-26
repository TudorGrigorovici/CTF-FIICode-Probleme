FROM debian:latest

# Install system packages
RUN apt-get update && apt-get install -y \
    apache2 \
    php \
    php-cli \
    php-sqlite3 \
    php-curl \
    php-mbstring \
    libapache2-mod-php \
    unzip \
    curl \
    git \
    sqlite3 \
    && apt-get clean

# Install Composer globally
RUN curl -sS https://getcomposer.org/installer | php && \
    mv composer.phar /usr/local/bin/composer

# Set working directory and copy app code
WORKDIR /var/www/html
COPY app/ /var/www/html/

# Copy flag
COPY flag.txt /flag.txt

# Remove default Apache index page
RUN rm -f /var/www/html/index.html

# Install PHP dependencies (JWT via Composer)
RUN composer require firebase/php-jwt

# Ensure proper permissions (optional but recommended)
RUN chown -R www-data:www-data /var/www/html
RUN chmod -R 755 /var/www/html/resources/songs/
RUN chown -R www-data:www-data /var/www/html/resources/songs/

RUN sqlite3 /var/www/html/dev/users.db "INSERT INTO users(name,password,artist) VALUES(\"AdaMilea\",\"27337ff43fdd493f53d46d958fa71dece63f4feb6b3c98d244184a9d61a234ea\",1);"
# Expose port 80
EXPOSE 80

# Start Apache in foreground
CMD ["apachectl", "-D", "FOREGROUND"]
