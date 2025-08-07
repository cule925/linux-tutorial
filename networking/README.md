# LINUX I MREŽE

Ovo su upute za Linux i mreže. Generalno, upute su sljedeće:

* za upravljanje mrežnim konekcijama: [*networkmanager*](networkmanager)
* za firewall: [*iptables*](iptables)

Tipični primjer lokalne mreže (*eng. LAN - Local Area Network*):

```
                                                                     INTERNET
                                                                        |
             LAN                                                        |
             +------------------+------------------+----------------+   |
             |                  |                  |                |   |
        +----+----+        +----+----+        +----+----+        +--+---+--+
        | HOST A  |        | HOST B  |        | HOST C  |        | ROUTER  |
        +----+----+        +----+----+        +----+----+        +----+----+
```

Uređaji koji su spojeni na lokalnu mrežu zovu se domaćini (*eng. hosts*) i pristupaju Internetu preko usmjernika (*eng. router*). Hostovi mogu biti povezani žičano ili bežično s usmjernikom. Podatci koji se prenose preko veze prenose se u paketima. Paketi se sastoje od zaglavlja (*eng. header*) i sadržaja odnosno podataka (*eng. payload*). Kada računalo šalje podatke nekom drugom računalu, mora se poštivati slijed protokola kako to napraviti. Takav slijed protokola definira se mrežnim stogom (*eng. Network Stack*) koji je apstraktni opis arhitekture mreže. Primjer mrežnog stoga je OSI model:

```
        +-------------------------------------------------------------------------------+
        | Aplikacijski sloj (eng. Application Layer)                                    |
        | - usluge aplikacijama                                                         |
        | - protokoli HTTP, FTP, DNS ...                                                |
        +-------------------------------------------------------------------------------+
        | Prezentacijski sloj (eng. Presentation Layer)                                 |
        | - kodiranje podataka (ASCII, UTF, ...)                                        |
        | - protokoli SSL/TLS, MIME                                                     |
        +-------------------------------------------------------------------------------+
        | Sloj sesije (eng. Session Layer)                                              |
        | - uspostava veze i sinkronizacija između krajnjih korisnika (primjerice,      |
        |   sinkronizacija tona i videa)                                                |
        +-------------------------------------------------------------------------------+
        | Transportni sloj (eng. Transport Layer)                                       |
        | - komunikacija između procesa na dva računala u mreži                         |
        | - protokoli TCP, UDP                                                          |
        +-------------------------------------------------------------------------------+
        | Mrežni sloj (eng. Network Layer)                                              |
        | - komunikacija između dva ciljna računala u mreži                             |
        | - protokoli IP, ICMP, ARP ...                                                 |
        +-------------------------------------------------------------------------------+
        | Sloj podatkovne veze (eng. Data Link Layer)                                   |
        | - komunikacija između dva "susjedna" računala (čvora) u mreži                 |
        | - kontrola grešaka koje se događaju na fizičkom sloju                         |
        | - Ethernet standard, Wi-Fi standard ...                                       |
        | - dva podsloja:                                                               |
        |     - LLC (eng. Logical Link Layer) - viši sloj - provjera grešaka,           |
        |       enkapsulacija i dekapsulacija paketa viših slojeva                      |
        |     - MAC (eng. Medium Access Control) - niži sloj - kontrola pristupa mreži, |
        |       rukovanje kolizijama CSMA/CD (Ethernet) i CSMA/CA (Wi-Fi) metodama      |
        +-------------------------------------------------------------------------------+
        | Fizički sloj (eng. Physical Layer)                                            |
        | - definira električna odnosno fizička svojstva medija za prijenos             |
        |   primjerice specifikacije kablova                                            |
        | - definira kako se informacija prenosi preko medija, kakvim električnim       |
        |   signalima i slično                                                          |
        | - Ethernet standard, Wi-Fi standard ...                                       |
        +-------------------------------------------------------------------------------+
```

