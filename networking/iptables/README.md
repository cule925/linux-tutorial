# IPTABLES

Alat [*iptables*](https://wiki.archlinux.org/title/iptables) je alat naredbenog retka koji služi za konfiguraciju vatrozida (*eng firewall*). Alat *iptables* se koristi za upravljanje paketima koje koriste IPv4 protokol dok se alat *ip6tables* koristi za upravljanje paketima koje koriste IPv6 protokol. Alat *iptables* koristi sučelje *Netfilter* projekta, dio Linux jezgre koji pruža upravljanje mrežnim operacijama.

## Koncept

Alat *iptables* se može koristiti za prosljeđivanje, filtriranje ili modificiranje mrežnih paketa. Kod za filtriranje paketa već je ugrađen u Linux jezgru i organiziran je u kolekciju tablica. Tablice su napravljene od lanaca koji sadrže pravila koje se čitaju po redu. Kada mrežni paket dođe ili želi otići s Linux sustava provjeravaju se određeni lanci odnosne provjeravaju se pravila u lancu. Čim se naiđe na pravilo koje se odnosi na paket primjenjuje se to pravilo, a ako ne postoji ni jedno pravilo za taj paket, gleda se opća politika lanca. Postoji pet tablica koje imaju svoje lance:

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

## Tablica *filter*

Izlistavanje trenutnih pravila za tablicu filter može se napraviti naredbom:

```
iptables -L -t filter
```

Tablica *filter* je uobičajena tablica pa se argument *-t filter* može izostaviti. Kako bi ispis bio čitljiviji može se koristiti naredba sa sljedećim brojem znakova:

```
iptables -L -vxn --line-numbers
```

Resetiranje *filter* tablice može se napraviti naredbom:

```
iptables -F 
iptables -X
```

Prazni argumenti *-F* i *-X* brišu sve uobičajene i sve neuobičajene (definirane od korisnika) lance u tablici *filter*.

Nad svakim paketom pri svakom izvršenom pravilu može se izvršiti jedna od sljedećih operacija odnosno ciljeva:

- *ACCEPT* - paket se prihvaća
- *DROP* - paket se odbija bez da se pošalje poruka da je paket odbijen
- *REJECT* - paket se odbije i pošalje se poruka da je paket odbijen
- *RETURN* - ispitivanje pravila vraća se iz trenutnog lanca (korisnički lanac) u lanac koji je pozvao trenutni lanac

Svaki lanac ima i svoju uobičajenu politiku odnosno što će se dogoditi ako ni jedno pravilo ne odgovara pridošlom paketu, te politike mogu biti: *ACCEPT* i *DROP*.

Osnovni argumenti za upravljanje pravila:

- *-A [lanac]* - dodaje pravilo lancu (*eng. append*)
- *-I [lanac] [pozicija]* - dodaj pravilo u lancu na točnoj poziciji (*eng. insert*)
- *-D [lanac] [pozicija]* - makni pravilo koje pripada lancu s točne pozicije (*eng. delete*)
- *-F [lanac]* - ukloni sva pravila na lancu (*eng. flush*)
- *-X [lanac]* - ukloni korisnički definiran lanac (*eng. delete chain*)
- *-N [lanac]* - stvori korisnički definiran lanac (*eng. new*)
- *-L [lanac]* - izlistaj sva pravila u lancu (*eng. list*)
- *-P [lanac] [cilj]* - postavi uobičajenu operaciju lancu (ako ni jedno pravilo ne odgovara za paket)

Osnovni argumenti za pravilo:

- *-p [tcp/udp]* - koji protocol paket u pitanju koristi (*eng. protocol*)
- *-s [IP adresa[/maska]]* - izvorišna IP adresa (*eng. source*)
- *-d [IP adresa[/maska]]* - odredišna IP adresa (*eng. destination*)
- *-g [lanac]* - skoči na lanac (*eng. go to*)
- *-j [ACCEPT|REJECT|DROP|RETURN]* - specificira što napraviti s paketom koji odgovara svim kriterijima u pravilu
- *--sport [port]* - izvorišni port, ispred ovog argumenta **mora** postojati argument *-p [tcp|udp]*
- *--dport [port]* - odredišni port, ispred ovog argumenta **mora** postojati argument *-p [tcp|udp]*

Više informacija se može saznati uz pomoć naredbe ```iptables -h```.

### Primjer postavljanje pravila u tablici *filter*

Recimo da imamo Arch Linux distribuciju koja pokreće *nginx* koji prisluškuje na portu 80. Izlistajmo iptables pravila za tablicu *filter* uz pomoć naredbe:

```
iptables -L -vxn --line-numbers
```

Ako nitko nije prčkao po *iptables* lancima, izlaz bi trebao biti sličan ovome:

```
Chain INPUT (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination

Chain FORWARD (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination

Chain OUTPUT (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination
```

Za početak, dobro bi bilo promijeniti politiku lanca u DROP ali mu i dodati pravilo da propušta sav promet naredbama:

```
iptables -A INPUT -j ACCEPT
iptables -P INPUT DROP
```

Ispis svih lanaca naredbom ```iptables -L -vxn --line-numbers``` bi trebao biti sličan ovome:

```
Chain INPUT (policy DROP ...)
num      pkts      bytes target     prot opt in     out     source               destination
1        ...       ...   ACCEPT     0    --  *      *       0.0.0.0/0            0.0.0.0/0

Chain FORWARD (policy ACCEPT 0 packets, 0 bytes)
num      pkts      bytes target     prot opt in     out     source               destination

Chain OUTPUT (policy ACCEPT 3246 packets, 269641 bytes)
num      pkts      bytes target     prot opt in     out     source               destination
```

Sada, ako se želi izričito odbiti sav promet usmjeren na port 80 s bilo koje IP adrese, potrebno je ispred pravila koje prihvaća bilo koji promet dodati pravilo koje odbija promet usmjeren k portu 80:

```
iptables -I INPUT 1 -p tcp --dport 80 -j REJECT
```

Sada bi ispis svih lanaca trebao biti:

```
Chain INPUT (policy DROP ...)
num      pkts      bytes target     prot opt in     out     source               destination
1         ...        ... REJECT     6    --  *      *       0.0.0.0/0            0.0.0.0/0            tcp dpt:80 reject-with icmp-port-unreachable
2         ...        ... ACCEPT     0    --  *      *       0.0.0.0/0            0.0.0.0/0

Chain FORWARD (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination

Chain OUTPUT (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination
```

Potrebno je uočiti da je pravilo blokiranja prometa prema portu 80 **prije** pravila propuštanja bilo kakvog prometa. Općenito vrijedi da su specifičnija pravila prva po redoslijedu, a zatim slijede generalna pravila. Ako se želi obrisati pravilo za blokiranje prometa prema portu 80, to se u ovom slučaju može napraviti naredbom ```iptables -D INPUT 1```. Resetiranje lanca radi se naredbama:

```
iptables -P INPUT ACCEPT
iptables -F INPUT
```

Ponovnim izvršavanjem naredbe za listanje lanaca u tablici *filter* bi trebao dati sljedeće rezultate:

```
Chain INPUT (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination

Chain FORWARD (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination

Chain OUTPUT (policy ACCEPT ...)
num      pkts      bytes target     prot opt in     out     source               destination
```

