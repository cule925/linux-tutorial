# VIRTUALIZACIJA

Slijede upute za pojedine virtualizacijske alate:

* alat QEMU: [*qemu*](qemu)

## Što je virtualizacija?

[Virtualizacija](https://en.wikipedia.org/wiki/Virtualization) je proces dijeljenja računalnih resursa na jedan ili više virtualnih računala koji ne ovise jedan o drugome odnosno međusobno su izolirani. Stvaranje tih virtualnih računala odnosno virtualnih strojeva radi hipervizor (*eng. hypervisor*). Hipervizor može biti običan softver na računalu domaćinu ili nekakav *firmware*. Još jedna uloga hipervizora je emuliranje hardvera koji se predstavlja virtualnom računalu: procesora, matične ploče, čipseta, mrežnih kartica, grafičkih kartica...

Dvije su vrste virtualizacije:
* na razini hardvera (tip 1)
* na razini operacijskog sustava (tip 2)

Hipervizor presreće instrukcije virtualnih strojeva i provjerava njihovu validnost, a u slučaju emulacije vrši i translaciju instrukcija. Međutim, ovakav pristup je dosta spor pa se koriste značajke poput paravirtualizacije ili hardverske značajke poput Intel VT-x (*Intel Virtualization Technology*) za Intelove procesore i AMD-V (*AMD Virtualization*) za AMD-ove procesore ako virtualni strojevi koriste iste procesore kao i računalo domaćin.
