# QEMU

[QEMU (*eng Quick Emulator*)](https://wiki.archlinux.org/title/QEMU) je generični emulator i virtualizator. Na Linuxu, QEMU koristi jezgreni modul [KVM](https://www.redhat.com/en/topics/virtualization/what-is-KVM) koji služi kao hipervizor. Kada virtualni strojevi koriste procesor domaćina (odnosno emulira se procesor domaćina), KVM će koristiti hardverske značajke poput Intel VT-x (*Intel Virtualization Technology*) za Intelove procesore ili AMD-V (*AMD Virtualization*) za AMD-ove procesore što će dati bolje performanse. Ovaj način rada pretvara domaćin Linux u tip 1 hipervizor.

KVM je dio Linux jezgre i on ima pristup upravitelju memorije, raspoređivaču procesa, ulazno-izlaznom stogu, upravljačkim programima, mrežnom stogu i slično. Svaki pokrenuti virtualni stroj je iz pogleda Linuxa jedan proces koji se raspoređuje raspoređivačem.

KVM je bio primarno dizajniran za x86_64 arhitekture jer koristi gore navedene hardverske značajke, ali u današnje vrijeme se koristi i na računalima ARM arhitekture.

## Instalacija QEMU alata na Arch Linuxu

Instalacija QEMU-a se može izvršiti naredbom:

```
sudo pacman -S qemu-full
```

Prethodna naredba će instalirati QEMU s podrškom grafičkog sučelja virtualnog stroja. Ako se želi instalirati QEMU bez navedene podrške, potrebno je samo instalirati paket ```qemu-base```.

Instalacijom QEMU-a, nudi nam se niz naredbi za emulaciju cjelokupnog sustava oblika:

```
qemu-system-[arhitektura za emulaciju]
```

Primjerice, ako se želi emulirati x86_64 arhitektura, virtualni stroj bi se pokretao naredbom ```qemu-system-x86_64```, dok bi se za ARM64 arhitekturu pokretala naredba ```qemu-system-aarch64```. Ako je računalo domaćin iste arhitekture kao i željena arhitektura virtualnog stroja, koristit će se značajka KVM kako bi se instrukcije virtualnog stroja direktno izvršavale na hardveru umjesto translacijom instrukcija.

## Stvaranje virtualnog tvrdog diska

Prije pokretanja virtualnog stroja, potrebno je stvoriti virtualni disk na kojem će se instalirati operacijski sustav koji će se pokretati u virtualnom stroju. Stvaranje virtualnog diska moguće je naredbom:

```
qemu-img create -f qcow2 [ime diska] [veličina]
```

Prethodna naredba će stvoriti datoteku koja predstavlja virtualni disk formata QCOW2. Format QCOW2 je prilagodljiv format specifično namijenjen za QEMU. Moguće je stvoriti i diskove drugih formata, primjerice VDI za VirtualBox ili VDMK za VMWare.

Postoje i opcije za promjenu veličine diska (```qemu-img resize ...```) te promjene formata (```qemu-img convert```).

## Pokretanje virtualnog stroja u BIOS načinu rada

Kao primjer emulirat će se x86_64 sustav na x86_64 uz korištenje KVM značajke. Za instalaciju operacijskog sustava na virtualni tvrdi disk potrebno je preuzeti odgovarajuću ISO datoteku (primjerice Arch Linux ISO datoteku). Naredba ```qemu-system-x86_64``` nudi mnogo opcija i one se mogu vidjeti zastavicom ```--help```.

### Jednostavno pokretanje bez root ovlasti (SLIRP mrežne postavke)

Primjer naredbe za pokretanje virtualnog stroja je sljedeći:

```
qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-cdrom archlinux.iso \
-drive format=qcow2,file=archlinux.qcow2
```

Objašnjenje argumenata:

* *-enable-kvm* - omogućuje KVM značajku
* *-m 8G* - specificira koliko RAM-a je potrebno alocirati virtualnom stroju, u ovom slučaju 8 GiB RAM-a
* *-cpu host* - specificira koji model procesora se emulira u virtualnom stroju, u ovom slučaju je isti model kao i procesor domaćina
* *-smp 4* - specificira koliko procesorskih jezgri alocirati virtualnom stroju
* *-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1*
	- konfigurira mrežne postavke virtualnog stroja
	- oznaka *user* definira tip umrežavanja koji koristi [SLIRP protokol](https://wiki.qemu.org/Documentation/Networking), omogućava virtualnom stroju pristup vanjskoj mreži bez dodatne konfiguracije
	- oznaka *id=net0* definira ime *user* tipa umrežavanja (u ovom slučaju je ime *net0*), ovom oznakom moguće je asocirati emuliranu mrežnu karticu s ovim tipom umrežavanja
	- oznaka *hostfwd=tcp::8022-:22* definira prosljeđivanje porta od računala domaćina do virtualnog stroja (u ovom slučaju od porta 8022 domaćina na port 22 virtualnog stroja)
	- oznaka *net=10.0.0.0/24* definira raspon adresa koji će se koristiti u toj mreži što su u ovom slučaju adrese od *10.0.0.0* do *10.0.0.255*
	- oznaka *host=10.0.0.1* označava IP adresu računala domaćina iz lokalne mreže *net0*, virtualni stroj može adresirati računalo domaćin uz tu adresu
* *-device virtio-net,netdev=net0*
	- emulira mrežni uređaj
	- oznaka *virtio-net-pci* emulira PCI mrežnu karticu koja je spojena na virtualni stroj
	- oznaka *netdev=net0* označava prethodne definirane mrežne postavke koje će emulirana mrežna kartica pratiti
* *-cdrom archlinux.iso*
	- emulira CDROM s umetnutim diskom koji sadrži ISO sliku (u ovom slučaju *archlinux.iso*)
* *-drive format=qcow2,file=archlinux.qcow2*
	- spaja virtualni tvrdi disk na virtualni stroj
	- oznaka *format=qcow2* označava format tvrdog diska
	- oznaka *file=archlinux.qcow2* označava ime datoteke koja predstavlja tvrdi disk

Ovakav SLIRP način rada je dosta sporiji i ima dosta veliki *overhead*, ali zato omogućuje pokretanje bez privilegiranih ovlasti. Nakon instalacije operacijskog sustava nije potrebno više pokretati virtualni stroj s emuliranim CDROM-om, dakle naredba za pokretanje virtualnog stroja je:

```
qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev user,id=net0,hostfwd=tcp::8022-:22,net=10.0.0.0/24,host=10.0.0.1 \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=archlinux.qcow2
```

Ovakav način rada je BIOS način rada. Na tvrdom disku je potrebno koristiti MBR shemu particioniranja.

### TAP sučelje

Kako je SLIRP način rada relativno spor zbog velikog *overheada*, moguće je virtualni stroj s drugim mrežnim postavkama, primjerice [TAP sučeljima](https://en.wikipedia.org/wiki/TUN/TAP) koje imaju znatno bolje performanse, ali zahtijevaju konfiguraciju na računalu domaćinu. TAP je jezgrin virtualni mrežni uređaj koji simulira mrežni uređaj koji implementira drugi sloj OSI modela odnosno sloj podatkovne poveznice. Ovo znači da TAP uređaj ima svoju MAC adresu. Kako bi spojili TAP uređaj na virtualni stroj, potrebno ga je stvoriti na računalu domaćinu naredbom:

```
sudo ip tuntap add tap0 mode tap user root
```

Ovo će stvoriti mrežni uređaj TAP imena *tap0*. Moguće mu je dodijeliti MAC adresu naredbom:

```
sudo ip link set tap0 address 02:FF:FF:FF:FF:01
```

Sljedeće što je potrebno je stvoriti virtualni mrežni most. Virtualni mrežni most se ponaša kao mrežni preklopnik, prosljeđujući pakete po MAC adresama. Virtualni mrežni most se može stvoriti naredbom:

```
sudo ip link add name br0 type bridge
```

Mostu se također može dodijeliti MAC adresa:

```
sudo ip link set br0 address 02:FF:FF:FF:FF:00
```

Prije povezivanja svih potrebnih sučelja, potrebno ih je aktivirati:

```
sudo ip link set br0 up
sudo ip link set tap0 up
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] up
```

Zatim je potrebno povezati fizičko Ethernet sučelje i TAP sučelje na most:

```
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] master br0
sudo ip link set tap0 master br0
```

Konačno, računalo krajnja točka komunikacije računala domaćina se nalazi na *br0* umjesto na Ethernet priključku stoga je potrebno zatražiti novu IP adresu od usmjernika naredbom:

```
sudo dhclient br0
```

Naredba za pokretanje virtualnog stroja je:

```
sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
[-cdrom archlinux.iso] \
-drive format=qcow2,file=archlinux.qcow2
```

Većina argumenata su isti osim:

* *-netdev tap,id=net0,ifname=tap0,script=no,downscript=no*
	- konfigurira mrežne postavke virtualnog stroja
	- oznaka *tap* definira tip umrežavanja koji koristi [TAP](https://docs.kernel.org/networking/tuntap.html), omogućava virtualnom stroju pristup mrežni pristup
	- oznaka *id=net0* definira ime *tap* tipa umrežavanja (u ovom slučaju je ime *net0*), ovom oznakom moguće je asocirati emuliranu mrežnu karticu s ovim tipom umrežavanja
	- oznaka *ifname=tap0* specificira ime TAP uređaja na računalu domaćinu (u ovom slučaju je *tap0*)
	- oznaka *script=no* specificira skriptu koja će se izvršiti prilikom pokretanja virtualnog stroja (u ovom slučaju se ne pokreće skripta), specificira upute što treba napraviti s TAP uređajem, primjerice pridruživanje mostu
	- oznaka *downscript=no* specificira skriptu koja će se izvršiti prilikom obustavljanja virtualnog stroja (u ovom slučaju se ne pokreće skripta), specificira upute što treba napraviti s TAP uređajem, primjerice razdruživanje od mosta

Kada virtualni stroj završi s radom potrebno je razmontirati sve. Za početak je potrebno deaktivirati Ethernet i TAP sučelja:

```
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] down
sudo ip link set tap0 down
```

Nakon toga je potrebno ukloniti sva sučelja s mosta:

```
sudo ip link set tap0 nomaster
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] nomaster
```

Onda je potrebno obrisati TAP sučelje:

```
sudo ip tuntap del tap0 mode tap
```

Sljedeće što je potrebno je deaktivirati most:

```
sudo ip link set br0 down
```

Zatim je potrebno obrisati most:

```
sudo ip link delete br0 type bridge
```

Naposljetku je potrebno aktivirati Ethernet sučelje i zatražiti od usmjernika novu IP adresu:

```
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] up
```

#### Skripte za naredbe

Sve prethodne naredbe moguće je utovariti u bash skripte. Primjerice, možemo imati tri skripte: *bridge_set.sh* (postavlja most), *boot.sh* (postavlja TAP sučelje, pokreće virtualni stroj, poništava TAP sučelje), *boot_with_iso.sh* (kao i *boot.sh* uz ISO sliku za instalaciju) i *bridge_unset.sh* (poništava most).

Skripta *bridge_set.sh* imat će sljedeći sadržaj:

```
#!/bin/bash

sudo ip link add name br0 type bridge
sudo ip link set br0 address 02:FF:FF:FF:FF:00
sudo ip link set br0 up

sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] up
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] master br0

sudo dhclient br0
```

Skripta *boot.sh* imat će sljedeći sadržaj:

```
#!/bin/bash

sudo ip tuntap add tap0 mode tap user root
sudo ip link set tap0 address 02:FF:FF:FF:FF:01

sudo ip link set tap0 master br0
sudo ip link set tap0 up

sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=archlinux.qcow2

sudo ip link set tap0 down
sudo ip link set tap0 nomaster

sudo ip tuntap del tap0 mode tap
```

Skripta *boot_with_iso.sh* imat će sljedeći sadržaj:

```
#!/bin/bash

sudo ip tuntap add tap0 mode tap user root
sudo ip link set tap0 address 02:FF:FF:FF:FF:01

sudo ip link set tap0 master br0
sudo ip link set tap0 up

sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-cdrom archlinux.iso \
-drive format=qcow2,file=archlinux.qcow2

sudo ip link set tap0 down
sudo ip link set tap0 nomaster

sudo ip tuntap del tap0 mode tap
```

Skripta *bridge_unset.sh* imat će sljedeći sadržaj:

```
#!/bin/bash

sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] down
sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] nomaster

sudo ip link set br0 down
sudo ip link delete br0 type bridge

sudo ip link set [fizičko Ethernet sučelje spojeno na Internet] up
```

Konačno, druga računala na mreži sada mogu izravno komunicirati s virtualnim strojem bez ikakvog potrebnog prosljeđivanja porta s domaćina. Virtualni stroj se ponaša kao da je izravno spojen na LAN mrežu uz računala domaćina.

### Prosljeđivanje zvuka

Kako bi proslijedili zvuk iz virtualnog stroja potrebno je koristiti *virtio*, QEMU-ov paravirtualiziran audio uređaj. Naredba za pokretanja QEMU-a bi trebala izgledati ovako:

```
sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=archlinux.qcow2 \
-audio driver=pa,model=virtio,server=/run/user/1000/pulse/native
```

Objašnjenje novih argumenata:

* *-audio driver=pa,model=virtio,server=/run/user/1000/pulse/native*
	- konfigurira postavke zvuka virtualnog stroja
	- oznaka *driver=pa* definira korištenje [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/) poslužitelja na računalu domaćinu
	- oznaka *model=virtio* definira paravirtualiziran audio uređaj koji će se koristiti u virtualnom stroju (u ovom slučaju je ime *virtio*)
	- oznaka *server=/run/user/1000/pulse/native* definira UNIX priključak PulseAudio poslužitelja kojim će virtualni stroj slati zvuk

Računalo domaćin u ovom slučaju mora koristiti PulseAudio audio poslužitelj.

### Poboljšanje rezolucije

Radi boljih performansi i promjene rezolucije, umjesto standardnog *std* [paravirtualizirani GPU](https://www.kraxel.org/blog/2019/09/display-devices-in-qemu/) potrebno je koristiti *virtio* paravirtualiziran GPU. Naredba za pokretanja QEMU-a bi trebala izgledati ovako:

```
sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=archlinux.qcow2 \
-audio driver=pa,model=virtio,server=/run/user/1000/pulse/native \
-vga virtio
```

### Prosljeđivanje USB uređaja

[Prosljeđivanje USB uređaja](https://qemu-project.gitlab.io/qemu/system/devices/usb.html) priključenog na računalo domaćin u virtualni stroj moguće prvenstveno saznanjem broja sabirnice USB uređaja i broja uređaja koji mu je operacijski sustav domaćina dodijelio. To se može izvršiti naredbom iz paketa *usbutils*:

```
lsusb
```

Izlazi bi trebali biti oblika:

```
Bus [broj sabirnice] Device [broj uređaja]: ID [ID proizvođača]:[ID uređaja] [opis uređaja]
```

Potrebno je imati na umu da se broj uređaja mijenja svakim ponovnim priključivanjem USB uređaja. Dakle, USB uređaj mora ostati uključen cijelo vrijeme tijekom rada virtualnog stroja. Naredba za pokretanje QEMU-a bi trebala izgledati ovako:

```
sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
-drive format=qcow2,file=archlinux.qcow2 \
-audio driver=pa,model=virtio,server=/run/user/1000/pulse/native \
-vga virtio \
-device qemu-xhci,id=xhci \
-device usb-host,hostbus=[broj sabirnice],hostaddr=[broj uređaja]
```

XHCI upravljač je USB upravljač koji ima potporu za USB 1.0, USB 2.0 i USB 3.0. Većina operacijskih sustava poslije 2010-e godine nudi podršku za ovaj upravljač. Prethodna naredba emulira XHCI.

## Pokretanje virtualnog stroja u UEFI načinu rada

Potrebno je stvoriti novu datoteku virtualnog diska i koristiti GPT shemu particioniranja.

OVMF je projekt koji omogućuje podršku za UEFI način rada u virtualnim strojevima. Instalacija OVMF-a na računalo domaćin u slučaju Arch Linuxa može se napraviti naredbom:

```
sudo pacman -S edk2-ovmf
```

Dvije datoteke koje su bitne u ovom kontekstu su */usr/share/edk2/x64/OVMF_CODE.4m.fd* i */usr/share/edk2/x64/OVMF_VARS.4m.fd*. Datoteka */usr/share/edk2/x64/OVMF_CODE.4m.fd* sadrži UEFI firmware dok */usr/share/edk2/x64/OVMF_VARS.4m.fd* služi za spremanje zapisa odnosno varijabli. Datoteka */usr/share/edk2/x64/OVMF_VARS.4m.fd* se obično kopira dok se */usr/share/edk2/x64/OVMF_CODE.4m.fd* namješta samo za čitanje. Dakle, datoteku zapisa se može kopirati na mjesto virtualnog tvrdog diska:

```
cp /usr/share/edk2/x64/OVMF_VARS.4m.fd [direktorij gdje se nalazi virtualni tvrdi disk]
```

Primjer pokretanja QEMU-a s UEFI firmware-om:

```
sudo qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
[-cdrom archlinux.iso] \
-drive format=qcow2,file=archlinux.qcow2 \
-audio driver=pa,model=virtio,server=/run/user/1000/pulse/native \
-vga virtio \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd
```

### Korisne skripte

U direktoriju [uefi-script](uefi-scripts) se nalaze skripte za pokretanje virtualnog stroja s TAP mrežnom konfiguracijom u UEFI načinu rada.
