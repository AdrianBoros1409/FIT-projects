# WHOIS tazatel
*************

Autor:	Adrián Boros (xboros03)  
E-mail: xboros03@stud.fit.vutbr.cz  
Dátum:	2.11.2019  

**Popis programu:**
	Program slúži ako WHOIS tazatel pre zistenie podrobností o vlastníkovi domény. Na vstupe špecifikuje IP adresu (IPv4 alebo IPv6) alebo hostname. Program najprv vypíše DNS informácie a následne aj informácie ktoré obdržal od WHOIS serveru. 

**Zoznam súborov:**
- Makefile 		- preloženie projektu
- isa-tazatel.c 	- subor so zdrojovym kodom
- manual.pdf		- dokumentacia	
- Readme			- kratky textovy popis programu

**Makefile:**
- $make		- preloženie projektu
- $make clean	- vymazanie preloženého projektu
- $make tar	- vytvorenie .tar archívu

**Spustenie:**
`$./isa-tazatel {-q <IP|hostname>} {-w <IP|hostname WHOIS serveru>} [-d <IP>] -h`
Poradie argumentov je lubovolne.

**Popis argumentov:**
- `q <IP|hostname>` - IP adresa alebo hostname domény, ktorého informácie sa majú získať, povinný argument
- ` w <IP|hostname WHOIS serveru>` - IP adresa alebo hostname WHOIS serveru ktorý bude dotazovaný, povinný argument
- ` d <IP>` - IP adresa DNS serveru ktorý bude dotazovaný, nepovinný argument
- `h` - vypise informácie o programe aj príklad spustenia

**Príklady spustenia:**
`$./isa-tazatel -w whois.nic.cz -q seznam.cz`
`$./isa-tazatel -w whois.ripe.net -q aktuality.sk -d 8.8.8.8`

**Rozšírenie:**
Je možné sa dotazovať na špecifikovaný DNS server, nielen na DNS resolver z operačného systému.
