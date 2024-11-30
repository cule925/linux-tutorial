# SIGURNOSNE KOPIJE

Slijede korisni linkovi za upute stvaranja sigurnosnih kopija:

- osnovni alati: [basic-tools](basic-tools)

## Što je sigurnosna kopija?

Sigurnosna kopija je kopija podataka koja se radi za slučajeve gubitka izvornih podataka. Sigurnosne kopije se mogu čuvati na lokalnim uređajima, računalima u oblaku, mrežnim pohranama i slično. Postoji nekoliko vrsta sigurnosnih kopija:

- puna sigurnosna kopija - kopija svih podataka
	- najsporije sigurnosno kopiranje
	- najbrža obnova podataka
- inkrementalna sigurnosna kopija - kopija podataka koji su promijenjeni od zadnje pune ili zadnje inkrementalne sigurnosne kopije podataka
	- najbrže sigurnosno kopiranje
	- najsporija obnova podataka
- diferencijalna sigurnosna kopija - kopija podataka koji su promijenjeni od zadnje pune sigurnosne kopije podataka
	- sigurnosno kopiranje brža od pune, ali sporija od inkrementalne
	- obnova podataka brža od inkrementalne, sporija od pune

### Primjer korištenja pune sigurnosne kopije

Može se uzeti primjer gdje se svaki tjedan puna sigurnosna kopija radi ponedjeljkom i petkom u 15:00 sati.

```
        F - full backup

        *--MON--*--TUE--*--WEN--*--THU--*--FRI--*--SAT--*--SUN--*
        |_______|_______|_______|_______|_______|_______|_______|
        |@15:00 |       |       |       |@15:00 |       |       |
        | -> F1 |       |       |       | -> F2 |       |       |
        |       |       |       |       |       |       |       |
        |       |       |       |       |       |       |       |
        |       |       |       |       |       |       |       |
        |_______|_______|_______|_______|_______|_______|_______|
```

U slučaju da se dogodi kvar, recimo četvrtkom u 11:00, podatci se vraćaju uzimanjem zadnje potpune sigurnosne kopije prije tog kvara.

```
        F - full backup
        E - data loss event

        *--MON--*--TUE--*--WEN--*--THU--*--FRI--*--SAT--*--SUN--*
        |_______|_______|_______|_______|_______|_______|_______|
        |@00:00 |       |       |@11:00 |@00:00 |       |       |
        | -> F1 |       |       | -> E  | -> F2 |       |       |
        |       |       |       |       |       |       |       |
        |       |       |       |       |       |       |       |
        |       |       |       |       |       |       |       |
        |_______|_______|_______|_______|_______|_______|_______|

        DATA RESTORE: F1
```

### Primjer korištenja inkrementalne sigurnosne kopije

Može se uzeti primjer gdje se svaki tjedan puna sigurnosna kopija radi ponedjeljkom (00:00 sati), a inkrementalna utorkom, srijedom, četvrtkom i petkom (sve u 00:00 sati).

```
        F - full backup
        I - incremental backup

        *--MON--*--TUE--*--WEN--*--THU--*--FRI--*--SAT--*--SUN--*
        |_______|_______|_______|_______|_______|_______|_______|
        |@00:00 |@00:00 |@00:00 |@00:00 |@00:00 |       |       |
        | -> F  | -> I1 | -> I2 | -> I3 | -> I4 |       |       |
        |       |       |       |       |       |       |       |
        |       |       |       |       |       |       |       |
        |_______|_______|_______|_______|_______|_______|_______|
```

U slučaju da se dogodi kvar, recimo četvrtkom u 17:00, podatci se vraćaju uzimanjem zadnje potpune sigurnosne kopije i svih inkrementalnih kopija od te zadnje potpune sigurnosne kopije do vremena kvara. Inkrementi se primjenjuju na potpunu sigurnosnu kopiju prateći redoslijed inkrementacije te se takva kopija onda vraća na glavni sustav.

```
        F - full backup
        I - incremental backup
        E - data loss event

        *--MON--*--TUE--*--WEN--*--THU--*--FRI--*--SAT--*--SUN--*
        |_______|_______|_______|_______|_______|_______|_______|
        |@00:00 |@00:00 |@00:00 |@00:00 |@00:00 |       |       |
        | -> F  | -> I1 | -> I2 | -> I3 | -> I4 |       |       |
        |       |       |       |@17:00 |       |       |       |
        |       |       |       | -> E  |       |       |       |
        |       |       |       |       |       |       |       |
        |_______|_______|_______|_______|_______|_______|_______|

        DATA RESTORE: F + I1 + I2 + I3

```

### Primjer korištenja diferencijalne sigurnosne kopije

Može se uzeti primjer gdje se svaki tjedan puna sigurnosna kopija radi ponedjeljkom (00:00 sati), a diferencijalna utorkom, srijedom, četvrtkom i petkom (sve u 00:00 sati).

```
        F - full backup
        D - differential backup

        *--MON--*--TUE--*--WEN--*--THU--*--FRI--*--SAT--*--SUN--*
        |_______|_______|_______|_______|_______|_______|_______|
        |@00:00 |@00:00 |@00:00 |@00:00 |@00:00 |       |       |
        |-> F   | -> D1 | -> D2 | -> D3 | -> D4 |       |       |
        |       |       |       |       |       |       |       |
        |       |       |       |       |       |       |       |
        |_______|_______|_______|_______|_______|_______|_______|
```

U slučaju da se dogodi kvar, recimo četvrtkom u 17:00, podatci se vraćaju uzimanjem zadnje potpune sigurnosne kopije i zadnje diferencijalne kopije od te zadnje potpune sigurnosne kopije do vrijeme kvara. Diferencijalna kopija se primjenjuje na potpunu sigurnosnu kopiju te se takva kopija onda vraća na glavni sustav.

```
        F - full backup
        D - differential backup
        E - data loss event

        *--MON--*--TUE--*--WEN--*--THU--*--FRI--*--SAT--*--SUN--*
        |_______|_______|_______|_______|_______|_______|_______|
        |@00:00 |@00:00 |@00:00 |@00:00 |@00:00 |       |       |
        |-> F   | -> D1 | -> D2 | -> D3 | -> D4 |       |       |
        |       |       |       |@17:00 |       |       |       |
        |       |       |       | -> E  |       |       |       |
        |       |       |       |       |       |       |       |
        |_______|_______|_______|_______|_______|_______|_______|

        DATA RESTORE: F + D3

```

