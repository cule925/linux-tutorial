# MEMORIJA S NASUMIČNIM PRISTUPOM

Memorija s nasumičnim pristupom je primarna računalna memorija čijem se sadržaju može izravno pristupiti jednakim vremenom bez obzira na kojoj se lokaciji taj sadržaj nalazi. Vrste RAM-a:

- statički RAM (SRAM)
	- za spremanje jednog bita koristi se jedna ćelija koja se sastoji od 6 tranzistora (MOSFET)
	- skuplja za proizvodnju nego dinamički RAM
	- brža nego dinamički RAM
	- koristi se za cache memoriju
- dinamički RAM (DRAM)
	- za spremanje jednog bita koristi se jedna ćelija koja se sastoji od 1 tranzistora (MOSFET) i jednog kondenzatora
	- jeftinija za proizvodnju nego statički RAM
	- sporija nego statički RAM
	- potrebno periodičko osvježavanje zbog propuštanja kondenzatora
	- koristi se kao glavna memorija u računalu

Skupom ćelija koje predstavljaju niz bitova može se pristupiti adresnim linijama. Općenito, kapacitet memorije je dan brojem adresnih linija formulom ```2^n * w``` gdje je *n* broj adresnih linija, a *w* broj ćelija koje jedna varijacija adrese odabire. Broj *w* se također naziva i širina.

## Hijerarhija memorije

Glavna memorija računala iako brza i dalje nije dovoljno brza zahtjevima procesora. Procesori mogu izvršavati milijardu instrukcija u sekundi, a ogromne memorije koje sadrže velik broj sklopova; multipleksora i dekodera, ne mogu stići posluživati zahtjeve za sljedeće instrukcije te takva situacija dovodi do čekanja. Kako bi se vrijeme dohvata sljedećih instrukcija i podataka smanjilo koristi se memorijska hijerarhija. Općenito, hijerarhija se može podijeliti na sljedeće komponente:

- registri u procesoru
	- najbrža memorija
	- u nju se stavljaju podatci iz cache memorije razine 1 (L1)
	- veličine par tisuća bajtova
- cache memorija razine 1 (L1)
	- sporija od samih registara u procesoru
	- podijeljena na instrukcijsku i podatkovnu memoriju
	- dohvaća podatke iz cache memorije razine 2 (L2)
	- veličine stotinjak kilobajta
- cache memorija razine 2 (L2)
	- sporija od cache memorije razine 1
	- podijeljena na instrukcijsku i podatkovnu memoriju
	- dohvaća podatke iz cache memorije razine 3 (L3)
	- veličine par megabajta
- cache memorija razine 3 (L3)
	- sporija od cache memorije razine 2
	- dohvaća podatke iz glavne memorije
	- veličine dosta više megabajta
- glavna memorija
	- sporija od cache memorije razine 3
	- dohvaća podatke iz sekundarnih računalnih memorija (diskovi i slično)
	- veličine od gigabajta na dalje

## Načini upravljanja memorijom

Pokretanjem procesa na računalu njegov se strojni kod učitava u glavnu memoriju računala uz pomoć Linux jezgre koja je već sama učitana u glavnu memoriju pokretanjem samog računala. Iskorištenost memorije može se vidjeti naredbom:

```
free -h
```

### Dinamičko raspoređivanje

Prvi od načina raspoređivanja je dinamičko raspoređivanje. Prvi dio memorije sadrži adresni prostor operacijskog sustava koji sadrži informacije o procesima koji zauzimaju ostatak glavne memorije. Procesi se u glavnu memoriju umeću kao segmenti.


```
             Trenutno stanje                    Stanje glavne memorije               Stanje glavne memorije nakon što je proces 1 završio s radom
          glavne memorije (RAM)              nakon što se pokrene proces 2                te je uklonjen (dogodila se fragmentacija memorije)

        |------------------------|            |------------------------|            |------------------------|
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |    Adresni prostor     |            |    Adresni prostor     |            |    Adresni prostor     |
        |        SUSTAVA         |            |        SUSTAVA         |            |        SUSTAVA         |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            Pokretanje procesa 3 neće
        |                        |            |                        |            |                        |            uspjeti zbog fragmentacije
        |========================|            |========================|            |========================|
        |                        |            |                        |            |                        |            |^^^^^^^^^^^^^^^^^^^^^^^^|
        |                        |            |                        |            |                        |            |                        |
        |    Adresni prostor     |            |    Adresni prostor     |            |    Slobodni prostor    |            |                        |
        |       PROCESA 1        |            |       PROCESA 1        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |    Veličina adresnog   |
        |------------------------|            |------------------------|            |------------------------|            |     prostora kojeg     |
        |                        |            |                        |            |                        |            |   PROCES 3 zahtijeva   |
        |                        |            |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |            |^^^^^^^^^^^^^^^^^^^^^^^^|
        |                        |            |    Adresni prostor     |            |    Adresni prostor     |
        |                        |            |       PROCESA 2        |            |       PROCESA 2        |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |    Slobodni prostor    |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |                        |            |------------------------|            |------------------------|
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |                        |            |    Slobodni prostor    |            |    Slobodni prostor    |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |                        |            |                        |            |                        |
        |------------------------|            |------------------------|            |------------------------|
```

