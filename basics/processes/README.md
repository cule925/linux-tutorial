# PROCESI

Program je kod namijenjen za izvođenje na nekom računalu. [Proces](https://helix979.github.io/jkoo/post/os-scheduler/) je program koji se trenutno izvodi. Svaki proces ima svoj adresni prostor u RAM-u koji mu je dodijelio operacijski sustav. Sigurnosni mehanizmi operacijskih sustava zabranjuju jednom procesu pristup adresnom prostoru drugog procesa osim ako to nije izričito dopušteno inter-procesnom komunikacijom (*eng. Inter Process Communication*). Adresni prostor procesa je tipično podijeljen u sljedeće dijelove:

- text
	- sadrži strojni kod
- data
	- sadrži inicijalizirane globalne varijable i konstante
- bss
	- sadrži neinicijalizirane globalne varijable
- heap
	- gomila, služi za dinamičku alokaciju memorije pozivom sistemskih funkcija (*eng. syscall*) poput *malloc()*, *calloc()* ili *free()*
- stack
	- stog, služi za spremanje povratne adrese prilikom poziva funkcije i alokaciju lokalnih varijabli

Proces se dijeli u manje jedinice zvane dretve koje dijele istu memoriju. Dretva je izvršni dio koda koji se izvodi na procesoru. Svaki proces ima barem jednu dretvi. Svaka dretva unutar koja pripada procesu ima svoj stog, ali dijele sve ostale dijelove adresnog procesa ostalim dretvama istog procesa. U kontekstu Linux jezgre, dretva se poistovjećuje sa zadatkom (*eng. task*).

## Stvaranje procesa

Stvaranje novih procesa se u Linux jezgri radi uz pomoć sistemskih poziva *fork()* i *exec()*. Svaki proces ima svog procesa roditelja (*eng. parent process*) osim *init* procesa kojeg je pokrenula Linux jezgra. Sistemski poziv *fork()* stvara novi proces koji se naziva proces dijete (*eng. child process*) i dodjeljuje mu kopiju memorijske tablice (*eng. memory table*) koji pokazuje na memorijske segmente odnosno stranice procesa koji je pozvao sistemski poziv *fork()*. Na neki način sada ova dva procesa dijele memoriju ali samo dok je čitaju. Svaki segment je u ovom slučaju označen sa zastavicom *COPY ON WRITE* što znači da u slučaju da proces dijete ili proces roditelj želi nešto pisati po toj memoriji procesu koji piše će se ukloniti pokazivač na taj segment i dodijelit će mu se novi segment u kojem će se nalaziti prava kopija prethodnog segmenta po kojem je proces namjeravao pisati. Također će se ukloniti zastavica *COPY ON WRITE* iz izvornog segmenta. Ovakav pristup služi efikasnom korištenju memorije.

Inače, *fork()* u izvedu i proces roditelj i proces dijete. Kod procesa roditelja *fork()* vraća procesni identifikator (PID) djeteta dok kod djeteta ta funkcija pri uspješnom izvršavanju vrati 0. U slučaju da je izvođenje funkcije *fork()* rezultiralo greškom u roditelju se vraća -1.

Funkcija *fork()* zapravo stvara kopiju procesa koji se izvodi. U slučaju da se želi učitati novi program proces dijete mora pozvati sistemsku funkciju *exec()* i kao argument joj dati putanju do binarne izvršne datoteke. Prethodna memorijska tablica će prestati pokazivati na sve memorijske segmente bili to segmenti koje je još dijelila s procesom roditeljem ili zasebne segmente koje je dobila kršeći zastavicu *COPY ON WRITE* koji će se osloboditi. Zatim će se procesu djetetu dodijeliti novi adresni prostor koji će imati učitan novi program iz binarne izvršne datoteke.

Primjer pozivanja funkcije *fork()*:

```
                                                            RAM nakon što proces                       RAM nakon što dijete pokušava pisati
                   RAM                                   roditelj 5000 izvede fork()                      po stogu, događa se kopiranje
                                                        i PID procesa djeteta bude 6000                  segmenta i uklanja se zastavica
                                                                                                                  COPY ON WRITE

        |                        |                        |                        |                        |                        |
        |          ...           |                        |          ...           |                        |          ...           |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |   STOG procesa 6000    |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |      STOG procesa      |                        |                        |
        |   STOG procesa 5000    |                        |      5000 i 6000       | COPY ON WRITE          |   STOG procesa 5000    |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |      KOD procesa       |                        |      KOD procesa       |
        |    KOD procesa 5000    |                        |      5000 i 6000       | COPY ON WRITE          |      5000 i 6000       | COPY ON WRITE
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |     GOMILA procesa     |                        |     GOMILA procesa     |
        |  GOMILA procesa 5000   |                        |      5000 i 6000       | COPY ON WRITE          |      5000 i 6000       | COPY ON WRITE
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |     GOMILA procesa     |                        |     GOMILA procesa     |
        |  GOMILA procesa 5000   |                        |      5000 i 6000       | COPY ON WRITE          |      5000 i 6000       | COPY ON WRITE
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |          ...           |                        |          ...           |                        |          ...           |
        |                        |                        |                        |                        |                        |

```

Primjer pozivanja funkcije *fork()* i *exec()*:

```
                                                            RAM nakon što proces                               RAM nakon što proces
                   RAM                                   roditelj 5000 izvede fork()                           dijete pozove exec()
                                                        i PID procesa djeteta bude 6000

        |                        |                        |                        |                        |                        |
        |          ...           |                        |          ...           |                        |          ...           |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |  GOMILA procesa 6000   |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |   STOG procesa 6000    |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |      STOG procesa      |                        |                        |
        |   STOG procesa 5000    |                        |      5000 i 6000       | COPY ON WRITE          |   STOG procesa 5000    |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |    KOD procesa 6000    |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |      KOD procesa       |                        |                        |
        |    KOD procesa 5000    |                        |      5000 i 6000       | COPY ON WRITE          |    KOD procesa 5000    |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |     GOMILA procesa     |                        |                        |
        |  GOMILA procesa 5000   |                        |      5000 i 6000       | COPY ON WRITE          |  GOMILA procesa 5000   |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |     GOMILA procesa     |                        |                        |
        |  GOMILA procesa 5000   |                        |      5000 i 6000       | COPY ON WRITE          |  GOMILA procesa 5000   |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |                        |                        |                        |                        |                        |
        |________________________|                        |________________________|                        |________________________|
        |                        |                        |                        |                        |                        |
        |          ...           |                        |          ...           |                        |          ...           |
        |                        |                        |                        |                        |                        |

```

Može se uočiti da su svi procesi potekli od nekog roditeljskog procesa osim *init* procesa:

```
                     PID 1
                   /       \
               PID 34     PID 65
             /        \          \
        PID 123      PID 99     PID 177
          ...         ...        ...
```

U slučaju da proces roditelj želi čekat na dovršetak jednog od procesa djeteta to se može napraviti sistemskim pozivom *wait()* u procesu roditelja. Sistemski poziv vraća PID djeteta koji je završio s izvođenjem.

### Procesna tablica

Prilikom poziva funkcije *fork()* stvara se zapis u [procesnoj tablici](https://exposnitc.github.io/os_design-files/process_table.html) koja se nalazi u RAM-u u prostoru Linux jezgre. Neke informacije koje taj zapis sadrži su:

- PID (*eng. process identificator*)
	- identifikator procesa
- PPID (*eng. parent process identificator*)
	- identifikator procesa roditelja
- USERID
	- identifikator korisnika koji je pokrenuo proces
- INODE INDEX
	- pokazivač na *inode* binarne izvršne datoteke koja se izvodi kao proces
- STATE
	- u kojem je stanju proces (RUNNING, READY, ...)

#### Zombi procesi

Zombi procesi (*eng. zombie processes*) su procesi koji su završili s izvođenjem, ali im se zapis još nalazi u procesnoj tablici.

#### Procesi bez roditelja

Procesi bez roditelja (*eng. orphaned processes*) su procesi čiji su roditelji završili s izvođenjem i kao takvima im se dodjeljuje novi roditelj s PID-om 1 odnosno *init* proces.

## Stvaranje dretvi

Dretva je dio izvršnog koda nekog procesa i ona je najmanja jedinica koja se može rasporediti pomoću raspoređivača procesa. Također, iz ugla Linux jezgre PID zapravo označava identifikator dretve a TGID (*eng. Thread Group ID*) proces kojem pripada ta dretva:

```

        _______________________________________________________                                          ______________________________________________________________________________________________________________________________
        |                                                     |                                          |                                                                                                                            |
        |                       PROCES 1                      |     ========== fork() ========== >>>     |                                                          PROCES 2                                                          |
        |                                                     |                                          |                                                                                                                            |
        |     ___________________________________________     |                                          |     ___________________________________________                            ___________________________________________     |
        |     |                                         |     |                                          |     |                                         |                            |                                         |     |
        |     |                 DRETVA 1                      |                                          |     |                                         |                            |                                         |     |
        |     |                                         |     |                                          |     |                                         |                            |                                         |     |
        |     |     _______________________________     |     |                                          |     |     _______________________________     |                            |     _______________________________     |     |
        |     |     |                             |     |     |                                          |     |     |                             |     |                            |     |                             |     |     |
        |     |     |    *pogled Linux jezgre*    |     |     |                                          |     |     |    *pogled Linux jezgre*    |     | == pthread_create() == >>> |     |    *pogled Linux jezgre*    |     |     |
        |     |     |                             |     |     |                                          |     |     |                             |     |                            |     |                             |     |     |
        |     |     |           PID = 1           |     |     |                                          |     |     |           PID = 2           |     |                            |     |           PID = 3           |     |     |
        |     |     |           TGID = 1          |     |     |                                          |     |     |           TGID = 2          |     |                            |     |           TGID = 2          |     |     |
        |     |     |           PPID = 0          |     |     |                                          |     |     |           PPID = 1          |     |                            |     |           PPID = 1          |     |     |
        |     |     |                             |     |     |                                          |     |     |                             |     |                            |     |                             |     |     |
        |     |     |_____________________________|     |     |                                          |     |     |_____________________________|     |                            |     |_____________________________|     |     |
        |     |     |                             |     |     |                                          |     |     |                             |     |                            |     |                             |     |     |
        |     |     |     *pogled korisnika*      |     |     |                                          |     |     |     *pogled korisnika*      |     |                            |     |     *pogled korisnika*      |     |     |
        |     |     |                             |     |     |                                          |     |     |                             |     |                            |     |                             |     |     |
        |     |     |           TID = 1           |     |     |                                          |     |     |           TID = 2           |     |                            |     |           TID = 3           |     |     |
        |     |     |           PID = 1           |     |     |                                          |     |     |           PID = 2           |     |                            |     |           PID = 2           |     |     |
        |     |     |           PPID = 0          |     |     |                                          |     |     |           PPID = 1          |     |                            |     |           PPID = 1          |     |     |
        |     |     |                             |     |     |                                          |     |     |                             |     |                            |     |                             |     |     |
        |     |     |_____________________________|     |     |                                          |     |     |_____________________________|     |                            |     |_____________________________|     |     |
        |     |                                         |     |                                          |     |                                         |                            |                                         |     |
        |     |_________________________________________|     |                                          |     |_________________________________________|                            |_________________________________________|     |
        |                                                     |                                          |                                                                                                                            |
        |_____________________________________________________|                                          |____________________________________________________________________________________________________________________________|

```

U primjeru vidimo oznake koje koristi Linux jezgra i oznake koje koristi naredba ```ps``` koja se izvršava u korisničkom pogledu:

- PID (*eng. Process ID*) iz pogleda jezgre je jednako TID (*eng. Thread ID*) iz pogleda korisnika
- TGID (*eng. Thread Group ID*) iz pogleda jezgre je jednako PID (*eng. Process ID*) iz pogleda korisnika
- PPID (*eng. Parent Process ID*) iz pogleda jezgre je jednako PID (*eng. Parent Process ID*) iz pogleda korisnika

Obično ćemo koristiti oznake iz pogleda korisnika. 

## Raspoređivanje dretvi

Moderni operacijski sustavi omogućuju pokretanje više dretvi na jednom računalu uz pomoć algoritama raspoređivanja. Kada se dretva izvodi na procesoru ona ima svoje registre stanja te se prilikom zamjene dretve ti registri pohranjuju u memoriju kako bi se znalo gdje je dretva stala s izvođenjem, gdje joj pokazuje pokazivač na stog, koja joj je povratna adresa i slično. Ovo se prilikom zamjene dretve naziva zamjena konteksta (*eng. context switching*). Ulogu odlučivanja koja će se dretva sljedeće izvoditi na procesoru je na raspoređivaču. Većina algoritama raspoređivanje temelji se na prioritetima dretvi. Generalna ideja je da se dretve s većim prioritetom prve izvode dok ove s manjim čekaju. Ovo dovodi do problema izgladnjivanja jer postoji mogućnost da dretve s manjim prioritetom nikako ne dođu na red.

### Stanja dretvi

U jednoprocesorskom sustavu samo se jedna dretva može izvoditi dok ostale moraju čekati na svoj red. Stoga, moguća [stanja dretvi](https://www.baeldung.com/linux/process-states) u Linuxu su:

- *Running*
	- dretva se izvodi na procesoru
- *Uninterruptible sleep*
	- dretva čeka na neki na oslobođenje resursa i ne može se prekinuti signalima
- *Interruptible sleep*
	- dretva čeka na neki na oslobođenje resursa i može se prekinuti signalima
- *Stopped state*
	- dretva se suspendira nakon što joj je poslan *SIGSTOP* signal (npr. programski ```kill -STOP [PID]```) ili *SIGSTP* signal (npr. pritiskom kombinacije tipki CTRL + Z)
	- dretva će izaći iz ovog stanja šaljući joj signal *SIGCONT*
- *Zombie state*
	- dretva je završila s izvođenjem, ali je proces kojoj pripada još uvijek zapisan u procesnoj tablici

### Vrste raspoređivanja

Dvije su vrste raspoređivanja u Linux jezgri:

- CFS (*eng Completely Fair Scheduler*) raspoređivanje
- raspoređivanje u stvarnom vremenu (*eng. Real Time Scheduling*)

#### CFS

CFS (*eng. Completely Fair Scheduler*) je algoritam raspoređivanja dretvi tako što se svakoj dretvi da određeni prioritet i po tom prioritetu se odlučuje koliki će se procesorskog vremena dodijeliti dretvi. Svakoj dretvi je garantirano neko procesorsko vrijeme. Prioriteti u CFS načinu raspoređivanja zovu se *nice* vrijednosti i one uzimaju raspon od -20 do 19 gdje je manji broj veći prioritet. Svaka *nice* vrijednost definira težinu odnosno koliko procesorskog vremena će određena dretva dobiti na raspolaganju. CFS koristi [crveno-crna stabla](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) pri odlučivanju koja dretva je sjedeća za izvođenje.

## Naredbe za upravljanje procesima

Linux jezgra nudi [virtualni datotečni sustav](https://docs.kernel.org/filesystems/proc.html) kao sučelje iz kojih se mogu izvući informacije o procesima iz podatkovnih struktura Linux jezgre. Taj virtualni datotečni sustav je montiran na lokaciji */proc/*. Direktorij */proc/* sadrži poddirektorije koje predstavljaju svaki proces na sustavu. Ti poddirektoriji su nazvani pod PID-u procesa.

### Pretraga i ispis procesa

Jedna od naredbi za ispis procesa je naredba *top*:

```
top
```

Naredba *top* daje ispis korisničkih i jezgrenih procesa u stvarnom vremenu. Osvježavanje se obavlja svake 3 sekunde. Naprednija verzija naredbe je *htop* koja ima naprednije interaktivno sučelje za terminal:

```
htop
``` 

Još jedna od korisnih naredbi je *pstree* koje ispisuje roditelj-djeca stablo procesa:

```
pstree
```

Također, postoji i naredba *pgrep* koja pretražuje procese po imenu:

```
pgrep
```

### Mijenjanje prioriteta procesa

Većina pokrenutih procesa ima *nice* razinu postavljenu na 0. Ako se želi pokrenuti proces s višom ili nižom razinom prioriteta to se može napraviti naredbom *nice*:

```
nice -[cijeli broj od -20 do 19] [izvršna datoteka]
```

Ako se želi promijenit prioritet već pokrenutog programa odnosno procesa to se radi naredbom *renice*:

```
renice [cijeli broj od -20 do 19] [PID]
```

### Slanje signala procesima

Za slanje signala procesima postoji par naredbi. Jedna od njih je naredba *kill*:

```
kill -[ime signala ili kod signala] [PID]
```

Moguće je slati i signale procesima po imenu naredbom *killall*:

```
killall -[ime signala ili kod signala] [ime]
```

Također, ako se želi poslati signal procesu koji u imenu sadrži navedeni uzorak, to se može napraviti naredbom *pkill*:

```
pkill -[ime signala ili kod signala] [uzorak]
```

Neki od signala i njihovi kodovi mogu biti:

- SIGHUP ili 1
	- šalje ga terminal svojim procesima djeci koji se gasi
- SIGINT ili 2
	- šalje ga terminal svojim procesima djeci kada se u terminalu pritisne CTRL + C kombinacija tipki
- SIGUSR1 ili 10
	- korisničko definirana funkcija
- SIGUSR2 ili 12
	- korisničko definirana funkcija
- SIGKILL ili 9
	- trenutačno gašenje procesa
- SIGTERM ili 15
	- programsko gašenje procesa
- SIGCONT ili 18
	- nastavak rada procesa
- SIGSTOP ili 19
	- šalje ga terminal svojim procesima djeci kada se u terminalu pritisne CTRL + Z kombinacija tipki
	- suspendira rad procesa djece

### Ljuska bash i poslovi

Kad se u ljuski pokrene proces ljuska stvara posao (*eng. job*). Posao procesu dodjeljuje atribute kao što su:

* terminal kojem pripada
* datoteke uređaja na koje pokazuje zapisi predstavljeni opisnicima STDIN, STDOUT i STDERR

Terminiranjem ljuske terminiraju se svi poslovi vezani za nju. Ovo znači da se toj grupi procesa šalje isti signal. Ta grupa procesa je zapravo predstavljena PGID (*eng. Process Group ID*) oznakom koji je jednak PID oznaci prvog pokrenutog procesa u toj grupi. Posao može biti pokrenut u prednjem planu (*eng. foreground*) jednostavnim izvršavanjem neke naredbe ili u pozadini:

```
[naredba] &
```

Nakon izvršenja ove naredbe ljuska ispisuje JID (*eng. Job ID*) i PID procesa. Ispis svih poslova koji se izvode u pozadini vezani za trenutni terminal može se napraviti naredbom:

```
jobs
```

Ako se neki posao želi prenijeti u prednji plan ili pokrenuti iz suspendiranog stanja i prenijeti u prednji plan to se može napraviti naredbom:

```
fg [JID]
```

Ako se proces koji je u prednjem planu želi suspendirati i poslati u pozadinu, to je moguće kombinacijom tipki CTRL + Z. Naredba za nastavak rada procesa u pozadini koji je zaustavljen je:

```
bg [JID]
```

## Inter-procesna komunikacija

Inter-procesna komunikacija je mehanizam komunikacije između dva procesa. Postoji nekoliko načina inter-procesne komunikacije u Linuxu:

- cjevovod
	- FIFO (*eng. First In First Out*) red
	- mogu biti imenovani i neimenovani
- red poruka
	- procesi pišu poruke u red i čitaju iz njega
- varijable okruženja
	- prilikom stvaranja procesa proces roditelj prosljeđuje nizove znakova oblika *ime=vrijednost* svakom procesu djetetu
- signali
	- poruke koje operacijski sustav prosljeđuje od jednog procesa do drugog
- pisanje u datoteku
	- procesi mogu pristupati istim datotekama
- mrežni socket
	- krajnje točke za komunikaciju dva procesa na dva različita računala spojenih u mrežu

### Opisnik datoteke

Opisnik datoteke je broj koji predstavlja zapis trenutno otvorene datoteke koji sadrži informacije gdje se nalaze memorijski segmenti koji reprezentiraju tu datoteku. Svaki proces koji ima otvorene datoteke ima niz takvih zapisa i broj odnosno opisnik datoteke asociran s njim. Svaki proces u Linuxu pri pokretanju ima automatski otvorena već tri opisnika:

* STDIN predstavljen s 0
* STDOUT predstavljen s 1
* STDERR predstavljen s 2

Zapisi asocirani uz ove opisnike sadrže informacije gdje se nalaze segmenti u memoriji koje koristi i terminal koji je pokrenuo ove procese i sami procesi. Primjerice, ako mi pokrenemo neki proces kroz terminal i on piše na STDOUT funkcijom *printf()* ili *write()*, on će pisati na memorijski segment naveden u zapisu koji je reprezentiran STDOUT-om odnosno opisnikom datoteke 1. Terminal će iščitati upisane podatke u taj memorijski segment i prikazati nam to.

U virtualnom datotečnom sustavu */proc/* mogu se vidjeti svi otvoreni opisnici za svaki proces na lokaciji */proc/[PID]/fd/* naredbom:

```
ls /proc/[PID]/fd/
```

Primjerice, ako prevedemo i pokrenemo C program:

```
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

	// Print PID
	pid_t pid = getpid();
	printf("PID of this process is %d\n", pid);

	// Infinite pause
	while(1) sleep(1);
	return 0;

}
```

I onda izvršimo ```ls /proc/[PID]/fd/ -la```, trebali bi dobiti ispis sličan ovome:

```
... .
... ..
... 0 -> /dev/pts/[broj pseudo terminala]
... 1 -> /dev/pts/[broj pseudo terminala]
... 2 -> /dev/pts/[broj pseudo terminala]
```

Datoteka uređaja koja je otvorena je zapravo naš pseudoterminal iz kojeg smo pokrenuli program.

### Signali

Signal je način komunikacije između procesa. Linux jezgra prosljeđuje signale određenim procesima. Pojava signala izaziva prekid u radu procesa kojem je namijenjen signal. Taj proces može obraditi signal nekom *handler* funkcijom, a ako *handler* funkcija nije zadana izvršava se neka zadana funkcija ili se signal jednostavno ignorira. Nakon obrade *handler* funkcije proces nastavlja s radom gdje je bio prekinut, osim ako nije bio poslan signal za okončanje rada.

Za C programe u Linuxu postoji biblioteka za pisanje vlastitih signal *handler* funkcija *signal.h*, primjer programa gdje se koristi:

```
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void sig_handler(int signo) {

	// Upon receiving SIGINT print this message
	if(signo == SIGINT) printf("Received SIGINT\n");

}

int main(void) {

	// Print PID
	pid_t pid = getpid();
	printf("PID of this process is %d\n", pid);
	
	// Register signal handler
	if(signal(SIGINT, sig_handler) == SIG_ERR) printf("\nCan't catch SIGINT\n");

	// Infinite pause
	while(1) sleep(1);
	return 0;

}
```

Funkcija [*signal()*](https://pubs.opengroup.org/onlinepubs/7908799/xsh/signal.html) će registrirati *handler()* za SIGINT signal. No ne mogu se svim signalima registrirati vlastite *handler* funkcije. Primjerice signalu SIGKILL se ne može registrirati *handler()* jer on služi za forsirani izlazak programa. Prevođenje programa može se napraviti GCC prevodiocem naredbom:

```
gcc [ime C datoteke] -o [ime izvršne datoteke]
```

Pokretanje programa u terminalu i pritiskanjem kombinacije tipki CTRL + C poslat će signal SIGINT procesu. Ako bi željeli prekinuti ovaj proces potrebno je izvršiti naredbu u drugom terminalu:

```
kill -SIGKILL [PID procesa]
```

### Neimenovani cjevovodi

Neimenovani cjevovod je FIFO red kojem proces roditelj i proces dijete mogu komunicirati. Redoslijed stvaranja i komunikacije neimenovanog cjevovoda:

* roditeljski proces prvo otvara dva opisnika datoteka koja pokazuje na zapis koji koristi jezgrin međuspremnik za cjevovode
* roditelj zatim stvara procese djecu
* obično je komunikacija cjevovodima jednosmjerna pa roditelj zatvara opisnik za čitanje iz cjevovoda, a dijete zatvara opisnik za pisanje (roditelj šalje djetetu) ili roditelj zatvara opisnik za pisanje, a dijete opisnik za čitanje (dijete šalje roditelju)

Primjer C programa u Linuxu koji koristi neimenovane cjevovode radi komunikacije između dva procesa gdje je jedan roditelj a drugi dijete:

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 100

int pipe_fd[2];

void parent_function() {

	// Message to be sent
	char write_msg[] = "Hello from parent to child!";

	// Print parent PID
	pid_t pid = getpid();
	printf("PID of the parent process is %d\n", pid);

	// Close the read end of the FIFO pipe
        close(pipe_fd[0]);

	// Print the message being sent
	printf("[PARENT]: Sending message: '%s'\n", write_msg);

	// Write to the pipe
	write(pipe_fd[1], write_msg, strlen(write_msg) + 1);

	// Close the write end of the FIFO pipe
        close(pipe_fd[1]);

}

void child_function() {

	// Buffer for the received message
	char read_msg[BUFFER_SIZE];

	// Print child PID
	pid_t pid = getpid();
	printf("PID of the child process is %d\n", pid);

	// Close the write end of the FIFO pipe
        close(pipe_fd[1]);

        // Read from the pipe
        read(pipe_fd[0], read_msg, BUFFER_SIZE);

	// Print the received message
	printf("[CHILD]: Received message: '%s'\n", read_msg);

	// Close the read end of the FIFO pipe
        close(pipe_fd[0]);

}

int main() {

	// Create a pipe
	if(pipe(pipe_fd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	// Fork a child process
	pid_t pid = fork();

	if(pid < 0) {
		perror("Forking error");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) parent_function();
        else child_function();

	// Wait for the child process to finish
	wait(NULL);

	return 0;

}
```

### Imenovani cjevovodi

Imenovani cjevovodi su zapravo stvorene datoteke u datotečnom sustavu koje funkcioniraju kao FIFO red. Ta datoteka se učitava u RAM prilikom otvaranja i procesi u nju mogu pisati i čitati. Ovdje procesi ne moraju biti u srodstvu jer se datoteka učitava u RAM preko apsolutne poznate putanje na datotečnom sustavu te se svako novo otvaranje te datoteke referira već na učitani segment u memoriji. Stvaranje FIFO cjevovoda može se napraviti naredbom:

```
mkfifo [ime cjevovoda]
```

Brisanje je jednostavno naredbom *rm* kao i sve ostale vrste datoteka. Primjer programa koji piše i čita iz cjevovoda *my-pipe* koji je stvoren naredbom ```mkfifo my-pipe```:

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 100
#define FILE_NAME "my-pipe"

void parent_function() {

	// Message to be sent
	char write_msg[] = "Hello from parent to child!";

	// Print parent PID
	pid_t pid = getpid();
	printf("PID of the parent process is %d\n", pid);

	// Open the file descriptor for writing
	int write_fd = open(FILE_NAME, O_WRONLY);
	if(write_fd == -1) {
		perror("open for write");
		exit(EXIT_FAILURE);
	}

	// Print the message being sent
	printf("[PARENT]: Sending message: '%s'\n", write_msg);

	// Write to the pipe
	write(write_fd, write_msg, strlen(write_msg) + 1);

	// Close the file descriptor
        close(write_fd);

}

void child_function() {

	// Buffer for the received message
	char read_msg[BUFFER_SIZE];

	// Print child PID
	pid_t pid = getpid();
	printf("PID of the child process is %d\n", pid);

	// Open the file descriptor for writing
	int read_fd = open(FILE_NAME, O_RDONLY);
	if(read_fd == -1) {
		perror("open for write");
		exit(EXIT_FAILURE);
	}

        // Read from the pipe
        read(read_fd, read_msg, BUFFER_SIZE);

	// Print the received message
	printf("[CHILD]: Received message: '%s'\n", read_msg);

	// Close the read end of the FIFO pipe
        close(read_fd);

}

int main() {

	// Fork a child process
	pid_t pid = fork();

	if(pid < 0) {
		perror("Forking error");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) parent_function();
        else child_function();

	// Wait for the child process to finish
	wait(NULL);

	return 0;

}
```

Potrebno je naglasiti da je ovdje cjevovod referenciran relativnom putanjom odnosno program je pokrenut iz direktorija gdje se nalazi i cjevovod. Također, stvorili smo procese roditelj i dijete iako smo mogli napraviti dva zasebna programa, jedan koji piše drugi koji čita. Imenovani cjevovodi ne spremaju stanje međuspremnika na datotečni sustav već samo metapodatke i ovlasti tko ga ima pravo koristiti.

