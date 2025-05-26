# Vibe Coder
Category: **Web**
Difficulty: **Medium**

## Description:
```
"Cand la gratii ma gandesc\nBrusc ma inveselesc - Ada Milea"

Autor: SOC
```

Users get access to the page, that's all, no source code.

## Solve:
- Robots.txt reveals /dev.
- In /dev you'll find an sqlite3 data base.
- There you'll find the other user.
- Brute force the JWT token to get access to it's account page (the user has an "artist" flag, so it can upload files).
- Bypass the upload filter to get a reverse shell. (extention and magic bytes)
- Read the flag from "/flag.txt"


### Misc:
```sql
CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, password TEXT NOT NULL, artist BOOLEAN);
INSERT INTO users(name,password,artist) VALUES("AdaMilea","27337ff43fdd493f53d46d958fa71dece63f4feb6b3c98d244184a9d61a234ea",1);-- sha256 for "ManeaCuLanturi"
```

### Flag: FIICODE25{lantur1_p3_php}