Jedan od glavnih problema ovakvog načina upravljanja memorijom je fragmentacija. Fragmentacija je pojava koja se događa kada je raspodjela slobodnog prostora u memoriji neoptimalna. Događa se kada postoji puno malih komadića slobodnog prostora. Kako svaki proces u ovom načinu rada zahtjeva kontinuirani slobodni prostor može se dogoditi da se program ne može učitati u memoriju iako je zbroj slobodnih prostora veći nego zahtjev samog programa.

Također, procesor radi zaštite memorije koristi logičke adrese počevši od nule. Prilikom svakog adresiranja događa se sklopovsko pribrajanje bazne adrese i logičke adrese. Bazna adresa označava prvu fizičku adresu koja pripada procesu u glavnoj memoriji. Pribrajanje adrese i zaštitu od neovlaštenog pristupa radi sklop za zaštitu od pogrešaka (*eng. Memory Protection Unit - MPU*):

```
        -------------------------PROCESOR-------------------------\                                 LA - logička adresa
                                                                  |                                 FA - fizička adresa
                                                                  |                                 BR - bazni registar
              <---------------prekid-----------------\            |                                 RO - registar ograde
                                                     | DA         |           |----RAM----|
                  ___________________           /---------\       |           |           |
                  | Registar ograde |---------->| RO > FA |       |           |           |
                  |_________________|           \---------/       |           |           |
                                                    /|\           |           |           |
                                                     |            |           |           |
          LA  ----------------->|---------\          |            |           |           |
                                | LA + BR |----------------------------FA---->|           |
                           /--->|---------/          |            |           |           |
                           |                         |            |           |           |
                           |                        \|/           |           |           |
                  _________|_________           /---------\       |           |           |
                  | Bazni registar  |---------->| BR > FA |       |           |           |
                  |_________________|           \---------/       |           |           |
                                                     | DA         |           |-----------|
              <---------------prekid-----------------/            |
                                                                  |
        ----------------------------------------------------------/
```

U adresnom prostoru sustava nalaze se informacije o svim procesima i njihovim pripadajućim dretvama. Za procese postoji procesni informacijski blok koji sadrži informacije o baznim adresama procesa te registru ograde (duljina adrese). Procesni informacijski blok sadrži i pokazivače na opisnike dretvi.

### Straničenje i virtualna memorija

Drugi način i danas češće u upotrebi na osobnim računalima jest straničenje. Glavna memorija se dijeli na okvire najčešće veličine 4 KiB. Svaki proces ima svoju vlastitu tablicu stranica (*eng. Process Page Table - PPT*) koja je zapisana u glavnoj memoriji u procesnom informacijskom bloku. Tablica stranica procesa pretvara virtualne (logičke) adrese u fizičke. Svim stranicama jednog procesa ne moraju biti dodijeljeni odgovarajući okviri u glavnoj memoriji već se mogu nalaziti na vanjskom spremniku. Tome služi bit prisutnosti tijekom pretvaranja logičke adrese u fizičku koji se nalazi u PPT-u, njegove vrijednosti označavaju sljedeće:

- 1 - stranica se nalazi u nekom okviru
- 0 - stranica se ne nalazi ni u jednom okviru, događa se prekid *page fault*

U slučaju prekida *page fault* potrebno je ubaciti traženu stranicu iz vanjskog diska u glavnu memoriju. Ako postoji slobodan okvir to je vrlo jednostavno. U slučaju da su svi okviri zauzeti potrebno je izbaciti jednu stranicu iz okvira u glavnoj memoriji i zamijeniti ju odgovarajućom potrebnom stranicom. Neki algoritmi koji se koriste za odluke izbacivanja stranice iz glavne memorije su:

- FIFO (*eng. First In First Out*)
	- izbacuje najstariju stranicu
- LRU (*eng. Least Recently Used*)
	- izbacuje stranicu koja najdulje nije koristila
- LFU (*eng. Least Frequently Used*)
	- izbacuje stranicu koja se najmanje puta koristila

