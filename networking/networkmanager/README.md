# NETWORKMANAGER

Alat [*NetworkManager*](https://networkmanager.dev/) omogućuje olakšano upravljanje Ethernet, Wi-Fi i mobilnim mrežnim sučeljima. Nudi potporu za većinu sigurnosnih protokola, primjerice bežični WPA, WPA2 (Personal i Enterprise), WPA3 (Personal i Enterprise), žičani 802.1x, MACsec i VPN-ove.

Kako bi se moglo upravljati mrežnim postavkama, potrebno je u pozadini imati pokrenut servis *NetworkManager*. Servisu je potrebno omogućiti pokretanje pri svakom pokretanju mikroračunala naredbom:

```
sudo systemctl enable NetworkManager
```

Onemogućivanje servisa radi se naredbom ```sudo systemctl disable NetworkManager```.

Trenutačno pokretanje servisa radi se naredbom:

```
sudo systemctl start NetworkManager
```

Zaustavljanje rada servisa se radi naredbom ```sudo systemctl start NetworkManager```, a ponovno pokretanje rada servisa naredbom ```sudo systemctl start NetworkManager```.

## Konfiguracijske datoteke

Iako *NetworkManager* servis ima zadanu konfiguracijsku datoteku */etc/NetworkManager/NetworkManager.conf*, vlastite konfiguracije se mogu dodavati u direktoriju */etc/NetworkManager/conf.d*. Nadalje, neki paketi mogu dodavati svoje konfiguracije u direktoriju */usr/lib/NetworkManager/conf.d* ili */run/NetworkManager/conf.d*. Redoslijed parsiranja konfiguracijskih datoteka je sljedeći:

- datoteke u */usr/lib/NetworkManager/conf.d*
- datoteke u */run/NetworkManager/conf.d*
- datoteka */etc/NetworkManager/NetworkManager.conf*
- datoteke u */etc/NetworkManager/conf.d*

Servis *NetworkManager* za svaki žičani Ethernet port namješta zadane postavke, primjerice DHCPv4 autokonfiguraciju ili IPv6 autokonfiguraciju što u poslužiteljskim okolinama možda i nije poželjno. Ove konekcije se mogu izlistati naredbom ```nmcli connection show``` te imaju naziv *Wired connection 1*, *Wired connection 2* i slično, ovisno koliko Ethernet sučelja postoji na računalu. Kako bi se to onemogućilo, u */etc/NetworkManager/NetworkManager.conf* datoteci je potrebno dodati:

```
[main]
no-auto-default=*
```

Ovo će onemogućiti stvaranje zadanih konfiguracija za sva Ethernet sučelja (oznaka *). Oznaka [sekcije](https://networkmanager.pages.freedesktop.org/NetworkManager/NetworkManager/NetworkManager.conf.html) *main* označava da se sljedeće postavke odnose na općenite postavke *NetworkManagera*.

Također, *NetworkManager* aktivira mrežno Ethernet mrežno sučelje samo ako postoji fizička konekcija kabelom na drugo Ethernet sučelje (odnosno *carrier*). Ovo možda nije pogodno za neke aplikacije koje zahtijevaju da sučelje bude odmah aktivno pri pokretanju računala. Dakle, ako se želi onemogućiti čekanje na fizičku konekciju i odmah omogućiti Ethernet sučelje, potrebno je u datoteku */etc/NetworkManager/NetworkManager.conf* dodati:

```
[main]
ignore-carrier=*
```

Ovo će onemogućiti čekanje na fizičku konekciju za sva Ethernet sučelja (oznaka *) i *NetworkManager* će ih odmah omogućiti. Oznaka [sekcije](https://networkmanager.pages.freedesktop.org/NetworkManager/NetworkManager/NetworkManager.conf.html) *main* označava da se sljedeće postavke odnose na općenite postavke.

Općenito, nakon uređivanja bilo kojih konfiguracijskih datoteka *NetworkManager* servisa, potrebno ih je ponovno učitati naredbom:

```
systemctl reload NetworkManager
```

### Isključivanje upravljanja sučeljem

Uobičajeno, *NetworkManager* servis upravlja svim mrežnim sustavima na sustavu. Ako se želi trenutačno isključiti pojedino mrežno sučelje iz upravljanja *NetworkManager* servisom, potrebno je izvršiti naredbu:

```
nmcli device set enp1s0 managed no
```

U slučaju da se ova postavka želi trajno primijenit, potrebno je u */etc/NetworkManager/NetworkManager.conf* datoteci ili u vlastitu *.conf* datoteku u direktoriju */etc/NetworkManager/conf.d* dodati:

```
[device-<ime sučelja>-unmanage]
match-device=interface-name:<ime sučelja>
managed=0
```

Naravno, potrebno je opet učitati konfiguracijske datoteke naredbom ```systemctl reload NetworkManager```.

## Osnovno korištenje

Za osnovno korištenje *NetworkManager* servisa koristi se alat *nmcli*. Neke osnovne naredbe su:

- ```sudo nmcli device``` - izlistaj sva mrežna sučelja
- ```sudo nmcli radio wifi on``` - uključi sve Wi-Fi radije
- ```sudo nmcli radio wifi off``` - isključi sve Wi-Fi radije
- ```sudo nmcli device wifi list``` - izlistaj skenirane Wi-Fi mreže
- ```sudo nmcli device wifi connect [SSID mreže ili BSSID mreže] password [zaporka]``` - spoji se na ciljanu Wi-Fi mrežu primarnim mrežnim sučeljem
- ```sudo nmcli device wifi connect [SSID mreže ili BSSID mreže] password [zaporka] ifname [bežično mrežno sučelje] name [ime profila konekcije koji će se stvoriti]``` - spoji se na ciljanu Wi-Fi mrežu ciljanim sučeljem
- ```sudo nmcli device disconnect ifname [mrežno sučelje]``` - odspoji mrežno sučelje s mreže
- ```sudo nmcli connection show``` - pokaži sve profile konekcije
- ```sudo nmcli connection up [ime profila konekcije ili UUID profila konekcije]``` - poveži se na mrežu koristeći profil konekcije
- ```sudo nmcli connection down [ime profila konekcije ili UUID profila konekcije]``` - odspoji se s mreže koristeći profil konekcije

### Profili konekcije

Profili konekcije su konfiguracije kojima upravlja *NetworkManager* i one se spremaju na lokaciji */etc/NetworkManager/system-connections* osim automatski stvorenih žičanih Ethernet profila konekcija (*Wired connection 1*, *Wired connection 2*, ...). Profili konekcije se mogu uređivati interaktivno naredbom:

```
sudo nmcli connection edit [ime profila konekcije ili UUID profila konekcije]
```

Ako se želi točno urediti neka svojstva profila bez interaktivnog načina rada, to se može napraviti naredbom:

```
sudo nmcli connection modify 
```

Moguće je stvoriti i vlastitu konfiguraciju profila konekcije naredbom:

```
sudo nmcli connection add [argumenti]
```

Korištenjem ```nmcli device connection add ...``` ili ```nmcli device wifi connect ...``` servis *NetworkManager* automatski stvara datoteku profila konekcije u direktoriju */etc/NetworkManager/system-connections*.

Isto tako, moguće ih je i ukloniti naredbom:

```
sudo nmcli connection delete [ime profila konekcije ili UUID profila konekcije]
```

Konfiguracije profila konekcija je moguće stvoriti i na drugi način: izravno stvaranje datoteka oblika *[ime profila konekcije].nmconnection* i popunjavanje odgovarajućih sekcija s konfiguracijama. **Ovdje treba bit vrlo oprezan pri konfiguraciji jer se zbog malih grešaka NetworkManager servis može ponašati nepredviđeno.**

#### Ručno postavljanje IP adresa DNS poslužitelja uz DHCP

Kada je značajka *ipv4.method* postavljena na *auto*, NetworkManager šalje svim uređajima na mreži DHCP zahtjev kao klijent gdje će DHCP poslužitelj (najčešće usmjernik) odgovoriti s par informacija uključujući možda i IP adrese DNS poslužitelja. Ako odgovor sadrži IP adrese DNS poslužitelja, NetworkManager će te adrese zapisati u datoteku */etc/resolv.conf*.

Ako se žele ignorirati IP adrese DNS poslužitelja dobivene DHCP odgovorom te postaviti vlastite IP adrese DNS poslužitelja, to se može naredbom:

```
sudo nmcli connection modify [ime profila konekcije ili UUID profila konekcije] ipv4.dns "[IP adrese DNS poslužitelja]" ipv4.ignore-auto-dns "yes"
```

Ako se nabraja više IP adresa DNS poslužitelja potrebno ih je odvojiti zarezima (primjerice "8.8.8.8,8.8.4.4"). Nakon izvršenja naredbe, potrebno je deaktivirati i ponovno aktivirati profil konekcije:

```
sudo nmcli connection down [ime profila konekcije ili UUID profila konekcije]
sudo nmcli connection up [ime profila konekcije ili UUID profila konekcije]
```

NetworkManager će zapisati nove IP adrese DNS poslužitelja u */etc/resolv.conf*. Vraćanje inicijalne postavke radi se naredbom: ```sudo nmcli connection modify [ime profila konekcije ili UUID profila konekcije] ipv4.dns "" ipv4.ignore-auto-dns "no"```.

Više o IPv4 postavkama se može naći [ovdje](https://networkmanager.dev/docs/api/latest/settings-ipv4.html).

#### Ručno postavljanje IP adrese računala, subnet maske mreže i IP adrese gatewaya uz DHCP

Opet, kada je značajka *ipv4.method* postavljena na *auto*, NetworkManager šalje svim uređajima na mreži DHCP zahtjev gdje odgovor na taj zahtjev sadrži i dodijeljenu IP adresu, subnet masku mreže i IP adresu gatewaya. Moguće je ignorirati te povratne vrijednosti postavljajući vlastite naredbom:

```
sudo nmcli connection modify [ime profila konekcije ili UUID profila konekcije] ipv4.addresses "[IP adresa/subnet maska mreže]" ipv4.gateway "[IP adrese gatewaya]"
```

Opet je potrebno deaktivirati i aktivirati profil konekcije kao u prethodnom primjeru.

Kada NetworkManager primi DHCP odgovor ignorirat će se dodijeljenu IP adresu i IP adresu gatewaya te će se koristiti korisničko definirane vrijednosti. Vraćanje inicijalnih postavki radi se naredbom: ```sudo nmcli connection modify [ime profila konekcije ili UUID profila konekcije] ipv4.addresses "" ipv4.gateway ""```.

#### Ručne postavke bez DHCP-a

Ako se želi u potpunosti onemogućiti NetworkManageru da šalje DHCP zahtjeve to je moguće postavljanjem značajke *ipv4.method* u *manual*. Minimalne postavke koje u tom slučaju treba postaviti kako bi računalo normalno funkcioniralo su IP adresa računala i subnet maska mreže, IP adresa gatewaya i IP adresa DNS poslužitelja. Dakle naredba ima oblik:

```
sudo nmcli connection modify [ime profila konekcije ili UUID profila konekcije] ipv4.addresses "[IP adresa/subnet maska mreže]" ipv4.gateway "[IP adrese gatewaya]" ipv4.dns "[IP adrese DNS poslužitelja]" ipv4.method manual
```

Kao i u prethodnim primjerima, potrebno je deaktivirati i ponovno aktivirati profil konekcije.

Vraćanje inicijalnih postavki radi se naredbom ```sudo nmcli connection modify [ime profila konekcije ili UUID profila konekcije] ipv4.addresses "" ipv4.gateway "" ipv4.dns "" ipv4.method auto```.

### Konfiguracija računala kao usmjernika

Prije konfiguracije, potrebno je instalirati [```dnsmasq``` DHCP i DNS poslužitelj](https://wiki.archlinux.org/title/NetworkManager#dnsmasq) i postavit ga kao temeljni DHCP i DNS poslužitelj NetworkManageru. Instalacija se na Arch Linuxu izvršava naredbom:

```
sudo pacman -S dnsmasq
```

Za Debian Linux naredba za instalaciju je:

```
sudo apt install dnsmasq
```

Ako servis *dnsmasq* nije zaustavljen ili onemogućen, potrebno je to napraviti naredbama:

```
sudo systemctl stop dnsmasq
sudo systemctl disable dnsmasq
```

Postavljanje *dnsmasq* kao DHCP i DNS poslužitelja NetworkManageru radi se tako što se stvori datoteka ```/etc/NetworkManager/conf.d/dns.conf``` i u nju se postavi sadržaj:

```
[main]
dns=dnsmasq
```

Ponovno učitavanje NetworkManager konfiguracije radi se naredbom:

```
sudo nmcli general reload
```

U slučaju da je računalo potrebno konfigurirati kao usmjernik gdje bi se usmjeravao promet s jednog sučelja na trenutačni *gateway*, to bi se moglo napraviti na sljedeći način izvršavajući naredbu:

```
nmcli connection add type ethernet ifname [ime Ethernet sučelja] con-name [ime konekcije] ipv4.method shared
```

DHCP poslužitelj će dodijeliti priključenom uređaju jednu od adresa iz podmreže 10.42.x.0/24. Ako bi se želio koristiti vlastiti raspon adresa, to se može napraviti naredbom:

```
nmcli connection modify [ime konekcije] ipv4.addresses [adresa koja će se dodijeliti sučelju/maska]
```

Ako se želi automatski pokrenuti profil kada se fizički Ethernet sučelje spoji:

```
nmcli connection modify [ime konekcije] connection.autoconnect yes
```

Kao jedna naredba:

```
nmcli connection add type ethernet ifname [ime Ethernet sučelja] con-name [ime konekcije] ipv4.method shared ipv4.addresses [adresa/maska] connection.autoconnect yes
```

Primjena profila radi se naredbom:

```
nmcli connection up [ime konekcije]
```

Ono što se zapravo dogodi (u većini distribucija) je sljedeće:
- dodjeljuje se statička IP adresa ciljanom sučelju
- započinje se instanca *dnsmasq* DHCP i DNS poslužitelja za ciljano sučelje
- omogućuje se IP prosljeđivanje u Linux jezgri (naredba ```sysctl -w  net.ipv4.ip_forward=1```)
- konfigurira se uz pomoć *iptables* opcija maskiranja (*eng. masquerading*) za NAT (*eng. Network Address Translation*) ili novije *nftables*

Prilikom spajanja uređaja na Ethernet priključak, dodijeljena IP adresa mu se može vidjeti naredbom:

```
ip neighbour show dev [ime Ethernet sučelja]
```

