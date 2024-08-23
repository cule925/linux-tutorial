# ZNAKOVNI UREĐAJ I IOCTL

Primjer modula s implementacijom naredbe *ioctl* i više datoteka uređaja gdje svaka datoteka uređaja ima vlastitu strukturu koja služi kao spremnik vrijednosti koju je moguće pisati i čitati naredbom *ioctl*. Operacije *write* i *read* nisu dovoljne za potpuno upravljanje uređajima iz programa. Tu se javlja naredba *ioctl* koja služi za slanje ili primanje informacija Linux jezgri. Iz korisničkog programa je moguće komunicirati sa znakovnim uređajima koji podržavaju operaciju *ioctl* funkcijom *ioctl()*.
