# ANSIBLE

[Ansible](https://www.ansible.com/how-ansible-works/) je alat za upravljanje konfiguracijama IT infrastrukture pisan u Pythonu. Zasniva se na konceptu upravljačkog čvora i upravljanim čvorovima. Upravljački čvor (primjerice osobno računalo ili laptop) komunicira s upravljanim čvorovima (primjerice grupa poslužitelja) uz pomoć SSH protokola (OpenSSH implementacija). Upravljački čvor mora biti čvor koji pokreće neku Linux distribuciju dok upravljani čvorovi mogu pokretat ili Linux ili Windows temeljene operacijske sustave.

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

Također, kako bi Debian virtualni strojevi mogli funkcionirati kao Ansible upravljani čvorovi, potrebno je instalirati Python 3:

```
sudo apt install python3
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

Potrebno je prijaviti se kao *root* korisnik (zaporka *passroot*) i urediti datoteku */etc/network/interfaces* tako što je potrebno sučelje koje je spojeno na mrežu (može se vidjeti s ```ip a```) postaviti u *static* umjesto *dhcp* te mu postaviti konfiguraciju na sljedeći način (uz pomoć uređivača teksta *nano*):

```
...
iface [ime sučelja] inet static
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

Sljedeće postavke se postavljaju kad se računalo domaćin koje pokreće Arch Linux ponovno pokrene ili se već postavljena mreža razmontirala. Prvo što treba napraviti je stvoriti virtualni mrežni most i TAP sučelja:

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
-netdev tap,id=net0,ifname=[ime TAP sučelja],script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=[ime virtualnog diska ciljanog virtualnog stroja]
```

Svakom virtualnom stroju potrebno je dodijeliti jedno TAP sučelje i jedan disk (disk-0.qcow2 za tap0, disk-1.qcow2 za tap1, disk-2.qcow2 za tap2,). Mogu se otvoriti tri zasebna terminala za pokretanje tri virtualna stroja odjednom, bitno je da se naredba pokrene iz direktorija *~/ansible-vms*.

Jesu li virtualni strojevi vidljivi na LAN mreži može se provjeriti naredbom *nmap* na Arch Linuxu (iz paketa *nmap*):

```
nmap -sn [IP adresa mreže/subnet maska]
```

### Brisanje mrežnih postavki nakon gašenja

Brisanje mrežne konfiguracije koja je bila postavljena na Arch Linuxu se radi sljedećim postupkom. Prvo je potrebno deaktivirati virtualni mrežni most, sva TAP sučelja i fizičko Ethernet sučelje:

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

## Primjer korištenja

Kao što je već navedeno, računalo domaćin koje pokreće Arch Linux će imati ulogu upravljačkog čvora dok će virtualni strojevi koji pokreću Debian imati ulogu upravljanih čvorova. Dakle, na računalu domaćinu koji koristi [Arch Linux](https://wiki.archlinux.org/title/Ansible) potrebno je instalirati *Ansible* naredbom:

```
sudo pacman -S ansible
```

Ako se izričito žele [namjestiti postavke Ansiblea](https://docs.ansible.com/ansible/latest/reference_appendices/config.html), to je moguće na sljedeći način:

- datoteka na koju pokazuje varijabla okruženja *ANSIBLE_CONFIG* ako postoji
- datoteka *ansible.cfg* u trenutačnom direktoriju
- datoteka *~/.ansible.cfg* u korisničkom *home* direktoriju
- datoteka */etc/ansible/ansible.cfg*

Datoteke će se pretraživati gornjim navedenim redoslijedom. Ako datoteka postoji prestaju se pretraživati sljedeće u redu. Stvaranje konfiguracijske datoteke u trenutačnom direktoriju sa svim zakomentiranim parametrima može se napraviti naredbom ```ansible-config init --disabled > ansible.cfg```. Značenja parametara mogu se naći [ovdje](https://docs.ansible.com/ansible/latest/reference_appendices/config.html#common-options).

### SSH ključevi

Ansible koristi SSH protokol kako bi pristupio upravljanim čvorovima. Potrebno je generirati par SSH ključeva i spremiti ih u direktorij *~/ansible-vms*:

```
ssh-keygen -t ed25519 -f ~/ansible-vms/ansible.key
```

Privatni ključ će se zvati *ansible.key* dok će se javni ključ zvati *ansible.key.pub*. Sada je potrebno kopirati javni ključ na pokrenute virtualne strojeve na lokaciji */home/user/.ssh/authorized_keys*:

```
ssh-copy-id -i ~/ansible-vms/ansible.key.pub user@192.168.1.201
ssh-copy-id -i ~/ansible-vms/ansible.key.pub user@192.168.1.202
ssh-copy-id -i ~/ansible-vms/ansible.key.pub user@192.168.1.203
```

### Inventar

Nakon instanciranja triju navedenih virtualnih strojeva, na računalu domaćinu je prvo potrebno specificirati u datoteku kojim čvorovima će Ansible upravljati. Ova datoteka se naziva [Ansible inventar (*eng. Ansible inventory*)](https://docs.ansible.com/ansible/latest/getting_started/get_started_inventory.html).

Inventar datoteke se mogu pisati u formatu INI ili YAML. U sljedećem primjeru koristit će se INI format. INI datoteke mogu imati sljedeći oblik:

```
[<ime grupe>]
<IP adresa ili ime prvog čvora> <parametar>=<vrijednost> <parametar>=<vrijednost> ...
<IP adresa ili ime drugog čvora> <parametar>=<vrijednost> <parametar>=<vrijednost> ...
...

[<ime grupe>:vars]
<paramerar grupe>=<vrijednost parametra grupe>
<paramerar grupe>=<vrijednost parametra grupe>
...

[<ime grupe>:children]
<ime grupe 2>
<ime grupe 3>
...

[<ime grupe 2>]
...

[<ime grupe 2>]
...
```

Prva sekcija ```[<ime grupe>]``` definira niz upravljanih čvorova i parametre koji će se koristiti za pristup njima. Sekcija ```[<ime grupe>:vars]``` definira zajedničke odnosno grupne parametre koji će se koristiti za pristup navedenim čvorovima i čvorovima djeci. Sekcija ```[<ime grupe>:children]``` definira druge nizove čvorova koji pripadaju ovom nizu odnosno definira čvorove djecu. Ovo daje fleksibilnost kod pokretanja zadataka nad grupama. Pokretanje zadataka nad roditeljskom grupom pokrenut će i zadatke nad svim grupama djece. Međutim, pokretanje zadataka nad grupom djeteta neće pokrenuti zadatke nad grupom roditelja. Više o Ansible inventaru može se saznati [ovdje](https://docs.ansible.com/ansible/latest/inventory_guide/intro_inventory.html).

Za primjer s tri virtualna stroja potrebno je u direktoriju *~/ansible-vms* stvoriti datoteku *inventory.ini* (naredba ```touch ~/ansible-vms/inventory.ini```) i popunit je sljedećim sadržajem (koristeći uređivač teksta *nano*):

```
[qemu_vms]
192.168.1.201
192.168.1.202
192.168.1.203

[qemu_vms:vars]
ansible_ssh_private_key_file=ansible.key
ansible_user=user
ansible_python_interpreter=/usr/bin/python3
```

Prethodni inventar definira niz čvorova u grupi *qemu_vms* u kojem je potrebno izvršiti zadatke. Također, definira se [grupna varijabla](https://docs.ansible.com/ansible/latest/inventory_guide/intro_inventory.html#connecting-to-hosts-behavioral-inventory-parameters) koja specificira SSH privatni ključ i korisničko ime koje treba koristiti pri spajanju na svaki navedeni čvor. Također definira i Python interpreter koji treba koristiti na udaljenim čvorovima.

Provjera jesu li svi navedeni čvorovi živi može se napraviti naredbom u direktoriju *~/ansible-vms/*:

```
ansible -i inventory.ini qemu_vms -m ping
```

Izlaz bi trebao biti sličan ovome:

```
192.168.1.201 | SUCCESS => {
    "changed": false,
    "ping": "pong"
}
192.168.1.202 | SUCCESS => {
    "changed": false,
    "ping": "pong"
}
192.168.1.203 | SUCCESS => {
    "changed": false,
    "ping": "pong"
}
```

### Moduli

[Ansible moduli (*eng. Ansible Modules*)](https://docs.ansible.com/ansible/2.8/user_guide/modules_intro.html) su gotove Python skripte sa specifičnim funkcijama koje Ansible izvršava. Primjer modula je navedeni modul [*ping*](https://docs.ansible.com/ansible/latest/collections/ansible/builtin/ping_module.html). Popis svih modula nalazi se [ovdje](https://docs.ansible.com/ansible/2.8/modules/list_of_all_modules.html). Modulima se mogu prosljeđivati razni argumenti u obliku *ključ=vrijednost*.

Neki moduli mogu tražiti izvršavanje s privilegiranim ovlastima na upravljanim čvorovima. Primjerice, izvršavanje naredbe ```ansible -i inventory.ini qemu_vms -m apt -a "update_cache=true"``` vratit će grešku jer naredba modula *apt* zahtijeva više ovlasti na upravljanim čvorovima (slično ```sudo apt update```). Rješenje ovog problema je dodavanjem zastavice *--become* i *--ask-become*:

```
ansible -i inventory.ini qemu_vms -m apt -a "update_cache=true" --become --ask-become
```

### Playbook

[Ansible Playbook](https://docs.ansible.com/ansible/latest/playbook_guide/playbooks_intro.html) je datoteka koja definira skup zadataka koji će se izvršavati na upravljanim čvorovima. Playbook se piše u YAML formatu i izvršava se redoslijedom odozgora prema dolje. Najbolje je pokazati primjer s datotekom *install_apache.yml* u direktoriju *~/ansible-vms* (naredba ```touch ~/ansible-vms/inventory.ini```) koja ažurira lokalnu bazu podataka dostupnih paketa, instalira Apache web poslužitelj i pokreće ga. Dakle datoteka *install_apache.yml* bi trebala izgledati ovako (koristeći uređivač teksta *nano*):

```
- name: Installing Apache Web Server on Debian servers
  hosts: qemu_vms
  become: yes
  tasks:
    - name: Update local package database
      apt:
        update_cache: yes
    - name: Install Apache
      apt:
        name: apache2
        state: present
    - name: Start Apache
      service:
        name: apache2
        state: started
        enabled: true
```

Pokretanje Playbooka radi se naredbom:

```
ansible-playbook install_apache.yml -i inventory.ini --ask-become-pass
```

Prethodni Playbook koristi module [*apt*](https://docs.ansible.com/ansible/latest/collections/ansible/builtin/apt_module.html) i [*service*](https://docs.ansible.com/ansible/latest/collections/ansible/builtin/service_module.html) za instalaciju i pokretanje Apache web poslužitelja. Poslužitelji su dostupni na adresama *http://192.168.1.201*, *http://192.168.1.202* i *http://192.168.1.203*.

Isječak odnosno početno zaglavlje iz datoteke *install_apache.yml*:

```
- name: Installing Apache Web Server on Debian servers
  hosts: qemu_vms
  become: yes
  tasks:
```

Objašnjenja pojmova:

- *name: Installing Apache Web Server on Debian servers*
	- opis što radi Playbook
	- ispisuje se pri izvršenju Playbooka
- *hosts: qemu_vms*
	- nad kojom grupom upravljanih čvorova će se izvršiti zadatci (grupa *qemu_vms* u datoteci *inventory.ini*)
- *become: yes*
	- izvrši zadatke kao privilegirani korisnik (zadatci se izvršavaju uz *sudo*)
- *tasks:*
	- niz zadataka kojih je potrebno izvršiti

Isječak zadatka koji iz datoteke *install_apache.yml* ažurira lokalnu bazu podataka dostupnih paketa:

```
    - name: Update local package database
      apt:
        update_cache: yes
```

Objašnjenja pojmova:

- *name: Update local package database*
	- opis što radi zadatak
	- ispisuje se pri izvršenju zadatka
- *apt:*
	- modul koji se koristi za [upravljanje APT paketima](https://docs.ansible.com/ansible/latest/collections/ansible/builtin/apt_module.html)
- *update_cache: yes*
	- argument modula, označava osvježavanje lokalne baze podataka dostupnih paketa

Zadatak je sličan naredbi ```sudo apt update```.

Isječak zadatka iz datoteke *install_apache.yml* koji instalira Apache web poslužitelj:

```
    - name: Install Apache
      apt:
        name: apache2
        state: present
```

Objašnjenja pojmova:

- *name: Install Apache*
	- opis što radi zadatak
	- ispisuje se pri izvršenju zadatka
- *apt:*
	- modul koji se koristi za [upravljanje APT paketima](https://docs.ansible.com/ansible/latest/collections/ansible/builtin/apt_module.html)
- *name: apache2*
	- ime paketa za instalaciju, u ovom slučaju *apache2*
- *state: present*
	- paket mora biti instaliran, ako već jest nije potrebna ponovna instalacija

Zadatak je sličan naredbi ```sudo apt install -y apache2```.

Isječak zadatka iz datoteke *install_apache.yml* koji pokreće Apache web poslužitelj:

```
    - name: Start Apache
      service:
        name: apache2
        state: started
        enabled: true
```

Objašnjenja pojmova:

- *name: Start Apache*
	- opis što radi zadatak
	- ispisuje se pri izvršenju zadatka
- *service:*
	- modul koji se koristi za [upravljanje servisima](https://docs.ansible.com/ansible/latest/collections/ansible/builtin/service_module.html)
	- podržava systemd, BSD, OpenRC, SysV i druge *init* sustave
- *name: apache2*
	- ime servisa koji se treba pokrenuti
- *state: started*
	- servis mora biti pokrenut, ako nije potrebno ga je pokrenuti
- *enabled: true*
	- servis mora biti omogućen tako da se pokrene pri svakom ponovnom pokretanju virtualnog stroja

Zadatak je sličan naredbama ```sudo systemctl start apache2``` i ```sudo systemctl enable apache2```.
