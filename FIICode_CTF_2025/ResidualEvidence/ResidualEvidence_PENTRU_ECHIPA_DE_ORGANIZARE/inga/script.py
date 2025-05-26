import os

handler = open("orig.txt", "rb")
content = handler.read()
handler.close()


ohandler = open("orig.txt.encrypted", "wb")
for x in content:
    ohandler.write((x ^ 123).to_bytes())
ohandler.close()