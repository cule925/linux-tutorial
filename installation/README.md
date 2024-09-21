# INSTALACIJA LINUX DISTRIBUCIJE

Ovo su upute za instalaciju pojedinih Linux distribucija. Upute su sljedeće:

* za Arch Linux: [*arch-linux*](arch-linux)

## Firmware i pokretanje sustava, BIOS i UEFI

[BIOS (Basic Input Output System)](https://en.wikipedia.org/wiki/BIOS) je firmware najčešće ugrađen u matičnu ploču računala. Pruža servise za inicijalizaciju sklopovlja računala. Prilikom pokretanja računala vrši inicijalizaciju hardvera i POST (*eng. Power On Self Test*) koji provjerava ispravnost komponenata. Nakon toga započinje proces podizanja sustava (*eng. Boot process*) gdje BIOS pokušava locirati sekundarnu pohranu (NVME, SSD, HDD, USB...) koja u svom prvom logičkom sektoru veličine 512 bajtova sadrži MBR (*eng. Master Boot Record*). Sekundarne pohrane su poredane po prioritetima i BIOS će nastaviti pretraživati uređaje sve dok ne naiđe na MBR zapis. MBR se prepoznaje po tome što u prvom sektoru na zadnja dva bajta ima zapis 0x55 0xAA. Nakon toga se MBR zapis učitava u RAM i započinje se izvršavanje x86 koda u MBR zapisu, to je program koji učitava kod s particije koja je označena s *bootable*. Takva particija sadrži *bootloader* ili jezgru OS-a. BIOS je poprilično star i u svim novijim računalima zamijenio ga je UEFI.

[UEFI (Unified Extensible Firmware Interface) Specification](https://uefi.org/specs/UEFI/2.10/01_Introduction.html) definira sučelje između operacijskog sučelja i firmwarea platforme. Platforma mora implementirati specifikaciju kako bi se kvalificirala kao UEFI. Prilikom pokretanja sustava UEFI također inicijalizira hardver i provjerava ispravnost komponenti. Zatim pretražuje EFI particije na sekundarnim pohranama i učitava *bootloader* u RAM gdje se onda može učitati OS po želji. Shema particioniranja može biti MBR ili GPT, bitno je samo da postoji barem jedna EFI sistemska particija (ESP) na disku koja sadrži *bootloader*. Za razliku od BIOS-a, x86 kod u MBR-u se ne izvršava kod UEFI-a.

## Sheme particioniranja, MBR i GPT?

Master Boot Record je zapis u prvom logičkom sektoru diska LBA 0, veličine 512 bajtova. Prvih 446 bajtova je x86 *bootstrap* kod, sljedećih 64 bajtova su informacije o 4 particijama (16 bajta za svaku), a zadnja dva bajta daju do znanja BIOS-u da sektor sadrži MBR zapis (bajt 0x55 i bajt 0xAA). Podjela diska može izgledati ovako:

```
            |  LBA-0  |  LBA-1 -> LBA-1000000  |  LBA-1000001 -> LBA-X  |
   DISK     |   MBR   |           P1           |           P2           |
```

X označava broj zadnjeg logičkog bloka. P1 i P2 su dvije primarne particije.

Maksimalna veličina diska sa sektorima veličine 512 bajta može biti veličine 2 TiB. Maksimalni broj particija je četiri primarnih s mogućnost proširenja četvrte particije na bilo koji broj logičkih particija. MBR biva lagano zamijenjen s GPT shemom particioniranja.

[GUID Partition Table (GPT)](https://uefi.org/specs/UEFI/2.10/05_GUID_Partition_Table_Format.html) je novija shema particioniranja. Disk s GPT shemom particioniranja sadrži dva GPT zaglavlja, jedan na LBA-1, drugi na LBA-X gdje je X zadnji logički sektor diska. LBA-0 sadrži tzv. *Protective MBR*, MBR zapis koji pokazuje na samo jednu particiju, raspona od LBA-1 do LBA-X. Ovaj MBR zapis se koristi kako bi se osigurala kompatibilnost alata koji ne prepoznaju GPT shemu particioniranja. GPT zaglavlje sadrži mnogo informacija kao što su primjerice jedinstveni zapis (GUID), početni i završni LBA koji se koristi za particije određivanje granice particija, CRC32 broja za provjeru korupcije zaglavlja, broj particija, veličina zapisa koji opisuje particiju, CRC32 broja za provjeru korupcije zapisa particija i slično. Iza prvog GPT zaglavlja i iza drugog GPT zaglavlja dolaze zapisi o particijama. Svaki zapis sadrži GUID tipa particije, početni LBA particije, završni LBA particije i slično. Svaki zapis je minimalne veličine 128 bajtova i može okupirati od LBA-2 pa sve do LBA-33 iza primarnog GPT zaglavlja odnosno LBA-(X-32) pa sve do LBA-(X-1) ispred sekundarnog GPT zaglavlja kod diskova gdje je jedan sektor veličine 512 bajtova. Podjela diska u kome je jedan sektor veličine 512 bajtova može izgledati ovako:

```
            |     LBA-0      |       LBA-1        |   LBA-2 -> LBA-33     | LBA-34 -> LBA-1000000 | LBA-1000001 -> LBA-(X-33) | LBA-(X-32) -> LBA-(X-1) |        LBA-X         |
   DISK     | Protective MBR | GPT primary header | GPT partition entries |          P1           |            P2             |  GPT partition entries  | GPT secondary header |
```

X označava broj zadnjeg logičkog bloka. P1 i P2 su dvije primarne particije.

## Datotečni sustavi, FAT32 i EXT4

Datotečni sustav opisuje način organizacije datoteka, pristup datotekama i strukture kojima su predstavljene te datoteke. Formatiranje particije je stvaranje novog datotečnog sustava na particiji. Jedan od starijih datotečnih sustava, ali još uvijek korišten je [*FAT32*](https://www.pjrc.com/tech/8051/ide/fat32.html). Particija se formatira na FAT32 tako što se u prvom sektoru u particiji upiše *Volume ID* pa se iza njega ostavi par sektora te onda slijedi *FAT#1* i *FAT#2* koje opisuju koji su klasteri zauzeti. *FAT#2* služi kao pričuvna kopija jer su se u starijim diskovima znale događati situacije s lošim sektorima. Zatim većinu sektora slijede klasteri što su zapravo nakupine sektora koje predstavljaju datoteku ili direktorij. Svaka datoteka ili direktorij je zapravo jednostrano povezana lista klastera. *Volume ID* pokazuje od kojeg klastera započinje korijenski direktorij. Također, FAT tablice sadrže i mapu klastera te se uz svaki klaster u toj mapi nalazi bit zauzetosti (0 slobodan, 1 zauzet) i indeks na sljedeći klaster koji čini strukturiranu datoteku. Ako je klaster zadnji u listi onda mu indeks na sljedeći klaster pokazuje na -1. Ako je klaster slobodan bit zauzetosti mu je 0. a indeks na sljedeći klaster mu je -1. Primjer rasporeda komponenti *FAT32* datotečnog sustava na particiji P1:

```
   PARTICIJA P1     | Volume ID | FAT#1 | FAT#2 | *klasteri* | *slobodan dio* |
```

Maksimalna veličina particije gdje se može iskoristiti FAT32 datotečni sustav je 2 TiB s veličinom sektora od 512 bajta dok je maksimalna veličina datoteke 4 GiB. Potrebno je naglasiti da *FAT32* format sve metapodatke o datoteci sprema u zapisu direktorija, ne u samom bloku podatka namijenjenog za datoteku.

Noviji datotečni sustav je [*EXT4*](https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout). Formatiranjem particije na *EXT4* datotečni sustav dobiva se sljedeći raspored:

* superblok
* opisnici grupe blokova
* tablica opisnika grupa
* *inode* (indeks čvor) bitmapa
* bitmapa blokova
* *inode* (indeks čvor) tablica
* blok grupe

*EXT4* datotečni sustav podijeljen je na grupe blokova koji u sebi sadrže blokove. Veličina jednog bloka je obično 4096 bajtova (4 KiB). Za svaku grupu blokova postoji po jedan opisnik grupe blokova, jedna bitmapa blokova, jedna *inode* bitmapa i jedna *inode* tablica. Primjer [rasporeda *EXT4* datotečnog sustava](https://blogs.oracle.com/linux/post/understanding-ext4-disk-layout-part-1) na particiji P2:

```
   PARTICIJA P2

-----------------------------------------------------
       *slobodnih 1024 bajtova za instalaciju*
-----------------------------------------------------
                     super blok
-----------------------------------------------------
                 *slobodni prostor*
-----------------------------------------------------
          blok opisnik za 0. grupu blokova
-----------------------------------------------------
          blok opisnik za 1. grupu blokova
-----------------------------------------------------
                         ...
-----------------------------------------------------
          blok opisnik za n. grupu blokova
-----------------------------------------------------
            bitmapa za 0. grupu blokova
-----------------------------------------------------
            bitmapa za 1. grupu blokova
-----------------------------------------------------
                         ...
-----------------------------------------------------
            bitmapa za n. grupu blokova
-----------------------------------------------------
     bitmapa inode čvorova za 0. grupu blokova
-----------------------------------------------------
     bitmapa inode čvorova za 1. grupu blokova
-----------------------------------------------------
                         ...
-----------------------------------------------------
     bitmapa inode čvorova za n. grupu blokova
-----------------------------------------------------
             prvi inode 0. grupe blokova
-----------------------------------------------------
             drugi inode 0. grupe blokova
-----------------------------------------------------
                         ...
-----------------------------------------------------
            zadnji inode 0. grupe blokova
-----------------------------------------------------
             prvi inode 1. grupe blokova
-----------------------------------------------------
             drugi inode 1. grupe blokova
-----------------------------------------------------
                         ...
-----------------------------------------------------
            zadnji inode 1. grupe blokova
-----------------------------------------------------
                         ...
                         ...
                         ...
-----------------------------------------------------
             prvi inode n. grupe blokova
-----------------------------------------------------
             drugi inode n. grupe blokova
-----------------------------------------------------
                         ...
-----------------------------------------------------
            zadnji inode n. grupe blokova
-----------------------------------------------------
                     blok grupa 0
-----------------------------------------------------
                     blok grupa 1
-----------------------------------------------------
                         ...
-----------------------------------------------------
                     blok grupa n
```

Jednostavnije:

```
   PARTICIJA P2     | slobodnih 1024 bajtova | superblok | opisnici grupa blokova | bitmape grupe blokova | bitmape inodeova | inodeovi (inode tablica) | grupe blokova |
```

Postoji još par [podatkovnih struktura u *EXT4* datotečnom sustavu](https://blogs.oracle.com/linux/post/understanding-ext4-disk-layout-part-2), ali se u njih neće ulaziti.

Superblok sadrži bitne informacije, primjerice broj *inodeova*, broj blokova, jeli datotečni sustav montiran i slično. Superblok se radi redundancije kopira na par mjesta u blok grupama. Blok deskriptori pokazuju na bitmape *inodeova* i blokova te na početak *inode* tablice. Bitmape grupe blokova i bitmape *inodeova* pokazuju na zauzetost blokova odnosno *inodeova*. Konačno, jedan *inode* se zauzima odnosno oslobađa prilikom stvaranja odnosno brisanja datoteke te sadrži sve metapodatke vezanu uz tu datoteku (vremensku oznaku, razne atribute...) te pokazivače na blokove u blok grupama koje sadrže podatke. *EXT4* datotečni sustav ima fiksiran broj *inodeova* što znači da se može dogoditi problem iscrpljenja *inodeova* ako se stvara mnogo malih datoteka odnosno ne bi više mogli stvarati datoteke jer smo iskoristili sve *inodeove* iako ima još slobodnog prostora za podatke odnosno nisu zauzeti svi blokovi u blok grupama. Jedan *inode* sigurno sadrži sljedeće:

* ID korisnika i grupe
* vrsta datoteke (čista datoteka, direktorij, ...)
* dopuštenja (čitanja, pisanja, uređivanja - rwx)
* vrijeme pristupa datoteci i vrijeme uređenja datoteke
* vrijeme uređenja ovog *inodea* i vrijeme brisanja datoteke
* broj *hard* poveznica
* 12 direktnih pokazivača na podatkovne blokove
* 1 indirektni pokazivač na podatkovni blok koji sadrži tablicu s pokazivačima na podatkovne blokove
* 1 dvostruko indirektni pokazivač na podatkovni blok koji sadrži tablicu s pokazivačima koji pokazuju na podatkovne blokove koji sadrže tablicu s pokazivačima na podatkovne blokove
* 1 trostruko indirektni pokazivač na podatkovni blok koji sadrži tablicu s pokazivačima koji pokazuju na podatkovne blokove koji sadrže tablicu s pokazivačima koji pokazuju na podatkovne blokove koji sadrže tablicu s pokazivačima na podatkovne blokove

Omjer stvorenih *inodeova* naspram kapaciteta cijele particije je najčešće [1:16 KiB](https://www.redhat.com/sysadmin/inodes-linux-filesystem). Kod obične datoteke *inode* pokazivači pokazuju na podatkovne blokove koji sadrže podatke te datoteke. Kod direktorija *inode* pokazivači pokazuju na podatkovne blokove koji sadrže zapise o lokacijama *inodeova* poddirektorija i datoteka koji se nalaze u trenutnom direktoriju čiji *inode* pregledavamo. Ti zapisi najčešće sadrže ime poddirektorija ili datoteke i pokazivač na njihov odgovarajući *inode*.

U terminalu, ispis broja *inodea* vezana uz datoteku može se učiniti naredbom: ```ls [putanja do datoteke] -i```

### Specijalni inodeovi

U *EXT4* datotečnom sustavu postoje [specijalni *inodeovi*](https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout#Special_inodes). Neki od njih su:

- *inode* 0
	- pokazivač na NULL
- *inode* 1
	- lista defektnih blokova
- *inode* 2 
	- **korijenski direktorij (/)**
	- ovo je razlog zašto korijenski direktorij nema ime, zato što nema roditeljskog direktorija u kojem bi njegovo ime bilo zapisano
- *inode* 5
	- *bootloader*

### *Soft* i *Hard* poveznice

*Soft* poveznice su poveznice kojima je stvoren vlastiti *inode* i sadržaj podatkovnog bloka na koji pokazuje taj *inode* pokazuje na putanju datoteke nad kojom je poveznica stvorena. Brisanje *Soft* poveznice samo označava *inode* i podatkovne blokove poveznice slobodnim, a *inode* same datoteke ostaje netaknut. Stvaranje *Soft* poveznice nad nekom datotekom u Linuxu može se napraviti u terminalu naredbom: ```ln -s [putanja do datoteke] [ime poveznice]```.

*Hard* poveznice su poveznice koje pokazuju na isti *inode* datoteke nad kojom se vrši poveznica. *Hard* poveznice su zapravo samo zapisi u podatkovnom bloku direktorija u kojem se nalaze i one pokazuju na isti *inode* kao i datoteka. Brisanjem same datoteke neće označiti *inode* te datoteke oslobođenim već će samo zapis u podatkovnom bloku direktorija biti obrisan. Sami *inode* će biti obrisan kad se uklone sve *Hard* poveznice iz drugih direktorija koje pokazuju na taj *inode*. Stvaranje *Hard* poveznice nad nekom datotekom u Linuxu može se napraviti u terminalu naredbom: ```ln [putanja do datoteke] [ime poveznice]```.

Brisanje oba tipa poveznica može se izvesti jednostavno naredbom ```rm [ime poveznice]```

### Montiranje i demontiranje particije

Uzmimo situaciju gdje imamo jedan disk s jednom particijom formatiranom uz pomoć *EXT4* datotečnog sustava i na njemu je instalirana neka Linux distribucija. Zatim imamo i USB stick koji ima jednu particiju formatiranu uz pomoć *FAT32* datotečnog sustava. Želimo pristupiti toj particiji na USB-u, kako to napraviti?

Svaki *EXT4* datotečni sustav ima svoj korijen (oznaka /) koji sadrži svoje poddirektorije i datoteke. Prilikom stvaranja, brisanja, pisanja ili čitanja tih direktorija ili datoteka, Linux jezgra nam nudi [*ext4* upravljački program](https://github.com/torvalds/linux/tree/master/fs/ext4) uz pomoć kojeg se izvode te operacije. Kada se na računalo spoji USB stick, moderna Linux jezgra ga prepoznaje, montira njene particije na neku određenu lokaciju (najčešće lokacija */run/media/[korisničko ime]/[nekakav ID]*) i pokreće upravljački program za odgovarajući datotečni sustav kroz kojeg će se vršiti operacije nad tim montiranim particijama u tom direktoriju. Primjerice, ako je samo jedna particija formatirana uz pomoć *FAT32* datotečnog sustava, Linux jezgra će pokrenuti [upravljački program *fat*](https://github.com/torvalds/linux/tree/master/fs/fat) ako već nije pokrenut i dati mu uputu da se brine za sve operacije koje se izvršavaju nad lokacijom */run/media/[korisničko ime]/[nekakav ID]* i njezinom podstablu.

Ako se automatsko montiranje iz nekog razloga nije dogodilo, moguće ga je napraviti ručno. Slikovito, recimo da imamo ovakvu prvu razinu stabla direktorija datotečnog sustava *EXT4* na disku:

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

Zatim priključimo USB stick u računalo. Izvršavanjem ```fdisk -l``` naredbe uočavamo da u ovoj situaciji USB stick predstavlja datoteka */dev/sdb*, particiju na njemu datoteka */dev/sdb1* dok recimo disk predstavlja datoteka */dev/sda*, a njenu particiju */dev/sda1*. Nakon toga je potrebno stvoriti direktorij na kojem će se montirati particija USB-a naredbom ```mkdir /my-usb/```. Konačno potrebno je izvršiti naredbu ```mount -t vfat /dev/sdb1 /my-usb/```. Stablo direktorija prve razine će sada izgledati ovako:

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
├── var
└── my-usb
```

Naredba *mount* je na direktorij */my-usb/* montirao *FAT32* datotečni sustav. Sve operacije nad ovim direktorijem i svim njegovim poddirektorijima odnosno pisanje po particiji USB sticka izvršavat će se upravljačkim programom *fat*. Pisanje i čitanje iz svih ostalih direktorija i njihovih poddirektorija odnosno particije na disku će se izvršavati uz pomoć upravljačkog programa *ext4*. Ako se želi otkvačiti particija USB sticka potrebno je izvršiti naredbu ```umount /dev/sdb``` ili ```umount /my-usb/```. Direktorij */my-usb/* se mora ručno izbrisati naredbom ```rmdir /my-usb/```.

## Linux struktura direktorija

Struktura direktorija Linux distribucija ravna se po [FHS-u (*eng. File System Hierarchy Standard*)](https://wiki.gentoo.org/wiki/Filesystem_Hierarchy_Standard) uz male preinake ovisno o distribuciji. Generalno od korijenskog direktorija (/) situacija je sljedeća:

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

Virtualni datotečni sustavi se ne nalaze na disku već u RAM-u. Pri svakom ponovnom pokretanju oni se stvaraju i montiraju na navedene lokacije.

## Proces učitavanja sustava

Recimo da smo instalirali bootloader [GRUB](https://wiki.archlinux.org/title/GRUB).

### Učitavanje bootloadera kad se koristi BIOS+MBR shema particioniranja

Kad se koristi [BIOS s MBR-om](https://wiki.archlinux.org/title/Arch_boot_process#Under_BIOS) on se instalira na sljedeći način:

* prvi dio GRUB-a se instalira kao MBR izvršni kod (boot.img)
* drugi dio se instalira u slobodnom prostoru ako postoji iza prvog sektora (diskboot.img), inače sve ide na particiji koja sadrži */boot/* direktorij
* treći dio se instalira na particiji u */boot/* direktoriju (normal.mod)

Prilikom pokretanja sustava događa se [sljedeće](http://www.pixelbeat.org/docs/disk/):

- prilikom paljenja računala BIOS izvodi POST (*eng. Power on self test*)
- nakon POST-a, BIOS inicijalizira hardver
- zatim BIOS prebacuje prvih 440 bajtova odnosno GRUB boot.img u RAM i procesor izvodi ove instrukcije, zadnja instrukcija pokazuje na diskboot.img
- onda se izvodi diskboot.img dio GRUB-a, ovaj dio implementira podršku za čitanje datotečnog sustava gdje se nalazi sljedeći dio
	- ako ovaj dio nije instaliran prethodni zadnja instrukcija prethodnog koraka direktno pokazuje na sektor na particiji gdje se nalazi normal.mod
- na kraju se izvodi normal.mod dio GRUB-a koji čita konfiguraciju */boot/grub/grub.cfg* i prezentira korisniku dostupne jezgre za pokrenuti

### Učitavanje bootloadera kad se koristi UEFI+GPT shema particioniranja

Kad se koristi [UEFI s GPT-om](https://wiki.archlinux.org/title/Arch_boot_process#Under_UEFI) na specijalnoj EFI particiji formatiranu uz pomoć FAT32 datotečnog sustava se instalira *EFI* izvršna datoteka (nastavak *.efi*). Ovakvu vrstu datoteka izvršava sam UEFI firmware.

Prilikom pokretanja sustava događa se sljedeće:

- prilikom paljenja računala UEFI izvodi POST (*eng. Power on self test*)
- nakon POST-a, UEFI inicijalizira hardver
- UEFI ima tablicu u NVRAM-u gdje se nalaze zapisi o svim *EFI* datotekama i lokacijama gdje se nalaze [(UEFI Boot Manager)](https://uefi.org/specs/UEFI/2.10/03_Boot_Manager.html) te pokušava ih izvršiti redom (tzv. Boot Order)
- u slučaju da datoteke ne postoje ili uopće ne postoji ni jedan zapis, kreće traženje EFI particija svih spojenih prostora za pohranu gdje prioritet imaju prenosivi tipovi pohrane (npr. USB uređaji) pa onda tek ostali tipovi pohrane
	- postoji tzv. [fallback boot način rada](https://www.boot-us.de/eng/gloss_uefi2.htm) koji učitava datoteku */efi/boot/bootx64.efi* ako postoji i ako za tu EFI particiju ne postoji zapis u NVRAM-u, ovo najčešće implementiraju ISO slike jer se ionako samo jednom učitava sustav iz njih (prilikom instalacije)
	- zapisi u NVRAM-u se mogu uređivati ulaskom u UEFI prilikom podizanja preko UEFI ljuske, ulaskom u UEFI grafičko sučelje ili preko Linuxovog alata *efibootmgr*
- pokrenuti bootloader čita konfiguraciju u */boot/grub/grub.cfg* i prezentira korisniku dostupne jezgre za pokrenuti

#### UEFI ljuska

[UEFI ljuska](https://wiki.archlinux.org/title/Unified_Extensible_Firmware_Interface#UEFI_Shell) je ljuska koja omogućuje pokretanje EFI datoteka i drugih alata. EFI datoteku ljuske također je potrebno instalirati na EFI particiju. Jedna od korisnih naredbi te ljuske je *bcfg* koji omogućuje modificiranje NVRAM-a UEFI-ja. Primjerice, naredba ```bcfg boot dump``` ispisuje sve zapise u NVRAM-u, dok naredba ```bcfg boot rm [ID zapisa]``` briše zapis iz NVRAM-a. Isključivanje sustava iz ljuske može se napraviti naredbom ```reset -s```.

#### OVMF

[OVMF (Open Virtual Machine Firmware)](https://wiki.archlinux.org/title/QEMU#Booting_in_UEFI_mode) je open source projekt koji omogućuje UEFI za virtualne strojeve i ima već ugrađenu UEFI ljusku. Prilikom instalacije OVMF-a na Linux distribuciju, potreno je baciti oko na dvije datoteke:

- */usr/share/edk2/x64/OVMF_CODE.4m.fd*
	- sadrži izvršni kod firmwarea za x64 procesore, u slučaju korištenja kod virtualnih strojeva
	- u ovoj datoteci se ništa ne smije pisat već samo čitat
- */usr/share/edk2/x64/OVMF_VARS.4m.fd*
	- ova datoteka služi kao NVRAM za zapise
	- ovu datoteku bi bilo bolje kopirati na neko zasebno mjesto za svaki virtualni stroj kako ne bi imali problema kad pokrećemo više virtualnih strojeva koje koriste ovu datoteku

### Učitavanje Linux jezgre

Bootloader nam daje opciju učitavanja jezgre po želji. [Jezgra](https://wiki.archlinux.org/title/Arch_boot_process#Kernel) se može u direktoriju */boot* naći u kompresiranom ili nekompresiranom obliku (*vmlinux-linux* ili *vmlinuz-linux*). Jezgra izvrši inicijalizaciju hardvera i raznih programa. Međutim, jezgra Linuxa je generična i nije prevedena za specifični sustav što znači da jezgra treba učitati jezgrene module (*eng. kernel modules*) što je ekvivalent upravljačkim programima. Za to će nam poslužiti *initramfs*.

#### Učitavanje initramfs

Nakon što se učita jezgra Linuxa, njoj je montiran prazni korijen odnosno [*rootfs*](https://docs.kernel.org/filesystems/ramfs-rootfs-initramfs.html) što je zapravo specijalna instanca virtualnog datotečnog sustava *ramfs* u RAM-u. Ovdje će se otpakirati [*initramfs*](https://wiki.archlinux.org/title/Arch_boot_process#initramfs) koji sadrži program koji se zove *init*. Program *init* je prvi proces koji se izvršava na Linuxu (ID procesa je 1) i on učitava jezgrene module, pokreće servise i montira pravi datotečni sustav. Prilikom instalacije Linux distribucije, naredba ```mkinitcpio -P``` stvara datoteku */boot/initramfs-linux.img*.

#### Proces *init*

Proces *init* je prvi proces koji se pokreće i njegov ID je 1. Neki od *init* procesa koji se mogu koristiti su:

* [systemd](https://systemd.io/)
* [OpenRC](https://wiki.gentoo.org/wiki/Project:OpenRC)
* [SysVinit](https://savannah.nongnu.org/projects/sysvinit)
* [BusyBox](https://busybox.net/)

