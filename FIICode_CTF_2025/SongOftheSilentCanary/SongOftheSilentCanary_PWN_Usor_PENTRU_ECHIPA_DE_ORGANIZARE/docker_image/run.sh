docker build -t secure-pwn .
docker run -d --name secure-pwn \
  --read-only --cap-drop=ALL \
  --security-opt no-new-privileges:true \
  --pids-limit 64 --memory 128m \
  -p 1337:1337 \
  secure-pwn