# SYSTEMD

Skup programa [systemd](https://wiki.archlinux.org/title/systemd) je skup alata za upravljanje sustavom i servisima. Servisi, u kontekstu *systemda* poznati još kao i *daemon-i*, su programi koji su pokrenuti u pozadini. Neki od alata koje nam nudi *systemd* su:

* systemctl - upravljanje servisima sustava
* hostnamectl - postavljanje imena sustava
* timedatectl - upravljanje s vremenom sustava
* journalctl - ispis log zapisa sustava

Za distribucije koje koriste skup alata *systemd* proces *systemd* je prvi proces koja Linux jezgra pokreće nakon što se učita u RAM (ID procesa je 1). Proces koji se prvi pokreće nakon učitavanja Linux jezgre u RAM se zove [*init*](https://wiki.archlinux.org/title/init) proces (inicijalni proces).

## Upravljanje sustavom uz pomoć systemda

Objekt kojim upravlja *systemd* naziva se *Unit* te su oni opisani uz pomoć Unit datoteka. Upravljanje *Unit-ima* se radi uz pomoć *systemctl* alata. Par korisnih naredbi su:

- ```systemctl status [Unit datoteka]```
	- ispisuje status *Unita*
- ```systemctl start [Unit datoteka]```
	- pokreće Unit
- ```systemctl stop [Unit datoteka]```
	- zaustavlja Unit
- ```systemctl restart [Unit datoteka]```
	- ponovno pokreće Unit
- ```systemctl reload [Unit datoteka]```
	- ponovno učitava konfiguraciju aplikacije *Unita* ako je to moguće
- ```systemctl enable [Unit datoteka]```
	- omogućuje Unit odnosno stvara simboličku poveznicu na njega
- ```systemctl disable [Unit datoteka]```
	- onemogućuje Unit odnosno uklanja simboličku poveznicu
- ```systemctl daemon-reload```
	- ponovno učitava sve Unit datoteke koje se nalaze u gore navedenim direktorijima u *systemd*

Također, alat *systemctl* sadrži i mogućnost upravljanja napajanja računala (gašenje, ponovno pokretanje...):

- ```systemctl reboot```
	- ponovno pokreni sustav
- ```systemctl poweroff```
	- gasi sustav
- ```systemctl suspend```
	- postavi sustav u stanje mirovanja
- ```systemctl hibernate```
	- postavi sustav u stanje hibernacije (piši RAM na disk)
- ```systemctl hybrid-sleep```
	- piši RAM na disk i onda postavi sustav u stanje mirovanja
- ```systemctl suspend-then-hibernate```
	- postavi sustav u stanje mirovanja te ga nakon nekog konfiguriranog vremena probudi i postavi u stanje hibernacije
- ```systemctl soft-reboot```
	- ponovno pokreni sustav ali ne tako što se ponovno učitava jezgra već se ponovno pokreću svi servisi

Simboličke poveznice na neke od ovih naredbi su već napravljene tako da ih je moguće pozvat jednostavno naredbama ```reboot``` i ```poweroff```.

### Unit datoteke

[Unit datoteke](https://www.digitalocean.com/community/tutorials/understanding-systemd-units-and-unit-files) predstavljaju objekte kojima odnosno *Unite* kojima upravlja *systemd*. Postoji par vrsta *Unita* odnosno Unit datoteka koji ih opisuju. Neke od tih vrsta su:

- *service*
	- opisuje kako upravljati servisom
	- najčešća vrsta Unit datoteke
- *socket*
	- opisuje mrežni priključak ili priključak za međuprocesnu komunikaciju (*eng. IPC - Inter Process Communication*)
- *device*
	- opisuje uređaj koji je upravljan uz pomoć [*udev*](https://wiki.archlinux.org/title/udev) upravljača uređaja koji je dio *systemda*
- *mount*
	- definira točku montiranja kojem će upravljati *systemd*
	- zapise u */etc/fstab* sustav *systemd* automatski montira pri svakom pokretanju sustava
- *target*
	- pruža sinkronizacijske točke za druge Unit datoteke
	- *systemd* dolazi s uobičajenim ciljevima koji predstavljaju razine pokretanja (*eng. runlevels*)
- *timer*
	- definira vremenski brojač (*eng. timer*) koji će biti upravljan *systemdom*
- *slice*
	- koristi se za Linux kontrolne grupe

Unit datoteke u imenu imaju nastavak vrstu *Unita* kojeg predstavljaju (primjerice *.service*, *.target*, ...).

### Razine pokretanja

Razina pokretanja (*eng. runlevel*) je način rada operacijskog sustava računala. Okruženje *systemda* nudi par takvih razina koji se ovdje nazivaju ciljevima (*eng. target*) i predstavljeni su Unit datotekama. Ciljevi služe za grupiranje Unit datoteka i njihovih ovisnosti te služe kao sinkronizacijske točke. Najbitniji ciljevi su:

- *poweroff.target*
	- zaustavlja sustav
- *rescue.target*
	- način rada jednog korisnika (*root* korisnika)
- *multi-user.target*
	- višekorisnički način rada
- *graphical.target*
	- višekorisnički, grafički način rada
- *reboot.target*
	- ponovno pokreće sustav
- *emergency.target*
	- način rada koji pokreće ljusku za hitne slučajeve

Ciljevi su predstavljeni Unit datotekama s nastavkom *.target*. Prilikom pokretanja *systemda* sustav ima postavljen uobičajeni cilj (*eng. default target*) koji specificira koju grupu servisa treba pokrenuti. Ako se želi saznati koji je uobičajeni cilj to se može učiniti naredbom:

```
systemctl get-default
```

Postavljanje uobičajenog cilja može se napraviti naredbom:

```
systemctl set-default [.target u /etc/systemd/system direktoriju]
```

Ova naredba stvara novu simboličku poveznicu ```/etc/systemd/system/default.target``` na odgovarajuću ```.target``` Unit datoteku. Ali ovo nije jedini način kako sustavu reći koji je uobičajeni cilj. Moguće mu je postaviti uobičajeni cilj kao argument Linux jezgre pri pokretanju tako što mu se može dodati kao parametar ```systemd.user=[.target u /etc/systemd/system direktoriju]```. Općenito, ako je uobičajeni cilj postavljen kao argument jezgre onda se samo taj cilj gleda. Ako ne postoji argument jezgre za uobičajeni cilj onda se gleda uobičajeni cilj na poveznici */etc/systemd/system/default.target*. I za kraj ako ni jedno između to dvoje nije postavljeno onda se gleda */usr/lib/systemd/system/default.target* kao poveznica na uobičajeni cilj.

### Lokacija Unit datoteka

Unit datoteke se nalaze na [sljedećim lokacijama](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/9/html/using_systemd_unit_files_to_customize_and_optimize_your_system/assembly_working-with-systemd-unit-files_working-with-systemd) poredanih po prioritetima:

- */etc/systemd/system*
	- najviši prioritet
- */run/systemd/system*
	- runtime Unit datoteke
- */lib/systemd/system*
	- ovdje su Unit datoteke instaliranih aplikacija

Ovdje */etc/systemd/system* ima najviši prioritet što znači da prepisuje istoimene Unit datoteke na drugim lokacijama.

### Općeniti sadržaj Unit datoteka

Unit datoteke se organiziraju po sekcijama čiji se početak označava parom uglatih zagrada (```[``` i ```]```). Ispod oznake početka nalaze se zapisi u obliku ključ-vrijednost (```direktiva=vrijednost```). Ako se želi koristiti neka uobičajena vrijednost nekog ključa to se može napraviti tako što se vrijednost ostavi praznom (```direktiva=```) ili je se uopće ne navede.

#### Direktive sekcije Unit

Prva sekcija u većini Unit datoteka je sekcija ```[Unit]``` koja definira metapodatke. Potrebno je pripaziti na označavanje sekcija jer su osjetljiva na velika i mala slova. Najbitnije direktive sekcije Unit su:

- *Description=*
	- tekstualni opis što radi *Unit*
- *Documentation=*
	- URI za dokumentaciju
- *Requires=*
	- drugi izlistani *Uniti* o kojem ovaj Unit ovisi
	- Uniti **moraju** biti zajedno pokrenuti
- *Wants=*
	- slično kao *Requires*, ali ako izlistani *Uniti* ne započnu s radom ovaj Unit će svejedno započeti s radom
	- **preferirano** je da *Uniti* budu zajedno pokrenuti ali **nije nužno**
- *BindsTo=*
	- slično kao *Requires*, ali omogućuje zaustavljanje Unita ako se zaustavi jedan od izlistanih *Unita*
- *Before=*
	- izlistani *Uniti* neće započinjati s radom dok se ne pokrene trenutni *Unit*, ova direktiva definira **eksplicitni redoslijed pokretanja**
- *After=*
	- trenutni *Unit* neće započeti s radom dok ne započnu svi izlistani *Uniti*, ova direktiva definira **eksplicitni redoslijed pokretanja**
- *Conflicts=*
	- lista *Unita* koji ne mogu biti pokrenuti istovremeno s trenutnim *Unitom*

Više o direktivama za sekciju ```[Unit]``` može se naći [ovdje](https://www.freedesktop.org/software/systemd/man/latest/systemd.unit.html).

#### Direktive sekcije Install

Zadnja sekcija u većini Unit datoteka je sekcija ```[Install]``` koja definira ponašanje u slučaju da je Unit datoteka omogućena ili onemogućena. Prilikom pokretanja računala *systemd* uopće ne čita ```[Install]``` sekciju već to radi naredba ```systemctl [enable|disable] [ime Unit datoteke]```. Neke od bitnih direktiva su sljedeće:

- *RequiredBy=*
	- u direktoriju /etc/systemd/system/* stvara (ako već nije stvorio) direktorij [vrijednost].requires u kojem će stvoriti simboličku poveznicu na trenutnu Unit datoteku
	- vrijedi pravilo da se trenutni *Unit* **mora** pokrenuti s navedenom listom drugih *Unita*
- *WantedBy=*
	- u direktoriju /etc/systemd/system/* stvara (ako već nije stvorio) direktorij [vrijednost].wants u kojem će stvoriti simboličku poveznicu na trenutnu Unit datoteku
	- vrijedi pravilo da se **preferira** pokretanje trenutnog *Unita* s *Unitima* navedenim u listi, ali **nije nužno**
	- recimo da je u nekoj Unit datoteci postavljena direktiva na ovaj način *WantedBy=multi-user.target.wants*:
		- u direktoriju */etc/systemd/system/* stvorit će se direktorij *multi-user.target.wants* (ako već nije) i u njemu će se stvoriti simbolička poveznica na trenutnu Unit datoteku
		- ovo znači da će se trenutna Unit datoteka pokušati pokrenuti kada se želi doseći cilj *multi-user.target* ako je tako postavljen uobičajeni cilj
- *Alias=*
	- omogućuje korištenje više imena za trenutnu Unit datoteku

Više o direktivama za sekciju ```[Install]``` može se naći [ovdje](https://www.freedesktop.org/software/systemd/man/latest/systemd.unit.html). Općenito ako pišemo vlastitu Unit datoteku i razrješavamo ovisnosti potrebno je koristiti direktive poput ```WantedBy=``` iz ```[Install]``` sekcije u toj istoj Unit datoteci umjesto da ulazimo u samu Unit datoteku ovisnosti na koju referenciramo i pišemo joj direktive poput ```Wants=``` iz ```[Unit]``` sekcije.

#### Sekcija Service i njezine direktive

Sekcija ```[Service]``` je sekcija namijenjena samo za Unit datoteke vrste *.service*. Njezine bitne direktive su:

- *Type=*
	- kategorizira servis po ponašanju
	- neke vrijednosti koje mogu biti su:
		- simple
			- najjednostavnija vrsta servisa
		- forking
			- ako servis stvara servise djecu
		- oneshot
			- kada servis radi kratkotrajno i sa završetkom
			- korisna direktiva koja se parira s ovom vrijednosti je *RemainAfterExit=yes* koja označava servisom aktivnim iako mu je glavni proces uspješno prestao s radom
- *ExecStart=*
	- put do programa s argumentima koji započinju servis
- *ExecStartPre=*
	- dodatne naredbe koje bi se izvodile prije direktive *ExecStart=*
- *ExecStartPost=*
	- dodatne naredbe koje bi se izvodile poslije direktive *ExecStart=*
- *ExecReload=*
	- naredba koja bi ponovno učitala konfiguraciju samog servisa (uz pomoć ```systemctl reload [ime servisa]```)
	- ova opcija ne mora biti dostupna svim servisima, ovisi od aplikacije do aplikacije
	- bit je da nije potrebno izvršiti ponovno pokretanje servisa već samo ponovno učitavanje servisa (primjerice kod web poslužitelja)
- *User=*
	- koji korisnik će izvršavati naredbe u direktivama poput *ExecStart=*
	- ako se ova stavka ne navede onda se naredbe izvršavaju u ime korisnika *root*

Više o direktivama za sekciju ```[Service]``` može se naći [ovdje](https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html).

### Uređivanje postojećih Unit datoteka

U slučaju da se želi urediti neka direktiva postojeće datoteke, primjerice u */etc/systemd/system/* direktoriju, potrebno je napraviti u istom direktoriju direktorij imena *[ime Unit datoteke].d/* i u njega staviti datoteku s nastavkom *.conf*. Ovakve datoteke se nazivaju *drop-in* datoteke. Takve *drop-in* datoteke nadjačavaju konfiguraciju u Unit datotekama. Primjerice, ako se želi nadjačati neku direktivu potrebno je napisati sljedeće u *drop-in* datoteku:

```
[ime sekcije u kojoj se nalazi ta direktiva]
[direktiva]=[vrijednost koja će se nadodati]
```

Ako se želi isprazniti direktiva i samo dodati nešto novo to se može napraviti na sljedeći način:

```
[ime sekcije u kojoj se nalazi ta direktiva]
[direktiva]=
[direktiva]=[nova vrijednost]
```

Kako bi se *systemd* učitao novu konfiguraciju potrebno je izvršiti naredbu ```systemctl daemon-reload```. Postoji i drugi način uređivanja *drop-in* datoteka. To se može napraviti naredbom ```systemctl edit [Unit datoteka] --drop-in=[ime .conf datoteke]``` (direktorij */etc/systemd/system/[ime Unit datoteke].d* će se stvorit automatski ako već nije). Ovo će rezultirati stvaranjem *.conf* datoteke u */etc/systemd/system/[ime Unit datoteke].d/* direktoriju imena *[ime .conf datoteke].conf*. Ako smo koristili ovakav pristup nije potrebno izvršavati ```systemctl daemon-reload```. Isto tako ako argument *--drop-in* nije naveden datoteka će se zvati *override.conf*.

Brisanje svih *drop-in* datoteka za pojedinačnu Unit datoteku može se napraviti naredbom ```systemctl revert [ime Unit datoteke]```.

### Stvaranje novih Unit datoteka

Preporučeno je vlastite Unit datoteke stavljati u direktorij */etc/systemd/system/*. Nakon svakog uređivanja tih Unit datoteka potrebno je izvršiti naredbu ```systemctl daemon-reload```.

#### Primjer vlastite Service Unit datoteke

Recimo da želimo jednostavan servis koji pri svakom pokretanju računala nadodaje u datoteku *echo-date.txt* u korisnikovom *home* direktoriju trenutni datum i vrijeme (naredba ```date```). Također prilikom izvršenja naredbe ```systemctl reload [ime Unit datoteke]``` servis treba ponovno nadodati u tu istu datoteku trenutni datum i vrijeme. Servis isto tako mora biti označen aktivnim iako je uspješno završio s izvođenjem. Za kraj *systemd* mora osigurati da je trenutni Unit pokrenut prije nego li se dođe do cilja *multi-user.target*. Dakle, potrebno je stvoriti datoteku *EchoDate.service* u direktoriju */etc/systemd/system/* i popunit ga sljedećim sekcijama i direktivama:

```
[Unit]
Description=Echoing date to a file on start or reload of service.

[Service]
User=[korisničko ime]
Type=oneshot
ExecStart=/bin/bash -c 'echo "$(date): Service started" >> ~/echo-date.txt'
ExecReload=/bin/bash -c 'echo "$(date): Service reloaded" >> ~/echo-date.txt'
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
```

Konfiguracija se u *systemd* učitava naredbom ```systemctl daemon-reload```. Servis se pokreće naredbom:

```
systemctl start EchoDate.service
```

Ova naredba će pokrenuti naredbu navedenu pod *ExecStart=* direktivom. Naredba za ponovno učitavanje konfiguracije aplikacije servisa je:

```
systemctl reload EchoDate.service
```

Naredba će pokrenuti naredbu koja je navedena u direktivi *ExecReload*. Ako se želi omogućiti pokretanje aplikacije servisa pri svakom ponovnom pokretanju računala to se može napraviti naredbom:

```
systemctl enable EchoDate.service
```

Nakon svakog ponovnog pokretanja računala izvršit će se naredba pod *ExecStart=* direktivom. Ako se servis želi u potpunosti ukloniti potrebno je prvo zaustaviti servis, onemogućiti ga i izbrisati mu *Unit* datoteku, ponovno učitati konfiguraciju u *systemd* i resetirati stanja drugih Unita kojima su se možda dogodile greške zbog nestanka Unit datoteke koja im je možda ovisnosti (ovo se neće dogoditi u ovom slučaju). To se radi nizom naredbi:

```
systemctl stop EchoDate.service
systemctl disable EchoDate.service
rm /etc/systemd/system/EchoDate.service
systemctl daemon-reload
systemctl reset-failed
```

Za naš primjer nemojmo zaboraviti obrisati datoteku *echo-date.txt* naredbom ```rm ~/echo-date.txt```.
