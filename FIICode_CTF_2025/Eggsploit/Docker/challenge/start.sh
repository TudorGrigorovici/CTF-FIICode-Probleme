#!/bin/bash

# Start internal Flask app (on port 5001)
cd /apps/internal
python3 0d6e4079e36703ebd37c00722f5891d28b0e2811dc114b129215123adcce3605.py &

# Start main Flask API (on port 5000)
cd /apps/api
python3 main.py &

# Start Apache in foreground
cd /var/www/html
apachectl -D FOREGROUND