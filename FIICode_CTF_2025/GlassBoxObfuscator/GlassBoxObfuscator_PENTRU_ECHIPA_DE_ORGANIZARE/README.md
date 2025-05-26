# Rezolvare

## Flag
flagul este: FIICODE25{r3v3rs3_and_dump_ram}

## Descriere
```
trebuie cautat in fisierul mare dupa MAGICAES; dupa acest MAGICAES, primii 32 bytes vor fi key, urmatorii 16 vor fi iv, urmatorul byte va fi xor_key
textul de decryptat este in overlay (incepand cu pozitia 0x3888 din fisier, 48 de caractere)
cipher = AES.new(aes_key, AES.MODE_CBC, aes_iv).decrypt(ch)
dupa, se face xor pe chiper cu xor_key;
dificultatea consta in intelegerea codului si trecerea peste layerele anti-debug; nu este obfuscat
```