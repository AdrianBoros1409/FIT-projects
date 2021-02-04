# 1. Úloha IOS (2018)
## Popis úlohy
Cílem úlohy je vytvorit skript (tzv. wrapper), který bude spouštet textový editor. Skript si bude pamatovat, které soubory byly v jakém adresári prostrednictvím skriptu wedi editovány. Pokud bude skript spušten bez parametru, vybere skript soubor, který má být editován.

## Specifikace chování skriptu
**JMÉNO**
- **wedi** - wrapper textového editoru s možností automatického výberu souboru 

**POUŽITÍ**
- wedi SOUBOR
- wedi [ADRESÁR]
- wedi -m [ADRESÁR]
- wedi -l [ADRESÁR]
- wedi -b|-a DATUM [ADRESÁR]

**POPIS**
- Pokud byl skriptu zadán soubor, bude editován.
- Pokud zadaný argument odpovídá existujícímu adresári, skript z daného adresáre vybere soubor pro editaci. Výber souboru je následující.
    -  Pokud bylo v daném adresári editováno skriptem více souboru, vybere se soubor, který byl pomocí skriptu editován jako **poslední**. Editací souboru se myslí treba i prohlížení jeho obsahu pomocí skriptu (tj. není nutné, aby byl soubor zmenen).
    -  Pokud byl zadán argument -m, vybere se soubor, který byl pomocí skriptu editován **nejčasteji**.
    -  Pokud nebyl v daném adresári editován ješte žádný soubor, jedná se o chybu.
- Pokud nebyl zadán adresár, predpokládá se aktuální adresár.
- Skript dokáže také zobrazit seznam všech souboru (argument -l), které byly v daném adresári editovány.
- Pokud byl zadán argument **-b** resp. **-a** (before, after), skript zobrazí seznam souboru, které byly editovány pred resp. po zadaném datu v čase 00:00:00.0 vcetne. DATUM je formátu YYYY-MM-DD.

**NASTAVENÍ A KONFIGURACE**
- Skript si pamatuje informace o svém spouštení v souboru, který je dán promennou **WEDI_RC**. Formát souboru není specifikován.
    - Pokud není promenná nastavena, jedná se o chybu.
    - Pokud soubor na ceste dané promennou **WEDI_RC** neexistuje, soubor bude vytvoren včetne cesty k danému souboru (pokud i ta neexistuje).
- Skript spouští editor, který je nastaven v promenné **EDITOR**. Pokud není promenná EDITOR nastavená, respektuje promennou VISUAL. Pokud ani ta není nastavená, použije se príkaz vi.

**NÁVRATOVÁ HODNOTA**
- Skript vrací úspech v prípade úspešné operace nebo v prípade úspešné editace. Pokud editor vrátí chybu, skript vrátí stejný chybový návratový kód. Interní chyba skriptu bude doprovázena chybovým hlášením.

**POZNÁMKY**
- Skript nebere v potaz soubory, se kterými dríve počítal a které jsou nyní smazané.
- Pri rozhodování relativní cesty adresáre je doporucené používat reálnou cestu (realpath). Duvod např.:
`$ wedi .`
`$ wedi ‘pwd‘`

**Implementacní detaily**
- Skript by mel mít v celém behu nastaveno **POSIXLY_CORRECT=yes**.
- Skript by mel bežet na všech bežných shellech (dash, ksh, bash). Mužete použít GNU rozšírení pro sed ci awk. Jazyk Perl nebo Python povolen není.
- Skript by mel ošetrit i chybový prípad, že na daném stroji utilita realpath není dostupná.
- Referencní stroj neexistuje. Skript musí bežet na bežne dostupných OS GNU/Linux a *BSD. Ve školním prostredí máte k dispozici pocítace v laboratorích (CentOS), stroj merlin (CentOS) a eva (FreeBSD). Pozor, na stroji merlin je shell /bin/ksh symbolický odkaz na bash (tj. nechová se
jako Korn shell jako na obvyklých strojích).
- Skript nesmí používat docasné soubory. Povoleny jsou docasné soubory neprímo tvorené príkazem sed (napr. argument sed -i).

**Príklady použití**
Následující príklady predpokládají, že skript wedi je dostupný v jedné z cest v
promenné PATH.
1. Editace ruzných souboru:  
`$ export WEDI_RC=$HOME/.config/wedirc`  
`$ date`  
`Mon 2 Mar 14:45:24 CET 2018`  
`$ wedi ~/.bashrc`    
`$ wedi ~/.ssh/config`  
`$ wedi ~/.local/bin/wedi`  
`$ wedi ~/.bashrc`  
`$ wedi ~/.indent.pro`  
`$ date`  
`Wed 4 Mar 19:51:02 CET 2018`  
`$ wedi ~/.bashrc`  
`$ wedi ~/.vimrc`  
2. Opetovná editace:  
`$ cd ~/.ssh`  
`$ wedi`  
... spustí se editace souboru ~/.ssh/config  
`$ wedi ~`  
... spustí se editace souboru ~/.vimrc  
`$ cd`  
`$ wedi -m`  
... spustí se editace souboru ~/.bashrc  
3. Zobrazení seznamu editovaných souboru:  
`$ wedi -l $HOME`  
`.bashrc`  
`.indent.pro`  
`.vimrc`  
`$ wedi -b 2018-03-04 $HOME`  
`.bashrc`  
`.indent.pro`  
`$ wedi -a 2018-03-03 $HOME`  
`.bashrc`  
`.vimrc`  
