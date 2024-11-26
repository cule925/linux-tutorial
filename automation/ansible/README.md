# ANSIBLE

[Ansible](https://www.ansible.com/how-ansible-works/) je alat za upravljanje konfiguracijama IT infrastrukture pisan u Pythonu. Zasniva se na konceptu upravljačkog čvora i upravljanim čvorovima. Upravljački čvor (primjerice osobno računalo ili laptop) komunicira s upravljanim čvorovima (primjerice grupa poslužitelja) uz pomoć SSH protokola (OpenSSH implementacija). Upravljački čvor mora biti čvor koji pokreće Linux bazirane operacijske sustave dok upravljani čvorovi mogu pokretat ili Linux ili Windows bazirane operacijske sustave.

U sljedećim primjerima, računalo koje pokreće Arch Linux će imati ulogu upravljačkog čvora dok će virtualni strojevi koji pokreću Debian imati ulogu upravljanih čvorova. Virtualni strojevi će se pokretati na istom računalu.

## Inicijalne postavke okoline prije primjera korištenja

Primjer korištenja Ansiblea će se pokazivati na računalu koje pokreće Arch Linux. Prije pokazivanja primjera korištenja potrebno je pokrenuti tri [QEMU](../../virtualization/qemu) virtualna stroja koja pokreću Debian te ih povezati virtualnim mrežnim mostom:

```
        *-------------------------------------------------*
        | ARCH LINUX HOST                                 |
        |                                                 |
        |    *---------*    *---------*    *---------*    |
        |    | QEMU VM |    | QEMU VM |    | QEMU VM |    |
        |    | RUNNING |    | RUNNING |    | RUNNING |    |
        |    | DEBIAN  |    | DEBIAN  |    | DEBIAN  |    |
        |    *--[ETH]--*    *--[ETH]--*    *--[ETH]--*    |
        |       [TAP]          [TAP]          [TAP]       |
        |         |              |              |         |
        |         *----------[BRIDGE]-----------*         |
        |                        |                        |
        *----------------------[ETH]----------------------*
```

Dakle, računalo domaćin i tri virtualna stroja moraju se nalaziti na istoj LAN mreži (primjer s kućnim usmjernikom i statičkim adresama):

```
        *-------------*    LAN                                                                                                         OTHER DEVICES
        | HOME ROUTER |---------------+----------------------+----------------------+----------------------+-----------------------... CONNECTED TO
        *-------------*               |                      |                      |                      |                           THE NETWORK
          192.168.1.1                 |                      |                      |                      |
                               *------|-----*         *------|-----*         *------|-----*         *------|-----*
                               | PC running |         | VM running |         | VM running |         | VM running |
                               | Arch Linux |         |   Debian   |         |   Debian   |         |   Debian   |
                               *------------*         *------------*         *------------*         *------------*
                               192.168.1.200          192.168.1.201          192.168.1.202          192.168.1.203
```

Na Arch Linuxu, potrebno je prvo napraviti direktorij *ansible-vms*, premjestiti se u njega i preuzeti ISO sliku Debiana (inačica Bookworm 12.8.0 u ovom slučaju):

```
mkdir ~/ansible-vms
cd ~/ansible-vms
wget http://debian.carnet.hr/debian-cd/12.8.0/amd64/iso-cd/debian-12.8.0-amd64-netinst.iso
```

Ako QEMU nije instaliran, potrebno ga je instalirati naredbom ```sudo pacman -S qemu-full```. Na virtualnim slikama diska će se instalirati minimalne verzije Debian distribucije. Iz toga slijedi da je preporučena veličina virtualnog tvrdog diska 4 GiB. Stvaranje virtualnog tvrdog diska radi se naredbom:

```
qemu-img create -f qcow2 disk-backup.qcow2 4G
```

Prethodna naredba će stvoriti datoteku *disk-backup.qcow2* na kojoj će se instalirati minimalna verzija Debiana. Nakon instalacije Debiana, ovaj disk će se kopirati tri puta. Disk *disk-backup.qcow2* će služiti kao sigurnosna kopija. Za instalaciju nije potrebno namještati nikakve mrežne postavke već je potrebno samo pokrenuti virtualni stroj sa SLIRP mrežnim postavkama naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 4 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom debian-12.8.0-amd64-netinst.iso \
-drive format=qcow2,file=disk-backup.qcow2
```

Ovakav način rada je BIOS način rada i on će se koristiti za sva buduća pokretanja virtualnih strojeva. To znači da je na tvrdom disku potrebno koristiti MBR shemu particioniranja. Parametri poput ukupne dodijeljene memorije (*-m*) ili broj dodijeljenih procesorskih jezgri (*-smp*) virtualnom stroju ne moraju biti točno jednaki predloženome već ih se može prilagoditi po potrebi i mogućnostima računala domaćina. Najlakši način instalacije Debiana na virtualni tvrdi disk je grafičkom instalacijom. Dakle, potrebno je odabrati *Graphical Install* te namjestiti sljedeću konfiguraciju:

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
- *Select disk to partition:* "SCSI1 (0,0,0) (sda) - 4.3 GB ATA QEMU HARDDISK"
- *Partitioning scheme:* "All files in one partition (recommended for new users)"
- odabrati "Finish partitioning and write changes to disk"
- *Write changes to disk:* "Yes"
- *Scan extra installation media:* "No"
- *Debian archive mirror country:* "Croatia"
- *Debian archive mirror:* "deb.debian.org"
- *HTTP proxy information:*
- *Participate in the package usage survey:* "No"
- *Choose software to install:* "SSH server" i "standard system utilities"
- *Install the GRUB boot loader to your primary drive:* "Yes"
- *Device for boot loader installation:* "/dev/sda (ata-QEMU_HARDDISK_QM00001)"

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

Na kraju je potrebno ugasiti virtualni stroj:

```
poweroff
```

Nakon gašenja stroja, virtualni tvrdi disk *disk-baskup.qcow2* je potrebno kopirati tri puta. Ovi diskovi će predstavljati diskove poslužitelja kojima će upravljati Ansible. Dakle, potrebno je izvršiti naredbe:

```
cp disk-backup.qcow2 disk-0.qcow2
cp disk-backup.qcow2 disk-1.qcow2
cp disk-backup.qcow2 disk-2.qcow2
```

### Inicijalne mrežne postavke

Svaki virtualni stroj i sami most će imati postavljene statičke IP adrese odnosno DHCP se **neće** koristiti. Kako bi se saznalo koje se IP adrese trenutačno ne koriste na LAN na kojeg je spojeno računalo domaćin koje pokreće Arch Linux, potrebno je upisati naredbu:

```
nmap -sn [IP adresa mreže/subnet maska]
```

U navedenim shemama navedene su IP adrese *192.168.1.200* za Arch Linux, *192.168.1.201* za prvi virtualni stroj, *192.168.1.202* za drugi virtualni stroj i *192.168.1.203* za treći virtualni stroj pa će se te adrese i koristiti za daljnju demonstraciju dok je će IP adresa *gatewaya* biti *192.168.1.1*. Arch Linux će koristiti već definiran DNS poslužitelj dok će se za virtualne strojeve koristiti DNS poslužitelji s IP adresama *8.8.8.8* i *8.8.4.4*.

Prije konfiguracije mrežnih postavki na Arch Linuxu, potrebno je na Debian virtualnim strojevima urediti datoteke [*/etc/network/interfaces*](https://wiki.debian.org/NetworkConfiguration#Setting_up_an_Ethernet_Interface) za postavljanje statične IP adrese i *gatewaya* te [*/etc/resolve.conf*](https://wiki.debian.org/NetworkConfiguration#Defining_the_.28DNS.29_Nameservers) za postavljanje IP adresa DNS poslužitelja. 

Dakle potrebno je pokrenuti svaki virtualni stroj naredbom:

```
qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 2 \
-netdev user,id=net0,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=[ime virtualnog diska ciljanog virtualnog stroja]
```

Potrebno je prijaviti se kao *root* korisnik (zaporka *passroot*) i urediti datoteku */etc/network/interfaces* tako što je potrebno sučelje *ens3* postaviti u *static* umjesto *dhcp* te mu postaviti konfiguraciju na sljedeći način (uz pomoć uređivača teksta *nano*):

```
...
iface ens3 inet static
	address [IP adresa ciljanog virtualnog stroja]/[subnet maska]
	gateway [IP adresa gatewaya]
```

Također, potrebno je u Debianu urediti datoteku */etc/resolve.conf* na sljedeći način (koristeći uređivač teksta *nano*):

```
nameserver 8.8.8.8
nameserver 8.8.4.4
```

Bilo bi dobro i preimenovati virtualne strojeve, primjerice u imena *debian-vm-0*, *debian-vm-1* i *debian-vm-2* naredbom:

```
hostnamectl set-hostname [ime virtualnog stroja]
```

Naposljetku, virtualne strojeve je potrebno ugasiti naredbom:

```
poweroff
```

### Konfiguracija mrežnih postavki pri svakom pokretanju

Sada je potrebno postaviti mrežne postavke n Arch Linuxu. Prvo što treba napraviti je stvoriti virtualni mrežni most i TAP sučelja:

```
sudo ip link add name br0 type bridge
sudo ip tuntap add tap0 mode tap user root
sudo ip tuntap add tap1 mode tap user root
sudo ip tuntap add tap2 mode tap user root
```

Nakon toga je potrebno virtualnom mrežnom mostu i TAP sučeljima dodijeliti MAC adrese:

```
sudo ip link set br0 address 02:FF:FF:FF:FF:00
sudo ip link set tap0 address 02:FF:FF:FF:FF:01
sudo ip link set tap1 address 02:FF:FF:FF:FF:02
sudo ip link set tap2 address 02:FF:FF:FF:FF:03
```

Ime fizičkog Ethernet sučelja koje je spojeno na mrežu može se saznati naredbom ```ip a```. Sada je potrebno povezati TAP sučelja i fizičko Ethernet sučelje na mrežni most te aktivirati sva TAP sučelja uključujući na kraju i virtualni mrežni most:

```
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] master br0
sudo ip link set tap0 master br0
sudo ip link set tap1 master br0
sudo ip link set tap2 master br0
sudo ip link set tap0 up
sudo ip link set tap1 up
sudo ip link set tap2 up
sudo ip link set br0 up
```

Postavljanje statičke IP adrese i IP adrese *gatewaya* virtualnog mrežnog mosta može se napraviti naredbama:

```
sudo ip address add 192.168.1.200/24 dev br0
sudo ip route append default via 192.168.1.1
```

### Pokretanje virtualnih strojeva

Naredba za pokretanje virtualnih strojeva je:

```
sudo qemu-system-x86_64 \
-enable-kvm \
-m 2G \
-cpu host \
-smp 2 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=[ime virtualnog diska ciljanog virtualnog stroja]
```

Mogu se otvoriti tri zasebna terminala za pokretanje tri virtualna stroja odjednom, bitno je da se naredba pokrene iz direktorija *~/ansible-vms*.

Jesu li virtualni strojevi vidljivi na LAN mreži može se provjeriti naredbom *nmap* na Arch Linuxu (iz paketa *nmap*):

```
nmap -sn [IP adresa mreže/subnet maska]
```

### Brisanje mrežnih postavki nakon gašenja

Obrnuti postupak konfiguracije mrežnih postavki na Arch Linuxu je sljedeći. Prvo je potrebno deaktivirati virtualni mrežni most, sva TAP sučelja i fizičko Ethernet sučelje:

```
sudo ip link set br0 down
sudo ip link set tap2 down
sudo ip link set tap1 down
sudo ip link set tap0 down
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] down
```

Zatim je potrebno odspojiti TAP sučelja i fizičko Ethernet sučelje s virtualnog mrežnog mosta:

```
sudo ip link set tap2 nomaster
sudo ip link set tap1 nomaster
sudo ip link set tap0 nomaster
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] nomaster
```

Onda je potrebno izbrisati sva TAP sučelja:

```
sudo ip tuntap del tap2 mode tap
sudo ip tuntap del tap1 mode tap
sudo ip tuntap del tap0 mode tap
```

Konačno, brisanje mrežnog mosta moguće je naredbom:

```
sudo ip link delete br0 type bridge
```

Za kraj, ponovna aktivacija Ethernet sučelja radi se naredbom:

```
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] up
```
