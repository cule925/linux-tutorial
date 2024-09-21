# SKRIPTE LJUSKE

[Skripta ljuske](https://www.geeksforgeeks.org/introduction-linux-shell-shell-scripting/?ref=lbp) je datoteka koja u sebi sadrži niz naredbi koje se mogu izravno izvoditi pozivajući ```[ljuska] [skriptna datoteka]```. Drugi način pokretanja skripte u ljusci je da skriptu pretvorimo u izvršnu datoteku naredbom *chmod* i onda je jednostavno pokrenemo kao običnu izvršnu datoteku. Izmjena dozvola za skriptu ljuske radi se naredbom:

```
chmod u+x [skriptna datoteka]
```

Pokretanje datoteke koja se nalazi u trenutnom direktoriju radi se naredbom:

```
./[skriptna datoteka]
```

## Osnovni pojmovi skripte ljuske

Najkorištenija ljuska u Linuxu je **bash** pa ćemo nju koristiti. Prvi redak u skripti ljuske bi trebao reći koju ljusku koristiti za izvršavanje skripte. Ovo će omogućiti izvršavanje skripte u ljuski za koju je napisana iako se pokreće iz neke druge ljuske. Dakle, prvi redak bi trebao započinjati tzv. [shebangom](https://www.geeksforgeeks.org/how-to-create-a-shell-script-in-linux/?ref=lbp) odnosno *#!* i onda putanjom do ljuske. Primjerice ako je skripta napisana za bash:

```
#!/bin/bash
```

### Varijable

Svaka varijabla je implicitno tipa *string*, dvije su vrste varijabli:

- globalne varijable - vidljive iz više ljuski
- lokalne varijable - vidljive samo iz lokalne ljuske

#### Varijable okoline

[Varijable okoline](https://wiki.archlinux.org/title/environment_variables) su varijable koje su definirane na razini sustava. Svaki proces koji se pokreće imat će dostupne kopiju ovih varijabli. Lista varijabli okoline definirane na sustavu mogu se dobiti naredbom:

```
printenv
```

Mijenjanje definicija varijabli okruženja na [razini sustava](https://wiki.archlinux.org/title/environment_variables#Globally) može se napraviti uređivanjem datoteka:

- */etc/profile*
	- inicijalizira varijable samo za ljusku s prijavom (*eng. login shell*)
- */etc/environment*
	- inicijalizira varijable na razini svake ljuske, ali prihvaća samo zapise vrste *VARIJABLA=VRIJEDNOST*

Mijenjanje definicija varijabli okruženja na [razini korisnika](https://wiki.archlinux.org/title/environment_variables#Per_user) specifično za bash ljusku može se napraviti uređivanjem datoteka:

- ~/.bash_profile
	- inicijalizira varijable samo za bash ljusku s prijavom specifično korisniku
- ~/.bashrc
	- inicijalizira varijable samo za bash ljusku specifično korisniku svaki put pri pokretanju nove ljuske

#### Stvaranje vlastitih varijabli i pridruživanje vrijednosti

Stvaranje i pridruživanje vrijednosti varijable radi se oblikom:

```
VARIJABLA=VRIJEDNOST
```

Potrebno je uočiti da nema razmaka ni s jedne strane znaka jednakosti. Pristup sadržaju varijable moguće je na sljedeći način:

```
$VARIJABLA
```

Naredba *unset* briše varijablu:

```
unset VARIJABLA
```

Varijablu je moguće i postaviti samo za čitanje. Nakon ovog joj više nije moguće promijeniti vrijednost, ali ju je i dalje moguće obrisati:

```
readonly VARIJABLA
```

#### Standardni izlaz neke naredbe u varijablu

Ako se želi pisati standardni izlaz neke naredbe u varijablu, to se može napraviti:

```
VARIJABLA=$(naredba)
```

#### Pretvaranje lokalne varijable u globalnu za procese djecu

U slučaju da iz ljuske pozivamo neke naredbe koje zahtijevaju specifičnu varijablu okoline, možemo im to omogućiti tako da prije njihovog pozivanja napravimo vlastitu varijablu globalnom:

```
export VARIJABLA
```

#### Varijabla koja je lokalna u bloku naredbi

Ako se varijabla želi koristiti samo u bloku nekih naredbi (primjerice u funkciji), to se može napraviti naredbom:

```
local VARIJABLA=VRIJEDNOST
```

### Uvjetovanje

Tri su naredbe za [kontrolu toka](https://www.geeksforgeeks.org/conditional-statements-shell-script/?ref=lbp):

* if
* elif
* else

Primjer gdje se koriste sve tri naredbe:

```
if [ uvjet ]; then
	naredbe
elif [ uvjet ]; then
	naredbe
else
	naredbe
fi
```

#### Operatori usporedbe cijelih brojeva

Operatori usporedbe cijelih brojeva su sljedeći:

- *-eq* - jednako
- *-ne* - nije jednako
- *-gt* - veće nego
- *-ge* - veće ili jednako nego
- *-lt* - manje nego
- *-le* - manje ili jednako nego

Primjer korištenja:

```
#!/bin/bash
VAR1=10
VAR2=20
if [ $VAR1 -ne $VAR2 ]; then
	echo "$VAR1 i $VAR2 nisu jednaki"
fi
```

#### Operatori usporedbe niza znakova

Operatori usporedbe niza znakova su sljedeći:

- *==* - jednako
- *!=* - nije jednako
- *\<* - manje nego (ASCII redoslijed)
- *\>* - veće nego (ASCII redoslijed)
- *-n* - vraća 0 ako niz ima duljinu veću od 0
- *-z* - vraća 0 ako je niz prazan odnosno duljina mu je 0

Primjer korištenja:

```
#!/bin/bash
VAR1=
VAR2="A string"
VAR3="A string"
if [ -z "$VAR1" ]; then
	echo "VAR1 ima duljinu 0"
fi
if [ "$VAR2" == "$VAR3" ]; then
	echo "VAR2 je jednak VAR3"
fi
```

#### Operatori datoteka

Postoje i operatori za ispitivanje datoteka:

- *-e* - datoteka postoji
- *-s* - datoteka ima veličinu veću od 0
- *-d* - datoteka je direktorij

Primjer korištenja:

```
#!/bin/bash
VAR1="/bin/"
if [ -d "$VAR1" ]; then
	echo "$VAR1 je direktorij"
fi
```

### Aritmetičke operacije nad varijablama

U slučaju da su vrijednosti varijable cijeli brojevi moguće ih je parsirati i nad njima vršiti aritmetičke operacije uz pomoć *expr* naredbe oblikom:

```
REZULTAT=$(expr $VARIJABLA1 operator $VARIJABLA2)
```

Gdje *operator* može biti jedna od sljedećih aritmetičkih operacija:

- *+* - zbrajanje
- *-* - oduzimanje
- *\\\** - množenje
- */* - cjelobrojno dijeljenje
- *%* - ostatak cjelobrojnog dijeljenja

### Petlje

Postoji više naredbi za [petlje](https://www.geeksforgeeks.org/looping-statements-shell-script/?ref=lbp). To su:

* while
* for
* until

#### Petlja while

Oblik petlje *while* je sljedeći:

```
while [ uvjet ]
do
	naredbe
done
```

Primjer korištenja (ispis brojeva od 0 do 9):

```
#!/bin/bash
ITERATOR=0
while [ $ITERATOR -lt 10 ]
do
	echo "Iterator = $ITERATOR"
	ITERATOR=$(expr $ITERATOR + 1)
done
```

#### Petlja for

Petlja *for* može se koristiti za iteraciju po listi. Oblik petlje *for* je sljedeći:

```
for VARIJABLA u LISTA
do
	naredbe
done
```

Primjer korištenja (ispis brojeva od 0 do 9)

```
#!/bin/bash
LIST="0 1 2 3 4 5 6 7 8 9"
for ITERATOR in $LIST
do
	echo "Iterator = $ITERATOR"
done
```

#### Petlja until

Petlja *until* je slična petlji *while* samo se ponavlja sve dokle je izraz u uvjetu *false* umjesto *true*, oblik joj je sljedeći:

```
until [ uvjet ]
do
	naredbe
done
```

Primjer korištenja (ispis brojeva od 0 do 9):

```
#!/bin/bash
ITERATOR=0
until [ $ITERATOR -ge 10 ]
do
	echo "Iterator = $ITERATOR"
	ITERATOR=$(expr $ITERATOR + 1)
done
```

#### Naredbe za izlazak i nastavak izvođenja petlje

Naredbe za [izlazak i nastavak izvođenja](https://www.geeksforgeeks.org/break-and-continue-keywords-in-linux-with-examples/?ref=lbp) petlje služe za upravljanje tijekom programa. Ključne naredbe su:

* break
* continue

##### Naredba break

Naredba *break* služi za prisilni izlazak iz petlje. Opći oblik joj je:

```
break [N]
```

N je broj ugnježđenja iz kojeg treba izaći. Ako se ne navede onda je 1.

Primjer korištenja (ispis brojeva od 0 do 4):

```
#!/bin/bash
ITERATOR=0
while [ $ITERATOR -lt 10 ]
do
	echo "Iterator = $ITERATOR"
	ITERATOR=$(expr $ITERATOR + 1)
	if [ $ITERATOR -eq 5 ]; then
		break
	fi
done
```

##### Naredba continue

Naredba *continue* služi za skok na sljedeću iteraciju petlje iako trenutna petlja nije završila s radom. Opći oblik joj je:

```
continue [N]
```

N je broj ugnježđenja na kojem treba nastaviti sljedeću iteraciju. Ako se ne navede onda je 1.

Primjer korištenja (ispis neparnih cijelih brojeva od 0 do 9):

```
#!/bin/bash
ITERATOR=0
while [ $ITERATOR -lt 10 ]
do
	ITERATOR=$(expr $ITERATOR + 1)
	if [ $(expr $ITERATOR % 2) -eq 0 ]; then	
		continue
	fi
	echo "Iterator = $ITERATOR"
done
```

### Funkcije

Ljuske nude mogućnost definiranja funkcija s argumentima. Opći oblik funkcije je:

```
ime_funkcije(){
	naredbe
	return "vrijednost"
}
```

Nije potrebno pisati *return* ako se ne vraća ništa. Argumenti koji su poslani u funkciju pristupaju se uz pomoć *$i*, gdje je *i* redni broj argumenta koji se šalje počevši od 1. Vrijednost *$0* je ime skripte. Vrijednost *$@* je lista svih argumenata poslanih u funkciju. Vrijednost *$#* je broj argumenata poslanih u funkciju. Povratna vrijednost funkcije može se dobiti uz pomoć *$?*. Pozivanje funkcije je jednostavno:

```
ime_funkcije
```

Ako se šalju argumenti:

```
ime_funkcije argument_1 argument_2 ...
```

Primjer korištenja (ispis jednog argumenta i povratak drugog):

```
#!/bin/bash
funkcija(){
	echo "Prvi argument: $1"
	return "$2"

}
funkcija 50 100
VAR=$?
echo "Povratna vrijednost funkcije: $VAR"
```

### Prosljeđivanje argumenata samoj skripti

Prosljeđivanje argumenata samoj skripti je jednostavno:

```
./[ime skripte] [argument 1] [argument 2] ...
```

Pristup tim argumentima je sličan kao i pristup argumentima funkcije. Pristupa im se uz pomoć *$i*, gdje je *i* redni broj argumenta koji se šalje počevši od 1. Vrijednost *$0* je ime skripte. Vrijednost *$@* je lista svih argumenata poslanih u skriptu. Vrijednost $# je broj argumenata poslanih u skriptu. Primjerice za 3 argumenta poslanih u skriptu:

```
./script 10 50 200
```

Vrijednosti se mogu dobiti:

```
#!/bin/bash
echo "Ime skripte: $0"
echo "Prvi argument: $1"
echo "Drugi argument: $2"
echo "Treći argument: $3"
echo "Svi argumenti: $@"
echo "Broj argumenata: $#"
```

### Lista posebnih parametara

Slijedi [lista posebnih parametara](https://www.geeksforgeeks.org/shell-script-to-demonstrate-special-parameters-with-example/?ref=lbp):

- *$#* - broj poslanih argumenata u skriptu ili funkciju
- *$0* - ime skripte u kojoj se izvršavaju naredbe
- *$i* - *i* predstavlja redni broj argumenta poslanog u skriptu ili funkciju počevši od 1
- *$\** - svi argumenti poslani u skriptu ili funkciju odvojeni prazninom
- *$!* - zadnji ID procesa (PID) koji se pokrenuo u pozadini
- *$?* - povratna vrijednost zadnje izvršene naredbe
- *$_* - zadnji argument koji se izvršio u prethodnoj naredbi
- *$$* - ID procesa (PID) trenutne ljuske
- *$@* - niz koji sadrži sve argumente poslane u skriptu ili funkciju
- *$-* - zastavice koje predstavljaju način rada ljuske
	- H - histexpand
	- m - monitor
	- h - hashall
	- B - braceexpand
	- i - interactive

### Čitanje standardnog ulaza

Čitanje sa standardnog ulaza može se napraviti naredbom *read*. Oblik joj je:

```
read [opcije] [niz varijabli u koje će se spremati unesene vrijednosti]
```

