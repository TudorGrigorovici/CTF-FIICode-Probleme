def crc32(data):    
    data_bytes = data.encode()
    table = []
    for i in range(256):
        crc = i
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xEDB88320
            else:
                crc >>= 1
        table.append(crc)
    crc = 0xFFFFFFFF
    for byte in data_bytes:
        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF]
    return crc

def rc4(key, data: bytes) -> bytes:
    S = list(range(256))
    j = 0
    key_length = len(key)
    key = key.encode()    

    for i in range(256):
        j = (j + S[i] + key[i % key_length]) % 256
        S[i], S[j] = S[j], S[i]

    i = j = 0
    result = bytearray()

    for byte in data:
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        k = S[(S[i] + S[j]) % 256]
        result.append(byte ^ k)

    return bytes(result)

def decrypt_labyrinth(key, fname, output):
    with open(fname, 'rb') as f:
        data = f.read()

    decrypted_data = rc4(key, data)
    with open(output, 'wb') as f:
        f.write(decrypted_data)

if __name__ == "__main__":
    solution = "DLLLLLLLLDDLD"
    numeric_key = crc32(solution)
    key = hex(numeric_key).upper()[2:]
    print(key)
    fname = "labiirinte/1_21/in_1.bin"
    decrypt_labyrinth(key, fname, "decriptat.txt")
