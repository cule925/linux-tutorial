# DOCKER

[Docker](https://docs.docker.com/get-started/docker-overview/) je platforma za razvoj, implementaciju i izvršavanje aplikacija u kontejnerima. Platforma koristi klijent-poslužitelj model. Docker klijent šalje REST zahtjeve Docker poslužitelju radi upravljanja kontejnerima. Klijent se instancira naredbom ```docker``` u terminalu dok je Docker poslužitelj pozadinski proces ```dockerd```.

## Instalacija Docker platforme na Arch Linuxu

[Instalacija Docker platforme](https://wiki.archlinux.org/title/Docker) na Arch Linuxu može se napraviti naredbama:

```
sudo pacman -Syu
sudo pacman -S docker docker-buildx docker-compose
```

Kako običan korisnika ne bi morao izvršavati naredbe s privilegiranim ovlastima, može ga se dodati u grupu *docker* naredbom:

```
sudo usermod -aG docker $USER
```

Nakon dodavanja trenutnog korisnika u grupu *docker* potrebno je ponovno se prijaviti u sustav. Konačno, Docker poslužitelj (*dockerd*) može se pokrenuti naredbom:

```
sudo systemctl start docker
```

## Docker kontejneri

[Docker kontejner (*eng. Docker Container*)](https://docs.docker.com/get-started/docker-concepts/the-basics/what-is-a-container/) je skup procesa koji se izvode u izoliranom okruženju. Za razliku od virtualnih strojeva gdje svaki virtualni stroj ima vlastiti operacijski sustav i gdje su virtualni strojevi upravljani hipervizorom, Docker kontejneri dijele Linux jezgru s računalom domaćinom što znači da zauzimaju manje prostora u memoriji nego virtualni strojevi, ali su manje sigurni jer imaju izravan pristup Linux jezgri domaćina. Docker kontejneri koriste mehanizme poput kontrolnih grupa [control groups - cgroups](https://wiki.archlinux.org/title/Cgroups) i imenskih prostora [namespaces](https://blog.nginx.org/blog/what-are-namespaces-cgroups-how-do-they-work) uz pomoć kojih im se dodjeljuje određena količina resursa i izolira ih se od ostatka sustava.

### Mehanizam Linux jezgre - kontrolne grupe

Kontrolne grupe (*control groups - cgroups*) je mehanizam Linux jezgre koji omogućuje dodjeljivanje određene količine resursa računala skupu procesa. Resursi mogu biti vrijeme izvođenja na procesoru, količina dostupne radne memorije, propusnost mreže i slično. Dodjeljivanje resursa kontrolnim grupama rade podsustavi koji se nazivaju upravljači kontrolnih grupa. Neki od tih upravljača su *cpu*, *cpuset*, *memory*, *io* i drugi. Postoje dvije inačice: [kontrolne grupe v2](https://docs.kernel.org/admin-guide/cgroup-v2.html) i [kontrolne grupe v1](https://docs.kernel.org/admin-guide/cgroup-v1/cgroups.html). Kontrolne grupe v2 je novija inačica kontrolnih grupa koja dolazi integrirana s Linux jezgrom inačice 4.5 na dalje. Kontrolne grupe su organizirane hijerarhijski u virtualnom datotečnom sustavu počevši od direktorija *sys/fs/cgroup* koji se još naziva korijenska kontrolna grupa. Korijenskoj kontrolnoj grupi pripadaju svi procesi kojima nisu dodijeljene specifične kontrolne grupe. Stvaranje vlastite kontrolne grupe, primjerice imena *my-cgroup*, moguće je napraviti jednostavno naredbom:

```
sudo mkdir /sys/fs/cgroup/my-cgroup
```

Stvaranje nove kontrolne grupe automatski stvara skup datoteka kojima se mogu definirati ograničenja procesa u toj grupi (naredba ```ls /sys/fs/cgroup/my-cgroup```):

- datoteke sučelja oblika *cgroups.\**
- datoteke specifične za kontrolere oblika *cpu.\**, *cpuset.\**}, *memory.\** ...

Lista procesa koji pripadaju toj grupi je zapisana u datoteci *cgroups.procs* i može se ispisati naredbom:

```
sudo cat /sys/fs/cgroup/my-cgroup/cgroup.procs
```

Inicijalno u ovoj kontrolnoj grupi ne pripada ni jedan proces, ali to se može promijeniti stvarajući dva procesa u terminalima naredbom:

```
echo $$; read
```

Naredbe će ispisati svoj identifikator procesa (*eng. Process ID - PID*) i čekati na nekakav ulaz. Kako bi se gore navedena dva procesa dodali u grupu *my-cgroup* umjesto u korijensku kontrolnu grupu potrebno im je procesne identifikatore dodati u datoteku *cgroups.procs*:

```
echo [PID prvog procesa] | sudo tee -a /sys/fs/cgroup/my-cgroup/cgroup.procs
echo [PID drugog procesa] | sudo tee -a /sys/fs/cgroup/my-cgroup/cgroup.procs
```

Inicijalne postavke kontrolne grupe *my-cgroup* su naslijeđene od korijenske kontrolne grupe. Primjerice, maksimalna količina memorije koja se može alocirati za sve procese u ovoj grupi se može saznati naredbom:

```
sudo cat /sys/fs/cgroup/my-cgroup/memory.max
```

U ovom slučaju ona je inicijalno postavljena na vrijednost *max* što znači da nema ograničenja. Ako bi se ova vrijednost postavila na 0 bajtova, aktivirala bi se komponenta Linux jezgre *Linux Out Of Memory Killer* koja bi ubila ova dva procesa u grupi:

```
echo 0 | sudo tee /sys/fs/cgroup/my-cgroup/memory.max
```

Oba procesa će u ovom slučaju biti terminirana. U slučaju želje za brisanjem kontrolne grupe, procesi koji joj pripadaju moraju završiti s radom ili biti terminirani. Brisanje kontrolne grupe *my-cgroup* radi se naredbom:

```
sudo rmdir /sys/fs/cgroup/my-cgroup
```

Kako se kontrolne grupe koriste s Docker platformom? Docker pozadinski proces *dockerd* komunicira s pozadinskim procesom [*containerd*](https://containerd.io/docs/) koji je zaslužan za pokretanje kontejnera i postavljanje im ograničenja. Kao primjer može se izvršiti naredba:

```
docker run --memory=256m nginx
```

Naredba će preuzeti *nginx* (poveznica https://nginx.org/en/) sliku s javnog *Docker Hub* repozitorija i pokrenuti kontejner koristeći tu sliku te zadati ograničenje korištenja memorije na 256 MiB. Servis *containerd* će stvoriti kontrolnu grupu na lokaciji */sys/fs/cgroup/system.slice/* i postavit će ograničenje u *memory.max* datoteku što se može u ovom slučaju vidjeti naredbom:

```
sudo cat /sys/fs/cgroup/system.slice/docker-[ID kontejnera].scope/memory.max
```

Izlaz ove naredbe u ovom slučaju treba biti točno 268435456 bajtova. Identifikator kontejnera se može vidjeti naredbom:

```
docker ps --no-trunc
```

Brisanje kontejnera će automatski i ukloniti kontrolnu grupu:

```
docker stop [ID kontejnera]
docker rm [ID kontejnera]
```

Po potrebi može se izbrisati i preuzeta *nginx* slika naredbom:

```
docker rmi nginx:latest
```

### Mehanizam Linux jezgre - imenski prostori

[Imenski prostori Linuxa (*eng. Linux Namespaces*)](https://blog.nginx.org/blog/what-are-namespaces-cgroups-how-do-they-work) je mehanizam Linux jezgre koji omogućava izolaciju resursa procesa. Postoji više imenskih prostora koji se mogu izolirati:

- korisnički imenski prostor
	- nudi izolaciju korisnika
	- korisnik u nekom imenskom prostoru može imati privilegirana ovlaštenja nad skupom procesa u tom imenskom prostoru
	- svaki proces može biti dio samo jednog korisničkog imenskog prostora

- imenski prostor procesnih identifikatora
	- nudi izolaciju procesa
	- procesi koji se nalaze u imenskom prostoru procesnih identifikatora imaju vlastite procesne identifikatora
	- procesi različitih imenskih prostora se međusobno ne vide

- mrežni imenski prostor
	- omogućuje izolaciju mrežnih sučelja između procesa koji pripadaju različitim mrežnim imenskim prostorima
	- dosta važno za Docker kontejnere, procesi koji se pokreću unutar kontejnera ne vide sva mrežna sučelja sustava, povezanost na vanjsku mrežu se ostvaruje preko \emph{veth} virtualna Ethernet sučelja

- imenski prostor montiranja
	- nudi izolaciju točaka montiranja

- imenski prostor međuprocesne komunikacije
	- nudi izolaciju resursa međuprocesne komunikacije primjerice redovima poruka

- imenski prostor Unix domena
	- izolacija imena računala i domena

- imenski prostor vremena
	- izolacija sustavskih satova za grupe procesa.

Svaki Linux operacijski sustav ima odgovarajući korijenski imenski prostor za sve imenske prostore. Izvršavanje naredbi *ps*, *ls* ili bilo koje druge naredbe bez posebnih argumenata se izvršavaju u korijenskim imenskim prostorima.

Stvaranje vlastitih imenskih prostora moguće je naredbom *unshare*, ali se to neće pokazati ovdje već će se pokazati primjer imenskih prostora Docker kontejnera. Neki primjeri za *unshare* naredbu se mogu naći u stranicama priručnika (*eng. man pages*) pod stavkom *EXAMPLES* (naredba ```man unshare```). U slučaju primjera za Docker kontejner potrebno je ponovno stvoriti kontejner koji koristi Docker *nginx* sliku, ali sad u pozadini:

```
docker run -d --memory=256m nginx
```

Naredba za pokretanje interaktivne *bash* ljuske u pokrenutom Docker kontejneru je:

```
docker exec -it [ID kontejnera] bash
```

Lista procesa koji pripadaju imenskom prostoru kontejnera može se saznati naredbom:

```
ls /proc | grep '^[0-9]*$'
```

Direktorij */proc* je zapravo dio virtualnog datotečnog sustava koji sadrži informacije o procesima. U ovom slučaju su vidljivi samo procesi u imenskom prostoru procesa kontejnera. Također proces s identifikatorom 1 nije nikakav *init* proces već je pokrenuti *nginx* proces što se može vidjeti naredbom:

```
cat /proc/1/cmdline
```

Međutim, ovaj proces se može vidjeti na računalu domaćinu samo je potrebno pronaći njegov PID. To se može napraviti naredbom:

```
docker inspect [ID kontejnera]
```

Pod stavkom *Pid* može se pronaći PID procesa koji odgovara *init* procesu u kontejneru. Ovdje je bitno da računalo domaćin može vidjeti sve procese pa i one koji su u kontejneru dok kontejner samo može vidjeti svoje procese. Još jedna primjer jest izolacija korisnički imenski prostor u kontejneru. Ispisom trenutnog korisnika u kontejneru naredbom ```whoami``` dobije se korisnik *root* iako se u kontejner ušlo kao običan neprivilegirani korisnik. Još je potrebno napomenuti da se koristi i imenski prostor montiranja odnosno kontejner koristi vlastite izvršne programe koji nisu dijeljeni domaćinom, vlastiti mrežni prostor i slično. Izlazak iz ljuske kontejnera moguć je naredbom:

```
exit
```

Brisanje kontejnera se radi nizom naredbi:

```
docker stop [ID kontejnera]
docker rm [ID kontejnera]
```

Po potrebi može se izbrisati i preuzeta *nginx* slika naredbom:

```
docker rmi nginx:latest
```

## Docker slika

[Docker slika (*eng. Docker Image*)](https://docs.docker.com/get-started/docker-concepts/the-basics/what-is-an-image/) služi kao uputa za izgradnju i pokretanje kontejnera. Sadrži sve datoteke, biblioteke i konfiguracije za pokretanje kontejnera. Obično se Docker slika temelji na nekoj drugoj Docker slici koja se pritom modificira za potrebe korisnika. Slike se mogu preuzeti s *Docker Hub* repozitorija u lokalni repozitorij ili ih je moguće izgraditi pisanjem Dockerfile datoteke. Kod izgradnje slika koristi se koncept slojeva (*eng. Layers*). Svaki sloj je neka modifikacija datotečnog sustava koji pripada toj slici. Ovi slojevi se mogu koristiti za izgradnju drugih slika. Izgradnja slike se pokreće naredbom:

```
docker build -t [Ime slike]:[Oznaka slike] [Ciljni direktorij gdje se nalaze datoteke za izgradnju (kontekst)]
```

Slike koje se nalaze u lokalnom repozitoriju mogu se izlistati naredbom:

```
docker images
```

Brisanje slike se može izvesti naredbom:

```
docker rmi [Ime slike]:[Oznaka slike]
```

Ako bi se slika ponovno počela graditi, primijetilo bi se da se slika puno brže izgrađuje nego prvi puta. To se događa zbog build cachea koji sadržava izgrađene slojeve. U slučaju da se žele izbrisati slojevi koji se ne koriste, to je moguće naredbom:

```
docker builder prune
```

### Dockerfile datoteka

[Dockerfile datoteka](https://docs.docker.com/build/concepts/dockerfile/) je datoteka koja specificira kako izgraditi Docker sliku. Tijekom izvođenja naredbe ```docker build ...```, Dockerfile datoteka se mora nalaziti u istom direktoriju odakle se pozvala naredba *build*, u suprotnom se mora specificirati lokacija Dockerfile datoteke zastavicom *-f*. Tipične Dockerfile instrukcije su:

- FROM [Slika s Docker Huba (i njena oznaka ako je potrebno)]
	- definira baznu sliku
- RUN [Naredba]
	- izvršava naredbu nakon što se izgradi prethodni sloj
- WORKDIR [Novi radni direktorij]
	- mijenja radni direktorij u Docker slici
- COPY [Iz konteksta] [U sliku]
	- kopira datoteke s lokacije konteksta u sliku
- CMD [Naredba]
	- izvršava naredbu kad se pokrene instanca slike odnosno kontejner
- EXPOSE [Port]
	- metapodatak koji govori koji port je potrebno proslijediti
- ENV [Varijabla okruženja]=[Vrijednost varijable okruženja]
	- postavlja varijablu okruženja za sve sljedeće izvršene naredbe

Kao primjer, može se uzeti Flask poslužitelj koji će ispisati "Hello World" kad se s računala domaćina u web pregledniku adresira *http:localhost:8000*. Potrebno je stvoriti direktorij *my-flask-app* i premjestit se u njega:

```
mkdir ~/my-flask-app
cd ~/my-flask-app
```

Trenutni direktorij odnosno kontekst mora sadržavati sljedeće datoteke:

- hello.py
	- Python kod koji koristi Flask radni okvir
- Dockerfile
	- definira način izgradnje datoteke

Datoteka *hello.py* ima sljedeći sadržaj:

```
from flask import Flask
app = Flask(__name__)

@app.route("/")
def hello():
    return "Hello World!"
```

Dok Dockerfile datoteka ima sljedeći sadržaj:

```
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y python3 python3-pip
RUN pip install flask==3.0.*

COPY hello.py /

ENV FLASK_APP=hello
EXPOSE 8000
CMD ["flask", "run", "--host", "0.0.0.0", "--port", "8000"]
```

Prethodna Dockerfile datoteka povlači sliku s Docker Huba, ažurira repozitorije i aplikacije, kopira datoteku *hello.py* iz konteksta u sliku, postavlja varijable okruženja, postavlja varijablu okruženja *FLASK_APP* na vrijednost *hello* te metapodatak prosljeđivanje porta na 8000 i naredbu koja će se izvršiti pri pokretanju (pokrenut će se *Flask* poslužitelj).

Kao što je već rečeno, izgradnja slike koristi koncept slojeva. U Dockerfileu prilikom izvršenja svake Dockerfile instrukcije koja na neki način uređuje datotečni sustav Docker slike, stvara se novi sloj koji sadrži razliku (deltu) prethodnog sloja. Tako će u ovom slučaju naredbe FROM, RUN, COPY stvoriti nove slojeve dok recimo EXPOSE neće jer je on samo metapodatak koji na nikakav način ne uređuje datotečni sustav.

Izgradnja slike se radi naredbom:

```
docker build -t my_flask_image:v1 .
```

Pokretanje slike s pravim prosljeđivanjem porta 8000 s domaćina na port 8000 u Docker kontejner radi se naredbom:

```
docker run -d -p 127.0.0.1:8000:8000 --name my_flask_container_1 -t my_flask_image:v1
```

Prethodna naredba stvorila je instancu slike odnosno kontejner. U web pregledniku se sada može upisati *http://localhost:8000*.

Zaustavljanje kontejnera, brisanje konačne slike i svih njenih slojeva te brisanje direktorija *my-flask-app* moguće je napraviti naredbama:

```
docker stop my_flask_container_1
docker rm my_flask_container_1
docker rmi my_flask_image:v1
cd ~
rm -rf my-flask-app
```

### Izgradnja slike uz pomoć unakrsnog prevoditelja

Naredba *docker build* gradi slike namijenjene za arhitekture iste kao i arhitektura prevodioca. No što ako se želi, primjerice, izgraditi Docker slika koja će se pokretati na 64 bitnoj ARM arhitekturi (ARM64) ako je arhitektura domaćina 64 bitna x86_64 arhitektura (AMD64)? Tu se nudi rješenje u unakrsnim prevoditeljima za [izgradnju slike koje su kompatibilne s drugim arhitekturama](https://docs.docker.com/build/building/multi-platform/).

U slučaju primjera izgradnje slike za ARM64 arhitekturu ako je računalo na kojem se gradi AMD64 arhitekture, prije izgradnje slike potrebno je stvoriti graditelj slike (*eng. image builder*) koji definira za koju platformu izgraditi sliku te odabrati za korištenje:

```
docker buildx create --platform linux/arm64 --name arm64-builder
docker buildx use arm64-builder
```

Nakon toga, slika se može izgraditi naredbom:

```
docker buildx build --platform linux/arm64 --tag [Ime slike]:[Oznaka slike] --load [Ciljni direktorij gdje se nalaze datoteke za izgradnju (kontekst)]
```

Izlistavanje dostupnih graditelja slika može se napraviti naredbom:

```
docker buildx ls
```

Postavljanje uobičajenog graditelja slike može se napraviti naredbom:

```
docker buildx use default
```

Brisanje graditelja slika može se napraviti naredbom:

```
docker buildx rm arm64-builder
```

## Docker repozitorij slika

[Docker repozitorij slika (*eng. Docker Registry*)](https://docs.docker.com/guides/docker-concepts/the-basics/what-is-a-registry/) je usluga koja pohranjuje Docker slike. Repozitorij može biti lokalan gdje se slike pohranjuju na lokalnom računalu (primjerice korištenjem naredbe *docker build ...) ili javan kao primjerice *Docker Hub* kojem se pristupa naredbama kao *docker pull* ili *docker push*.

### Docker Hub

[Docker Hub](https://www.docker.com/products/docker-hub/) je javni repozitorij koji pohranjuje Docker slike. Te slike mogu biti javne odnosno dostupne svima za preuzimanje ili privatne odnosno dostupne samo autoru te slike. Docker Hub je uobičajeni javni repozitorij za Docker te se sve naredbe za upravljanje javnim repozitorijima koje se izvrše odnose na njega.

Preuzimanje slike s Docker Huba moguće je izvest naredbom:

```
docker image pull [Ime slike]:[Oznaka slike]
```

#### Prijenos vlastite izgrađene slike na Docker Hub

Ako je slika već izgrađena naredbom ```docker build ...```, potrebno je prvo registrirati se i stvoriti repozitorij na [Docker Hubu](https://hub.docker.com/repositories/) klikom na "Create repository". Je li repozitorij javan ili privatan je u ovom slučaju svejedno. Imenski prostor (oznaka *Namespace*) neka ostane imenom korisnika. Ostali menadžment repozitorija (uređivanje, brisanje i slično) se također radi na Docker Hubu.

Nakon stvaranja repozitorija na Docker Hubu, potrebno je [prijaviti se](https://docs.docker.com/reference/cli/docker/login/) na Docker Hub iz terminala:

```
docker login -u [Korisničko ime na Docker Hubu]
```

Zatim je potrebno postaviti novu oznaku na novoizgrađenu sliku. Nova oznaka mora sadržavati korisničko ime na Docker Hubu, ime repozitorija i nekakvu oznaku koja označava novu verziju slike:

```
docker tag [Ime novoizgrađene slike]:[Oznaka novoizgrađene slike] [Ime imenskog prostora repozitorija na Docker Hubu (korisničko ime)]/[Ime repozitorija na Docker Hubu]:[Oznaka koja označava verziju slike koja će se prikazivati na Docker Hubu]
```

Konačno, [prijenos slike](https://docs.docker.com/reference/cli/docker/image/push/) radi se naredbom:

```
docker push [Ime imenskog prostora repozitorija na Docker Hubu (korisničko ime)]/[Ime repozitorija na Docker Hubu]:[Oznaka koja označava verziju slike koja će se prikazivati na Docker Hubu]
```

Odjava s Docker Huba radi se naredbom:

```
docker logout
```
