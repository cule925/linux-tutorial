# ARCH LINUX

[Arch Linux](https://wiki.archlinux.org/) je Linux distribucija namijenjena za x86-64 procesorske arhitekture. Instalira se kao minimalni osnovni sustav koji se kasnije onda konfigurira po korisnikovim željama. Konfiguracija sustava se najčešće izvodi preko ljuske. Arch Linux nudi svoj vlastiti upravitelj *pacman*. Arch Linux koristi *rolling release* način rada koji omogućuje konstantno ažuriranje paketa bez reinstalacije cijelog sustava. Ovo omogućuje da sustav bude ažuran.

Sljedeće upute opisuju načine instalacije Arch Linuxa na računalo. Također, vrijedilo bi pogledati instalacije s Arch Linuxa s [enkripcijom diska](disk-encryption).

## Preuzimanje slike

Arch Linux se može preuzeti na mnogo načina, moguće je direktno preuzeti uz pomoć HTTP-a ili uz pomoć BitTorrenta. Moguće je također preuzeti sliku za virtualni stroj ili pak Docker sliku ako ga se ne želi izravno instalirati na sustav. Više stvari o načinima preuzimanja se mogu vidjeti [ovdje](https://archlinux.org/download/).

U Linuxu se integritet preuzete datoteke može provjeriti SHA256 sažetkom. SHA256 sažetak ISO slike može se dobiti u Linux terminalu naredbom:

```
sha256sum [ISO datoteka]
```

Potrebno je provjeriti odgovara li sažetak sažetku sa [službene stranice instalacije](https://archlinux.org/download/). Također, autentičnost i integritet moguće je provjeriti preuzimanjem datoteke digitalnog potpisa za ISO sliku (datoteka s nastavkom *sig*) sa [službene stranice](https://archlinux.org/download/) i onda izvršavanjem naredbe:

```
gpg --keyserver-options auto-key-retrieve --verify [SIG datoteka] [ISO datoteka]
```

### Priprema Arch Linuxa za instalaciju na računalo 

Preuzetu ISO datoteku potrebno je zapisati na USB stick. Na Windows se može preuzeti aplikacija koja to radi primjerice [ISO Image Writer](https://apps.kde.org/isoimagewriter/). Ako se koristi Linux distribucija slika se može jednostavno zapisati naredbom u terminalu:

```
dd bs=4M if=[ISO datoteka] of=[datoteka uređaja koja predstavlja cijeli USB, ne samo particiju] status=progress oflag=sync
```

### Priprema Arch Linuxa za instalaciju na virtualni disk koji se pokreće QEMU-om

Ako trenutno koristimo Arch Linux i želimo instalirati [Arch Linux na virtualni disk](https://wiki.archlinux.org/title/QEMU) i pokretat ga takvog u virtualnom stroju potrebno je prvo instalirati [QEMU-u](https://www.qemu.org/) uz pomoć terminala:

```
sudo pacman -S qemu-full
```

Zatim je potrebno stvoriti direktorij *~/arch-linux-vm* i premjestiti se u njega te stvoriti virtualnu sliku diska imena *archlinux.qcow2*, formata QCOW2 i veličine 20 GiB:

```
mkdir ~/arch-linux-vm
cd ~/arch-linux-vm
qemu-img create -f qcow2 archlinux.qcow2 20G
```

#### BIOS + MBR pokretanje

Ako se želi koristiti emulirani BIOS s MBR shemom particioniranja virtualni stroj se jednostavno može pokrenuti naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 4G \
-cpu host \
-smp 4 \
-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1 \
-cdrom [ISO datoteka] \
-device virtio-net,netdev=net0 -drive format=qcow2,file=archlinux.qcow2
```

Nakon instalacije, svako sljedeće pokretanje može se napraviti naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 4G \
-cpu host \
-smp 4 \
-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net,netdev=net0 -drive format=qcow2,file=archlinux.qcow2

```

#### UEFI + GPT pokretanje

U slučaju da se želi emulirati UEFI i koristiti GPT shemu particioniranja potrebno je instalirati i [OVMF](http://www.linux-kvm.org/page/OVMF):

```
sudo pacman -S edk2-ovmf
```

Zatim je potrebno kopirati datoteku */usr/share/edk2/x64/OVMF_VARS.4m.fd* u trenutni direktorij gdje se nalazi virtualna slika diska:

```
cp /usr/share/edk2/x64/OVMF_VARS.4m.fd ~/arch-linux-vm/
```

Konačno, virtualni stroj se jednostavno može pokrenuti naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 4G \
-cpu host \
-smp 2 \
-device virtio-net,netdev=net0 \
-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1 \
-cdrom [ISO datoteka] \
-drive format=qcow2,file=archlinux.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Nakon instalacije, svako sljedeće pokretanje može se napraviti naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 4G \
-cpu host \
-smp 2 \
-device virtio-net,netdev=net0 \
-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1 \
-drive format=qcow2,file=archlinux.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

## Proces instalacije

[Učitavanjem instalacijskog programa](https://wiki.archlinux.org/title/installation_guide) dočekat će nas terminal. Ako se koristi UEFI, potvrda u kojem smo načinu rada (32 bitni ili 64 bitni) se može dobit naredbom: ```cat /sys/firmware/efi/fw_platform_size```.

### Raspored tipkovnice i font

Prvo je potrebno postaviti hrvatski raspored tipkovnice (ako se koristi hrvatska tipkovnica) i font za dijakritičke znakove (č, ć, ž, š, đ):

```
loadkeys croat
setfont Lat2-Terminus16
```

### Mreža

Provjera jesmo li spojeni na mrežu može se napraviti naredbom ```ip address``` dok isprobavanje Internet povezanosti može napraviti naredbom: ```ping archlinux.org```.

#### Spajanje na bežičnu mrežu

U slučaju da nismo spojeni žičanom mrežom (Ethernet), spajanje na mrežu bežično (WiFi) može se napraviti uz pomoć alata [*iwctl*](https://wiki.archlinux.org/title/Iwd#iwctl). Pokretanje konzole *iwctl* se radi naredbom ```iwctl```. Izlistavanje svih WiFi uređaja se može napraviti naredbom:

```
device list
```

Ako je uređaj ugašen, upaliti ga se može naredbom ```device [ime sučelja] set-property Powered on```. Skeniranje mreža i njihovo izlistavanje može se napraviti naredbama:

```
station [ime sučelja] scan
station [ime sučelja] get-networks
```

Konačno, spajanje na mrežu može se izvršiti naredbom:

```
station [ime sučelja] connect [SSID odnosno ime mreže]
```

Izlazak iz konzole *iwctl* može se napraviti naredbom ```quit```.

### Vremenska zona i NTP poslužitelj

Postavljanje vremenske zone i korištenje NTP poslužitelja za vremensku sinkronizaciju može se napraviti naredbama:

```
timedatectl set-timezone Europe/Zagreb
timedatectl set-ntp true
```

Provjera postavki može se napraviti naredbom ```timedatectl```.

### Particioniranje

Ovisno o tome jesmo li koristili BIOS ili UEFI tako ćemo particionirati po MBR shemi ili GPT shemi. Prvo je potrebno izlistat sve diskove naredbom ```fdisk -l``` i onda odabrati ispravan disk za particioniranje naredbom ```fdisk [datoteka uređaja kojeg će se particionirati]``` (za izlazak iz alata može se izvršiti naredba: ```q```).

#### BIOS + MBR particioniranje

Potrebno je stvoriti barem 1 particiju:

* Linux particiju veličine cijelog diska namijenjenu za */* direktorij

Obično se stvara i jedna particija za */home/* direktorij gdje će se spremati korisnički podatci. Ovo je u redu ako želimo odvojiti podatke običnih korisnika od privilegiranog *root* korisnika, ali dovodi do fragmentacije diska. Ako je disk već bio pisan nečime, potrebno je obrisati sve particije naredbom ```d```.

Stvaranje nove MBR particijske sheme može se napraviti naredbom ```o```.

Stvaranje nove Linux particije veličine ostatka diska može se napraviti izvršavanjem naredbi ```n```. Konfiguracija mora biti sljedeća:

* vrsta particije: primarna
* broj particije: 1
* prvi sektor: *default*
* zadnji sektor: *default*

Vrsta particije se mijenja naredbom ```t```. Konfiguracija mora biti sljedeća:

* vrsta particije: 83

Ispis trenutne konfiguracije može se napraviti naredbom ```p```. Konačno, zapis promjena na disk i izlazak iz alata radi se naredbom:

```
w
```

#### UEFI + GPT particioniranje

Potrebno je stvoriti barem 2 particije:

* EFI particiju veličine 512 MiB namijenjenu za */efi/* direktorij
* Linux particiju veličine ostatka diska namijenjenu za */* direktorij

Obično se stvara i jedna particija za */home/* direktorij gdje će se spremati korisnički podatci. Ovo je u redu ako želimo odvojiti podatke običnih korisnika od privilegiranog *root* korisnika, ali dovodi do fragmentacije diska. Ako je disk već bio pisan nečime, potrebno je obrisati sve particije naredbom ```d```.

Stvaranje nove GPT particijske sheme može se napraviti naredbom ```g```.

Stvaranje nove EFI particije veličine 512 MiB može se napraviti izvršavanjem naredbi ```n```. Konfiguracija mora biti sljedeća:

* broj particije: 1
* prvi sektor: *default*
* zadnji sektor: +512M

Vrsta prve particije se mijenja naredbom ```t```. Konfiguracija mora biti sljedeća:

* vrsta particije: 1

Stvaranje nove Linux particije veličine ostatka diska može se napraviti izvršavanjem naredbi ```n```. Konfiguracija mora biti sljedeća:

* broj particije: 2
* prvi sektor: *default*
* zadnji sektor: *default*

Vrsta druge particije se mijenja naredbom ```t```. Konfiguracija mora biti sljedeća:

* broj particije: 2
* vrsta particije: 20

Ispis trenutne konfiguracije može se napraviti naredbom ```p```. Konačno, zapis promjena na disk i izlazak iz alata radi se naredbom:

```
w
```

### Formatiranje

Stvorenim particijama je potrebno napraviti datotečne sustave ovisno o tome koje smo sheme particioniranja koristili.

#### BIOS + MBR formatiranje

Potrebno je formatirati Linux particiju na EXT4 datotečni sustav:

```
mkfs.ext4 [datoteka uređaja koja predstavlja particiju]
```

#### UEFI + GPT formatiranje

Prvo je potrebno formatirati prvu EFI particiju na FAT32 datotečni sustav:

```
mkfs.fat -F 32 [datoteka uređaja koja predstavlja prvu particiju]
```

Zatim je potrebno formatirati drugu Linux particiju na EXT4 datotečni sustav:

```
mkfs.ext4 [datoteka uređaja koja predstavlja drugu particiju]
```

### Montiranje

Particije je potrebno montirati kako bi se mogli instalirati paketi na njima. Montiranje ovisi o tome koju smo shemu particioniranja koristili.

#### BIOS + MBR montiranje

Potrebno je montirati particiju. Točka montiranja je direktorij */mnt*:

```
mount /dev/[particija] /mnt
```

#### UEFI + GPT montiranje

Prvo je potrebno montirati **drugu** odnosno **root** particiju pa onda tek EFI. Točka montiranja je direktorij */mnt*:

```
mount /dev/[druga particija] /mnt
```

Zatim je potrebno stvoriti direktorij */mnt/efi* i tu montirati drugu particiju:

```
mount --mkdir /dev/[prva particija] /mnt/efi
```

Pregled svih montiranih uređaja može se dobiti naredbom ```lsblk```.

### Instalacija paketa

Prije instalacije, potrebno je osvježiti GPG ključeve naredbom:

```
pacman-key --refresh-key
```

Zatim je potrebno uz pomoć alata [*pacstrap*](https://wiki.archlinux.org/title/Pacstrap) instalirati Linux jezgru, datoteke za firmware i osnovne alate za rad s Linuxom:

```
pacstrap -K /mnt/ base base-devel linux linux-firmware
```

Alat *pacstrap* će automatski instalirati jezgru u */boot direktoriju*. Također, *pacstrap* će stvoriti *initramfs.img* datoteku uz pomoć *mkinitcpio* alata. Nakon instalacije, potrebno je stvoriti *fstab* datoteku. Datoteka *fstab* govori kako montirati particije prilikom svakog ponovnog pokretanja sustava. To se može napraviti naredbom:

```
genfstab -U /mnt/ | tee /mnt/etc/fstab
```

Opcija *-U* definira particije po njihovim UUID-ovima umjesto nazivima. Nakon ovoga potrebno je promijeniti korijenski direktorij naredbom:

```
arch-chroot /mnt/
```

Naredba *arch-chroot* mijenja korijenski direktorij za sve buduće procese koji se budu pokretali. Ovo se može smatrati nekom vrstom izolacije.

### Postavljanje sustava

Nakon instalacije Arch Linux distribucije cilj ju je konfigurirati prema našim željama.

#### Postavljanje vremenske zone, sata, jezika, ...

Postavljanje vremenske zone i postavljanje RTC-a na sustavski sat na novoinstaliranom sustavu:

```
ln -sf /usr/share/zoneinfo/Region/City /etc/localtime
hwclock --systohc
```

Sinkronizacija sustava s Arch Linux repozitorijima te instalacija *nano* uređivača teksta može se učiniti naredbom:

```
pacman -Sy
pacman -S nano
```

Sada je potrebno otvoriti datoteku */etc/locale.gen* (naredba ```nano /etc/locale.gen```) i otkomentirati redove:

```
...
en_US.UTF-8 UTF-8
...
hr_HR.UTF-8 UTF-8
...
```

Zatim je potrebno izvršiti naredbu:

```
locale-gen
```

Ovo će generirati skup postavku koje će određivati kako prikazivati i obraditi tekstualne podatke kao primjerice jezik, vremenski formati, valute i slično, ovisno o postavkama u */etc/locale.gen* datoteci.

Jezik sustava se postavlja tako što se uređuje datoteka */etc/locale.conf* (naredba ```nano /etc/locale.conf```). Primjerice, ako se želi postaviti jezik sustava engleski (preporučeno), u datoteku je potrebno pisati:

```
LANG=en_US.UTF-8
```

Raspored tipkovnice i font se za novi sustav postavlja uređujući datoteku */etc/vconsole.conf* (naredba ```nano /etc/vconsole.conf```):

```
KEYMAP=croat
FONT=Lat2-Terminus16
```

Ime računala se može postaviti naredbom:

```
echo "[ime računala]" | tee /etc/hostname
```

#### Mreža i servis NetworkManager

Instalacija paketa za upravljanje mrežom može se učiniti naredbom:

```
pacman -S networkmanager
```

Omogućivanje servisa NetworkManager koji služi za upravljanje mrežom radi se naredbom:

```
systemctl enable NetworkManager
```

#### Postavljanje zaporke korisnika root

Zaporka korisnika *root* postavlja se naredbom:

```
passwd
```

#### Instalacija mikrokoda (ovo ne vrijedi za virtualne strojeve)

Prvo je potrebno instalirati [mikrokod](https://wiki.archlinux.org/title/Microcode) za AMD ili Intel. Mikrokod je na neki način firmware samog procesora. Tijekom stvaranja GRUB konfiguracije ona će uključiti mikrokod u */boot/* direktoriju ako postoji. Mikrokod se **ne instalira ako se Arch Linux pokreće iz virtualnog stroja**.

##### Instalacija mikrokoda za AMD procesore

Instalacija mikrokoda u */boot/* direktorij može se jednostavno izvršiti naredbom:

```
pacman -S amd-ucode
```

Stvorit će se datoteka */boot/amd-ucode.img*.

##### Instalacija mikrokoda za Intel procesore

Instalacija mikrokoda u */boot/* direktorij može se jednostavno izvršiti naredbom:

```
pacman -S intel-ucode
```

Stvorit će se datoteka */boot/intel-ucode.img*.

#### Instalacija GRUB bootloadera


##### BIOS + MBR instalacija bootloadera

Za početak je potrebno preuzeti [GRUB bootloader](https://wiki.archlinux.org/title/GRUB) i alat za detektiranje drugih operacijskih sustava na diskovima:

```
pacman -S grub os-prober
```

Zatim je potrebno instalirati GRUB bootloader na disk (MBR, prostor iza MBR-a i u */boot/* direktoriju):

```
grub-install --target=i386-pc [datoteka uređaja koja predstavlja disk (ne particiju)]
```

##### UEFI + GPT instalacija bootloadera

Za početak je potrebno preuzeti [GRUB bootloader](https://wiki.archlinux.org/title/GRUB), efibootmgr koji služi za pisanje zapisa u NVRAM, alat za detektiranje drugih operacijskih sustava na diskovima i alate za pristup MS-DOS diskovima:

```
pacman -S grub efibootmgr os-prober dosfstools mtools
```

Zatim je potrebno instalirati GRUB bootloader na EFI particiju koja je montirana na */efi* direktoriju:

```
grub-install --target=x86_64-efi --efi-directory=/efi --bootloader-id=grub_uefi --recheck
```

Stvorit će se GRUB EFI datoteka */efi/EFI/grub_uefi/grubx64.efi*.

#### Stvaranje GRUB konfiguracijske datoteke

Sada je potrebno stvoriti konfiguraciju za GRUB. To se radi naredbom:

```
grub-mkconfig -o /boot/grub/grub.cfg
```

Stvorit će se datoteka */boot/grub/grub.cfg* koja sadrži zapise što je moguće učitati iz */boot/* direktorija. Također za svaki zapis daje redoslijed kako učitati primjerice prilikom odabira standardnog *vmlinuz-linux* jezgre prvo će se učitati *vmlinuz-linux* jezgra pa mikrokod ako postoji i onda tek onda *initramfs.img*.

##### Detekcija drugih operacijskih sustava

U slučaju da postoje drugi operacijski sustavi na diskovima i ako ih se želi dodati kao opcije učitavanja (*eng. Multiboot*) potrebno je urediti datoteku */etc/default/grub* (naredbom ```nano /etc/default/grub```) i otkomentirati sljedeći redak:

```
...
GRUB_DISABLE_OS_PROBER=false
...
```

##### Uređivanje NVRAM-a uz pomoć efibootmgr alata

U slučaju da se želi uređivati NVRAM UEFI-ja to se može alatom *efibootmgr*. Primjerice, ako se žele izlistati svi zapisi to se može naredbom ```efibootmgr``` ili primjerice brisanje po ID-u se može izvest naredbom ```efibootmgr -b [Boot ID] -B```.

#### Izlazak iz chroota, demontiranje particija i ponovno pokretanje

Izlazak iz *chroota* može se napraviti naredbom:

```
exit
```

Demontiranje svih particija radi se naredbom:

```
umount -R /mnt
```

Na računalima, treba i izbaciti USB stick naredbom ```eject [datoteka uređaja koja predstavlja taj USB]```. Ponovno pokretanje računala može izvest naredbom ```reboot```, a gašenje virtualnih strojeva kako bi ih sljedeći put pokrenuli bez ISO datoteke se može izvest naredbom ```poweroff```.

### Prva prijava, prve postavke i instalacija dodatnih paketa

Prva prijava radi se s privilegiranim korisnikom *root*.

#### Spajanje na bežičnu mrežu

Spajanje na bežičnu mrežu (WiFi) radi se preko Network Manager servisa. Provjeravanje jeli WiFi radio omogućen radi se naredbom ```nmcli radio wifi```. Omogućivanje WiFi radija radi se naredbom ```nmcli radio wifi on```. Skeniranje dostupnih mreža radi se naredbom:

```
nmcli dev wifi list
```

Spajanje na specifičnu mrežu može se učinit naredbom:

```
nmcli dev wifi connect [SSID] password [zaporka]
```

Ispis svih spremljenih mreža radi se naredbom ```nmcli con show```. Odspajanje s trenutno spojene mreže radi naredbom ```nmcli con down [SSID]```, a spajanje na spremljenu mrežu naredbom ```nmcli con up [SSID]```. Potpuno zaboravljanje mreže radi se naredbom ```nmcli con delete [SSID]```.

#### Konfiguracija običnog korisnika

Običan korisnik se stvara naredbom:

```
useradd -m -G wheel [ime korisnika]
```

Zastavica *-m* stvara i korisnički direktorij, a zastavica *-G* govori da se korisnika doda i u grupu *wheel* što je ekvivalent *sudo* grupi u drugim distribucijama. Postavljanje zaporke korisniku radi se naredbom:

```
passwd [ime korisnika]
```

Omogućivanje novostvorenom korisniku izvršavanje privilegiranih naredbi uz pomoć *sudo* alata radi se tako što se uredi datoteka */etc/sudoers* (naredba ```EDITOR=nano visudo```) i otkomentira se redak:

```
...
%wheel ALL=(ALL:ALL) ALL
...
```

#### Uključivanje NTP servisa

Uključivanje NTP servisa na novoinstaliranom sustavu radi se naredbom:

```
timedatectl set-ntp true
```

#### Razlučivanje imena

Kako bismo mogli razlučivati imena tipa *localhost* na adresu *127.0.0.1*, potrebno je urediti datoteku */etc/hosts* (naredbom ```nano /etc/hosts```):

```
...
127.0.0.1	localhost
127.0.1.1	[ime računala]
::1		ip6-[ime računala]
```

Alatom *ping* može se isprobati ispravnost rada razrješivanje imena (naredbama ```ping localhost```, ```ping [ime računala]``` i ```ping ip6-[ime računala]```).

#### Instalacije upravljačkih programa za grafiku

U slučaju da se koristi virtualni stroj preko QEMU-a nije potrebno instalirati upravljački program za grafičku karticu.

##### Instalacija upravljačkog programa za AMD grafičke kartice

Instalacija *open source* upravljačkog programa za AMD grafičke kartice radi se naredbom:

```
pacman -S mesa
```

Prilikom ponovnog pokretanja jezgreni modul *amdgpu* bi se trebao automatski učitati u jezgru. Provjera jeli se to stvarno dogodilo se može napraviti naredbom ```lspci -k | grep -A 3 -E "(VGA|3D)"```. Postoji i *closer source* upravljački program, ali je lošiji nego *open source*.

##### Instalacija upravljačkog programa za NVIDIA grafičke kartice

Ovisno o [seriji](https://nouveau.freedesktop.org/CodeNames.html#NV160) [NVIDIA](https://wiki.archlinux.org/title/NVIDIA) grafičkih kartica, *open source* upravljački program se može instalirati za grafičke kartice serije Turing. Kojoj seriji grafička kartica pripada može se vidjeti naredbom ```lspci -k | grep -A 3 -E "(VGA|3D)"``` (oznaka TUxxx).

Instalacija *open source* upravljačkog programa za NVIDIA grafičke kartice serije Turing radi se naredbom:

```
pacman -S nvidia-open
```

Prilikom ponovnog pokretanja jezgreni modul *nvidia* bi se trebao automatski učitati u jezgru. Provjera jeli se to stvarno dogodilo se može napraviti naredbom ```lspci -k | grep -A 3 -E "(VGA|3D)"```.

#### Instalacija X Window Systema i GNOME desktop okoline

Instalacija XORG poslužitelja, [GNOME desktop okoline](https://wiki.archlinux.org/title/GNOME) i omogućavanje upravitelja zaslona gdm-a radi se naredbama:

```
pacman -S xorg-server gnome gnome-tweaks
systemctl enable gdm
```

Sada je potrebno ponovno pokrenuti računalo ili virtualni stroj naredbom:

```
reboot
```

### Završne preporuke

Preporuke:

- postavljanje prečaca u postavkama
	- naredba za terminal je *kgx*
- dodavanje načina unosa u postavke
- promjena teme
- uređivanje */etc/pacman.conf* datoteke (naredba ```nano /etc/pacman.conf```)
	- otkomentiranje zapisa:
		```
		...
		Color
		...
		ParallelDownloads = 5
		...
		```
	- dodavanje zapisa odmah ispod gore navedenih:
		```
		...
		ILoveCandy
		...
		```
- uređivanje *prompta* odnosno varijable okruženja *PS1* u *.bashrc* datoteci:
	- potrebno je dati vlastitu vrijednost varijabli *PS1* u obliku ```PS1='\[\033[01;38;2;R;G;Bm\]\u@\h:\w \$\[\033[00m\] '```
		- *R*, *G* i *B* predstavljaju intenzitete boja, rasponi su im od 0 uključivo do 255 uključivo
	- primjerice za crvenu boju *prompta*:
		```
		...
		PS1='\[\033[01;38;2;255;0;0m\]\u@\h:\w \$\[\033[00m\] '
		...
		```

#### Još korisnih paketa

Dodatni paketi koji bi možda bili korisni:
	- *git* : alat za verzioniranje
	- *htop* : interaktivni ispis trenutno aktivnih procesa
	- *fastfetch* : ispis informacija i sustavu
	- *firefox* : web preglednik
	- *ffmpeg* : alat naredbenog retka za uređivanje videa i ostale multimedije
	- *gst-libav* : plugin za H264 codec, potreban za reprodukciju videa
	- *gedit* : uređivač teksta
	- *man-db* : ispisuje stranice uputa za svaku naredbu (ako postoje)
	- *wget* : za preuzimanje datoteka preko mreže
	- *curl* : za prijenos datoteka preko mreže
	- *cmake* : alat za izgradnju
	- *zip* i *unzip* : za kompresiju i dekompresiju
	- *bluez* i *bluez-utils* : za upravljanje Bluetoothom, nakon instalacije je potrebno omogućiti servis bluetooth i započeti njegov rad (naredbe ```systemctl enable bluetooth``` i ```systemctl start bluetooth```)
	- *usbutils* : za ispisivanje informacija o USB uređajima
	- *lsof* : alat za izlistavanje svih otvorenih datoteka
	- *net-tools*, *inetutils* i *nmap* : raznorazni mrežni alati
	- *dhclient* : DHCP klijent
	- *code* : VSCode uređivač teksta
	- *discord* : Discord

Naredba za instalaciju svih navedenih paketa je:

```
pacman -S git htop fastfetch firefox ffmpeg gst-libav gedit man-db wget curl cmake zip unzip bluez bluez-utils usbutils lsof net-tools inetutils nmap dhclient code discord
```

### Završna napomena

Arch Linux prati tzv. Rolling Release Model što znači da se sustav kontinuirano ažurira. Ovo znači da ne postoje konkretne verzije Arch Linuxa već se softverski paketi neprestano ažuriraju. Kako bi se sustav održao ažurnim potrebno je ažurirati sve pakete naredbom:

```
sudo pacman -Syu
```

Također, prije instalacije novih paketa također bi bilo poželjno prvo ažurirati sve pakete na sustavu.
