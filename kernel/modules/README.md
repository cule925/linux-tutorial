# LINUX JEZGRENI MODULI

[Linux jezgreni moduli](https://wiki.archlinux.org/title/Kernel_module) su programi koji proširuju funkcionalnost Linux jezgre. Slični su upravljačkim programima. Mogu se ubaciti i izbaciti po želji bez ponovnog pokretanja Linuxa. Neki moduli se automatski učitavaju prilikom pokretanja sustava (definirani u */usr/lib/modules/$(uname -r)/* direktoriju). Također postoji mogućnost ugradnje modula prilikom prevođenja Linux jezgre. Jezgreni moduli odnosno jezgrene objektne datoteke koje pružaju dodatne funkcionalnosti Linux jezgri imaju nastavak *.ko*. Prilikom učitavanja jezgrenih modula imena koja sadrže *-* se pretvaraju u *_* primjerice učitavanje jezgrene objektne datoteke *my-module.ko* će prilikom izlistavanja svih modula imati naziv *moj_modul*. Ovo se događa radi pridržavanja konvencijama o imenovanju u jeziku C.

Jezgreni moduli, iako mogu raditi u korisničkom prostoru, najčešće rade u prostoru jezgre. Ovo znači da modul ima najveću privilegiju izvršavanja, primjerice direktno pisanje ili čitanje po sklopovlju što može biti vrlo opasno. Stoga moduli moraju biti kvalitetno napisani bez ikakvih grešaka kako ne bi došlo do zastoja sustava.

## Korisnički i jezgreni način rada

Obični korisnički programi izvode se u korisničkom načinu rada. To znači da im je direktan pristup resursima zabranjen bez poziva jezgre (*eng. syscall*). Jezgra u slučaju poziva od programa za neki resurs provjerava prava pristupa programa tom resursu. Kod modula je drugačija priča. Moduli se direktno ugrađuju u jezgru i koriste adresni prostor jezgre umjesto dodijeljeni programski adresni prostor. Stoga je vrlo opasno ubacivati neispravne module jer mogu zaglaviti cijelu jezgru.

Način rada jezgre se obično naziva *ring 0* način rada dok se način rada korisnika naziva *ring 1* način rada.

## Upravljanje modulima

Moduli koji su trenutno učitani mogu se dobiti naredbom:

```
lsmod
```

Korisne informacije o modulu mogu se saznati naredbom:

```
modinfo [.ko datoteka]
```

Učitavanje jezgrenog modula koji se nalazi u direktoriju ```/usr/lib/modules/$(uname -r)/``` (s parametrima ako je potrebno) moguće je naredbom:

```
modprobe [ime modula] [ime_parametra_1=vrijednost_parametra_1] [ime_parametra_2=vrijednost_parametra_2] ...
```

Učitavanje jezgrenog modula koji po imenu datoteke prevedene *.ko* datoteke (s parametrima ako je potrebno):

```
insmod [.ko datoteka] [ime_parametra_1=vrijednost_parametra_1] [ime_parametra_2=vrijednost_parametra_2] ...
```

Izbacivanje jezgrenog modula moguće je naredbom s uklanjanjem nekorištenih ovisnosti:

```
modprobe -r [ime modula]
```

Izbacivanje modula moguće je napraviti i naredbom:

```
rmmod [ime modula]
```

## Pisanje vlastitog jezgrenog modula

Prije [pisanja vlastitog jezgrenog modula](https://sysprog21.github.io/lkmpg/) potrebno je instalirati zaglavlja Linux jezgre ako već nisu. Zaglavlja pružaju sučelje s kojim će modul komunicirati s Linux jezgrom. Na Arch Linux distribuciji zaglavlja trenutne učitane Linux jezgre mogu se instalirati naredbom:

```
sudo pacman -S linux-headers
```

Na lokaciji */usr/lib/modules/$(uname -r)/* stvorit će se direktorij *build/* u kojoj će se nalaziti potrebne datoteke za izgradnju modula. Također, stvorit će se direktorij */usr/src/linux/* u kojoj će se nalaziti kopija sadržaja *build/* direktorija.

### Jednostavni primjer modula - ulazna i izlazna funkcija

U direktoriju po želji potrebno je stvoriti datoteku *my-module.c* (ili nekim drugim imenom) i datoteku *Makefile*.

#### Datoteka Makefile

Makefile datoteka mora imati sljedeći sadržaj:

```
obj-m += my-module.o

PWD := $(CURDIR)

all:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

Varijabla **CURDIR** je varijabla koju alat *make* sam postavi i ona pokazuje na trenutni direktorij. Varijabla **PWD** je inače varijabla okruženja koja bi se naslijedila pokretanjem naredbe *make* i ona bi također pokazivala na direktorij odakle je naredba pokrenuta. No problem dolazi pokretanja naredbe u privilegiranom načinu rada (kao *root* korisnik) uz pomoć *sudo*. Naime, uobičajena sigurnosna politika *sudo* naredbe je da naredba pokrenuta s *root* privilegijama prosljeđuje minimalan broj varijabli okruženja ([env_reset](https://www.sudo.ws/docs/man/sudoers.man/#Command_environment) zastavica). U ovom slučaju se varijabla **PWD** neće naslijediti iz okruženja ako se izgradnja pokrene kao ```sudo make``` umjesto ```make```. Varijabla **CURDIR** osigurava da **PWD** pokazuje na trenutni direktorij bez obzira jesmo li je pokrenuli u privilegiranom načinu rada ili ne. Zašto onda ne koristiti samo varijablu **CURDIR**? Zbog navike.

Cilj *all* pokreće novi Makefile iz direktorija */lib/modules/$(shell uname -r)/build* s ciljem *modules*. Također mu se prosljeđuje varijabla **M** koja ima vrijednost direktorija u kojem se nalazi direktorij s izvornim kodom modula (prethodno spomenuti **PWD**). Izgradnja uključuje i našu Makefile datoteku odakle će se nadodati ime objekte datoteke **my-module** u varijablu **obj-m**. Rezultat izgradnje je datoteka **my-module.ko** koja predstavlja modul.

Cilj *clean* briše sve datoteke nastale izgradnjom.

#### Datoteka izvornog koda modula

Za prvi primjer datoteka *my-module.c* bi trebala imati sljedeći sadržaj:

```
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

static int __init my_module_init(void) { 

    pr_info("Hello from module!\n"); 
    return 0; 

}

static void __exit my_module_exit(void) {

    pr_info("Goodbye from module!\n"); 

}

module_init(my_module_init); 
module_exit(my_module_exit); 

MODULE_LICENSE("GPL");
```

Makro MODULE_LICENSE() označava licencu modula. Makroi *module_init()* i *module_exit()* (definirani u *linux/module.h*) služe za definiranje funkcija koje će se izvršavati pri ubacivanju modula u jezgru i izbacivanje modula iz jezgre. Funkcije makroa *__init* i *__exit* (definirane u *linux/init.h*) su različite ovisno o tome je li modul ugrađen u jezgru tijekom prevođenja ili je modul vanjski (u ovom primjeru):

* za module ugrađene u jezgru
	* funkcija označena __init makroom se postavlja u posebni dio u memoriji te se taj dio oslobađa nakon što se jezgra učita u radnu memoriju i izvrše se sve funkcije označene __init makroom (to se može vidjeti na poruci nakon učitavanja jezgre tipa "Freeing unused kernel memory: ...")
	* funkcija označena __exit makroom se uopće ni ne učitava u memoriju na ni jednu lokaciju jer neće biti nikad ni pozvana (modul je ugrađen u jezgru)
* za vanjske module
	* funkcija označena __init nema nikakvog efekta na funkciju
	* funkcija označena __exit nema nikakvog efekta na funkciju

Funkcija *pr_info()* piše dani niz znakova u jezgreni međuspremnika za zapise (*eng. log buffer*). Zapisi jezgrenog međuspremnika mogu se iščitati naredbom:

```
sudo dmesg
```

Izgradnja modula se pokreće u direktoriju gdje se nalazi izvorni kod modula i Makefile naredbom:

```
make all
```

Izvršenjem ove naredbe stvorit će se par novih datoteka od kojih je najbitnija *my-module.ko*. Umetanje modula moguće je naredbom:

```
sudo insmod my-module.ko
```

Naredbom ```sudo dmesg``` može se vidjeti zapis iz inicijalizacijske funkcije "Hello from module!". Također, naredbom ```lsmod | grep "my"``` može se uočiti da je ime module *my_module* umjesto *my-module*. Uklanjanje modula može se izvršiti naredbom:

```
sudo rmmod my_module
```

Naredbom ```sudo dmesg``` može se vidjeti zapis iz funkcije za čišćenje "Goodbye from module!". Na kraju, brisanje svih pomoćnih datoteka kao i datoteke *my-module.ko* može se napraviti iz direktorija gdje se nalazi izvorni kod modula i Makefile naredbom:

```
make clean
```

Potrebno je uočiti da su funkcije, a i buduće varijable u drugim primjerima označene oznakom *static*. Ovo znači da su im imena vidljiva samo u lokalnoj datoteci *my-module.c*. Linux jezgra je ogromna i sastoji se od mnogo prevedenih datoteka čije varijable ili funkcije možda imaju ista imena kao i varijable ili funkcije u datoteci *my-module.c*. Ovo bi inače predstavljalo problem u povezivanju modula s Linux jezgrom ako se ne bi koristila *static* oznaka.

### Drugi primjer modula - prosljeđivanje argumenata

Prosljeđivanje parametara vanjskim modulima radi se tijekom ubacivanja modula u jezgru:

```
insmod [.ko datoteka] [ime_parametra_1=vrijednost_parametra_1] [ime_parametra_2=vrijednost_parametra_2] ...
```

Kako dohvatiti parametre u programu? Uzmimo sljedeći izvorni kod modula kao primjer:

```
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>

static int myInt = 10;
static char *myString = "Adios!";
static int myIntArray[2] = {100, 200};

static int arrayArgc = 0;

static int __init my_module_init(void) { 

    pr_info("Hello from module!\n");

    pr_info("myInt is an integer: %d\n", myInt);
    pr_info("myString is a character string: %s\n", myString);

    pr_info("myIntArray is an array of integers: %d %d\n", myIntArray[0], myIntArray[1]);
    pr_info("arguments received for myIntArray: %d\n", arrayArgc);

    return 0;

}

static void __exit my_module_exit(void) {

    pr_info("Goodbye from module!\n");

}

module_param(myInt, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myInt, "An integer");
module_param(myString, charp, 0000);
MODULE_PARM_DESC(myString, "A character string"); 
module_param_array(myIntArray, int, &arrayArgc, 0000);
MODULE_PARM_DESC(myIntArray, "An array of integers"); 

module_init(my_module_init);
module_exit(my_module_exit); 

MODULE_LICENSE("GPL");
```

Svaki argument mora imati nekakvu uobičajenu vrijednost ako mu se nikakva vrijednost ne proslijedi tijekom umetanja modula. Objašnjenja makroa su sljedeća:

* *module_param()* - definira parametar modula, uzima ime varijable u programu za koje parametar vezan, tip varijable i dopuštenja nad datotekom koja predstavlja taj parametar (na */sys/module/my_module/parameters*)
* *module_param_array()* - kao i kod *module_param()* uz dodatni argument brojača proslijeđenih argumenata nizu
* *MODULE_PARAM_DESC()* - za dokumentaciju parametara, uzima ime i opis parametra (naredba ```modinfo my-module.ko``` ispisuje sažetak modula)

Ako se primjerice izvrši naredba ```sudo insmod my-module.ko myInt=15 myString="Until_we_meet_again"``` varijable *myInt* i *myString* dobit će nove vrijednosti (ispis se može vidjeti naredbom ```sudo dmesg```). Drugi primjer može biti naredba ```sudo insmod my-module.ko myInt=15 myString="Until_we_meet_again" myIntArray=300,400``` gdje će sve varijable dobiti neku vrijednost.

# POGLEDATI DALJE

Nastavak uputa:

* za module kao upravljačke programe uređaja: [devices](devices)