Kada jedno računalo šalje podatak drugom on se enkapsulira prolazeći od aplikacijskog sloja do fizičkog sloja. Enkapsulirani podatak se zatim odašilje preko medijuma i tako stiže na ciljno računalo gdje se zatim dekapsulira prolazeći od fizičkog sloja do aplikacijskog. Svaki sloj definira nekakvo svoje zaglavlje koje se dodaje/uklanja prilikom enkapsulacije/dekapsulacije. Zaglavlje koje se dodaje/uklanja + podatak višeg sloja naziva se PDU (*eng. Protocol Data Unit*). Za OSI model, PDU-ovi su:

- aplikacijski sloj - podatci
- prezentacijski sloj - podatci
- sloj sesije - podatci
- transportni sloj - segment (TCP) ili datagram (UDP)
- mrežni sloj - paket
- sloj podatkovne veze - okvir
- fizički sloj - bit ili simbol

OSI model je previše apstraktan te se u stvarnosti neki dijelovi stapaju zajedno (radi efikasnosti). Kao realniji model za opis mrežne arhitekture, koristi se TCP/IP model:

```
        +-------------------+
        | Aplikacijski sloj | -> uključuje aplikacijski, prezentacijski i sloj sesije OSI modela
        +-------------------+
        | Transportni sloj  | -> uključuje transportni sloj OSI modela
        +-------------------+
        | Mrežni sloj       | -> uključuje mrežni sloj OSI modela
        +-------------------+
        | Fizički sloj      | -> uključuje sloj podatkovne veze i fizički sloj OSI modela
        +-------------------+
```

## Osnovne naredbe za konfiguraciju mreže

Ako se na računalo spoji mrežna kartica ili nekakav drugi mrežni uređaj, on se neće prikazati u ```/dev``` direktoriju kao što bi se prikazao blok uređaj (tvrdi disk) ili znakovni uređaj (tipkovnica). Razlog je zbog toga što se nad mrežnim uređajima ne pozivaju sistemski pozivi ```read()``` ili ```write()```. Međutim, Linux jezgra ih i dalje registrira i zapisuje u svojoj podatkovnoj strukturi. Ti uređaji se mogu vidjeti kroz ```/sys``` pseudodatotečni sustav naredbom:

```
ls /sys/class/net/
```

Međutim, za općenito rukovanje mrežnim uređajima odnosno njihovim sučeljima koristi se naredba ```ip```. Primjerice, za izlistavanje svih mrežnih sučelja koristi se naredba:

```
ip link
```

Za informacije o dodijeljenim IP adresama na tim sučeljima koristi se naredba:

```
ip addr
```

### Tablica usmjeravanja

Kako bi računalo komuniciralo s drugim računalima u lokalnoj mreži, ali i na Internetu, potrebna mu je tablica usmjeravanja. Ispis trenutačne tablice usmjeravanja može se napraviti naredbom:

```
route -n
```

Primjerice, izlaz ove naredbe je mogao biti ovakav:

```
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.1.1     0.0.0.0         UG    100    0        0 eth0
192.168.1.0     0.0.0.0         255.255.255.0   U     100    0        0 eth0
```

U primjeru su navedena dva pravila rutiranja:
- prvo pravilo govori da se sve pakete s bilo kojom odredišnom adresom (oznaka ```0.0.0.0```, maska ```0.0.0.0``` odnosno ```/0```) šalje na *gateway* (izlazni usmjernik) koji ima IP adresu ```192.168.1.0``` odnosno usmjernik
- drugo pravilo govori da se sve pakete s odredišnom adresom od ```192.168.1.0``` do ```192.168.1.255``` (maska ```255.255.255.0``` odnosno ```/24```) šalje "direktno" odnosno ne koristi se *gateway* -> ovo znači da se odredišni uređaj nalazi u lokalnoj mreži dakle nije potrebna pomoć usmjernika

