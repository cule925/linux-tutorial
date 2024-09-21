# UPRAVLJANJE DATOTEKAMA I DIREKTORIJIMA

U većini instalacija Linux distribucija korisnici instaliraju Linux distribuciju na particiju koja koristi [*EXT4*](https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout) datotečni sustav. Struktura direktorija Linux distribucija ravna se po [FHS-u (*eng. File System Hierarchy Standard*)](https://wiki.gentoo.org/wiki/Filesystem_Hierarchy_Standard) uz male preinake ovisno o distribuciji. Generalno od korijenskog direktorija (/) situacija je sljedeća:

- */bin/*
	- sadrži temeljne binarne izvršne datoteke za upravljanje sustavom (npr. mkdir, rmdir, touch, rm, ...)
- */sbin/*
	- sadrži temeljne binarne izvršne datoteke za upravljanje sustavom koje samo privilegirani korisnik može koristiti
- */boot/*
	- sadrži datoteke potrebne za podizanje sustava (npr. kompresirana jezgra Linuxa, GRUB bootloader, initramfs, ...)
- */dev/*
	- direktorij gdje je montiran virtualni datotečni sustav tipa *devtmpfs*
	- sadrži datoteke uređaja, u Unix filozofiji sve je datoteka pa tako su i uređaji datoteke nad kojima možemo vršiti razne operacije
- */etc/*
	- sadrži datoteke koje definiraju razne konfiguracije za programe na razini sustava
- */home/*
	- sadrži korisničke direktorije, svaki novostvoreni korisnik ima svoj direktorij u */home/* direktoriju
- */lib/*
	- sadrži temeljne biblioteke potrebne za rad binarnih izvršnih datoteka u */bin/* i */sbin/*
- */lib32/*
	- sadrži biblioteke za 32-bitne izvršne datoteke
- */lib64/*
	- sadrži biblioteke za 64-bitne izvršne datoteke
- */media/*
	- točke montiranja vanjskih datotečnih sustava, npr. na USB-u ili CD-u
- */mnt/*
	- služi za ručno montiranje datotečnih sustava
- */opt/*
	- lokacija gdje se smješta ručno instalirani softver, bez nekog upravitelja paketa (iako se mogu naći i neke datoteke vezane uz pakete ovdje)
- */proc/* 
	- direktorij gdje je montiran virtualni datotečni sustav tipa *proc*, sadrži informacije o pokrenutim procesima i informacije o jezgri
- */root/*
	- direktorij korisnika *root*
- */run/*
	- direktorij gdje je montiran virtualni datotečni sustav tipa *tmpfs*
	- ovdje se spremaju runtime informacije (npr. tko je ulogiran, pokrenuti servisi, ...) za procese kojima je ovo potrebno
- */srv/*
	- sadrži datoteke za primjerice FTP poslužitelj, web poslužitelj i slično
- */sys/*
	- direktorij gdje je montiran virtualni datotečni sustav tipa *sysfs*
	- sadrži informacije o sustavu: uređaji, upravljački programi i značajke jezgre
- */tmp/*
	- direktorij gdje je montiran virtualni datotečni sustav tipa *tmpfs*
	- služi za spremanje pomoćnih datoteka raznih procesa
- */usr/*
	- sadrži aplikacije instalirane uz pomoć upravitelja paketa (*/usr/bin/*, */usr/sbin/*, */usr/lib/*, ...)
- */var/*
	- sadrži datoteke za koje se očekuje da će rasti, primjerice log datoteke

## GNU i Linux

Linux je ime za jezgru koju je u početku razvio Linus Torvalds ([izvorni kod](https://github.com/torvalds/linux)). [GNU Project](https://www.gnu.org/software/software.html) je inicijativa pokrenuta od Richarda Stallmana kojoj je cilj bio razviti operacijski sustav koji je sličan Unixu. GNU projekt je razvio mnoge alate za upravljanje operacijskim sustavom, ali im je nedostajala jezgra. Tu je poslužila Linux jezgra. Kad se netko referira na Linux često se misli na GNU alate i Linux jezgru zajedno.

Operacijski sustav služi kao posrednik između aplikacija i sklopovlja. Linux jezgra upravlja hardverom i resursima sustava, primjerice upravljanje procesima, upravljanje memorijom (*eng. Memory Management Unit - MMU*), upravljanje uređajima, datotečnim sustavom. Aplikacije rade zahtjeve za pristup nekim resursima uz pomoć sustavskih poziva (*eng. system calls*).

## Terminal i ljuska

Terminal je tekstno sučelje koje se može koristiti za pisanje podataka u računalo i čitanje podataka s računala. Prije se na terminal mislilo na sučelje koje pruža fizički uređaj zvan teleprinter (*eng. teletypewriter*), ali je taj uređaj zamijenio računalni terminal, a kasnije emulator terminala na računalima. Ljuska je program koji interpretira naredbe korisnika (primjerice pokretanje aplikacije) koje on upisuje u emulator terminala i tako šalje zahtjeve Linux jezgri. Najpoznatija ljuska koja se koristi u Linuxu je [bash](https://www.gnu.org/software/bash/) koji je dio GNU softvera.

U Linuxu, terminali su predstavljeni datotekama uređaja u */dev/* direktoriju: *tty0*, *tty1*, *tty2*, ..., ovo su virtualni terminala i korisnik se može prebacivati s između njih kombinacijom tipki *Ctrl + Alt + F1...F7. Međutim, ako imamo instalirano grafičko sučelje, najčešće se preko njega koriste pseudo terminali (programsko emulirani terminali - primjerice *kgx*) predstavljeni s datotekama uređaja *pts/N*.

Ako smo instalirali neku Linux distribuciju bez grafičkog sučelja, prilikom pokretanja računala pojavit će nam se terminal za prijavu. To je upravo jedan od virtualnih terminala *ttyX*. Ako je kojim slučajem instalirano grafičko sučelje, upravitelj zaslona (primjerice *gdm* ako je instaliran *GNOME* desktop) će proslijediti korisničko ime i lozinku terminalu i korisnik će se na kraju opet prijaviti preko pseudo terminala.

## Osnovne naredbe za rad preko terminala koristeći ljusku bash

Sljedeće naredbe su se izvodile koristeći ljusku bash. Direktoriji su u Linuxu organizirani kao stablo počevši od korijenskog direktorija (*/ - root*). Nad tim stablom se na određenim lokacijama mogu montirati datotečni sustavi s drugih particija. Ovo znači da smo efektivno produžili granu stabla prikvačivši stablo direktorija s druge particije na jednu od grana stabla direktorija trenutne particije. Primjerice, stablo direktorija prve razine na particiji gdje je instalirana Linux distribucija može izgledati ovako:

