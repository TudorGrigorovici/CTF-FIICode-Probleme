# Robots-Sapiens
Category: **Web**
Difficulty: **Easy**

## Descriere:
```
Fan Specii la facultatea de info...

Autor: SOC
```

## Hints:
1. Robots
2. Flagul e in /flag


### Cum merge treaba:
- robots.txt e un fisier facut special pentru web scrapere ca sa le zica ce pagini pot sa acceseze si ce pagini nu pot.
- acolo apara si file-uri mai interesante
- vezi ca sunt 2 pathuri acolo si iei sa le vezi pe ambele
- la flagul.php exista un arbitrary file read si doar trebuie sa folosesti un payload simplut ../../../../../../../../../../../../etc/passwd (in general asa se verifica arbitrary file read)
- din robots.txt vezi ca zice de un file important de pe sistem
- le folosesti pt admin.php si cam aia e

### Flag: FIICODE25{R0b0S4pi3ns_ar3_c00l}