# IPTABLES

Alat [*iptables*](https://wiki.archlinux.org/title/iptables) je alat naredbenog retka koji služi za konfiguraciju vatrozida (*eng firewall*). Alat *iptables* se koristi za upravljanje paketima koje koriste IPv4 protokol dok se alat *ip6tables* koristi za upravljanje paketima koje koriste IPv6 protokol. Alat *iptables* koristi sučelje *Netfilter* projekta, dio Linux jezgre koji pruža upravljanje mrežnim operacijama.

## Koncept

Alat *iptables* se može koristiti za prosljeđivanje, filtriranje ili modificiranje mrežnih paketa. Kod za filtriranje paketa već je ugrađen u Linux jezgru i organiziran je u kolekciju tablica. Tablice su napravljene od od lanaca koji sadrže pravila koje se čitaju po redu. Kada mrežni paket dođe ili želi otići sa Linux sustava provjeravaju se određeni lanci odnosne provjeravaju se pravila u lancu. Čim se naiđe na pravilo koje se odnosi na paket primjenjuje se to pravilo, a ako ne postoji ni jedno pravilo za taj paket, gleda se opća politika lanca. Postoji pet tablica koje imaju svoje lance:

* filter - ova tablica se uređuje kad se postavlja vatrozid, koristit ćemo je
* nat - ova tablica se postavlja kad se želi postaviti prosljeđivanje adresa i portova, koristit ćemo je
* raw - tablica za specijalizirano modificiranje paketa
* mangle - tablica za specijalizirano modificiranje paketa
* security - tablica za SELinux

Jednostavni tok mrežnih paketa kroz jedan Linux sustav:

```
                               XXXXXXXXXXXXXXXXXX
                             XXX     Network    XXX
                               XXXXXXXXXXXXXXXXXX
                                       +
                                       |
                                       v
 +-------------+              +------------------+
 |table: filter| <---+        | table: nat       |
 |chain: INPUT |     |        | chain: PREROUTING|
 +-----+-------+     |        +--------+---------+
       |             |                 |
       v             |                 v
 [local process]     |           ****************          +--------------+
       |             +---------+ Routing decision +------> |table: filter |
       v                         ****************          |chain: FORWARD|
****************                                           +------+-------+
Routing decision                                                  |
****************                                                  |
       |                                                          |
       v                        ****************                  |
+-------------+       +------>  Routing decision  <---------------+
|table: nat   |       |         ****************
|chain: OUTPUT|       |               +
+-----+-------+       |               |
      |               |               v
      v               |      +-------------------+
+--------------+      |      | table: nat        |
|table: filter | +----+      | chain: POSTROUTING|
|chain: OUTPUT |             +--------+----------+
+--------------+                      |
                                      v
                               XXXXXXXXXXXXXXXXXX
                             XXX    Network     XXX
                               XXXXXXXXXXXXXXXXXX
```

Shema prikazuje korištenje tablica *filter* i *nat* od kojih svaki ima po tri lanca. Primjerice ako koristimo Linux na običnom računalu i neki proces (aplikacija) prisluškuje na portu 30000, paket će prvo doći odozgora iz mreže i proći kroz pravila lanca *PREROUTING* tablice *nat* koji će odlučiti što će s njim. Kako je paket namijenjen samom računalu on će skrenuti lijevo (osim ako lanac *PREROUTING* nije drugačije rekao). Zatim će proći kroz pravila lanca *INPUT* tablice *filter* i na kraju doći do procesa koji prisluškuje na portu 30000.

## Primjer postavljanja postavki za tablicu *filter*

Izlistavanje trenutnih pravila za tablicu filter može se napraviti naredbom:

```
sudo iptables -L -t filter
```

Tablica *filter* je uobičajena tablica pa se argument *-t filter* može izostaviti. Kako bi ispis bio čitljiviji može se koristiti naredba sa sljedećim brojem znakova:

```
sudo iptables -L -vxn --line-numbers
```

Resetiranje *filter* tablice može se napraviti naredbom:

```
sudo iptables -F 
sudo iptables -X
```

Prazni argumenti *-F* i *-X* brišu sve uobičajene i sve neuobičajene (definirane od korisnika) lance u tablici *filter*.