Zastavica ```U``` označava da je sučelje ```eth0``` aktivno, a zastavica ```G``` da se koristi *gateway*. Stupac "Metric" označava prioritet rute gdje manji broj označava viši prioritet (u slučaju više *gatewaya* izlaznih sučelja). Stupac "Ref" je zastarjeli podatak i ne koristi se više, a stupac "Use" govori koliko je paketa usmjereno tom rutom.

Za prethodni primjer za primjerice odredišnu adresu 192.168.1.40 oba pravila će biti ispunjena. Kako onda Linux jezgra zna gdje poslati paket? Linux jezgra daje prednost "specifičnijem" pravilu odnosno pravilu čija je maska dulja. Dakle, pravilo s maskom ```/24``` imat će prednost nad pravilom s maskom ```/0```.

### Alat za provjeru povezanosti i dostupnosti računala u mreži

Jedan od alata za provjeru dostupnosti računala u mreži je naredba ```ping```. Naredba šalje ICMP pakete ciljanom računalu IPv4 adresom i očekuje odgovore, primjerice:

```
ping [IPv4 adresa|ime domene]
```

Ako se želi provjeriti dostupnost IPv6 adresom (ICMP6 protokolom) to se može napraviti:

```
ping -6 [IPv4 adresa|ime domene]
```

### DNS postavke

DNS (*eng. Domain Name Service*) je protokol aplikacijskog sloja koji služi za razlučivanje IP adrese danog imena. Konfiguracija u datoteci ```/etc/nsswitch.conf``` definira plan razlučivanja. U toj datoteci je obično postavljena datoteka ```/etc/hosts``` kao prva datoteka za pregled za razlučivanje imena. Tipično se u njoj može nalaziti sljedeći sadržaj:

```
127.0.0.1	localhost
127.0.1.1	[ime računala]
::1		ip6-[ime računala]
```

Tradicionalno, ako se nije uspjela razlučiti adresa u prethodnoj datoteci, slat će se upiti DNS poslužiteljima definirani u datoteci ```/etc/resolv.conf```.

Moderni sustavi koriste ```systemd-resolved```, ```dnsmasq```, ```nscd``` i slične servise kako bi cacheirali DNS odgovore.

#### Multicast DNS (mDNS)

Multicast DNS je protokol koji omogućuje razlučivanje imena računala UP adrese unutar lokalne mreže bez potrebe za centralnim DNS poslužiteljem. Računalo koje želi saznati IP adresu nekog drugog računala u lokalnoj mreži šalje *multicast* upit na ime ciljnog računala s nadodanim sufiksom *.local* (primjerice ```ping mycomputer.local```). Razrješitelj imena definiran u ```/etc/nsswitch.conf``` (primjerice ```systemd-resolved```) šalje *multicast* upit u lokalnu mrežu, a ciljno računalo odgovara sa svojom IP adresom. Kako bi ciljno računalo moglo odgovarati na mDNS upite mora imati instaliran i pokrenut ```avahi``` servis (ili neki drugi mDNS servis).

### Provjera stanja portova

Za provjeru stanja portova može se koristiti naredba ```netstat```. Provjera stanja TCP konekcija može se napraviti naredbom:

```
netstat -nt
```

Provjera stanja UDP konekcija može se napraviti naredbom ```netstat -nu```, a Unix *socketa* ```netstat -nx```. Ispis svih TCP portova koji su u stanju prisluškivanja može se napraviti naredbom:

```
netstat -ntl
```

Procesi koji su pokretani s *root* ovlastima mogu prisluškivati od porta 1 do porta 1023.

### DHCP postavke

Gotovo svaka moderna lokalna mreža ima DHCP poslužitelj koji dinamički dodjeljuje IP adrese spojenim uređajima. Tu ulogu najčešće ima usmjernik. Ako se korist servis *NetworkManager*, on će automatski pozvati program ```dhclient``` kada na sučelje koje se spoji na neku mrežu. 

### Translacija adresa i portova

