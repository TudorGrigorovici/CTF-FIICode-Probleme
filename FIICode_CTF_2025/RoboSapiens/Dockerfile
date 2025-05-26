FROM debian:latest

# necessary install
RUN apt-get update 
RUN apt upgrade -y
RUN apt install apache2 php libapache2-mod-php -y

# get the web pages and all that jaz
COPY app/ /var/www/html/

RUN rm /var/www/html/index.html

RUN mkdir /chimie
RUN touch /chimie/.parole

# Flagul ala legendar
COPY creds /chimie/.parole
# expose access port
EXPOSE 80

# run apache server
CMD ["apachectl", "-D", "FOREGROUND"]