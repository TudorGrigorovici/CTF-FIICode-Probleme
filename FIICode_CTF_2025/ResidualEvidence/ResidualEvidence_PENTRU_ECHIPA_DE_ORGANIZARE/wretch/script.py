import os

handler = open("wretch.sys", "rb")
content = handler.read()
handler.close()

s = ""

for x in content:
    s += chr(((x ^ 123) + 13) ^ 13)

handler = open("flag.txt", "w")
handler.write(s)
handler.close()