# ENKRIPCIJA CJELOKUPNOG SUSTAVA

Sljedeća enkripcija diska napravljena je korištenjem [*dm-crypt* Linux podsustava](https://wiki.archlinux.org/title/Dm-crypt/Device_encryption). Alat naredbenog retka koji služi za interakciju s *dm-crypt* podsustavom naziva se *cryptsetup*. Podsustav *dm-crypt* nudi značajku LUKS (*eng. Linux Unified Key Setup*) koji apstrahira particiju koja se enkriptira i njezine kriptografske ključeve.

## Primjer enkripcije ROOT particije na disku bez enkripcije Linux jezgre (UEFI + GPT)

Proces instalacije Arch Linuxa s enkriptiranom ROOT particijom je sličan uobičajenoj instalaciji uz par dodatnih naredbi i konfiguracija.

### Spajanje na mrežu

Preporučuje se korištenje žičane konekcije (Ethernet). Bežično spajanje se može izvesti naredbom ```iwctl``` (upute za korištenje su u osnovnim uputama za instalaciju Arch Linuxa).

### Raspored tipkovnice, font, vremenska zona i NTP poslužitelj

Potrebno je postaviti raspored tipkovnice (npr. ```loadkeys croat```), font (npr. ```setfont Lat2-Terminus16```) i vremensku zonu (```timedatectl set-timezone Europe/Zagreb```) po želji te uključiti korištenje NTP poslužitelja.

### Particioniranje diska

Disk mora biti na sljedeći način particioniran:

```
        GPT SHEMA
        +-----------------------+-----------------------+-----------------------+
        | EFI particija         | BOOT particija        | LUKS enkriptirana     |
        |                       |                       | ROOT particija        |
        | /efi/                 | /boot/                |                       |
        |                       |                       | /                     |
        |                       |                       |                       |
        |                       |                       | /dev/mapper/root      |
        |                       |                       |                       |
        |                       |                       |                       |
        +-----------------------+-----------------------+-----------------------+
```

**Potrebno je uočiti da se ovdje sama Linux jezgra ne enkriptira (jer se nalazi na zasebnoj BOOT particiji), u ovom slučaju Linux jezgra prilikom inicijalizacije montira i otključava enkriptiranu particiju.**

Dakle alatom *fdisk* (```fdisk [datoteka uređaja kojeg će se particionirati]```) potrebno je napraviti tri particije koristeći GPT shemu particioniranja (naredba ```g```).

EFI particija mora imati sljedeće konfiguracije (naredba ```w```):

* broj particije: 1
* prvi sektor: *default*
* zadnji sektor: +512M

Vrsta particije 1 se postavlja naredbom ```t```, broj vrste je 1.

Boot particija mora imati sljedeće konfiguracije (naredba ```w```):

* broj particije: 2
* prvi sektor: *default*
* zadnji sektor: +512M

Vrsta particije 2 se postavlja naredbom ```t```, broj vrste je 20.

LUKS enkriptirana ROOT particija mora imati sljedeće konfiguracije (naredba ```w```):

* broj particije: 3
* prvi sektor: *default*
* zadnji sektor: *default*

Vrsta particije 3 se postavlja naredbom ```t```, broj vrste je 20.

Ispis trenutne konfiguracije koja će se postaviti može se vidjeti naredbom ```p```, a konačno particioniranje izvodi se naredbom ```w```.

### Enkriptiranje ROOT particije

Kako bi [postavili enkriptiranu ROOT particiju](https://wiki.archlinux.org/title/Dm-crypt/Encrypting_an_entire_system#LUKS_on_a_partition), potrebno je izvršiti sljedeću naredbu:

```
cryptsetup -v luksFormat [datoteka uređaja koja predstavlja particiju ROOT koja će biti enkriptirana]
```

Prethodna naredba formatira ROOT particiju u [LUKS format](https://en.wikipedia.org/wiki/Linux_Unified_Key_Setup). Particija sada sadrži LUKS2 zaglavlje koje ima mogućnost spremanja ključa za dekriptiranje same particije. Ukratko, ono što se dogodilo izvršenjem prethodne naredbe:

* stvorilo se LUKS2 zaglavlje na particiji ROOT
* nasumično se generirao glavni ključ za enkripciju diska
* upisala se lozinka nad kojom se koristio algoritam za derivaciju ključa Argon2 (prije se uobičajeno koristio PBKDF2)
* napravio se sažetak (*eng. hash*) glavnog ključa (uobičajeno SHA256 algoritam)
* derivirani ključ se koristio za enkripciju glavnog ključa (AES256 algoritam), rezultat te enkripcije se spremio u prvi utor za ključ (*eng Key Slot*) od njih 32

Inicijalizacija ukratko i pojednostavljeno:

```
        +------------------------+     +----------------+
        |     STVORENO LUKS2     |---->|   STVARANJE    |
        | ZAGLAVLJE NA PARTICIJI |     | GLAVNOG KLJUČA |
        +------------------------+     +----------------+
                                               ||
                                               \/
        +-------------------+          +--------------+
        | DERIVACIJA KLJUČA |<---------| UPIS LOZINKE |
        |    IZ LOZINKE     |          +--------------+                                                                                         
        +-------------------+
                 ||
                 \/
        +-----------------------+      +--------------------+
        | IZRAČUNAVANJE SAŽETKA |----->| PISANJE SAŽETKA U  |
        |    GLAVNOG KLJUČA     |      | PRVI UTOR ZA KLJUČ |
        +-----------------------+      +--------------------+
                                                 ||
                                                 \/
        +------------------------+     +----------------+
        | PISANJE ENKRIPTIRANOG  |<----| ENKRIPTIRANJE  |
        | GLAVNOG KLJUČA NA DISK |     | GLAVNOG KLJUČA |
        +------------------------+     +----------------+
```

Pisanje i čitanje po enkriptiranom disku radi se naredbom:

```
cryptsetup open [datoteka uređaja koja predstavlja enkriptiranu particiju ROOT] [ime mapirane dekriptirane particije]
```

Ime mapirane dekriptirane particije može biti primjerice *cryptroot* ili samo *root*.

U trenutku otvaranja enkriptirane particije zatražit će se upis lozinke, postupak je ovakav:

```
        +--------------------+              +-------------------+
        |   UPIS LOZINKE     |------------->| DERIVACIJA KLJUČA |
        +--------------------+              |    IZ LOZINKE     |
                                            +-------------------+
                                                     ||
                                                     \/
        +--------------------------+        +-----------------------+
        | DEKRIPTIRANJE PROČITANOG |<-------| ČITANJE ENKRIPTIRANOG |
        |   ENKRIPTIRANOG KLJUČA   |        |    KLJUČA IZ UTORA    |
        +--------------------------+        +-----------------------+
                     ||                                        /\      Ako sažetci nisu jednaki, idi na sljedeći utor
                     \/                                        ||
        +------------------------------+    +-------------------------------------+
        |     IZRAČUNAVANJE SAŽETKA    |--->|  USPOREDBA IZRAČUNATOG SAŽETKA SA   |
        | DEKRIPTIRANOG GLAVNOG KLJUČA |    | SPREMLJENIM SAŽETKOM U UTORU KLJUČA |
        +------------------------------+    +-------------------------------------+
                                                               ||
                                                               ||      Ako su sažetci jednaki, spremi glavni ključ u RAM
        +----------------------------+                         \/
        | PISANJE I ČITANJE PODATAKA |      +-------------------------------------+
        | NA DISK SE ODVIJA UZ POMOĆ |<-----| GLAVNI KLJUČ SE SADA NALAZI U RAM-U |
        |    DM-CRYPT PODSUSTAVA I   |      +-------------------------------------+
        |        GLAVNOG KLJUČA      |
        +----------------------------+
```

Naredbom ```lsblk``` primijetit će se da je stvoren virtualni blok uređaj */dev/mapper/cryptroot*. Ime *cryptroot* je bilo zadano prethodnom naredbom iako se virtualni blok uređaj odnosno dekriptirana particija mogla nazvati bilo kako. Tek se sada može formatirati i montirati root particija na */mnt*. Dakle:

```
mkfs.ext4 /dev/mapper/[ime mapirane dekriptirane particije]
mount /dev/mapper/[ime mapirane dekriptirane particije] /mnt
```

Također, potrebno je formatirati EFI i BOOT particiju te ih montirati na */mnt/efi* i */mnt/boot*:

```
mkfs.vfat -F 32 [datoteka uređaja koja predstavlja EFI particiju]
mkfs.ext4 [datoteka uređaja koja predstavlja BOOT particiju]
mount --mkdir [datoteka uređaja koja predstavlja EFI particiju] /mnt/efi
mount --mkdir [datoteka uređaja koja predstavlja BOOT particiju] /mnt/boot
```

### Instalacija sustava

Postupak instalacije je uobičajen, sve do konfiguracije GRUB bootloadera:

```
# Instalacija paketa i generiranje fstab datoteke
pacman-key --refresh-key
pacstrap -K /mnt/ base base-devel linux linux-firmware vim nano networkmanager grub efibootmgr os-prober dosfstools mtools
genfstab -U /mnt/ | tee /mnt/etc/fstab
arch-chroot /mnt/

# Postavljanje sustava
ln -sf /usr/share/zoneinfo/Europe/Zagreb /etc/localtime
hwclock --systohc
sed -i 's/^#\s*\(en_US.UTF-8\sUTF-8\)/\1/' /etc/locale.gen
sed -i 's/^#\s*\(hr_HR.UTF-8\sUTF-8\)/\1/' /etc/locale.gen
locale-gen
echo "LANG=en_US.UTF-8" | tee /etc/locale.conf
echo "KEYMAP=croat" | tee /etc/vconsole.conf
echo "FONT=Lat2-Terminus16" | tee -a /etc/vconsole.conf
echo "krypton" | tee /etc/hostname
systemctl enable NetworkManager
passwd
```

Potrebno je također instalirati odgovarajući mikrokod. Za AMD-ove procesore je naredba ```pacman -S amd-ucode``` dok je za Intelove procesore naredba ```pacman -S intel-ucode```. Ako se sustav pokreće u virtualnom stroju mikrokod se ne instalira.

### Konfiguracija initramfs datotečnog sustava

Kako bi Linux jezgra znala montirati enkriptiranu particiju potrebno je [regenerirati *initramfs*](https://wiki.archlinux.org/title/Dm-crypt/Encrypting_an_entire_system#Configuring_mkinitcpio) ali sa sljedećim postavkama u datoteci */etc/mkinitcpio.conf*:

```
...
HOOKS=(base udev autodetect microcode modconf kms keyboard keymap consolefont block encrypt filesystems fsck)
...
```

[HOOK-ovi](https://wiki.archlinux.org/title/Mkinitcpio#Configuration) su skripte koje će se pokrenuti kada se pokrene inicijalizacijski sustav (*eng. init system*). Ako ne postoje, potrebno je dodati HOOK-ove *udev*, *keyboard*, *keymap*, *consolefont* i *encrypt*. Ovakva konfiguracija se zove *busybox* konfiguracija.

Datoteka *initramfs* se može regenerirati naredbom:

```
mkinitcpio -P
```

### Konfiguracija GRUB bootloadera

Za instalaciju GRUB bootloadera potrebno je izvršiti sljedeću naredbu:

```
grub-install --target=x86_64-efi --efi-directory=/efi --bootloader-id=grub_uefi --recheck
```

Bootloader treba Linux jezgri proslijediti parametre za otključavanje ispravne particije koja je enkriptirana. Potrebno je urediti datoteku */etc/default/grub* na sljedeći način:

```
...
LINUX_KERNEL_PARAMETERS="... cryptdevice=UUID=[UUID enkriptirane particije]:root root=/dev/mapper/[ime mapirane dekriptirane particije]..."
...
```

UUID enkriptiranog diska se može dobiti naredbom:

```
blkid -o value -s UUID [datoteka uređaja koja predstavlja enkriptiranu particiju]
```

##### 1. Ubacivanje odgovarajućeg UUID-a u /etc/default/grub uz pomoć alata SED

Ubacivanje UUID-a kao parametar Linux jezgre uz pomoć alata SED može se napraviti naredbama:

```
PART_ID=$(blkid -o value -s UUID [datoteka uređaja koja predstavlja enkriptiranu particiju])
SED_REGEX="s/^GRUB_CMDLINE_LINUX=\\\"\\([^\"]*\\)*\\\"/GRUB_CMDLINE_LINUX=\\\"\\1 cryptdevice=UUID=${PART_ID}:root root=\\/dev\\/mapper\\/[ime mapirane dekriptirane particije]\\\"/"
sed -i "$SED_REGEX" /etc/default/grub
SED_REGEX="s/^GRUB_CMDLINE_LINUX=\" \([^"]*\)"/GRUB_CMDLINE_LINUX=\"\1\"/"
sed -i "$SED_REGEX" /etc/default/grub
```

##### 2. Ubacivanje odgovarajućeg UUID-a u /etc/default/grub uz pomoć alata nano

Alatom *blkid* i *tee* se UUID enkriptirane particije može nadodati na kraj */etc/default/grub* datoteke ```blkid -o value -s UUID [datoteka uređaja koja predstavlja enkriptiranu particiju] | tee -a /etc/default/grub```. Zatim se datoteku */etc/default/grub* može uređivati *nano* uređivačem teksta (```nano /etc/default/grub```) te se UUID može izrezati kombinacijom tipki *CTRL+K* i zalijepiti na odgovarajuće mjesto kombinacijom tipki *CTRL+U*.

#### Generiranje konačne konfiguracije GRUB-a

Konačna konfiguracija GRUB-a se generira naredbom:

```
grub-mkconfig -o /boot/grub/grub.cfg
```

**ZNAČAJKA OS-PROBER NE FUNKCIONIRA DOBRO KADA SE KORISTI S ENKRIPTIRANE PARTICIJE, GRUB ZAPISE DRUGIH OPERACIJSKIH SUSTAVA POTREBNO JE RUČNO DODATI!**

##### Dodatak: Dodavanje vlastitih zapisa u GRUB bez korištenja os-prober-a za svrhu Dual Bootinga

Dodavanje vlastitih zapisa u GRUB može se napraviti uređivanjem datoteke */etc/grub.d/40_custom*. Za početak, potrebno je saznati UUID-ove ostalih diskova i particija:

```
blkid
```

Primjerice, za zapis koji **pokazuje na Linux jezgru** (*/boot je ovdje zasebna particija*) koju treba učitati (*vmlinuz-linux*) i initramfs (initramfs-linux.img) koje se obično nalaze u */boot* direktoriju, to se može napraviti dodavanjem sljedećeg koda ispod *exec tail* linije:

```
menuentry 'My Linux Entry' --class arch --class gnu-linux --class gnu --class os $menuentry_id_option 'gnulinux-simple-[UUID BOOT particije gdje se nalazi Linux jezgra]' {

	insmod gzio
	insmod part_gpt
	insmod ext2
	search --no-floppy --fs-uuid --set=root [UUID BOOT particije gdje se nalazi Linux jezgra]
	echo 'Loading Linux linux ...'
	linux /vmlinuz-linux root=[UUID BOOT particije gdje se nalazi Linux jezgra] rw  loglevel=3 quiet
	echo 'Loading initial ramdisk ...'
	initrd [/amd-ucode.img, /intel-ucode.img ili ništa] /initramfs-linux.img

}
```

Objašnjenje pojmova:

- *'My Linux Entry' --class arch --class gnu-linux --class gnu --class os $menuentry_id_option 'gnulinux-simple-[UUID BOOT particije gdje se nalazi Linux jezgra]'*
	- ime zapisa koje će biti vidljivo na ekranu
	- *--class [ime klase]* - koriste se za grupiranje zapisa, proizvoljnog su imena
	- *$menuentry_id_option '[Identifikator za zapis]'* - makro koji sprema jedinstveni identifikator za navedeni zapis koji će GRUB interno koristiti
- *insmod gzio, insmod part_gpt, insmod ext2* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#insmod))
	- ubacivanje modula za podršku rada s GZip, podršku rada s GTP particijama i podršku rada s EXT2/EXT3/EXT4 datotečnim sustavima, lista GRUB modula i njihova objašnjenja se mogu naći [ovdje](https://www.linux.org/threads/understanding-the-various-grub-modules.11142/)
- *search --no-floppy --fs-uuid --set=root [UUID BOOT particije gdje se nalazi Linux jezgra]* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#search))
	- pretražuje postoji li navedena particija
	- *--no-floppy* - preskače pretraživanje Floppy diskova jer su spori
	- *--fs-uuid* - pretražuj datotečne sustave po njihovim UUID-ovima
	- *--set=root* - ako se pronađe traženi datotečni sustav po UUID-u, postavi korijenski direktorij na početak pronađenog datotečnog sustava
- *linux /vmlinuz-linux root=[UUID BOOT particije gdje se nalazi Linux jezgra] rw  loglevel=3 quiet* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#linux))
	- učitava Linux jezgru
	- */vmlinuz-linux* - lokacija Linux jezgre (na BOOT particiji)
	- *root=[UUID BOOT particije gdje se nalazi Linux jezgra]* - postavljanje korijenskog direktorija odakle početi pretraživati datoteku Linux jezgre
	- *rw* - način rada čitanja i pisanja po navedenoj particiji
	- *loglevel=3* - razina ispisa logova tijekom učitavanja Linux jezgre
	- *quiet* - ispisuju se samo bitnije poruke
- *initrd [/amd-ucode.img, /intel-ucode.img ili ništa] /initramfs-linux.img* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#initrd))
	- učitava redom *ramfs* datotečne sustave koji će se privremeno montirati tijekom inicijalizacije

Za zapis koji **pokazuje na Windows Boot Manager** (*/efi/Microsoft/Boot/bootmgfw.efi* na zasebnoj particiji) kojeg treba učitati kod glasi ovako:

```
menuentry 'My Windows Entry' {
	insmod part_gpt
	insmod fat
	search --no-floppy --fs-uuid --set=root [UUID EFI datotečnog sustava gdje se nalazi Window Boot Manager]
	chainloader /efi/Microsoft/Boot/bootmgfw.efi
}
```

- *'My Windows Entry'*
	- ime zapisa koje će biti vidljivo na ekranu
- *insmod part_gpt* i *insmod ext2* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#insmod))
	- ubacivanje modula za podršku rada s GTP particijama i podršku rada s FAT datotečnim sustavima
- *search --no-floppy --fs-uuid --set=root [UUID EFI particije gdje se nalazi Window Boot Manager]* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#search))
	- pretražuje postoji li navedena particija
	- *--no-floppy* - preskače pretraživanje Floppy diskova jer su spori
	- *--fs-uuid* - pretražuj datotečne sustave po njihovim UUID-ovima
	- *--set=root* - ako se pronađe traženi datotečni sustav po UUID-u, postavi korijenski direktorij na početak pronađenog datotečnog sustava
- *chainloader /efi/Microsoft/Boot/bootmgfw.efi* (više [ovdje](https://www.gnu.org/software/grub/manual/grub/grub.html#chainloader))
	- učitava Windowsov bootloader (pretpostavljajući da se tu nalazi) i prepušta mu kontrolu

### Izlaz, odmontiranje i ponovno pokretanje

Izlaz, odmontiranje i zatvaranje enkriptirane particije radi se sljedećim naredbama:

```
exit
umount -R /mnt
cryptsetup close [ime mapirane dekriptirane particije]
reboot
```

Ostali postupak postavljanja je isti kao i za Arch Linux bez enkriptirane particije.

### Problem ovakve konfiguracije

Problem ovakve konfiguracije je što se samo enkriptira *ROOT* particija, dok *BOOT* particija na kojoj se nalazi Linux jezgra nije enkriptirana. Ovo dovodi do mogućnost malicioznog petljanja s Linux jezgrom na BOOT particiji.

## Primjer enkripcije ROOT particije s enkripcijom Linux jezgre (UEFI + GPT)

Proces instalacije Arch Linuxa s enkriptiranom ROOT particijom je sličan je prethodnoj instalaciji uz par dodatnih naredbi i konfiguracija.

### Spajanje na mrežu

Opet, preporučuje se korištenje žičane konekcije (Ethernet). Bežično spajanje se može izvesti naredbom ```iwctl``` (upute za korištenje su u osnovnim uputama za instalaciju Arch Linuxa).

### Raspored tipkovnice, font, vremenska zona i NTP poslužitelj

Potrebno je postaviti raspored tipkovnice (npr. ```loadkeys croat```), font (npr. ```setfont Lat2-Terminus16```) i vremensku zonu (```timedatectl set-timezone Europe/Zagreb```) po želji te uključiti korištenje NTP poslužitelja.

### Particioniranje diska

Disk mora biti na sljedeći način particioniran:

```
        GPT SHEMA
        +-----------------------+-----------------------+
        | EFI particija         | LUKS enkriptirana     |
        |                       | ROOT particija        |
        | /efi/                 |                       |
        |                       | /                     |
        |                       |                       |
        |                       | /dev/mapper/root      |
        |                       |                       |
        |                       |                       |
        +-----------------------+-----------------------+
```

**Potrebno je uočiti da će se sada i Linux jezgra enkriptirati, odnosno na bootloaderu je zadatak otključati enkriptiranu particiju.**