```
                 Glavna memorija (RAM)

              |------------------------|
              |                        |
              |                        |
              |      Dio sustava       |
              |       koji nije        |
              |       straničen        |
              |                        |
              |                        |
              |------------------------|
            0 |                        |
              |------------------------|
            1 |                        |
              |------------------------|   Okviri sustava
              |                        |   (X broj stranica veličine 4 KiB)
              |          ....          |
              |                        |
              |------------------------|
          X-1 |                        |
              |========================|                        Tablica stranica procesa (PPT)
            X |                        |                        procesa P1
              |------------------------|
          X+1 |                        |                        |---|------|-|
              |------------------------|                        | 0 |  X+4 |1|
          X+2 |         P1 kod         |                        | 1 | x+10 |1|
              |------------------------|                        | 2 |  X+7 |1|
          X+3 |                        |                        | 3 |  X+2 |1|
              |------------------------|                        | 4 |      |0|
          X+4 |         P1 kod         |                        |    ....    |
              |------------------------|                        |---|------|-|
          X+5 |                        |
              |------------------------|
          X+6 |                        |
              |------------------------|   Okviri korisničkih
          X+7 |         P1 kod         |   aplikacija
              |------------------------|   (Y broj okvira)
          X+8 |                        |
              |------------------------|
          X+9 |                        |
              |------------------------|
         X+10 |         P1 kod         |
              |------------------------|
              |                        |
              |          ....          |
              |                        |
              |------------------------|
          X+Y |                        |
              |------------------------|
```

Pretvaranje same adrese radi jedinica za upravljanje memorijom (*eng. MMU - Memory Management Unit*).

```
        ----------     ---------  logička adresa
        |PROCESOR|---->| 2 | P |
        ----------     --|---|--
                         |   --------------|                           Glavna
                         |                 |                       memorija (RAM)
                        \|/                |                 |          ....          |
                       |---|------|-|      |                 |                        |
                       | 0 |  X+4 |1|      |                 |------------------------|
                       | 1 | x+10 |1|      |             X+3 |                        |
                       | 2 |  X+7 |1|      |                 |------------------------|
                       | 3 |  X+2 |1|      |      |----> X+7 |                        |
                       | 4 |      |0|      |      |          |------------------------|
                       |    ....    |      |      |      X+8 |                        |
                       |---|------|-|      |      |          |------------------------|
                              |            |      |          |                        |
                              |            |      |          |          ....          |
                              |   ---------|      |
                              |   |               |
                             \|/ \|/              |
                           ---|---|---            |
                           | X+7 | P |------------|
                           -----------
                         fizička adresa
```

Međutim, pristup glavnoj memoriji kako bi se dohvaćala tablica radi prevođenja je dosta vremenski skup proces. Stoga se koristi međuspremnik preslika adresa (*eng. TLB - Translation Lookaside Buffer*). Prilikom prevođenja adrese prvo se gleda TLB i ako se stranica tu ne nalazi onda se dohvaća PPT te se pokazivač na stranicu u okvir ubacuje u TLB nekim navedenim algoritmom, a ako stranica nema pokazivač u PPT-u samu stranicu se učitava s vanjskog diska u glavnu memoriju te joj se pokazivač postavlja u PPT i piše u TLB. TLB može sadržavati više informacija kao što su primjerice identifikator procesa kojem te stranice pripadaju tako da se TLB može koristiti za više procesa.

```
                             |-----------------------------------|                                   S - adresa stranice
                             |                                   |                                   P - pomak
        ----------     ------|--  logička adresa                 |                                   O - adresa okvira
        |PROCESOR|---->| S | P |                                 |                                   x - nebitna vrijednost
        ----------     --|------                                 |
                         |           ____TLB____                 |
                         |          |__x__|__x__|                |
                         |          |__x__|__x__|                |
                         |--------->|__S__|__O__|------------|   |
                         |          |__x__|__x__|            |   |
                         |          |__x__|__x__|    pogodak |   |
                         |          |     |     |            |   |
                         |          | ... | ... |           \|/ \|/  fizička adresa
                         |          |     |     |          --|---|--
                         |          |__x__|__x__|          | O | P |================>|----RAM----|
                         |                                 --|------                 |           |
                promašaj |                                  /|\                      |           |
                         |           ____PPT____             |                       |           |
                         |          | 0 |__x__|0|            |                       |           |
                         |          | 1 |__x__|0|            |                       |           |
                         ---------->| 2 |__O__|1|------------|                       |           |
                                    | 3 |__x__|0|                                    |-----------|
                                    |   |     | |
                                    |   | ... | |
                                    |   |     | |
                                    |n-1|__x__|0|
```

U modernim računalima koriste se razine posredovanja (*eng. indirection*) kako bi se smanjila veličina tablice stranice procesa. Jedan proces ima više tablica stranica procesa povezane u stablo. Linux koristi [4 razine posredovanja](https://0xax.gitbooks.io/linux-insides/content/Theory/linux-theory-1.html) za x86_64 računalne sustave.
