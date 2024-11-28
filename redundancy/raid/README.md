# RAID

[RAID (*eng. Redundant Array of Independent Disks*)](https://wiki.archlinux.org/title/RAID) je tehnologija koja osigurava redundanciju kombinirajući više diskova u jednu logičku cjelinu koja se naziva RAID polje. Podatci se raspoređuju u toj logičkoj cjelinu na više načina odnosno RAID razina.

RAID diskovi se mogu upravljati na više načina:

- softverski RAID (SoftwareRAID)
	- diskovi se upravljaju softverski:
		- apstraktnim slojem (primjerice uz alat *mdadm*)
		- logičkim particijama (primjerice uz alat *lvm*)
		- kao komponenta sustava (datotečni sustavi *ZSF* ili *Btrfs*)
	- prednosti su u pristupačnosti, fleksibilnosti i neovisnost o hardveru
	- mana je što koristi procesor za obnašanje RAID funkcionalnosti

- hardverski RAID (HardwareRAID)
	- diskovi su upravljani ugrađenim hardverskim sklopom
	- prednosti su u malo boljim performansama i neovisnost o operacijskom sustavu jer se RAID diskovi operacijskom sustavu predstavljaju kao jedna cjelina
	- mana je u tome što je potreban dedicirani sklop koji je dosta skup te ne podržava neke značajke koje softverski RAID podržava, također, u slučaju da RAID sklop zakaže najčešće je potreban identičan sklop za pristup podatcima

- lažni RAID (FakeRAID)
	- softver se nalazi u firmwareu (BIOS/UEFI)
	- ima mnogo mana:
		- mala pouzdanost
		- nema prednosti ni hardverskog RAID-a ni softverskog RAID-a
		- ako matična ploča zakaže pristup podatcima je vjerojatno moguć samo s identične matične ploče
		- nisu implementirane sve RAID razine, najčešće su implementirane samo razine 0 i 1
	- neke slabije hardverske RAID kartice također spadaju u ovu kategoriju

## RAID razine

RAID razine se dijele na dvije skupine: standardne i ugniježđene. Ugniježđene koriste kombinacije standardnih razina. Najčešće korištene standardne razine su: 0, 1, 5 i 6. Najčešće ugniježđene razine koje se koriste su: 1+0.

### RAID 0

RAID 0 razina zapravo se ne bi ni trebala smatrati RAID razinom jer ne unosi nikakvu redundanciju. Za korištenje ove razine potrebno je koristiti minimalno dva diska u RAID polju. Podatci se spremaju u trakama (*eng. stripes*) koji su definirane pri konfiguraciji RAID 0 sustava. Kad se neki skup podataka piše na RAID 0 konfigurirane diskove, on se dijeli na trake. Traka se dijeli na broj diskova koji postoje u RAID 0 sustavu i tako se ti dijelovi pišu paralelno na svaki disk. Obrnuto je kod čitanja skupa podataka iz RAID 0 konfiguriranih diskova. Iz sustava se paralelno čitaju dijelovi trake gdje se oni sastavljaju u jednu cjelinu. Iščitane trake se sastavljaju u skup podataka. Primjer RAID 0 sustava s dva diska:

```
        Data D in RAID 0

        *--------*          *--------*
        |___D0___|          |___D1___|        -> One stripe (D0:D1)
        |___D2___|          |___D3___|
        |___D4___|          |___D5___|
        |___D6___|          |___D7___|
        |___D8___|          |___D9___|
        |________|          |________|
        |________|          |________|
        |________|          |________|
        |________|          |________|
        |________|          |________|
        |________|          |________|

          DISK 1              DISK 2
```

U prethodnoj shemi skup podataka "D" se nalazi u trakama. Ovo ubrzava performanse pisanja podataka u pohranu i čitanje podataka iz pohrane. Iskorištenost diska za pohranu podataka je 100 %.

U slučaju kvara jednog od diska, cijeli RAID 0 sustav pada. RAID 0 konfiguracija ne povećava toleranciju kvara.

### RAID 1

RAID 1 razina koristi zrcaljenje podataka. Za korištenje ove razine potrebno je koristiti minimalno dva diska u RAID polju. Primjer RAID 1 sustava s dva diska:

```
        Data D in RAID 1

        *--------*          *--------*
        |___D0___|          |___D0___|
        |___D1___|          |___D1___|
        |___D2___|          |___D2___|
        |___D3___|          |___D3___|
        |___D4___|          |___D4___|
        |___D5___|          |___D5___|
        |___D6___|          |___D6___|
        |___D7___|          |___D7___|
        |___D8___|          |___D8___|
        |___D9___|          |___D9___|
        |________|          |________|

          DISK 1              DISK 2
```

U prethodnoj shemi skup podataka "D" se nalazi na dva diska. Općenito brzina pisanja podataka u pohranu jednaka je brzini pisanja diska najsporijeg diska (jer podatci moraju biti zrcaljeni). Brzina čitanja podataka iz diska je jednaka brzini čitanja najbržeg diska. Iskorištenost diska za pohranu podataka je 100 %.

RAID 1 sustav će nastaviti raditi sve dok je funkcionalan bar i jedan disk. RAID 1 konfiguracija povećava toleranciju kvara.

### RAID 5

RAID 5 razina dijeli podatke na trake (*eng. stripes*) unoseći i dodatni blok pariteta po traci. Za korištenje ove razine potrebno je koristiti minimalno tri diska u RAID polju. Primjer RAID 5 sustava s tri diska:

```
        Data D in RAID 5 with parity (P) block X

        *--------*          *--------*          *--------*
        |_D0_____|          |_D1_____|          |_X0_____| (P)    -> One stripe (D0:D1:X0)
        |_D2_____|          |_X1_____| (P)      |_D3_____|
        |_X2_____| (P)      |_D4_____|          |_D5_____|
        |_D6_____|          |_D7_____|          |_X3_____| (P)
        |_D8_____|          |_X4_____| (P)      |_D9_____|
        |_X5_____| (P)      |_D10____|          |_D11____|
        |_D12____|          |_D13____|          |_X6_____| (P)
        |_D14____|          |_X7_____| (P)      |_D15____|
        |________|          |________|          |________|
        |________|          |________|          |________|
        |________|          |________|          |________|

          DISK 1              DISK 2              DISK 3
```

U prethodnoj shemi skup podataka "D" se nalazi u trakama. U jednoj traci uključen je i blok pariteta. Performanse pisanje i čitanja su ubrzane, slično kao i u RAID 0 razini zbog paralelnog pisanja trake na disk. Međutim, kod pisanja je potrebno uzeti u obzir da je pri svakom pisanju (ili izmjeni) trake potrebno ponovno računati paritet. Iskorištenost diska za pohranu podataka je u ovom slučaju 66.7 %.

U slučaju kvara jednog od diska, RAID 5 sustav nastavlja s radom, podatci dijela trake na pokvarenom disku se mogu izračunati uz ostale dijelove trake na drugim diskovima računanjem pariteta. Dakle tolerira se kvar jednog diska.

### RAID 6

RAID 6 razina također dijeli podatke na trake (*eng. stripes*) unoseći i dva dodatna bloka pariteta po traci. Za korištenje ove razine potrebno je koristiti minimalno četiri diska u RAID polju. Primjer RAID 6 sustava s četiri diska:

```
        Data D in RAID 6 with parity (P) blocks X and Y

        *--------*          *--------*          *--------*          *--------*
        |_D0_____|          |_D1_____|          |_X0_____| (P)      |_Y0_____| (P)    -> One stripe (D0:D1:X0:Y0)
        |_D2_____|          |_X1_____| (P)      |_Y1_____| (P)      |_D3_____|
        |_X2_____| (P)      |_Y2_____| (P)      |_D4_____|          |_D5_____|
        |_Y3_____| (P)      |_D6_____|          |_D7_____|          |_X3_____| (P)
        |_D8_____|          |_D9_____|          |_X4_____| (P)      |_Y4_____| (P)
        |_D10____|          |_X5_____| (P)      |_Y5_____| (P)      |_D11____|
        |_X6_____| (P)      |_Y6_____| (P)      |_D12____|          |_D13____|
        |_Y7_____| (P)      |_D14____|          |_D15____|          |_X7_____| (P)
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|

          DISK 1              DISK 2              DISK 3              DISK 4
```

U prethodnoj shemi skup podataka "D" se nalazi u trakama. U jednoj traci uključena su i dva bloka pariteta. Performanse pisanje i čitanja su ubrzane, slično kao i u RAID 0 razini zbog paralelnog pisanja trake na disk. Međutim, kod pisanja je potrebno uzeti u obzir da je pri svakom pisanju (ili izmjeni) trake potrebno ponovno računati paritete. Iskorištenost diska za pohranu podataka je u ovom slučaju 50 %.

U slučaju kvara jednog od diska, RAID 5 sustav nastavlja s radom, podatci dijela trake na pokvarenom disku se mogu izračunati uz ostale dijelove trake na drugim diskovima računanjem pariteta. Dakle tolerira se kvar dva diska.

### RAID 10

RAID 10 je kombinacija RAID 1 i RAID 0 razina. Koriste se trake čiji su dijelovi zrcaljeni. Za korištenje ove razine potrebno je koristiti minimalno četiri diska. Primjer RAID 10 sustava s četiri diska:

```
        Data D in RAID 10

        *--------*          *--------*          *--------*          *--------*
        |___D0___|          |___D0___|          |___D1___|          |___D1___|
        |___D2___|          |___D2___|          |___D3___|          |___D3___|
        |___D4___|          |___D4___|          |___D5___|          |___D5___|
        |___D6___|          |___D6___|          |___D7___|          |___D7___|
        |___D8___|          |___D8___|          |___D9___|          |___D9___|
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|
        |________|          |________|          |________|          |________|

          DISK 1              DISK 2              DISK 3              DISK 4
```

U prethodnoj shemi skup podataka "D" se nalazi u trakama čiji su dijelovi zrcaljeni. Uz performanse ubrzane performanse pisanja i čitanja uvodi i redundantnost.

RAID 10 može preživjeti dva kvara diska pod uvjetom da oba pokvarena diska nisu diskovi koji su zrcaljeni.

## Inicijalne postavke okoline prije primjera korištenja

Primjer korištenja RAID-a će se pokazivati u virtualnom stroju na računalu koje pokreće Arch Linux. Virtualni stroj će pokretati Debian distribuciju. Prije pokazivanja primjera korištenja potrebno je pokrenuti jedan [QEMU](../../virtualization/qemu) virtualni stroj koji pokreće Debian:

```
        *---------------------------*
        | ARCH LINUX HOST           |
        |                           |
        |    *-----------------*    |
        |    | QEMU VM RUNNING |    |
        |    | DEBIAN          |    |
        |    |                 |    |
        |    |                 |    |
        |    *-----------------*    |
        |                           |
        *---------------------------*
```

Na Arch Linuxu, potrebno je prvo napraviti direktorij *raid-vm*, premjestiti se u njega i preuzeti ISO sliku Debiana (inačica Bookworm 12.8.0 u ovom slučaju):

```
mkdir ~/raid-vm
cd ~/raid-vm
wget http://debian.carnet.hr/debian-cd/12.8.0/amd64/iso-cd/debian-12.8.0-amd64-netinst.iso
```

Ako QEMU i OVMF nisu instalirani, potrebno ih je instalirati naredbom ```sudo pacman -S qemu-full edk2-ovmf```. Na virtualnoj slici diska će se instalirati minimalna verzija Debian distribucije. Iz toga slijedi da je preporučena veličina virtualnog tvrdog diska 4 GiB. Stvaranje virtualnog tvrdog diska radi se naredbom:

```
qemu-img create -f qcow2 disk-backup.qcow2 4G
```

Prethodna naredba će stvoriti datoteku *disk-backup.qcow2* na kojoj će se instalirati minimalna verzija Debiana. Nakon instalacije Debiana, ovaj disk će se kopirati jednom. Disk *disk-backup.qcow2* će služiti kao sigurnosna kopija.

Također, potrebno je kopirati i OVMF (UEFI) spremnik varijabli u trenutačni direktorij:

```
cp /usr/share/edk2/x64/OVMF_VARS.4m.fd .
```

Neće biti potrebno namještati nikakve mrežne postavke već će se virtualni stroj pokretati samo sa SLIRP mrežnim postavkama naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive if=virtio,format=qcow2,file=disk-backup.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Ovakav način rada je UEFI način rada i on će se koristiti za sva buduća pokretanja virtualnog stroja. To znači da je na tvrdom disku potrebno koristiti GPT shemu particioniranja jer se inače takve sheme koriste u RAID sustavima koji imaju veličine diska veće od 2 TiB. Parametri poput ukupne dodijeljene memorije (*-m*) ili broj dodijeljenih procesorskih jezgri (*-smp*) virtualnom stroju ne moraju biti točno jednaki predloženome već ih se može prilagoditi po potrebi i mogućnostima računala domaćina. Najlakši način instalacije Debiana na virtualni tvrdi disk je grafičkom instalacijom. Dakle, potrebno je odabrati *Graphical Install* te namjestiti sljedeću konfiguraciju:

- *Language:* "English"
- *Location:* "United States"
- *Keymap:* [ovisno o rasporedu tipkovnice]
- *Hostname:* "debian"
- *Domain name:*
- *Root password:* "passroot"
- *Full name for the new user:*
- *Username for your account:* "user"
- *User password:* "passuser"
- *Timezone:* "Eastern"
- *Partitioning Method:* "Guided - use entire disk"
- *Select disk to partition:* "Virtual disk 1 (vda) - 4.3 GB Virtio Block Device"
- *Partitioning scheme:* "All files in one partition (recommended for new users)"
- odabrati "Finish partitioning and write changes to disk"
- *Write changes to disk:* "Yes"
- *Scan extra installation media:* "No"
- *Debian archive mirror country:* "Croatia"
- *Debian archive mirror:* "deb.debian.org"
- *HTTP proxy information:*
- *Participate in the package usage survey:* "No"
- *Choose software to install:* "SSH server" i "standard system utilities"

Nakon instalacije, sustav na virtualnom stroju će se ponovno pokrenuti. Potrebno je prijaviti se *root* računom i lozinkom *passroot*. Zatim je potrebno postaviti ispravnu vremensku zonu, za Hrvatsku bi to bilo naredbom:

```
timedatectl set-timezone Europe/Zagreb
```

Nakon toga je potrebno instalirati alat *sudo* koji će korisnicima koji su u grupi *sudo* omogućiti izvršavanje privilegiranih ovlasti:

```
apt install sudo
```

Nakon instalacije alata, korisnika *user* je potrebno dodati u grupu *sudo* naredbom:

```
usermod -aG sudo user
```

Za postavljanje RAID polja koristit će se alat [mdadm](https://man.archlinux.org/man/mdadm.8). Alat *mdadm* služi za konfiguriranje softverskih RAID razina. Na virtualnom stroju ga se može instalirati naredbom:

```
apt install mdadm
```

Više informacija u vezi korištenja alata *mdadm* može se naći [ovdje](https://wiki.archlinux.org/title/RAID).

Na kraju je potrebno ugasiti virtualni stroj:

```
poweroff
```

Nakon gašenja stroja, virtualni tvrdi disk *disk-baskup.qcow2* je potrebno kopirati jednom:

```
cp disk-backup.qcow2 disk.qcow2
```

## Primjer korištenja

U sljedećim primjerima demonstrirat će se RAID 0, RAID 1, RAID 5, RAID 6 i RAID 10 razine. Više informacija o korištenju alata *mdadm* može se naći [ovdje](https://linux.die.net/man/8/mdadm).

### Primjer RAID 0 polja

Prije pokretanja virtualnog stroja, bit će potrebno stvoriti dva diska veličine 32 MiB čije će particije biti dio RAID 0 polja. Dakle potrebno je izvršiti naredbe:

```
qemu-img create -f qcow2 raid-disk-1.qcow2 32M
qemu-img create -f qcow2 raid-disk-2.qcow2 32M
```

Naredba za pokretanje virtualnog stroja će u ovom slučaju biti:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive if=virtio,format=qcow2,file=disk.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-1.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-2.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Na virtualni stroj je potrebno prijaviti se kao korisnik *root* (zaporkom *passroot*) i particionirati diskove */dev/vdb* i */dev/vdc* uz pomoć GPT sheme particioniranja. Dakle, alatom *fdisk* (naredba ```fdisk [disk]```) potrebno je:

- postaviti GPT shemu particioniranja: naredba *g*
- stvoriti novu particiju jednu jedinu particiju maksimalne veličine: naredba *n*
- postaviti tip particije na 42, odnosno *Linux RAID*: naredba *t*
- zapisati navedene promjene: naredba *w*

RAID polje postavljat će se nad particijama diskova umjesto nad cijelim diskovima zbog problema koji nastaje kada se mijenja pokvareni RAID disk u RAID polju.

#### Inicijalizacija RAID 0 polja

Stvaranje RAID 0 polja s particijama */dev/vdb1* i */dev/vdc1* radi se naredbom:

```
mdadm --create /dev/md/debian:raid0 --verbose --level=0 --homehost=debian --raid-devices=2 /dev/vdb1 /dev/vdc1
```

Na particijama */dev/vdb1* i */dev/vdc1* stvorit će se RAID superblok odnosno metapodatci koji govore kojem RAID polju diskovi pripadaju. Kada se virtualni stroj ponovno pokrene, *mdadm* servis će detektirati diskove koji su dio nekog RAID polja čitajući im RAID superblok te sastavljajući ih u RAID polje. Nakon toga, kada se *udev* servis pokrene, u slučaju RAID polja stvaraju se datoteke uređaja koje predstavljaju polja oblika */dev/[lokacija]/[vrijednost homehost varijable]:[ime datoteke uređaja]*. U slučaju navedenog primjera, stvorit će se poveznica */dev/md/debian:raid0* koja će pokazivati na datoteku uređaja */dev/md[broj]* koja predstavlja RAID 0 polje. Zbog konvencija o imenovanju RAID simboličkih poveznica (*Metadata version 1.2*), sljedećim pokretanjem virtualnog stroja prilikom automatske konfiguracije RAID 0 polja, */dev/md/[ime računala]:raid0* će se zvati */dev/md/[vrijednost homehosta]:raid0*.

Prije aktivacije RAID 0 polja, poželjno je eksplicitno njegovu konfiguraciju dodati u datoteku */etc/mdadm/mdadm.conf*:

```
mdadm --detail --scan | tail -n 1 | tee -a /etc/mdadm/mdadm.conf
```

Iako servis *mdadm* zna prepoznati RAID diskove čitajući im superblok, dodatne informacije kao što je prethodna stavka omogućuje brže prepoznavanje RAID polja što je korisno kod sustava s velikim brojem RAID diskova.

Konačno, aktivacija RAID 0 polja se može napraviti naredbom:

```
mdadm --assemble --scan
```

Ova naredba automatski prepoznaje RAID diskove i aktivira ih. Stanje polja može se vidjeti naredbom:

```
cat /proc/mdstat
```

Formatiranje datotečnog sustava u RAID 0 polju moguće je naredbom:

```
mkfs.ext4 -v -b 4096 -E stride=128,stripe-width=256 /dev/md/debian\:raid0
```

Prije formatiranja se mora uzeti u obzir veličina i trake i djelića trake koji se piše na jednom disku u RAID 0 polju. Dakle, parametri su sljedeći:

- veličina jedne trake (*eng. chunk size*): 512 KiB (saznato naredbom ```mdadm --detail /dev/md/md0 | grep 'Chunk Size'```)
- veličina jednog EXT4 bloka (*eng. EXT4 block size*): 4 KiB (saznato naredbom ```cat /etc/mke2fs.conf``` pod općenitom stavkom "blocksize")
- broj EXT4 blokova u djeliću trake (*eng. stride*): 128 (saznato dijeljenjem: ```512 KiB / 4 KiB```)
- broj EXT4 blokova u jednoj traci (*eng. stripe size*): 256 (saznato množenjem: ```128 * 2```, 2 je broj diskova koji se koriste u RAID 0 polju)

Montiranje EXT4 datotečnog sustava RAID 0 polja na točku */mnt/raid0* moguće je naredbom:

```
mkdir /mnt/raid0
mount /dev/md/debian\:raid0 /mnt/raid0
```

#### Primjer kvara diska odnosno particije u RAID 0 polju

RAID 0 polje ne nudi nikakvu redundanciju. Kvarom jednog diska odnosno particije onemogućuje se pristup svim diskovima odnosno particijama.

Ako se virtualni stroj pokrene bez jednog RAID diska koji je u RAID 0 polju, RAID 0 polje neće postati aktivno i neće ga se moći montirati.

#### Deinicijalizacija RAID 0 polja

Ako već nije napravljeno, prvo je potrebno odmontirati RAID 0 polje i obrisati direktorij */mnt/raid0* naredbama:

```
umount /mnt/raid0
rmdir /mnt/raid0
```

Deaktivacija RAID 0 polja radi se naredbom:

```
mdadm --stop /dev/md/debian\:raid0
```

Uklanjanje superbloka s particija koji su bili dio RAID 0 polja radi se naredbama:

```
mdadm --misc --zero-superblock /dev/vdb1
mdadm --misc --zero-superblock /dev/vdc1
```

Za kraj, potrebno je ukloniti nadodanu liniju u konfiguracijskoj datoteci */etc/mdadm/mdadm.conf*.

### Primjer RAID 1 polja

Prije pokretanja virtualnog stroja, bit će potrebno stvoriti dva diska veličine 32 MiB čije će particije biti dio RAID 1 polja. Dakle potrebno je izvršiti naredbe:

```
qemu-img create -f qcow2 raid-disk-1.qcow2 32M
qemu-img create -f qcow2 raid-disk-2.qcow2 32M
```

Naredba za pokretanje virtualnog stroja će u ovom slučaju biti:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive if=virtio,format=qcow2,file=disk.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-1.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-2.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Na virtualni stroj je potrebno prijaviti se kao korisnik *root* (zaporkom *passroot*) i particionirati diskove */dev/vdb* i */dev/vdc* uz pomoć GPT sheme particioniranja. Dakle, alatom *fdisk* (naredba ```fdisk [disk]```) potrebno je:

- postaviti GPT shemu particioniranja: naredba *g*
- stvoriti novu particiju jednu jedinu particiju maksimalne veličine: naredba *n*
- postaviti tip particije na 42, odnosno *Linux RAID*: naredba *t*
- zapisati navedene promjene: naredba *w*

RAID polje postavljat će se nad particijama diskova umjesto nad cijelim diskovima zbog problema koji nastaje kada se mijenja pokvareni RAID disk u RAID polju.

#### Inicijalizacija RAID 1 polja

Stvaranje RAID 1 polja s particijama */dev/vdb1* i */dev/vdc1* radi se naredbom:

```
mdadm --create /dev/md/debian:raid1 --verbose --level=1 --homehost=debian --raid-devices=2 /dev/vdb1 /dev/vdc1
```

Ako se pojavi obavijest da RAID polje ima metapodatke na početku i da polje nije možda sposobno biti pohrana za */boot* datoteke, potrebno je i dalje nastaviti sa stvaranjem polja upisivanjem "y".

Na particijama */dev/vdb1* i */dev/vdc1* stvorit će se RAID superblok odnosno metapodatci koji govore kojem RAID polju diskovi pripadaju. Kada se virtualni stroj ponovno pokrene, *mdadm* servis će detektirati diskove koji su dio nekog RAID polja čitajući im RAID superblok te sastavljajući ih u RAID polje. Nakon toga, kada se *udev* servis pokrene, u slučaju RAID polja stvaraju se datoteke uređaja koje predstavljaju polja oblika */dev/[lokacija]/[vrijednost homehost varijable]:[ime datoteke uređaja]*. U slučaju navedenog primjera, stvorit će se poveznica */dev/md/debian:raid1* koja će pokazivati na datoteku uređaja */dev/md[broj]* koja predstavlja RAID 1 polje. Zbog konvencija o imenovanju RAID simboličkih poveznica (*Metadata version 1.2*), sljedećim pokretanjem virtualnog stroja prilikom automatske konfiguracije RAID 1 polja, */dev/md/[ime računala]:raid1* će se zvati */dev/md/[vrijednost homehosta]:raid1*.

Prije aktivacije RAID 1 polja, poželjno je eksplicitno njegovu konfiguraciju dodati u datoteku */etc/mdadm/mdadm.conf*:

```
mdadm --detail --scan | tail -n 1 | tee -a /etc/mdadm/mdadm.conf
```

Iako servis *mdadm* zna prepoznati RAID diskove čitajući im superblok, dodatne informacije kao što je prethodna stavka omogućuje brže prepoznavanje RAID polja što je korisno kod sustava s velikim brojem RAID diskova.

Konačno, aktivacija RAID 1 polja se može napraviti naredbom:

```
mdadm --assemble --scan
```

Ova naredba automatski prepoznaje RAID diskove i aktivira ih. Stanje polja može se vidjeti naredbom:

```
cat /proc/mdstat
```

Formatiranje datotečnog sustava u RAID 1 polju moguće je naredbom:

```
mkfs.ext4 -v -b 4096 /dev/md/debian\:raid1
```

RAID 1 ne koristi trake pa nije potrebno postavljati dodatne parametre.

Montiranje EXT4 datotečnog sustava RAID 1 polja na točku */mnt/raid1* moguće je naredbom:

```
mkdir /mnt/raid1
mount /dev/md/debian\:raid1 /mnt/raid1
```

#### Primjer kvara diska odnosno particije u RAID 1 polju

RAID 1 polje nudi redundanciju. Sustav će u slučaju kvara jednog diska nastaviti raditi, ali u degradiranom stanju.

Kao primjer, može se pisati na montiranom mjestu pisati datoteka veličine 1 MiB i datoteka koja sadrži sažetak prethodne datoteke:

```
dd if=/dev/urandom of=/mnt/raid1/test.bin bs=1M count=1
sha256sum /mnt/raid1/test.bin > /mnt/raid1/test_checksum.txt
```

Integritet datoteke */mnt/raid1/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid1/test.bin -c /mnt/raid1/test_checksum.txt
```

Particiju */dev/vdc1* je potrebno označiti pokvarenom:

```
mdadm --manage /dev/md/debian\:raid1 --fail /dev/vdc1
```

Stanje RAID 1 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[U_]" koja označava da je jedan disk nedostupan. Da su dva diska dostupna pokazivalo bi "[UU]". Pisanje i čitanje na montiranu točku je i dalje omogućeno, ali će se nove datoteke spremati samo na jedan disk odnosno ispravnu particiju */dev/vdb1*.

Primjer pisanja datoteke veličine 2 MiB na preostali funkcionalni disk:

```
dd if=/dev/urandom of=/mnt/raid1/test_2.bin bs=2M count=1
```

Pokvareni disk odnosno particija se može zamijeniti s ispravnim diskom odnosno particijom (u ovom slučaju istom particijom) naredbama:

```
mdadm --manage /dev/md/debian\:raid1 --remove /dev/vdc1
mdadm --manage /dev/md/debian\:raid1 --add /dev/vdc1
```

Rezultat zrcaljenja podataka s */dev/vdb1* particije na novu particiju */dev/vdc1* može se vidjeti s naredbom:

```
dmesg
```

Stanje RAID 1 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UU]" što znači da su svi diskovi u RAID 1 polju funkcionalni. Integritet datoteke */mnt/raid1/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid1/test.bin -c /mnt/raid1/test_checksum.txt
```

#### Deinicijalizacija RAID 1 polja

Ako već nije napravljeno, prvo je potrebno odmontirati RAID 1 polje i obrisati direktorij */mnt/raid1* naredbama:

```
umount /mnt/raid1
rmdir /mnt/raid1
```

Deaktivacija RAID 1 polja radi se naredbom:

```
mdadm --stop /dev/md/debian\:raid1
```

Uklanjanje superbloka s particija koji su bili dio RAID 1 polja radi se naredbama:

```
mdadm --misc --zero-superblock /dev/vdb1
mdadm --misc --zero-superblock /dev/vdc1
```

Za kraj, potrebno je ukloniti nadodanu liniju u konfiguracijskoj datoteci */etc/mdadm/mdadm.conf*.

### Primjer RAID 5 polja

Prije pokretanja virtualnog stroja, bit će potrebno stvoriti tri diska veličine 32 MiB čije će particije biti dio RAID 5 polja. Dakle potrebno je izvršiti naredbe:

```
qemu-img create -f qcow2 raid-disk-1.qcow2 32M
qemu-img create -f qcow2 raid-disk-2.qcow2 32M
qemu-img create -f qcow2 raid-disk-3.qcow2 32M
```

Naredba za pokretanje virtualnog stroja će u ovom slučaju biti:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive if=virtio,format=qcow2,file=disk.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-1.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-2.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-3.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Na virtualni stroj je potrebno prijaviti se kao korisnik *root* (zaporkom *passroot*) i particionirati diskove */dev/vdb* i */dev/vdc* uz pomoć GPT sheme particioniranja. Dakle, alatom *fdisk* (naredba ```fdisk [disk]```) potrebno je:

- postaviti GPT shemu particioniranja: naredba *g*
- stvoriti novu particiju jednu jedinu particiju maksimalne veličine: naredba *n*
- postaviti tip particije na 42, odnosno *Linux RAID*: naredba *t*
- zapisati navedene promjene: naredba *w*

RAID polje postavljat će se nad particijama diskova umjesto nad cijelim diskovima zbog problema koji nastaje kada se mijenja pokvareni RAID disk u RAID polju.

#### Inicijalizacija RAID 5 polja

Stvaranje RAID 5 polja s particijama */dev/vdb1*, */dev/vdc1* i */dev/vdd1* radi se naredbom:

```
mdadm --create /dev/md/debian:raid5 --verbose --level=5 --homehost=debian --raid-devices=3 /dev/vdb1 /dev/vdc1 /dev/vdd1
```

Na particijama */dev/vdb1*, */dev/vdc1* i */dev/vdd1* stvorit će se RAID superblok odnosno metapodatci koji govore kojem RAID polju diskovi pripadaju. Kada se virtualni stroj ponovno pokrene, *mdadm* servis će detektirati diskove koji su dio nekog RAID polja čitajući im RAID superblok te sastavljajući ih u RAID polje. Nakon toga, kada se *udev* servis pokrene, u slučaju RAID polja stvaraju se datoteke uređaja koje predstavljaju polja oblika */dev/[lokacija]/[vrijednost homehost varijable]:[ime datoteke uređaja]*. U slučaju navedenog primjera, stvorit će se poveznica */dev/md/debian:raid5* koja će pokazivati na datoteku uređaja */dev/md[broj]* koja predstavlja RAID 5 polje. Zbog konvencija o imenovanju RAID simboličkih poveznica (*Metadata version 1.2*), sljedećim pokretanjem virtualnog stroja prilikom automatske konfiguracije RAID 5 polja, */dev/md/[ime računala]:raid5* će se zvati */dev/md/[vrijednost homehosta]:raid5*.

Prije aktivacije RAID 5 polja, poželjno je eksplicitno njegovu konfiguraciju dodati u datoteku */etc/mdadm/mdadm.conf*:

```
mdadm --detail --scan | tail -n 1 | tee -a /etc/mdadm/mdadm.conf
```

Iako servis *mdadm* zna prepoznati RAID diskove čitajući im superblok, dodatne informacije kao što je prethodna stavka omogućuje brže prepoznavanje RAID polja što je korisno kod sustava s velikim brojem RAID diskova.

Konačno, aktivacija RAID 5 polja se može napraviti naredbom:

```
mdadm --assemble --scan
```

Ova naredba automatski prepoznaje RAID diskove i aktivira ih. Stanje polja može se vidjeti naredbom:

```
cat /proc/mdstat
```

Formatiranje datotečnog sustava u RAID 5 polju moguće je naredbom:

```
mkfs.ext4 -v -b 4096 -E stride=128,stripe-width=256 /dev/md/debian\:raid5
```

Prije formatiranja se mora uzeti u obzir veličina i trake i djelića trake koji se piše na jednom disku u RAID 5 polju. Dakle, parametri su sljedeći:

- veličina jedne trake (*eng. chunk size*): 512 KiB (saznato naredbom ```mdadm --detail /dev/md/md0 | grep 'Chunk Size'```)
- veličina jednog EXT4 bloka (*eng. EXT4 block size*): 4 KiB (saznato naredbom ```cat /etc/mke2fs.conf``` pod općenitom stavkom "blocksize")
- broj EXT4 blokova u djeliću trake (*eng. stride*): 128 (saznato dijeljenjem: ```512 KiB / 4 KiB```)
- broj EXT4 blokova u jednoj traci (*eng. stripe size*): 256 (saznato množenjem: ```128 * 2```, 2 je broj **podatkovnih** diskova koji se koriste u RAID 5 polju, znači u izračunu se **ne uključuje paritetni disk**)

Montiranje EXT4 datotečnog sustava RAID 5 polja na točku */mnt/raid5* moguće je naredbom:

```
mkdir /mnt/raid5
mount /dev/md/debian\:raid5 /mnt/raid5
```

#### Primjer kvara diska odnosno particije u RAID 5 polju

RAID 5 polje nudi redundanciju. Sustav će u slučaju kvara jednog diska nastaviti raditi, ali u degradiranom stanju.

Kao primjer, može se pisati na montiranom mjestu pisati datoteka veličine 1 MiB i datoteka koja sadrži sažetak prethodne datoteke:

```
dd if=/dev/urandom of=/mnt/raid5/test.bin bs=1M count=1
sha256sum /mnt/raid5/test.bin > /mnt/raid5/test_checksum.txt
```

Integritet datoteke */mnt/raid5/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid5/test.bin -c /mnt/raid5/test_checksum.txt
```

Particiju */dev/vdd1* je potrebno označiti pokvarenima:

```
mdadm --manage /dev/md/debian\:raid5 --fail /dev/vdd1
```

Stanje RAID 5 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UU_]" koja označava da je jedan disk nedostupan. Da su sva tri diska dostupna pokazivalo bi "[UUU]". Pisanje i čitanje na montiranu točku je i dalje omogućeno, ali će se nove datoteke odnosno djelići trake spremati samo na dva diska odnosno ispravne particije */dev/vdb1* i */dev/vdc1*. Treći dijelovi traka koji su se trebali spremati na pokvareni disk se mogu izračunati uz pomoć ostala dva dijela trake.

Primjer pisanja datoteke veličine 2 MiB na preostale funkcionalne diskove:

```
dd if=/dev/urandom of=/mnt/raid5/test_2.bin bs=2M count=1
```

Pokvareni disk odnosno particija se može zamijeniti s ispravnim diskom odnosno particijom (u ovom slučaju istom particijom) naredbama:

```
mdadm --manage /dev/md/debian\:raid5 --remove /dev/vdd1
mdadm --manage /dev/md/debian\:raid5 --add /dev/vdd1
```

Rezultat računanja trećih dijelova traka uz pomoć dijelova s particija */dev/vdb1* i */dev/vdc1* te spremanje tih dijelova na novu particiju */dev/vdd1* može se vidjeti s naredbom:

```
dmesg
```

Stanje RAID 5 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UUU]" što znači da su svi diskovi u RAID 5 polju funkcionalni. Integritet datoteke */mnt/raid5/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid5/test.bin -c /mnt/raid5/test_checksum.txt
```

#### Deinicijalizacija RAID 5 polja

Ako već nije napravljeno, prvo je potrebno odmontirati RAID 5 polje i obrisati direktorij */mnt/raid5* naredbama:

```
umount /mnt/raid5
rmdir /mnt/raid5
```

Deaktivacija RAID 5 polja radi se naredbom:

```
mdadm --stop /dev/md/debian\:raid5
```

Uklanjanje superbloka s particija koji su bili dio RAID 5 polja radi se naredbama:

```
mdadm --misc --zero-superblock /dev/vdb1
mdadm --misc --zero-superblock /dev/vdc1
mdadm --misc --zero-superblock /dev/vdd1
```

Za kraj, potrebno je ukloniti nadodanu liniju u konfiguracijskoj datoteci */etc/mdadm/mdadm.conf*.

### Primjer RAID 6 polja

Prije pokretanja virtualnog stroja, bit će potrebno stvoriti četiri diska veličine 32 MiB čije će particije biti dio RAID 6 polja. Dakle potrebno je izvršiti naredbe:

```
qemu-img create -f qcow2 raid-disk-1.qcow2 32M
qemu-img create -f qcow2 raid-disk-2.qcow2 32M
qemu-img create -f qcow2 raid-disk-3.qcow2 32M
qemu-img create -f qcow2 raid-disk-4.qcow2 32M
```

Naredba za pokretanje virtualnog stroja će u ovom slučaju biti:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive if=virtio,format=qcow2,file=disk.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-1.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-2.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-3.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-4.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Na virtualni stroj je potrebno prijaviti se kao korisnik *root* (zaporkom *passroot*) i particionirati diskove */dev/vdb* i */dev/vdc* uz pomoć GPT sheme particioniranja. Dakle, alatom *fdisk* (naredba ```fdisk [disk]```) potrebno je:

- postaviti GPT shemu particioniranja: naredba *g*
- stvoriti novu particiju jednu jedinu particiju maksimalne veličine: naredba *n*
- postaviti tip particije na 42, odnosno *Linux RAID*: naredba *t*
- zapisati navedene promjene: naredba *w*

RAID polje postavljat će se nad particijama diskova umjesto nad cijelim diskovima zbog problema koji nastaje kada se mijenja pokvareni RAID disk u RAID polju.

#### Inicijalizacija RAID 6 polja

Stvaranje RAID 6 polja s particijama */dev/vdb1*, */dev/vdc1*, */dev/vdd1* i */dev/vde1* radi se naredbom:

```
mdadm --create /dev/md/debian:raid6 --verbose --level=6 --homehost=debian --raid-devices=4 /dev/vdb1 /dev/vdc1 /dev/vdd1 /dev/vde1
```

Na particijama */dev/vdb1*, */dev/vdc1*, */dev/vdd1* i /dev/vdd1 stvorit će se RAID superblok odnosno metapodatci koji govore kojem RAID polju diskovi pripadaju. Kada se virtualni stroj ponovno pokrene, *mdadm* servis će detektirati diskove koji su dio nekog RAID polja čitajući im RAID superblok te sastavljajući ih u RAID polje. Nakon toga, kada se *udev* servis pokrene, u slučaju RAID polja stvaraju se datoteke uređaja koje predstavljaju polja oblika */dev/[lokacija]/[vrijednost homehost varijable]:[ime datoteke uređaja]*. U slučaju navedenog primjera, stvorit će se poveznica */dev/md/debian:raid6* koja će pokazivati na datoteku uređaja */dev/md[broj]* koja predstavlja RAID 6 polje. Zbog konvencija o imenovanju RAID simboličkih poveznica (*Metadata version 1.2*), sljedećim pokretanjem virtualnog stroja prilikom automatske konfiguracije RAID 6 polja, */dev/md/[ime računala]:raid6* će se zvati */dev/md/[vrijednost homehosta]:raid6*.

Prije aktivacije RAID 6 polja, poželjno je eksplitno njegovu konfiguraciju dodati u datoteku */etc/mdadm/mdadm.conf*:

```
mdadm --detail --scan | tail -n 1 | tee -a /etc/mdadm/mdadm.conf
```

Iako servis *mdadm* zna prepoznati RAID diskove čitajući im superblok, dodatne informacije kao što je prethodna stavka omogućuje brže prepoznavanje RAID polja što je korisno kod sustava s velikim brojem RAID diskova.

Konačno, aktivacija RAID 6 polja se može napraviti naredbom:

```
mdadm --assemble --scan
```

Ova naredba automatski prepoznaje RAID diskove i aktivira ih. Stanje polja može se vidjeti naredbom:

```
cat /proc/mdstat
```

Formatiranje datotečnog sustava u RAID 6 polju moguće je naredbom:

```
mkfs.ext4 -v -b 4096 -E stride=128,stripe-width=256 /dev/md/debian\:raid6
```

Prije formatiranja se mora uzeti u obzir veličina i trake i djelića trake koji se piše na jednom disku u RAID 6 polju. Dakle, parametri su sljedeći:

- veličina jedne trake (*eng. chunk size*): 512 KiB (saznato naredbom ```mdadm --detail /dev/md/md0 | grep 'Chunk Size'```)
- veličina jednog EXT4 bloka (*eng. EXT4 block size*): 4 KiB (saznato naredbom ```cat /etc/mke2fs.conf``` pod općenitom stavkom "blocksize")
- broj EXT4 blokova u djeliću trake (*eng. stride*): 128 (saznato dijeljenjem: ```512 KiB / 4 KiB```)
- broj EXT4 blokova u jednoj traci (*eng. stripe size*): 256 (saznato množenjem: ```128 * 2```, 2 je broj **podatkovnih** diskova koji se koriste u RAID 6 polju, znači u izračunu se **ne uključuju paritetni diskovi**)

Montiranje EXT4 datotečnog sustava RAID 6 polja na točku */mnt/raid6* moguće je naredbom:

```
mkdir /mnt/raid6
mount /dev/md/debian\:raid6 /mnt/raid6
```

#### Primjer kvara diska odnosno particije u RAID 6 polju

RAID 6 polje nudi redundanciju. Sustav će u slučaju kvara dva diska nastaviti raditi, ali u degradiranom stanju.

Kao primjer, može se pisati na montiranom mjestu pisati datoteka veličine 1 MiB i datoteka koja sadrži sažetak prethodne datoteke:

```
dd if=/dev/urandom of=/mnt/raid6/test.bin bs=1M count=1
sha256sum /mnt/raid6/test.bin > /mnt/raid6/test_checksum.txt
```

Integritet datoteke */mnt/raid6/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid6/test.bin -c /mnt/raid6/test_checksum.txt
```

Particije */dev/vdd1* i */dev/vde1* je potrebno označiti pokvarenima:

```
mdadm --manage /dev/md/debian\:raid6 --fail /dev/vdd1
mdadm --manage /dev/md/debian\:raid6 --fail /dev/vde1
```

Stanje RAID 6 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UU__]" koja označava da su dva diska nedostupna. Da su sva četiri diska dostupna pokazivalo bi "[UUUU]". Pisanje i čitanje na montiranu točku je i dalje omogućeno, ali će se nove datoteke odnosno djelići trake spremati samo na dva diska odnosno ispravne particije */dev/vdb1* i */dev/vdc1*. Treći i četvrti dijelovi traka koji su se trebali spremati na pokvarene diskove se mogu izračunati uz pomoć ostala dva dijela trake.

Primjer pisanja datoteke veličine 2 MiB na preostale funkcionalne diskove:

```
dd if=/dev/urandom of=/mnt/raid6/test_2.bin bs=2M count=1
```

Pokvareni diskovi odnosno particije se mogu zamijeniti s ispravnim diskovima odnosno particijama (u ovom slučaju istim particijama) naredbama:

```
mdadm --manage /dev/md/debian\:raid6 --remove /dev/vdd1
mdadm --manage /dev/md/debian\:raid6 --remove /dev/vde1
mdadm --manage /dev/md/debian\:raid6 --add /dev/vdd1
mdadm --manage /dev/md/debian\:raid6 --add /dev/vde1
```

Rezultat računanja trećih dijelova traka uz pomoć dijelova s particija */dev/vdb1* i */dev/vdc1* te spremanje tih dijelova na novu particiju */dev/vdd1* može se vidjeti s naredbom:

```
dmesg
```

Stanje RAID 6 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UUUU]" što znači da su svi diskovi u RAID 6 polju funkcionalni. Integritet datoteke */mnt/raid6/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid6/test.bin -c /mnt/raid6/test_checksum.txt
```

#### Deinicijalizacija RAID 6 polja

Ako već nije napravljeno, prvo je potrebno odmontirati RAID 6 polje i obrisati direktorij */mnt/raid6* naredbama:

```
umount /mnt/raid6
rmdir /mnt/raid6
```

Deaktivacija RAID 6 polja radi se naredbom:

```
mdadm --stop /dev/md/debian\:raid6
```

Uklanjanje superbloka s particija koji su bili dio RAID 6 polja radi se naredbama:

```
mdadm --misc --zero-superblock /dev/vdb1
mdadm --misc --zero-superblock /dev/vdc1
mdadm --misc --zero-superblock /dev/vdd1
mdadm --misc --zero-superblock /dev/vde1
```

Za kraj, potrebno je ukloniti nadodanu liniju u konfiguracijskoj datoteci */etc/mdadm/mdadm.conf*.

### Primjer RAID 10 polja

Prije pokretanja virtualnog stroja, bit će potrebno stvoriti četiri diska veličine 32 MiB čije će particije biti dio RAID 10 polja. Dakle potrebno je izvršiti naredbe:

```
qemu-img create -f qcow2 raid-disk-1.qcow2 32M
qemu-img create -f qcow2 raid-disk-2.qcow2 32M
qemu-img create -f qcow2 raid-disk-3.qcow2 32M
qemu-img create -f qcow2 raid-disk-4.qcow2 32M
```

Naredba za pokretanje virtualnog stroja će u ovom slučaju biti:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive if=virtio,format=qcow2,file=disk.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-1.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-2.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-3.qcow2 \
-drive if=virtio,format=qcow2,file=raid-disk-4.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Na virtualni stroj je potrebno prijaviti se kao korisnik *root* (zaporkom *passroot*) i particionirati diskove */dev/vdb* i */dev/vdc* uz pomoć GPT sheme particioniranja. Dakle, alatom *fdisk* (naredba ```fdisk [disk]```) potrebno je:

- postaviti GPT shemu particioniranja: naredba *g*
- stvoriti novu particiju jednu jedinu particiju maksimalne veličine: naredba *n*
- postaviti tip particije na 42, odnosno *Linux RAID*: naredba *t*
- zapisati navedene promjene: naredba *w*

RAID polje postavljat će se nad particijama diskova umjesto nad cijelim diskovima zbog problema koji nastaje kada se mijenja pokvareni RAID disk u RAID polju.

#### Inicijalizacija RAID 10 polja

Stvaranje RAID 10 polja s particijama */dev/vdb1*, */dev/vdc1*, */dev/vdd1* i */dev/vde1* radi se naredbom:

```
mdadm --create /dev/md/debian:raid10 --verbose --level=10 --homehost=debian --raid-devices=4 /dev/vdb1 /dev/vdc1 /dev/vdd1 /dev/vde1
```

Na particijama */dev/vdb1*, */dev/vdc1*, */dev/vdd1* i /dev/vdd1 stvorit će se RAID superblok odnosno metapodatci koji govore kojem RAID polju diskovi pripadaju. Kada se virtualni stroj ponovno pokrene, *mdadm* servis će detektirati diskove koji su dio nekog RAID polja čitajući im RAID superblok te sastavljajući ih u RAID polje. Nakon toga, kada se *udev* servis pokrene, u slučaju RAID polja stvaraju se datoteke uređaja koje predstavljaju polja oblika */dev/[lokacija]/[vrijednost homehost varijable]:[ime datoteke uređaja]*. U slučaju navedenog primjera, stvorit će se poveznica */dev/md/debian:raid10* koja će pokazivati na datoteku uređaja */dev/md[broj]* koja predstavlja RAID 10 polje. Zbog konvencija o imenovanju RAID simboličkih poveznica (*Metadata version 1.2*), sljedećim pokretanjem virtualnog stroja prilikom automatske konfiguracije RAID 10 polja, */dev/md/[ime računala]:raid10* će se zvati */dev/md/[vrijednost homehosta]:raid10*.

Prije aktivacije RAID 10 polja, poželjno je eksplicitno njegovu konfiguraciju dodati u datoteku */etc/mdadm/mdadm.conf*:

```
mdadm --detail --scan | tail -n 1 | tee -a /etc/mdadm/mdadm.conf
```

Iako servis *mdadm* zna prepoznati RAID diskove čitajući im superblok, dodatne informacije kao što je prethodna stavka omogućuje brže prepoznavanje RAID polja što je korisno kod sustava s velikim brojem RAID diskova.

Konačno, aktivacija RAID 10 polja se može napraviti naredbom:

```
mdadm --assemble --scan
```

Ova naredba automatski prepoznaje RAID diskove i aktivira ih. Stanje polja može se vidjeti naredbom:

```
cat /proc/mdstat
```

Formatiranje datotečnog sustava u RAID 10 polju moguće je naredbom:

```
mkfs.ext4 -v -b 4096 -E stride=128,stripe-width=256 /dev/md/debian\:raid10
```

Prije formatiranja se mora uzeti u obzir veličina i trake i djelića trake koji se piše na jednom disku u RAID 10 polju. Dakle, parametri su sljedeći:

- veličina jedne trake (*eng. chunk size*): 512 KiB (saznato naredbom ```mdadm --detail /dev/md/md0 | grep 'Chunk Size'```)
- veličina jednog EXT4 bloka (*eng. EXT4 block size*): 4 KiB (saznato naredbom ```cat /etc/mke2fs.conf``` pod općenitom stavkom "blocksize")
- broj EXT4 blokova u djeliću trake (*eng. stride*): 128 (saznato dijeljenjem: ```512 KiB / 4 KiB```)
- broj EXT4 blokova u jednoj traci (*eng. stripe size*): 256 (saznato množenjem: ```128 * 2```, 2 je broj **podatkovnih** diskova koji se koriste u RAID 10 polju, znači u izračunu se **ne uključuju zrcaljeni diskovi**)

Montiranje EXT4 datotečnog sustava RAID 10 polja na točku */mnt/raid10* moguće je naredbom:

```
mkdir /mnt/raid10
mount /dev/md/debian\:raid10 /mnt/raid10
```

#### Primjer kvara diska odnosno particije u RAID 10 polju

RAID 10 polje nudi redundanciju. Sustav će u slučaju kvara dva diska koja **nisu međusobno zrcaljeni** nastaviti raditi, ali u degradiranom stanju.

Kao primjer, može se pisati na montiranom mjestu pisati datoteka veličine 1 MiB i datoteka koja sadrži sažetak prethodne datoteke:

```
dd if=/dev/urandom of=/mnt/raid10/test.bin bs=1M count=1
sha256sum /mnt/raid10/test.bin > /mnt/raid10/test_checksum.txt
```

Integritet datoteke */mnt/raid10/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid10/test.bin -c /mnt/raid10/test_checksum.txt
```

Prije označavanje dvije particije pokvarenim, potrebno je saznati koje dvije particije međusobno nisu zrcaljene. To se može naredbom:

```
mdadm --detail /dev/md/debian\:raid10
```

Particije */dev/vdb1* i */dev/vdc1* su zrcaljene i particije */dev/vdd1* i */dev/vde1* su zrcaljene. Dakle, u primjeru će se particije */dev/vdc1* i */dev/vde1* označiti pokvarenima:

```
mdadm --manage /dev/md/debian\:raid10 --fail /dev/vdc1
mdadm --manage /dev/md/debian\:raid10 --fail /dev/vde1
```

Stanje RAID 10 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UU__]" koja označava da su dva diska nedostupna. Da su sva četiri diska dostupna pokazivalo bi "[UUUU]". Pisanje i čitanje na montiranu točku je i dalje omogućeno, ali će se nove datoteke spremati samo na dva diska odnosno ispravne particije */dev/vdb1* i */dev/vdc1*.

Primjer pisanja datoteke veličine 2 MiB na preostale funkcionalne diskove:

```
dd if=/dev/urandom of=/mnt/raid10/test_2.bin bs=2M count=1
```

Pokvareni diskovi odnosno particije se mogu zamijeniti s ispravnim diskovima odnosno particijama (u ovom slučaju istim particijama) naredbama:

```
mdadm --manage /dev/md/debian\:raid10 --remove /dev/vdd1
mdadm --manage /dev/md/debian\:raid10 --remove /dev/vde1
mdadm --manage /dev/md/debian\:raid10 --add /dev/vdd1
mdadm --manage /dev/md/debian\:raid10 --add /dev/vde1
```

Rezultat računanja trećih dijelova traka uz pomoć dijelova s particija */dev/vdb1* i */dev/vdc1* te spremanje tih dijelova na novu particiju */dev/vdd1* može se vidjeti s naredbom:

```
dmesg
```

Stanje RAID 10 polja se može vidjeti naredbom:

```
cat /proc/mdstat
```

Potrebno je uočiti oznaku "[UUUU]" što znači da su svi diskovi u RAID 10 polju funkcionalni. Integritet datoteke */mnt/raid10/test.bin* može se provjeriti naredbom:

```
sha256sum /mnt/raid10/test.bin -c /mnt/raid10/test_checksum.txt
```

#### Deinicijalizacija RAID 10 polja

Ako već nije napravljeno, prvo je potrebno odmontirati RAID 10 polje i obrisati direktorij */mnt/raid10* naredbama:

```
umount /mnt/raid10
rmdir /mnt/raid10
```

Deaktivacija RAID 10 polja radi se naredbom:

```
mdadm --stop /dev/md/debian\:raid10
```

Uklanjanje superbloka s particija koji su bili dio RAID 10 polja radi se naredbama:

```
mdadm --misc --zero-superblock /dev/vdb1
mdadm --misc --zero-superblock /dev/vdc1
mdadm --misc --zero-superblock /dev/vdd1
mdadm --misc --zero-superblock /dev/vde1
```

Za kraj, potrebno je ukloniti nadodanu liniju u konfiguracijskoj datoteci */etc/mdadm/mdadm.conf*.
