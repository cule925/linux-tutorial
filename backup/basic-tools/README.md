# OSNOVNI ALATI ZA SIGURNOSNE KOPIJE

Sljedeći primjeri pokazivat će osnovne alate za rad sa sigurnosnim kopijama. Primjeri su isprobani na Arch Linuxu. 

## Lokalno arhiviranje i kompresija

Alat [*tar*](https://wiki.archlinux.org/title/Archiving_and_compression) je jedan od temeljnih alata Linuxa. Alat služi za arhiviranje i dearhiviranje te kompresiranje i dekompresiranje datoteka i direktorija. Arhiva je datoteka koja je nastala spajanjem metapodataka i više datoteka u jedan paket. Njime se olakšava čuvanje, prijenos i organizacija podataka. Kompresija je proces smanjenja veličine podataka uklanjajući redundantne podatke i optimizirajući način pohrane tih podataka. Kompresijom se štedi prostor na kojem je skup podataka pohranjen.

Potrebno je napraviti direktorij *tar-example* u korisničkom *home* direktoriju i premjestit se u njega. Dakle naredba koju je potrebno izvest je:

```
mkdir ~/tar-example
cd ~/tar-example
```

### Primjer arhiviranja i kompresije

Kao prvi primjer, potrebno je stvoriti datoteke *file1.txt*, *file2.txt* te direktorij *dir1* koji sadrži datoteku *file3.txt* naredbama:

```
echo "Hello World" > file1.txt
echo "John Smith" > file2.txt
mkdir dir1
echo "Computer" > dir1/file3.txt
```

Arhiviranje ovih datoteka i direktorija radi se naredbama:

```
tar -cf files.tar file1.txt file2.txt dir1
```

Nad TAR arhivom je moguće vršit mnogo operacija. Primjerice, izlistavanje datoteka i direktorija koji se nalaze u arhivi *files.tar* može se naredbom:

```
tar -tf files.tar
```

Kompresija TAR arhive GZIP formatom radi se naredbom:

```
gzip -c files.tar > files.tar.gz
```

Međutim, arhiviranje i kompresija se inače radi odjednom, naredbom:

```
tar -czf files.tar.gz file1.txt file2.txt dir1/
```

Kompresirana TAR arhiva se inače naziva **tarball**.

### Primjer dekompresije i dearhiviranja

Koristit će se prethodno stvorena GZIP datoteka. Potrebno je obrisati sve ostale stvorene datoteke naredbom:

```
rm -r file1.txt file2.txt dir1 files.tar
```

Dekompresija TAR arhive iz GZIP formata radi se naredbom:

```
gzip -d files.tar.gz
```

Ako se želi zadržati originalna datoteka, potrebno je dodati i zastavicu ```-k```. Ako se doda zastavica ```-c```, rezultat dekompresije se šalje na standardni izlaz što omogućuje imenovanje izlazne datoteke (u ovom slučaju naredba ```gzip -dc files.tar.gz > files.tar```).

Dearhiviranje datoteka i direktorija radi se naredbom:

```
tar -xf files.tar
```

Ako se želi dearhivirati arhiva u nekom drugom direktoriju, to se može napraviti opcijom ```-C``` (u ovom slučaju naredba ```tar -xf files.tar -C [ciljni direktorij]```).

Kao i u prethodnom primjeru, dekompresiranje i dearhiviranje se inače radi odjednom, naredbom:

```
tar -xzf files.tar.gz
```

## Kopiranje datoteka lokalno i na udaljeno računalo

Datoteke u EXT4 imaju sljedeće vremenske oznake koje se mogu vidjeti naredbom ```stat [ime datoteke]```:

- vrijeme stvaranja datoteke (*birth time*)
- vrijeme pristupa datoteci (*access time* ili *atime*)
- vrijeme promjene podataka datoteke (*modification time* ili *mtime*)
- vrijeme promjene metapodataka datoteke (*change time* ili *ctime*)

Alat [*rsync*](https://wiki.archlinux.org/title/Rsync#) je alat koji pruža inkrementalne udaljene (i lokalne) transfere datoteka. Prilikom kopiranja, pregledava vrijeme modifikacije datoteka (*mtime*) i njihovu veličinu od izvora i na odredištu ako datoteke postoje i tako odlučuje koje datoteke treba prenijeti. Također, omogućuje prijenos samo izmijenjenih dijelova datoteka što skraćuje vrijeme prijenosa.

Instalacija *rsync* alata moguća je naredbom:

```
sudo pacman -S rsync
```

Alat *rsync* mora biti instaliran i na lokalnom i na udaljenom računalu. Također *rsync* zadano koristi SSH protokol za udaljenu komunikaciju. Ako se koristi neki drugi port za SSH pristup na udaljenom računalu umjesto porta 22, to se specificira zastavicom i argumentom ```-e 'ssh -p [novi port]'```.

Općeniti oblik kopiranja s lokalnog na udaljeno računalo je:

```
rsync [direktorij ili datoteka na lokalnom računalu] [korisničko ime na udaljenom računalu]@[IP adresa udaljenog računala]:[direktorij ili datoteka na udaljenom računalu]
```

Općeniti oblik kopiranja s udaljenog na lokalno računalo je:

```
rsync [korisničko ime na udaljenom računalu]@[IP adresa udaljenog računala]:[direktorij ili datoteka na udaljenom računalu] [direktorij ili datoteka na lokalnom računalu]
```

### Inicijalne postavke okoline prije primjera korištenja

Primjer korištenja alata *rsync* će se pokazivati u virtualnom stroju koji će pokretati Debian distribuciju. Prije pokazivanja primjera korištenja potrebno je pokrenuti jedan [QEMU](../../virtualization/qemu) virtualni stroj koji pokreće Debian:

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

Na Arch Linuxu, potrebno je prvo napraviti direktorij *rsync-vm*, premjestiti se u njega i preuzeti ISO sliku Debiana (inačica Bookworm 12.8.0 u ovom slučaju):

```
mkdir ~/rsync-vm
cd ~/rsync-vm
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

Ovakav način rada je UEFI način rada i on će se koristiti za sva buduća pokretanja virtualnog stroja. Parametri poput ukupne dodijeljene memorije (*-m*) ili broj dodijeljenih procesorskih jezgri (*-smp*) virtualnom stroju ne moraju biti točno jednaki predloženome već ih se može prilagoditi po potrebi i mogućnostima računala domaćina. Najlakši način instalacije Debiana na virtualni tvrdi disk je grafičkom instalacijom. Dakle, potrebno je odabrati *Graphical Install* te namjestiti sljedeću konfiguraciju:

- *Language:* "English"
- *Location:* "United States"
- *Keymap:* [ovisno o rasporedu tipkovnice]
- *Hostname:* "debian"
- *Domain name:*
- *Root password:* "passroot"
- *Full name for the new user:*
- *Username for your account:* [**identično korisniku na Arch Linuxu**]
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

Nakon instalacije alata, korisnika je potrebno dodati u grupu *sudo* naredbom:

```
usermod -aG sudo [identično korisniku na Arch Linuxu]
```

Kako bi se koristile *rsync* funkcionalnosti potrebno je alat instalirati naredbom:

```
apt install rsync
```

Na kraju je potrebno ugasiti virtualni stroj:

```
poweroff
```

Nakon gašenja stroja, virtualni tvrdi disk *disk-baskup.qcow2* je potrebno kopirati jednom:

```
cp disk-backup.qcow2 disk.qcow2
```

### Primjer kopiranja datoteka na udaljeno računalo

Prvo je potrebno pokrenuti prethodni virtualni stroj, dakle:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1,hostfwd=tcp::8022-:22 \
-device virtio-net-pci,netdev=net0 \
-drive if=virtio,format=qcow2,file=disk.qcow2 \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

Nakon toga, potrebno je otvoriti novi terminal te stvoriti SSH ključeve ako već nisu (pogledati direktorij *.ssh*) naredbom ```ssh-keygen -t ed25519``` i kopirati javni ključ na udaljeno računalo naredbom ```ssh-copy-id -p 8022 $USER@localhost```. Promjene u virtualnom stroju se mogu pratiti u QEMU prozoru (prijavom kao obični korisnik) ili otvaranjem trećeg terminala i prijavom na virtualni stroj naredbom ```ssh -p 8022 $USER@localhost```.

Na lokalnom računalu, potrebno je premjestiti se u direktorij *rsync-vm*, stvoriti direktorij *data* te u njemu stvoriti datoteke *file1.txt*, *file2.txt* te direktorij *dir1* koji sadrži datoteku *file3.txt* naredbama:

```
cd ~/rsync-vm
mkdir data
echo "Hello World" > data/file1.txt
echo "John Smith" > data/file2.txt
mkdir data/dir1
echo "Computer" > data/dir1/file3.txt
```

Kopiranje direktorija *data* na virtualni stroj u *home* direktoriju udaljenog korisnika radi se naredbom:

```
rsync -avz -e 'ssh -p 8022' data $USER@localhost:~/
```

Podatci se prenose kao komprimirana arhiva (zastavice ```-avz```). Arhiva sadržava i metapodatke datoteka i direktorija kao što su vremenske oznake, vlasništva i simbolične poveznice. Ako se dogodi promjena nad jednom od datoteka ili direktorija, samo će se ta promjena prenijeti. Primjerice, ako se datoteci *file1.txt* na lokalnom računalu doda neki podatak (naredba ```echo "I'm a file" >> data/file1.txt```) i te se ponovno izvrši naredba za kopiranje (naredba ```rsync -avz -e 'ssh -p 8022' data $USER@localhost:~/```), samo će se ta promjena prenijeti.

### Primjer kopiranja datoteka na lokalno računalo

Nastavkom na prethodni primjer, na udaljenom računalu (u virtualnom stroju) je potrebno u korisnikovom direktoriju stvoriti direktorij *data-vm* te u njemu stvoriti datoteke *file4.txt*, *file5.txt* te direktorij *dir2* koji sadrži datoteku *file6.txt* naredbama:

```
cd ~/
mkdir data-vm
echo "Goodbye World" > data-vm/file4.txt
echo "Mark Baker" > data-vm/file5.txt
mkdir data-vm/dir2
echo "Calculator" > data-vm/dir2/file3.txt
```

Na lokalnom računalu, kopiranje direktorija *data-vm* s virtualnog stroja u *home* direktoriju lokalnog korisnika radi se naredbom:

```
rsync -avz -e 'ssh -p 8022' $USER@localhost:~/data-vm .
```

Podatci se prenose kao komprimirana arhiva (zastavice ```-avz```). Arhiva sadržava i metapodatke datoteka i direktorija kao što su vremenske oznake, vlasništva i simbolične poveznice. Ako se dogodi promjena nad jednom od datoteka ili direktorija, samo će se ta promjena prenijeti. Primjerice, ako se datoteci *file4.txt* na udaljenom računalu (u virtualnom stroju) doda neki podatak (primjerice naredba ```echo "I'm a file" >> data-vm/file4.txt```) i te se na lokalnom računalu ponovno izvrši naredba za kopiranje (naredba ```rsync -avz -e 'ssh -p 8022' $USER@localhost:~/data-vm .```), samo će se ta promjena prenijeti.

### Kloniranje cijelog stabla direktorija

Alat *rsync* može poslužiti kao alat za [kloniranje cijelog stabla direktorija](https://wiki.archlinux.org/title/Rsync#Full_system_backup). No, potrebno je isključiti virtualne direktorije poput */proc*, */sys*, */dev* i slično koji se nalaze u RAM-u te još direktorije na koje se inače montiraju druge particije. Naredba za kopiranje svih datoteka krenuvši od korijenskog direktorija s lokalnog računala na udaljeno je:

```
rsync -aAXHv --exclude='/dev/*' --exclude='/proc/*' --exclude='/sys/*' --exclude='/tmp/*' --exclude='/run/*' --exclude='/mnt/*' --exclude='/media/*' --exclude='/lost+found/' / /path/to/backup
```

**Postoji mogućnost korupcije podataka ako postoje otvorene datoteke u koje se piše tijekom kloniranja. Potrebno je sve datoteke zatvoriti.**

Naredba za kopiranje svih datoteka krenuvši od korijenskog direktorija udaljenog računala na lokalno je:

```
rsync -aAXHv --exclude='/dev/*' --exclude='/proc/*' --exclude='/sys/*' --exclude='/tmp/*' --exclude='/run/*' --exclude='/mnt/*' --exclude='/media/*' --exclude='/lost+found/' /path/to/backup /
```

## Kloniranje particija i diskova

Alat [*dd*](https://wiki.archlinux.org/title/Dd) je alat koji primarno služi kopiranju. Kopira podatke bajt po bajt i ne ovisi o datotečnom sustavu. Ovo ga čini pogodnim za kloniranje diskova i particija. Alat je vrlo koristan, ali i opasan jer nema ugrađene metode provjere integriteta pri kopiranju.

Kloniranje particije radi se naredbom:

```
dd if=[datoteka uređaja koja predstavlja particiju koja se klonira] of=[datoteka uređaja koja predstavlja odredišnu particiju] bs=64K conv=noerror,sync status=progress
```

Ako se želi postaviti unikatni UUID nakon kloniranja na novoj particiji, to se može naredbom:

```
tune2fs [datoteka uređaja koja predstavlja odredišnu particiju] -U random
```

**Odredišna particija mora postojati i mora biti veća ili jednaka particiji koja se klonira. Ako odredišna particija ne postoji stvorit će se datoteka koja će biti veličine te particije.**

Kloniranje diska radi se naredbom:

```
dd if=[datoteka uređaja koja predstavlja disk koji se klonira] of=[datoteka uređaja koji predstavlja odredišni disk] bs=64K conv=noerror,sync status=progress
```

**Odredišna particija mora postojati i mora biti veća ili jednaka particiji koja se klonira. Ako odredišna particija ne postoji stvorit će se datoteka koja će biti veličine te particije.**

Postoji i opcija stvaranje LZ4 komprimirane datoteke slike diska naredbom:

```
dd if=[datoteka uređaja koja predstavlja disk koji se klonira] conv=sync,noerror bs=64M status=progress | lz4 -z  > [ime odredišne datoteke].img.lz4
```

Pisanje te slike na odredišni disk radi se naredbom:

```
lz4 -dc [ime izvorne datoteke].img.lz4 | dd of=[datoteka uređaja koji predstavlja odredišni disk] status=progress
```

**Postoji mogućnost korupcije podataka ako postoje otvorene datoteke u koje se piše tijekom kloniranja. Potrebno je sve datoteke zatvoriti.**
