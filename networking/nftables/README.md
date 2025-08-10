# NFTABLES

Radni okvir [*nftables*](https://wiki.nftables.org/wiki-nftables/index.php/What_is_nftables%3F) je moderni radni okvir Linux jezgre koji služi za klasifikaciju mrežnih paketa. Objedinjuje *iptables*, *ip6tables*, *arptables* i *ebtables* infrastrukturu u jedan (i više od toga). Radni okvir je dostupan u Linux jezgri od verzije 3.13.

Alat za administraciju *nftables* radnog okvira je ```nft```. Instalacija alata na [Arch Linuxu](https://wiki.archlinux.org/title/Nftables) može se napraviti naredbom:

```
sudo pacman -S nftables
```

Ako se želi spremiti trenutačna konfiguracija, to se može napraviti sljedećom naredbom:

```
nft list ruleset | tee [ime datoteke]
```

Ako se želi učitati konfiguracija pri svakom pokretanju, potrebno je omogućiti servis ```nftables``` koji će kada se pokrene učitati konfiguraciju u datoteci ```/etc/nftables.conf```. Dakle, prvo je potrebno spremiti konfiguraciju u datoteku:

```
nft list ruleset | tee /etc/nftables.conf
```

Ako se želi odmah učitati konfiguracija to je moguće pokretanjem servisa naredbom ```systemctl start nftables```, a ako se želi učitati konfiguracija pri svakom pokretanju potrebno je omogućiti servis naredbom ```systemctl enable nftables```.

Ako se želi ukloniti trenutačna konfiguracija, to se može napraviti naredbom:

```
nft flush ruleset
```

## Tablice

Tablica (*eng. Table*) u kontekstu *nftablesa* je struktura za grupiranje pravila i lanaca. Svaka tablica podupire pravila i lance za sljedeće vrste filtriranja:

- *ip* -> za pravila koja se odnose na IPv4 promet
- *ip6* -> za pravila koja se odnose na IPv6 promet
- *inet* -> za pravila koja se odnose na IPv4 i IPv6 promet
- *arp* -> za pravila koja se odnose na ARP promet
- *bridge* -> za pravila koja se odnose na promet preko mrežnog mosta
- *netdev* -> za pravila koja se odnose na mrežni promet na mrežnom sučelju (niska razina)

### Osnovne operacije nad tablicama

Stvaranje tablice može se napraviti naredbom:

```
nft add table [ip|ip6|inet|arp|bridge|netdev] [ime tablice]
```

Izlistavanje postojećih tablica može se napraviti naredbom:

```
nft list tables
```

Brisanje tablice može se napraviti naredbom:

```
nft delete table [ip|ip6|inet|arp|bridge|netdev] [ime tablice]
```

Brisanje svih lanaca odnosno pravila iz tablice može se napraviti naredbom:

```
nft flush table [ip|ip6|inet|arp|bridge|netdev] [ime tablice]
```

## Lanci

Lanac (*eng. Chain*) je niz pravila i dodaje se u tablicu. Postoje dvije vrste lanaca:

- bazni lanac - povezani s *hookovima* Linux jezgre
- obični lanac - pomoćni lanac na koje se može "skočiti" s baznog lanca, dodatna pravila stavljena u zasebni lanac radi organizacije

Točka obrade mrežnog paketa u Linux jezgri naziva se *hook*. Primjerice, u Linux jezgri postoji 5 glavnih *hookova*:

- *prerouting* - obrada paketa prije nego li se donese odluka o rutiranju
- *input* - obrada paketa koji je namijenjen lokalnom računalu
- *forward* - obrada paketa koji se rutira kroz lokalno računalo
- *output* - obrada paketa koji je stvoren lokalno i namijenjen je za van lokalnog računala
- *postrouting* - obrada paketa nakon što se donese odluka o rutiranju
- *ingress* - obrada paketa koji se tek pojavio na sučelju, obrađuje se prije nego li se pozovu bilo kakvi *handleri* za mrežni sloj
- *egress* - obrada paketa prije nego li se pošalje na sučelje, obrađuje se nakon što su se pozvali bilo kakvi *handleri* za mrežni sloj

Tablice vrste:

- *ip*, *ip6*, *inet* i *bridge* prihvaćaju lance s *hookovima*: *prerouting*, *input*, *forward*, *output*, *postrouting*
- *arp* prihvaća lance s *hookovima*: *input*, *output*
- *netdev* prihvaća lance s *hookovima*: *ingress*, *egress*

### Bazni lanci

Tijekom dodavanja baznog lanca, tri parametra su bitna:

- vrsta lanca - *filter*, *nat* ili *route*
- vrsta tablice kojoj pripada - *ip*, *ip6*, *inet*, *arp*, *bridge*
- na koji *hook* se lanac povezuje - *prerouting*, *input*, *forward*, *output*, *postrouting*, *ingress*, *egress*

Ne može se bilo koji tip lance umetnuti u bilo kakvu tablicu i na bilo koji *hook*, vrijede sljedeća pravila za tablice:

- *arp*, *bridge* - prihvaćaju samo lance vrste *filter*
- *ip*, *ip6*, *inet* - prihvaćaju lance vrste:
	- *filter* - s *hookovima*: *prerouting*, *input*, *forward*, *output*, *postrouting*
	- *nat* - s *hookovima*: *prerouting*, *input*, *output*, *output*
	- *route* - s *hookom*: *output*

### Politika lanca

Ako ni jedno pravilo u lancu ne zadovolji paket koji se obrađuje, lanac može imati jednu od dvije politike:

- *accept* - prihvati paket ako ni jedno pravilo nije zadovoljeno (zadana postavka), omogućuje propagiranje na više lanaca na istom *hooku*
- *drop* - odbaci paket ako ni jedno pravilo nije zadovoljeno u trenutačnom lancu

### Prioritet lanca

Definira koji lanci na istom *hooku* imaju prednost izvršavanja. Vrijednost je 4-bajtni cijeli broj gdje manji broj označava veći prioritet. Postoje standardni makroi koje označavaju vrijednosti:

- raw - za sve *hookove* tablica *ip*, vrijednost -300
- mangle - za sve *hookove*, vrijednost -150
- dstnat - za *prerouting* *hook*, vrijednost -100
- filter - za sve *hookove*, vrijednost 0
- security - za sve *hookove*, vrijednost 50
- srcnat - za *postrouting* *hook*, vrijednost 100
- dstnat - za *prerouting* *hook*, vrijednost -300
- filter - za sve *hookove*, vrijednost -200
- out - za *output* *hook*, vrijednost 100
- srcnat - za *postroutinh* *hook*, vrijednost 300

### Osnovne operacije nad baznim lancima

Stvaranje baznog lanca može se napraviti naredbom:

```
nft add chain [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca] '{ type [filter|nat|route] hook [prerouting|input|forward|output|postrouting] priority [cijeli broj]; policy [accept|drop]}'
```

Izlistavanje postojećih lanaca može se napraviti naredbom:

```
nft list chains
```

Brisanje lanca može se napraviti naredbom:

```
nft delete chain [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca]
```

Brisanje svih pravila iz lanca može se napraviti naredbom:

```
nft flush chain [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca]
```

## Skupovi

Skupovi (*eng. Sets*) omogućuju grupiranje više elemenata kao što su primjerice IP adrese, MAC adrese, portove i slično. Mogu se referencirati u pravilima.

## Pravila

Pravila (*eng. Chains*) su instrukcije što napraviti s paketima koji zadovoljavaju određene uvjete.

### Osnovne operacije nad pravilima

Stvaranje pravila i dodavanje na kraj lanca može se napraviti naredbom:

```
nft add rule [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca] [izraz]
```

Stvaranje pravila s handleom i dodavanje na kraj lanca može se napraviti naredbom:

```
nft add rule [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca] handle [handle] [izraz]
```

Stvaranje pravila i dodavanje na početak lanca može se napraviti naredbom:

```
nft insert rule [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca] [izraz]
```

Stvaranje pravila s handleom i dodavanje na početak lanca može se napraviti naredbom:

```
nft insert rule [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca] handle [handle] [izraz]
```
Izlistavanje postojećih pravila u lancu može se napraviti naredbom:

```
nft list chain [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca]
```

Prethodna naredba će i izlistat *handleove* po kojima će se moći brisati pravila.

Brisanje pravila u lancu po *handleu* može se napraviti naredbom:

```
nft delete rule [ip|ip6|inet|arp|bridge|netdev] [ime tablice] [ime lanca] handle [handle]
```

## Osnovni primjeri s izrazima

Izraz sadržava uvjete i akcije koje će se izvršiti ako je uvjet zadovoljen. Akcije mogu biti:

- *accept* - prihvati paket
- *drop* - ignoriraj paket
- *jump* - skoči na novi lanac s povratom
- *goto* - skoči na novi lanac bez povrata
- *continue* - nastavi dalje po lancu (zadano za svako pravilo bez definirane akcije)
- *return* - prekini isprobavanje pravila po lancu i vrati se na prethodni lanac ili ako je trenutačni lanac bazni izvrši politiku lanca
- *queue* - šalje paket u korisnički prostor za neku aplikaciju koja čeka obradu

Uvjeta kojih se zadaju ima mnogo. Više o njima se može naći [ovdje](https://man.archlinux.org/man/nft.8#EXPRESSIONS). Neki od uvjeta koji mogu biti su sljedeći:

```
meta:
  oif <output interface INDEX>
  iif <input interface INDEX>
  oifname <output interface NAME>
  iifname <input interface NAME>

  (oif and iif accept string arguments and are converted to interface indexes)
  (oifname and iifname are more dynamic, but slower because of string matching)

icmp:
  type <icmp type>

icmpv6:
  type <icmpv6 type>

ip:
  protocol <protocol>
  daddr <destination address>
  saddr <source address>

ip6:
  daddr <destination address>
  saddr <source address>

tcp:
  dport <destination port>
  sport <source port>

udp:
  dport <destination port>
  sport <source port>

sctp:
  dport <destination port>
  sport <source port>

ct:
  state <new | established | related | invalid>
```

Nakon svakog primjera potrebno je izbrisati konfiguraciju naredbom ```nft flush ruleset```.

### Primjer vatrozida

#### Primjer 1 - dozvoli TCP promet na portovima 80 i 443

Prvo je potrebno dodati tablicu imena *my_filter* vrste *inet* (IPv4 + IPv6):

```
nft add table inet my_filter
```

Zatim je potrebno dodati lanac imena *my_filter_input* vrste *filter* prioriteta 0 na *hook* *input* s politikom *drop*:

```
nft add chain inet my_filter my_filter_input '{ type filter hook input priority 0; policy drop; }'
```

Nakon izvršavanja prethodne naredbe svi paketi će biti odbačeni, čak i sa sučelja *localhost*. Da bi omogućili sav promet s *localhosta* odnosno *lo* sučelja, potrebno je dodati sljedeće pravilo:

```
nft add rule inet my_filter my_filter_input iif lo accept
```

Sljedeće je potrebno prihvatiti sve pakete koji pripadaju postojećoj TCP konekciji ili su na neki način povezani s postojećom TCP konekcijom.

```
nft add rule inet my_filter my_filter_input ct state established,related accept
```

Za kraj, potrebno je dodati pravilo koje će prihvaćati sve pakete za uspostavu TCP konekcija na portu 80 i 443.

```
nft add rule inet my_filter my_filter_input tcp dport {80,443} ct state new accept
```

**NAPOMENA: Iako je UDP protokol za razliku od TCP protokola protokol bez stanja, nftables koristi praćenje UDP veze na temelju perioda i parametara paketa. Dakle, isto se preporučuje koristiti new, established i related stanja.**

#### Primjer 2 - odbaci sve pakete s podmreže 10.0.40.0/24

Prvo je potrebno dodati tablicu imena *my_filter* vrste *inet* (IPv4 + IPv6):

```
nft add table inet my_filter
```

Zatim je potrebno dodati lanac imena *my_filter_input* vrste *filter* prioriteta 0 na *hook* *input* s politikom *accept*:

```
nft add chain inet my_filter my_filter_input '{ type filter hook input priority 0; policy accept; }'
```

Onda je potrebno dodati lanac imena *my_filter_input* vrste *filter* prioriteta 0 na *hook* *output* s politikom *accept*:

```
nft add chain inet my_filter my_filter_output '{ type filter hook output priority 0; policy accept; }'
```

Nakon izvršavanja prethodnih naredbi potrebno je onemogućiti primanje paketa s podmreže 10.0.40.0/24 i onemogućiti slanje paketa u podmrežu 10.0.40.0/24

```
nft add rule inet my_filter my_filter_input ip saddr 10.0.40.0/24 drop
nft add rule inet my_filter my_filter_output ip daddr 10.0.40.0/24 drop
```

