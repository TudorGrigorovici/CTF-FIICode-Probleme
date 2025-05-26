# ResidualEvidence

- Categorie: Forensics, RE, Misc
- Dificultate: Hard
- Autor: Drd. Asist. Gheorghe Balan,  Prof. dr. Alboaie Lenuta

## Descriere

Internul companiei, aflat in perioada de practica, cu access la soft educational, a fost surprins de colegi copiind ceva pe un stick USB in timpul unei pauze. Dupa cateva zile, unul dintre servere a fost compromis, dar logurile de acolo nu mai sunt disponibile.
Singura piesa de proba ramasa este imaginea creata din acel stick USB. Echipa de securitate ti-a pus-o la dispozitie.
Ne poti ajuta sa intelegem ce facea internul cu acel stick si daca exista vreo legatura cu compromiterea serverului?

Imagine dispozitiv: https://drive.google.com/file/d/1ilK7fbE_OHQpLDO0Xs-NPRAZbRTCmvr6/view?usp=sharing

Hints: 
- imagine de Windows - vezi ce a descarcat internul


## Rezolvare

target install: Windows 11 Education (se utilizeaza fie dism, fie instalare) <- pe celelalte versiuni posibile nu este

in folder-ul user downloads (pentru userul Public) sunt 4 imagini tematice:
- but_beware_monsters.png 
- deceiver.png - Un monstru din FROM ce contine un scris format din -, [a-z] si .
- la o scurta inspectie a fisierului binar se observa, la finalul acestuia, you_can_start_Take_a_look_in_windows_folder
- hello_there.png 
- lets_take_a_short_walk.png 

- in folderul Windows, avem adaugat werewolf.exe (care e trecut ca system file si hidden si nu e vizibil din explorerul normal, trebuie un tool mai avansat)
- (trebuie facut un RE usor si gasesc ca e dependent de fisierul “werewolf_low_points.txt”). 
- Citeste date din acel fisier, si in functie de dimensiune citita afiseaza pe ecran un string din urmatoarele: appcompat, apppatch, AppReadiness, DigitalLocker, ELAMBKUP, Fonts, GameBarPresenceWriter, Resources, schemas, Web;
- Secretul este sa numere punctele “low” din imaginea deceiver (fara cele de la i) si sa scrie in acel fisier un string de dimensiune 6

- in fiecare din folderele amintite mai sus se vor regasi un fisier, VanHelsing.exe (RE):
- doar folderul GameBarPresenceWriter va contine si un fisier aditional (“inga.txt”) cu un hint pentru  pasii urmatori
- contine un fisier txt cu textul “Invention, it must be humbly admitted, does not consist in creating out of void, but out of chaos. What a strange substracting mail client…”, textul e encryptat cu un XOR cu 123 (ar trebui sa fie o referinta catre Frankenstein)
- executabilelor vor afisa un “I'm sorry to inform you, but the key is not in this folder!” daca se indeplineste o conditie simpla (existenta unei valori specifice, 123 in cheia de registru HKLM\\Software\\VanHelsing, “VanHelsingBAD”, altfel vor afisa “Please make sure that you understood the assignment!”)
- scopul aici este sa descopere cheia de registru: “Software\\Clients\\Mail\\Frankenstein” din HKLM ce contine o cheie cu un executabil base64 encoded (se face decode pe base64 si dupa trebuie un XOR decrypt pentru a scoate executabilul; cheie e un word, primul byte 123, al doilea e numar de minus (13) din deceiver.png -> ar trebui sa iasa si cu bruteforce daca nu se prind din hint)
- executabilul contine algorimul de encryptie al flagului (ceva simplu) ce se bazeaza pe 3 key (key1 = 123, key2= 13, key3 are un hint, numarul de degete ale monstrilor (5 din poza beware_of_monsters.png + 8 din deceiver.png). 

## Flag

Flagul este: FIICODE25{7ou_re_not_@fr@id_ar3_n_t_yo$?_just_look_b3hind_y@u}

