# SKRIPTE ZA POKRETANJE QEMU VIRTUALNIH STROJEVA U UEFI NAČINU RADA

Stablo direktorija i datoteka:

```
.
├── bridge_mac.txt
├── bridge_set.sh
├── bridge.txt
├── bridge_unset.sh
├── ethernet.txt
└── machine-0
    ├── boot.sh
    ├── disk.qcow2 (stvara se skriptom init.sh)
    ├── init.sh
    ├── iso_path.txt
    ├── OVMF_VARS.4m.fd (stvara se skriptom init.sh)
    ├── tap-setup
    │   ├── tap_mac.txt
    │   ├── tap_set.sh
    │   ├── tap.txt
    │   └── tap_unset.sh
    └── target_disk.txt
```

Objašnjenje datoteka:

* *bridge_set.sh* - postavlja mrežni most, čita iz datoteke *ethernet.txt*, *bridge.txt* i *bridge_mac.txt*
* *bridge_unset.sh* - razmontira mrežni most, čita iz datoteke *ethernet.txt* i *bridge.txt*
* *ethernet.txt* - naziv Ethernet priključka s Internet pristupom koji se spaja na mrežni most
* *bridge.txt* - naziv virtualnog mrežnog mosta
* *bridge_mac.txt* - MAC adresa virtualnog mrežnog most
* *machine-0/init.sh* - stvara prazan virtualni tvrdi disk i kopira OVMF bazi varijabli, sprema disk na lokaciju koja je zapisana u datoteci *target_disk.txt*
* *machine-0/target_disk.txt* - putanja do diska
* *machine-0/iso_path.txt* - apsolutna putanja do ISO datoteke
* *machine-0/boot.sh* - pokreće virtualni stroj, poziva skripte *machine-0/tap-setup/tap_set.sh* i *machine-0/tap-setup/tap_unset.sh*
* *machine-0/tap-setup/tap_set.sh* - postavlja TAP sučelje, spaja ga na mrežni most, čita iz datoteke *machine-0/tap-setup/tap.txt* i *machine-0/tap-setup/tap_mac.txt*
* *machine-0/tap-setup/tap_unset.sh* - razmontira TAP sučelje, čita iz datoteke *machine-0/tap-setup/tap.txt*
* *machine-0/tap-setup/tap.txt* - naziv TAP priključka koji se spaja na mrežni most
* *machine-0/tap-setup/tap_mac.txt* - MAC adresa TAP priključka koji se spaja na mrežni most

Prvo korištenje korištenja:

* Pokrenuti *machine-0/init.sh* skriptu koja će stvoriti prazni virtualni tvrdi disk i kopirati OVMF bazu varijabli
* Saznati Ethernet priključak koji je spojen na Internet (naredba ```ip a```)
* Upisati ime priključka u datoteku *ethernet.txt*
* Pokrenuti *bridge_set.sh* kako bi se stvorio most
* Saznati apsolutnu putanju ISO slike za instalaciju operacijskog sustava (naredba ```realpath [lokacija ISO slike]```)
* Upisati apsolutnu putanju ISO slike u datoteku *machine-0/iso_path.txt*
* Pokrenuti *machine-0/boot.sh* kako bi se pokrenuo virtualni stroj (ako treba nazvati TAP sučelje drugačijim nazivom od danog uređujući datoteku *machine-0/tap-setup/tap.txt*)
* Nakon završetka rada most se može razmontirati naredbom *bridge_unset.sh*

Svako sljedeće korištenje:

* Ako nije pokrenuta skripta *brige_set.sh* potrebno ju je pokrenuti kako bi se stvorio most
* Pokrenuti *machine-0/boot.sh* kako bi se pokrenuo virtualni stroj
* Nakon završetka rada most se može razmontirati naredbom *bridge_unset.sh*

Dodatno, ako se želi spojiti više virtualnih strojeva:

* Potrebno je napraviti kopiju *machine-0* direktorija, po mogućnosti imena *machine-1*, *machine-2*...
* Zatim je potrebno promijeniti *tap.txt* i *tap_mac.txt* datoteke u *machine-1/tap-setup/*, *machine-2/tap-setup/* i sličnim direktorijima tako da se sva imena i MAC adrese TAP priključaka međusobno razlikuju
* Postupak korištenja je sličan osim što je kod prvog korištenja potrebno umjesto samo jedne *machine-0/init.sh* i jedne *machine-0/boot.sh* skripte pokrenuti još dodatne skripte za svaki dodatni virtualni stroj: *machine-1/init.sh*, *machine-1/boot.sh*, *machine-2/init.sh*, *machine-2/boot.sh* i slično
