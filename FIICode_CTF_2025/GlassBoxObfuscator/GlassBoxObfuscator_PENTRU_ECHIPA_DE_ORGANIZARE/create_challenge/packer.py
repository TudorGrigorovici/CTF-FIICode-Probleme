#!/usr/bin/env python3
"""
Builds the hardened CTF challenge:

    python3 packer.py loader  challenge

Steps
  * Generate random AES-256 key & IV
  * Generate random 1-byte XOR key for the VM
  * Assemble VM byte-code (MOV+PRINT+HALT) with junk NOPs
  * XOR-encode only the *code* section (flag stays clear)
  * Pad, encrypt with AES-CBC (PKCS#7)
  * Embed key, IV, XOR-key into the loader
  * Write  <loader><ciphertext><len_LE>  ? challenge
"""

import os, sys, struct, random
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

MAGIC  = b"MAGICAES"
FLAG   = b"FIICODE25{r3v3rs3_and_dump_ram}"      # <-- put your flag here

# --- helpers ----------------------------------------------------------
def put32(val: int) -> bytes:
    return struct.pack("<I", val)

# --- build VM stub with junk NOPs -------------------------------------
def build_stub(flag: bytes, xor_key: int) -> bytes:
    code = bytearray()

    def add_nops():
        for _ in range(random.randint(0, 2)):   # sprinkle 0-2 NOPs
            code.append(0x00)                   # real NOP (pre-XOR)

    # MOV R0, <offset>
    code += b"\x10\x00" + b"\x00\x00\x00\x00"
    add_nops()

    # PRINT R0
    code += b"\x21\x00"      #aici e pus 21 in loc de 20
    add_nops()

    # HALT
    code += b"\xFF"

    # patch offset to flag (after code length)
    offset = len(code) 
    code[2:6] = put32(offset)

    # assemble final blob: <code><flag><0>
    blob = bytearray(code) + flag + b"\x00"

    # XOR-encode only the code part
    for i in range(len(blob)):
        blob[i] ^= xor_key

    return bytes(blob)

# --- main -------------------------------------------------------------
def main():
    if len(sys.argv) != 3:
        print("usage: packer.py <loader> <challenge>", file=sys.stderr)
        sys.exit(1)

    loader_path, out_path = sys.argv[1:]

    loader = bytearray(open(loader_path, "rb").read())

    # per-build secrets
    aes_key = os.urandom(32)
    aes_iv  = os.urandom(16)
    vm_key  = random.randint(1, 255) & 0xFF

    # patch key / iv / vm_key into the loader
    idx = loader.find(MAGIC)
    if idx < 0:
        sys.exit("MAGIC tag not found in loader")

    loader[idx + 8              : idx + 8 + 32] = aes_key
    loader[idx + 8 + 32         : idx + 8 + 32 + 16] = aes_iv
    loader[idx + 8 + 32 + 16] = vm_key

    handler = open("generated_aes_key", "wb")
    handler.write(aes_key)
    handler.close()

    handler = open("generated_aes_iv", "wb")
    handler.write(aes_iv)
    handler.close()

    handler = open("generated_vm_key", "wb")
    handler.write(bytes([vm_key]))
    handler.close()

    # assemble & encrypt stub
    stub_plain = build_stub(FLAG, vm_key)
    stub_padded = pad(stub_plain, AES.block_size)
    cipher = AES.new(aes_key, AES.MODE_CBC, aes_iv).encrypt(stub_padded)

    handler = open("generated_chiper", "wb")
    handler.write(cipher)
    handler.close()

    # final packet: <loader><cipher><len_LE>
    packet = loader + cipher + put32(len(cipher))
    open(out_path, "wb").write(packet)
    print(f"[+] challenge       : {out_path}")
    print(f"    size            : {len(packet)} bytes")
    print(f"    VM XOR-key      : 0x{vm_key:02x}")

if __name__ == "__main__":
    main()