Translacija mrežnih adresa (*eng. Network Address Translation*) je tehnika mapiranja jedne IP adrese na drugu IP adresu. Ova tehnika se naročito koristi kod IPv4 adresa, razdvajajući privatne adrese (adrese na lokalnoj mreži) od javnih adresa (adrese na Internetu). Razlog korištenja kod IPv4 adresa je što postoji ograničen broj dodjeljivih IPv4 adresa naspram brojem uređaja (koji s vremenom još više raste). Tehnika je također poznata kao i IP maskiranje (*eng. IP masquerading*).

U običnim kućnim mrežama uređaj koji radi NAT je obično usmjernik. Usmjerniku je na javnoj strani (Internet) dodijeljena jedna javna IP adresa. Na privatnoj strani (lokalna mreža) spojeno je više uređaja, svaki sa svojom privatnom IP adresom. Također, usmjernik na privatnoj strani isto ima svoju privatnu adresu.

Ako neki uređaj napravi HTTP zahtjev prema nekom poslužitelju na Internetu, usmjernik će presrest pakete, zamijeniti izvorišnu privatnu IP adresu (IP adresu uređaja) s usmjernikovom javnom IP adresom te proslijediti pakete dalje. Poslužitelj će tada vratiti pakete koji sadrže stranicu s odredišnom IP adresom koja je javna IP adresa usmjernika. Kada paket stigne do usmjernika, on će zamijeniti odredišnu javnu IP adresu s privatnom IP adresom uređaja koji je napravio HTTP zahtjev.

Međutim, kako onda usmjernik zna kome poslat koji paket kada više uređaja na privatnoj strani napravi HTTP zahtjeve? Odgovor se nalazi u portovima. Naime, kada uređaj pošalje HTTP zahtjev poslužitelju na Internetu, na usmjerniku će se zabilježiti informacija o izvorišnoj IP adresi i portu te odredišnoj IP adresi i portu, primjerice:

```
        +----SRC-IP----+-SRC-PORT-+----DEST-IP----+-DEST-PORT-+
        | 192.168.1.2  | 34000    | 203.0.113.110 | 80        |
```

Usmjernik zatim radi translaciju izvorišne IP adrese i porta te prosljeđuje promet dalje na Internet:

```
        +----SRC-IP----+-SRC-PORT-+----DEST-IP----+-DEST-PORT-+
        | 192.168.1.2  | 34000    | 203.0.113.110 | 80        |
        |              |          |               |           |
        |       |      |     |    |               |           |
        |       |      |     |    |               |           |
        |       V      |     V    |               |           |
        |              |          |               |           |
        | 203.0.113.40 | 52000    | 203.0.113.110 | 80        |
```

Kada poslužitelj pošalje odgovor na zahtjev, nad prometom će se radit translacija odredišne IP adrese i porta:

```
        +-----SRC-IP----+-SRC-PORT-+---DEST-IP----+-DEST-PORT-+
        | 203.0.113.110 | 80       | 203.0.113.40 | 52000     |
        |               |          |              |           |
        |               |          |       |      |     |     |
        |               |          |       |      |     |     |
        |               |          |       V      |     V     |
        |               |          |              |           |
        | 203.0.113.110 | 80       | 192.168.1.2  | 34000     |
```

Promet se zatim prosljeđuje na izvorni uređaj:

```
        +-----SRC-IP----+-SRC-PORT-+---DEST-IP----+-DEST-PORT-+
        | 203.0.113.110 | 80       | 192.168.1.2  | 34000     |
```

Usmjernik mora raditi translaciju porta kako bi razlikovao sesije odnosno uređaje koji koriste isti izvorišni/odredišni port.

Uz podatke za translaciju IP adrese i porta, usmjernik pamti je li korišteni protokol TCP ili UDP (TCP i UDP su različite sesije). TCP je protokol gdje se uspostavlja veza prije slanja i prekida veza poslije slanja te je zbog toga pouzdan, ali sporiji. UDP ne uspostavlja vezu i zbog toga je brz i nepouzdan.
