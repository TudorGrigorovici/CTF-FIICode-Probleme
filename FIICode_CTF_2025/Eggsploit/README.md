# Eggsploit
Difficulty: **Hard**
Category: **Web**

## Description:
```
Gaseste lantul de vulnerabilitati...

Autor: SOC
```

## Flag: FIICODE25{ch1ck3n_pwn_p13_tod4y_only_at_c0c0ric0}

## Solve:

- sql injection on the orders page leaks credentials and a part of the flag
- the admin page allows for SSRF to an internal app and an api
- api has an outdated api version vulnerable to arbitrary read
- arbitrary read allows for credentials extraction (the token)
- use token to gain access to the internal app which is vulnerable to SSTI
- SSTI (last part)
```python
{{config.items()}}
```

## Misc:
```sql
sqlite> CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL
)
   ...> ;
sqlite> CREATE TABLE flag (flag_pt_1 TEXT);
sqlite> INSERT INTO flag (flag_pt_1) VALUES('FIICODE25{ch1ck3n');
INSERT INTO users (username,password) VALUES ('admin','Fall3nChicken');
```
