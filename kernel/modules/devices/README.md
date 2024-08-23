# LINUX MODULI KAO UPRAVLJAČKI PROGRAMI UREĐAJA

Na Unix sustavima pa tako i Linux sustavima svaki hardver je predstavljen datotekom uređaja u */dev/* direktoriju. Na zahtjev modula koji imaju ulogu upravljačkih programa jezgra stvara sučelja kojima korisnički programi mogu komunicirati s hardverom preko upravljačkih programa. Naime, pozivom primjerice funkcije *write()* ili *read()* iz korisničkog programa nad jednom datotekom uređaja, poziva se implementacija *write()* ili *read()* implementacije upravljačkog programa odnosno modula koji dalje komunicira s hardverom. Svaka datoteka uređaja ima dodijeljena dva broja od jezgre:

* veliki (*eng. major*) broj - označava broj upravljačkog programa kojem resurs pripada, jezgra svakom modulu dodjeljuje jedinstveni broj
* mali (*eng. minor*) broj - označava broj resursa koji pripada upravljačkom programu, jednom upravljačkom programu može pripasti više resursa

Veliki i mali brojevi pojedinih resursa odnosno datoteka uređaja mogu se vidjeti naredbom ```ls -l /dev/```. Kada se datoteci uređaja pristupa (operacijom otvaranja, pisanja, čitanja, zatvaranja) jezgra prvo gleda veliki broj pa time nalazi upravljački program odgovoran za implementaciju navedene operacije. Ovo znači da sam upravljački program niti ne zna koji mu je veliki broj dodijeljen. Međutim, upravljačkom programu je bitan mali broj kojem može razlikovat resurs kojem je namijenjena operacija.

Općenito, uređaji se dijele na tri vrste:

* znakovni uređaji (*eng. character devices*) - čitanje i pisanje se radi u bajtovima
* blok uređaji (*eng. block devices*) - imaju međuspremnik koji sprema zahtjeve programa za čitanje i pisanje te se podatci pišu i čitaju u blokovima (SATA HDD, SATA SSD, NVME SSD, ...), datoteke uređaja su najčešće oblika */dev/sda*, */dev/sdb*, ..., */dev/nvme0*, */dev/nvme1*, ...
* mrežna sučelja

Koristeći naredbu ```ls -l /dev/``` znakovni uređaji označeni su oznakom **c**, blok uređaji oznakom **b**. Mrežna sučelja nemaju svoje datoteke u ```/dev/``` direktoriju već ih se upravlja uz pomoć Linux mrežnog podsustava.

Linux nudi i definiciju klase uređaja (*eng device class*). Klasa uređaja omogućuje lakšu organizaciju uređaja po sličnim ulogama i daje nekakvu višu apstrakciju nad uređajima. Definicija klasa se nalazi u */sys/class/* direktoriju virtualnog datotečnog sustava *sysfs*. Direktorij */sys/* izlaže strukture Linux jezgre, razne informacije i konfiguracije sustava korisniku. Klase u direktoriju */dev/class* sadrže simboličke poveznice na datoteke uređaja koje pripadaju tim klasama.

Korisni primjeri:

* najosnovniji primjer: [example-1](example-1)
* primjer s implementacijom *ioctl()* funkcije: [example-2](example-2)
* primjer s implementacijom jezgrinog FIFO međuspremnika: [example-3](example-3)