```
/
├── bin
├── boot
├── dev
├── etc
├── home
├── lib
├── lib64
├── lost+found
├── mnt
├── opt
├── proc
├── root
├── run
├── sbin
├── srv
├── sys
├── tmp
├── usr
└── var
```

Svaka naredba u terminalu obično ima sljedeći oblik:

```
[naredba] [opcije] [argumenti]
```

Opcije utječu na ponašanje naredbe i često su oblika:

* kratke, počinju s -
* duge, počinju s --

U slučaju da se želi saznati kako koristiti naredbu i koje su joj mogućnosti, naredbe najčešće dolaze s ugrađenim opcijama ```-h``` (kratka) i ```--help``` (duga). Također, postoji i Linux sustav uputa (*eng. man pages*) gdje se može saznati više informacija o samoj naredbi. To se može učiniti pisanjem naredbe:

```
man [informacije o naredbi koja nas zanima]
```

### Najosnovnije naredbe

U Linuxu se položaj direktorija može zadati:

* apsolutno, od korijena (*/*)
* relativno, od direktorija u kojem se nalazimo

#### Promjena direktorija i ispis trenutnog direktorija

Promjena direktorija radi se s bash ugrađenom naredbom *cd*. Ovo znači da se ova naredba ne nalazi u */bin* ili */usr/bin* direktoriju, već je dio bash ljuske. Oblik naredbe je:

```
cd [položaj direktorija]
```

Druga naredba koja je ugrađena u ljusku je *pwd* koja ispisuje trenutni direktorij u kojem se ljuska nalazi. Oblik joj je jednostavno ```pwd```.

##### Posebne oznake za direktorije

Postoje već rezervirane oznake za direktorije:

* oznaka *~* pokazuje na korisnikov *home* direktorij
* oznaka *.* pokazuje na trenutni direktorij
* oznaka *..* pokazuje na roditeljski direktorij trenutnog direktorija, u slučaju korijenskog direktorij ovo pokazuje opet na sam korijen jer on nema roditeljski direktorij

#### Izlistavanje direktorija

Naredba za izlistavanje direktorija je naredba *ls*. Opcija *-l* izlistava više informacija o direktorijima i datotekama:

```
ls -l [direktorij|datoteka]
```

Dodavanje opcija *-h* (čitljiviji format) i *-a* (ispis skrivenih direktorija i datoteka) može se napisati ovako:

```
ls -lah [direktorij|datoteka]
```

Ispis vrste datoteke može se napraviti dodavanjem opcije *-d*. U Linuxu je sve datoteka pa i direktoriji.

#### Stvaranje direktorija

Stvaranje direktorija može se napraviti naredbom *mkdir*:

```
mkdir [ime direktorija]
```

Ako se želi stvoriti niz direktorija gdje je svaki sljedeći poddirektorij prethodnog to se može opcijom *-p*:

```
mkdir -p [ime direktorija]
```

#### Uklanjanje direktorija

Uklanjanje direktorija može se napraviti naredbom *rmdir*:

```
rmdir [direktorij]
```

Ova naredba će uspjeti samo ako je direktorij prazan, inače je potrebno koristiti naredbu *rm* s opcijom *-r*,

#### Stvaranje datoteka

Na Linux (i Unix) sustavima zapravo je sve datoteka (i direktoriji su također datoteke). Stvaranje prazne datoteke je moguće naredbom *touch*:

```
touch [ime datoteke]
```

Poveznice su specijalne vrste datoteka. Dvije su vrste:

* hard - datoteka na sustavu postoji sve dok se sve poveznice ovakvog tipa koje pokazuju na tu datoteku ne obrišu
* soft - ako se obriše temeljna datoteka na koje ova poveznica pokazuje poveznica više ne funkcionira (pokazuje na datoteku koja ne postoji)

Poveznice vrste *hard* se stvaraju naredbom:

```
ln [datoteka na koju pokazuje] [ime poveznice]
```

Poveznice vrste *soft* se stvaraju dodajući opciju *-s* naredbom:

```
ln -s [datoteka na koju pokazuje] [ime poveznice]
```

#### Brisanje datoteka

Brisanje datoteka i poveznica može se napraviti naredbom *rm*:

```
rm [datoteka ili poveznica]
```

U slučaju da se želi obrisati direktorij koji sadrži u sebi datoteke ili pak prazni direktorij, potrebno je dodati opciju *-r* za rekurzivno brisanje:

```
rm -r [direktorij]
```

Postoji i opcija *-f* koja prisilno briše datoteke bez pokazivanja greški ako datoteka na postoji.

#### Kopiranje datoteka i direktorija

Kopiranje datoteka i direktorija radi se naredbom *cp*. U slučaju datoteka:

```
cp [datoteka] [putanja gdje se kopira]
```

Putanja gdje se kopira može biti put do direktorija. U tom slučaju će se datoteka kopirati u direktorij. Ako putanja navodi i ime neke datoteke koja postoji ili ne postoji, ta datoteka će postati datoteka koja se kopira.

Ako se kopira direktorij potrebno je dodati zastavicu *-r* za rekurzivno kopiranje:

```
cp -r [direktorij] [putanja gdje se kopira]
```

Ako putanja gdje se kopira pokazuje na direktorij koji postoji, direktorij će se kopirati u direktorij koji postoji. Ako putanja koja pokazuje gdje treba kopirati direktorij ne postoji on će biti stvoren kao kopija prvog, ali drugim imenom.

#### Premještanje datoteka i direktorija

Premještanje datoteka i direktorija radi se naredbom *mv*. U slučaju datoteka:

```
mv [datoteka] [putanja gdje se premješta]
```

Putanja gdje se kopira može biti put do direktorija. U tom slučaju će se datoteka ili direktorij premjestiti u taj direktorij. Ako putanja navodi i ime neke datoteke ili direktorija koji ne postoji, ta datoteka ili direktorij će postati datoteka ili direktorij koji se premješta.

#### Ispis sadržaja datoteka

Postoji nekoliko naredbi za ispis sadržaja datoteka. Jedna od njih je naredba *cat*. Oblik joj je:

```
cat [datoteka]
```

U slučaju da je datoteka prevelika, moguće je koristiti naredbu *head* za ispis redova počevši od početka i *tail* za ispis redova počevši od kraja:

```
head -n [broj redaka za ispisati od početka datoteke] [datoteka]
```

```
tail -n [broj redaka za ispisati od kraja datoteke] [datoteka]
```

Ako se želi ispisivati sadržaj datoteke stranicu po stranicu, može se koristiti naredba *more* ili novija *less*. Ove naredbe koristi *man*. Izlazak iz pregledavanja radi se pritiskom tipke q. Oblici naredbi su:

```
more [datoteka]
```

```
less [datoteka]
```

#### Ispis zauzeća po particijama

Ispis za zauzeće datoteka po particijama radi se naredbom *df*. Najčešće se koristi i opcija *-h* radi čitljivijeg ispisa:

```
df -h
```

## Korisnici i grupe

U [Linuxu postoji više korisnika](https://wiki.archlinux.org/title/users_and_groups#). Svaki korisnik ima svoj UID (*eng User ID*) koji je jedinstven. Također, svaki Linux uvijek ima privilegiranog korisnika *root* kojem je UID 0. Datoteka gdje su izlistani korisnici je datoteka [*/etc/passwd*](https://wiki.archlinux.org/title/users_and_groups#File_list) koja sadrži jedan zapis oblika:

```
[korisničko ime]:[lozinka]:[UID]:[GID]:[informacije]:[matični direktorij]:[korisnička ljuska]
```

Ako na mjestu lozinke postoji znak *x* to znači da je lozinka premještena u posebnu datoteku */etc/shadow* radi sigurnosti. Lozinkama je u tom slučaju izračunat sažetak (*eng. hash*) i takvi su sažetci spremljeni u toj datoteci s raznim drugim informacijama slično zapisima u */etc/passwd*:

```
[korisničko ime]:[hash lozinke]:[informacije]
```

Uređivanje datoteke */etc/passwd* preporučljivo je raditi uređivačem *vipw* koji osigurava zaštitu od paralelnog pisanja.

### Ispis informacija o korisniku

Ispis informacija o prijavljenom korisniku može se dobiti naredbom:

```
who -a
```

Kraći oblik ove naredbe je ```whoami``` koji ispisuje samo korisničko ime korisnika koji je prijavljen na terminal. Prikaz još informacija o korisniku može se dobiti naredbom:

```
pinky -l [ime korisnika]
```

Ova naredba izravno čita datoteku */etc/passwd*. Još informacija o korisnicima može se dobiti naredbom ```w```.

### Mijenjanje korisnika i privremeno dobivanje administrativnih ovlasti

Naredba *su* služi za mijenjanje korisnika odnosno stvaranje nove instance ljuske u kojem je prijavljen drugi korisnik. Dva su bitna oblika naredbe:

```
su [korisničko ime]
```

* zadržava varijable okoline

```
su - [korisničko ime]
```

* stvara nove varijable okoline svojstvene korisniku

Postoji i sučelje za privremeno dobivanje administrativnih ovlasti *sudo*. Sučelje mogu koristiti svi korisnici prema dozvolama iz datoteke */etc/sudoers* koja se radi zaštite paralelnog pisanja uređuje naredbom ```visudo```.

### Korisnici i grupe

Grupe imaju vlastiti jedinstveni GID (*eng Group ID*). Svaki korisnik pripada primarnoj grupi koja je najčešće istog imena kao i korisnik te je zapisana u */etc/passwd* datoteci kao GID uz korisničko ime. Međutim, korisnik može pripadati više grupa koje se zovu sekundarne grupe. Lista grupa nalazi se u datoteci */etc/group* koji sadrži zapise oblika:

```
[ime grupe]:[lozinka]:[GID]:[lista korisnika koji pripadaju ovoj grupi odvojeni zarezima]
```

Ako na mjestu lozinke postoji znak *x* to znači da je lozinka premještena u posebnu datoteku */etc/gshadow* radi sigurnosti. Naredbom *id* može se saznati kojoj grupi trenutni korisnik pripada:

```
id
```

### Stvaranje novog korisnika

Stvaranje novog [korisnika](https://wiki.archlinux.org/title/users_and_groups#Example_adding_a_user) može se napraviti naredbom:

```
useradd [korisnik]
```

Dodavanjem zastavice *-m* korisniku se stvara i njegov *home* direktorij (*/home/[korisničko ime]/*):

```
useradd -m [korisnik]
```

Prilikom stvaranja korisnikovog *home* direktorija, kostur poddirektorija i datoteka koji će se stvoriti definiran je u */etc/skel/* direktoriju.

### Brisanje korisnika

Brisanje korisnika može se napraviti naredbom:

```
userdel [korisnik]
```

Brisanje korisnika i njegovog *home* direktorija:

```
userdel -r [korisnik]
```

### Stvaranje nove grupe

Stvaranje nove [grupe](https://wiki.archlinux.org/title/users_and_groups#Group_management) može se napraviti naredbom:

```
groupadd [grupa]
```

### Brisanje grupe

Brisanje grupe može se napraviti naredbom:

```
groupdel [grupa]
```

### Dodavanje korisnika u grupu

Korisnika se može dodati u postojeću grupu naredbom:

```
usermod -aG [grupa] [korisnik]
```

Opcija *-a* označava dodavanje korisnika u grupu, dok opcija *-G* označava da iza te opcije slijedi lista grupa razmaknute zarezima.

### Uklanjanje korisnika iz grupe

Korisnika se može ukloniti iz postojećih grupa naredbom:

```
usermod -rG [grupa] [korisnik]
```

Opcija *-a* označava uklanjanje korisnika iz grupe, dok opcija *-G* označava da iza te opcije slijedi lista grupa razmaknute zarezima.

## Informacije o datotekama

Svaka datoteka sadrži mnogo informacija odnosno [metapodataka](https://wiki.archlinux.org/title/users_and_groups#Permissions_and_ownership) o njoj primjerice vrijeme stvaranja, izmjene i pristupa, kome pripada, koje dozvole ima i slično. Naredba ```ls -l``` izlistava par karakterističnih polja:

- mode
	- oblik ---------- od 10 bitova gdje:
		- prvi bit predstavlja vrstu datoteke, ništa za običnu datoteku, *d* za direktorij, *c* za znakovni uređaj, *b* za blok uređaj, ...
		- sljedeća tri bita predstavljaju *rwx* (*eng. Read Write Execute*) dopuštenja za vlasnika korisnika datoteke koji omogućuju čitanje, pisanje i izvršavanje datoteke
		- nakon toga sljedeća tri bita predstavljaju *rwx* za vlasnika grupu datoteke
		- zadnja tri bita predstavljaju *rwx* dopuštenja za korisnike koji nisu vlasnici datoteke niti pripadaju navedenoj grupi datoteke
- vlasnik korisnik
	- korisnik koji je vlasnik datoteke
- vlasnik grupa
	- grupa koja dijeli vlasništvo nad tom datotekom

### Promjena dozvola nad datotekom

Promjena dozvola nad datotekom moguće je naredbom *chmod* na mnogo načina:

* dodaj dozvole vlasniku korisniku: ```chmod u+[r|w|x] [datoteka]```
* ukloni dozvole vlasniku korisniku: ```chmod u-[r|w|x] [datoteka]```
* dodaj dozvole vlasniku grupi: ```chmod g+[r|w|x] [datoteka]```
* ukloni dozvole vlasniku grupi: ```chmod g-[r|w|x] [datoteka]```
* dodaj dozvole ostalima: ```chmod o+[r|w|x] [datoteka]```
* ukloni dozvole ostalima: ```chmod o-[r|w|x] [datoteka]```

Za rekurzivno mijenjanje pravila recimo nad nekim direktorijem dodaje se opcija *-R*:

```
chmod -R [dopuštenja] [direktorij]
```

Kad je *r* postavljen nad direktorijem to nam daje pravo izlistavanja direktorija. Ako je *w* postavljen to nam daje pravo stvaranja i brisanja datoteka u tom direktoriju. I na kraju kad je *x* postavljen to nam daje pravo premještanja u taj direktorij.

#### Posebne dozvole: X

Dozvola *X* je koristan nad rekurzivnim promjenama dopuštenja nad direktorijem jer postavlja samo direktorijima *x* zastavicu dok ostale vrste datoteka ne dira po tom pitanju. Primjerice:

```
chmod -R [u|g|o]+X [direktorij]
```

#### Posebne dozvola: T sticky bit

Dozvola *T* označava da je brisanje direktorija dozvoljeno samo vlasniku i root korisniku dok kod datoteka označava da proces ostaje u memoriji nakon izvršavanja (ovo više nije relevantno). Dozvola se nalazi u *o* grupi na trećem mjestu. Ako je kojim slučajem aktivan i *x* u isto vrijeme na istom mjestu, na tom mjestu će biti oznaka *t*. Primjer naredbe je:

```
chmod o+t [direktorij]
```

Ne postoji mogućnost ručno dodati dopuštenje *T* već je potrebno ručno dodavati i micati opciju *x*.

#### Posebne dozvola: SUID i SGID

Ako se neka datoteka pokreće kao proces ovo znači da korisnik koji ju je pokrenuo ima sve ovlasti kao i vlasnik korisnik ili vlasnik grupa datoteke. Ovo pokazuje dozvola *s* ili *S* na trećem mjestu grupe *u* ili grupe *g* ovisno jeli u isto vrijeme bila aktivna *x* dozvola (*s* ako je i *x* aktivan, *S* ako *x* nije aktivan). Primjer postavljanje dozvole korisniku:

```
chmod u+s [datoteka]
```

Primjer postavljanje dozvole grupi:

```
chmod g+s [datoteka]
```

Slično kao i kod *sticky bita*, ne postoji mogućnost ručno dodati dopuštenje *S* već je potrebno ručno dodavati i micati opciju *s*.

### Promjena vlasnika korisnika datoteke

Promjena vlasnika korisnika datoteke može se napraviti naredbom *chown* na način:

```
chown [korisnik] [datoteka]
```

### Promjena vlasnika grupu datoteke

Promjena vlasnika grupu datoteke može se napraviti naredbom *chgrp* na način:

```
chgrp [grupa] [datoteka]
```

### Istovremeno mijenjanje korisnika i grupe

Moguće je istovremeno promijeniti korisnika i grupu datoteke jednom naredbom:

```
chown [korisnik]:[grupa] [datoteka]
```

### MAC vremena

Svaka datoteka ima definirana tri vremena (tzv. MAC vremena):

* vrijeme zadnje promjene (mtime), primjerice pisanje u datoteku
* vrijeme zadnjeg pristupa (atime), primjerice otvaranje datoteke ili ispis sadržaja datoteke
* vrijeme zadnje promjene metapodataka datoteke (ctime), primjerice promjena ime datoteke

Naredba *touch* postavlja sva tri vremena datoteka na istu vrijednost. Sve tri informacije ispisuje naredba *stat*:

```
stat [datoteke]
```

## Preusmjeravanje ulaza i izlaza

Naredbe koje se izvršavaju preko ljuske su procesi. U Linuxu, svi procesi imaju definirane [sljedeće ulaze i izlaze](https://www.digitalocean.com/community/tutorials/an-introduction-to-linux-i-o-redirection):

* standardni ulaz (*eng. stdin*)
* standardni izlaz (*eng. stdout*)
* standardni izlaz za greške (*eng. stderr*)

Svi ovi ulazi su vezani za terminal. Proces komunicira s tim ulazima i izlazima uz pomoć opisnika datoteka (*eng. file descriptors*). Opisnici datoteka za *stdin* *stdout* i *stderr* su redom 0, 1 i 2. Ako proces želi pisati u datoteku ili čitati iz datoteke, on mora otvoriti novi opisnik datoteke koji će imati redni broj 3 ili više za svaki sljedeći otvoreni opisnik.

### Preusmjeravanje u datoteku

Recimo da izlaz neke naredbe želimo spremiti u datoteku. Ovo bi uključivalo preusmjeravanje *stdouta* naredbe na *stdin* programa koji stvara i piše u datoteku. To se može napraviti operatorom *>*:

```
[naredba] > [datoteka]
```

Međutim, ovaj će operator svaki put iznova prepisati datoteku. Ako se želi nadodati nešto na datoteku to se može napraviti operatorom *>>*:

```
[naredba] >> [datoteka]
```

Preusmjeravanje izlaza za greške *stderr* naredbe u datoteku operatorom *2>*:

```
[naredba] 2> [datoteka]
```

Nadodavanje izlaza za greške *stderr* naredbe u datoteku operatorom *2>>*:

```
[naredba] 2>> [datoteka]
```

Preusmjeravanje izlaza za greške *stderr* naredbe na gdje god ide standardni izlaz *stdout* naredbe:

```
[naredba] [>> ili >] [datoteka] 2>&1
```

### Preusmjeravanje iz datoteke

Ako se želi sadržaj neke datoteke preusmjeriti u naredbu odnosno *stdout* naredbe ispisivanja datoteke je potrebno proslijediti na ulaz *stdin* neke naredbe, to se može operatorom *<*:

```
[naredba] < [datoteka]
```

### Preusmjeravanje iz datoteke u datoteku

Ako se želi preusmjerit izlaz ispisa datoteke na ulaz pisanja u datoteku, to se može napraviti naredbom:

```
cat < [prva datoteka] > [druga datoteka]
```

Efektivno smo napravili kopiranje.

### Preusmjeravanje između programa uz pomoć cjevovoda

Ako se preusmjeravati između naredbi ili se možda želi ulančavati preusmjeravanje, to se može napraviti uz pomoć cjevovoda (*eng. pipe*):

```
[naredba 1] | [naredba 2] | [naredba 3] | ...
```

Efektivno smo dobili:

```
stdin 1 --- stdout 1 -> stdin 2 --- stdout 2 -> stdin 3 --- stdout 3 -> ...
```

#### Preusmjeravanje u datoteku uz pomoć cjevovoda

Preusmjeravanje u datoteku s cjevovodom se može napraviti uz pomoć naredbe *tee*:

```
[naredba] | tee [datoteka]
```

Ako se želi nadodati u datoteku potrebno je dodati argument *--append*:

```
[naredba] | tee --append [naredba]
```

### Here document

Dio skriptnog koda koji se tretira kao zasebna datoteka. Sastoji se od operatora *<<* i graničnika po želji (često EOF). Primjerice, s preusmjeravanjem u datoteku:

```
cat > [datoteka] <<EOF
[sadržaj koji će se upisati u datoteku]
EOF
```

## Statusni kodovi

Naredbe pri završetku vraćaju neki završni kod od 0 do 255. Broj 0 označava uspješno izvršenje koda dok ostali brojevi označavaju greške. Naredbe se mogu ulančati tako da se njihovo izvršavanje uvjetuje. Primjerice ako želimo ulančati naredbe tako da svaka ovisi o uspješnom izvršetku prethodne to se može napraviti operatorom *&&*:

```
[naredba 1] && [naredba 2] && [naredba 3] && ...
```

Ako se žele ulančati naredbe tako da svaka ovisi o neuspješnom završetku prethodne to se može napraviti operatorom *||*:

```
[naredba 1] || [naredba 2] || [naredba 3] || ...
```

## Manipulacija tekstualnim datotekama

Postoje mnogi alati za dobivanje raznih informacija o tekstualnim datotekama.

### Brojanje znakova, riječi i linija

Brojanje znakova, riječi i linija može se koristiti naredba *wc* s opcijama *-m* *-w* i *-l*. Oblik naredbe je:

```
wc [-m|-w|-l] [datoteka]
```

### Pretraživanje teksta

Pretraživanje teksta radi se naredbom *grep*. Često korištene opcije su *-i* gdje se ignoriraju velika i mala slova i *r* gdje se rekurzivno pretražuju direktoriji. Oblik naredbe je:

```
grep [opcije] [uzorak] [datoteka]
```

### Sortiranje teksta

Sortiranje teksta može se napraviti naredbom *sort*. Primjer naredbe za sortiranje:

```
sort [opcije] [datoteka]
```

### Duplikati redove

Za traženje i manipulaciju redova koristi se naredba *uniq*. Neke korisne opcije su primjerice *-u* koja ispisuje linije koje se ne ponavljaju i *-d* koja ispisuje linije koje se ponavljaju. Oblik naredbe je:

```
uniq [opcije] [datoteka]
```

### Izdvajanje pojedinih polja redova

U slučaju da se želi izdvojiti pojedina polja u redovima nekog teksta za to će nam poslužiti naredba *cut*. Neke od korisnih opcija su *-d* koji definira znak razgraničenja između polja i *-f[N]* definicija koja polja propustiti na izlaz počevši od 1.

## Pretraga po datotečnom sustavu

U slučaju da se želi vršiti pretraga po stablu direktorija, par naredbi nam je tu od koristi.

### Naredba za pronalazak izvršne datoteke naredbe:

Naredbe za pronalazak izvršnih datoteka naredbi su naredbe *which* i *whereis* (nalazi i lokaciju stranice uputa). Najjednostavniji oblik *which* naredbe je: ```which [naredba]```, a *whereis* naredbe: ```whereis [naredba]```.

### Naredba za pronalazak raznih tipova datoteka

Naredba *find* je jedna kompleksna naredba koja služi za pretraživanje direktorija i datoteka. Opći oblik joj je: ```find [opcije] [direktoriji] [uvjeti]```. Jedni od najkorištenijih uvjeta su:

* *-name [ime]*
* *-type [vrsta datoteke]*
* *-size [veličina datoteke]*

