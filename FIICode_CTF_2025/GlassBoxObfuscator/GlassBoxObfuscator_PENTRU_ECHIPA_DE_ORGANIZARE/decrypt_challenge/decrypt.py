#are nevoie sa dropezi de mana (pentru validare), 48 de bytes de la pozitia 0x3888

import os, sys, struct, random
from Crypto.Cipher import AES

handler = open("challenge.drop", "rb")
ch = handler.read()
handler.close()
            
handler = open("generated_aes_iv", "rb")
aes_iv = handler.read()
handler.close()

handler = open("generated_aes_key", "rb")
aes_key = handler.read()
handler.close()

handler = open("generated_vm_key", "rb")
vm_key = int.from_bytes(handler.read())
handler.close()


cipher = AES.new(aes_key, AES.MODE_CBC, aes_iv).decrypt(ch)
xx = b""
for i in cipher:
    xx += bytes([i ^ vm_key])
print(xx)

h = open("decoded_sequence", "wb")
h.write(xx)
h.close()