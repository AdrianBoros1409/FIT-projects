
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**											  Vypracoval: Adrián Boros [xboros03], 2018/2019
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int errflg;
int solved;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	L->Act = NULL;
	L->First = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	tDLElemPtr toDelete;
	while (L->First != NULL)	//budem rusit kym prvy prvok nebude ukazovat na NULL
	{
		toDelete = L->First;
		L->First = L->First->rptr;	//posuniem ukazatel na dalsi prvok zoznamu
		free(toDelete);
	}
	L->Act = NULL;	//ukazatele nastavim na NULL, zoznam bude prazdny
	L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr newNode = (tDLElemPtr)malloc(sizeof(struct tDLElem));	//alokacia pamate pre novy prvok
	if (newNode == NULL)	//ak nastala chyba pri alokovani pamate, volam funkciu DLError()
		DLError();
	else
	{
		newNode->data = val;	//priradenie hodnoty val
		newNode->rptr = L->First;	//pravy ukazatel noveho prvku ukazuje na prvy prvok
		newNode->lptr = NULL;	//lavy ukazatel noveho prvku ukazuje na NULL
		if (L->First != NULL)	//ak zoznam nie je prazdny
			L->First->lptr = newNode;	//prvy prvok bude dolava ukazovat na novy prvok
		else
		{
			L->Last = newNode;	//vkladanie do prazdneho zoznamu
		}
		L->First = newNode;	//prvy prvok je novym prvkom
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr newNode = (tDLElemPtr)malloc(sizeof(struct tDLElem));	////alokacia pamate pre novy prvok
	if (newNode == NULL)	//ak nastala chyba pri alokovani pamate, volam funkciu DLError()
		DLError();
	else
	{
		newNode->data = val;	//priradenie hodnoty val
		newNode->rptr = NULL;	//pravy ukazatel noveho prvku ukazuje na NULL
		newNode->lptr = L->Last;	//lavy ukazatel noveho prvku ukazuje na posledny prvok
		if (L->First != NULL)	//ak zoznam nie je prazdny
		{
			L->Last->rptr = newNode;	//pravy ukazatel posledneho prvku ukazuje na novy prvok
		}
		else
		{
			L->First = newNode;	//ak je zoznam prazdny, novy prvok bude sucasne aj prvym aj poslednym prvkom
		}
		L->Last = newNode;	//posledny prvok bude novo vlozeny prvok
	}
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->First == NULL)	//ak je zoznam prazdny, vola sa funkcia DLError()
		DLError();
	else
		*val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->Last == NULL)	//mozno upravit na L->First
		DLError();
	else
		*val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	tDLElemPtr toDelete;
	if (L->First != NULL)	//kontrola ci je zoznam prazdny, ak je tak sa nic nedeje, ak nie je maze sa prvy prvok
	{
		toDelete = L->First;
		if (L->Act == L->First)	//ak je prvy prvok aktivny, rusi sa jeho aktivita
			L->Act = NULL;
		if (L->First == L->Last)	//ak je v zozname jediny prvok, zrusi sa, nastavia sa jeho ukazatele na NULL
		{
			L->First = NULL;
			L->Last = NULL;
		}
		else
		{
			L->First = L->First->rptr;	//druhy prvok sa stane prvym
			L->First->lptr = NULL;	//lavy ukazatel druheho prvku bude ukazovat na NULL
		}
		free(toDelete);	//zrusim prvy prvok v zozname
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	tDLElemPtr toDelete;
	if (L->Last != NULL)	//kontrola ci je zoznam prazdny, ak je tak sa nic nedeje, ak nie je maze sa prvy prvok
	{
		toDelete = L->Last;
		if (L->Act == L->Last)	//ak je aktivny posledny prvok, jeho aktivita sa rusi
			L->Act = NULL;
		if (L->First == L->Last)	//ak je jediny prvok v zozname, nastavia sa jeho ukazatele na NULL
		{
			L->First = NULL;
			L->Last = NULL;
		}
		else
		{
			L->Last = L->Last->lptr;	//novym poslednym prvok bude predposledny prvok
			L->Last->rptr = NULL;	//pravy ukazatel posledneho prvku bude ukazovat na NULL
		}
		free(toDelete);	//zrusim posledny prvok v zozname
	}
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if (L->Act != NULL)	//kontrola ci je zoznam prazdny, ak nie tak sa maze prvok za aktualnym
	{
		if (L->Act->rptr != NULL)	//kontrola ci aktualny prvok nie je poslednym v zozname
		{
			tDLElemPtr toDelete;
			toDelete = L->Act->rptr;	//ukazatel na prvok ktory ma byt ruseny
			L->Act->rptr = toDelete->rptr;	//pravy ukazatel aktualneho prvku ukazuje na prvok za prvkom ktory ma byt vymazany
			if (toDelete == L->Last)	//ak my byt rusenym prvkom posledny
				L->Last = L->Act;		//aktivnym prvkom sa stava posledny prvok
			else
				toDelete->rptr->lptr = L->Act;	//prvok za rusenym ukazuje dolava na aktualny prvok
			free(toDelete);	//mazanie prvku
		}
	}
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if (L->Act != NULL)	//kontrola ci je zoznam prazdny, ak nie tak sa maze prvok pred aktualnym
	{
		if (L->Act->lptr != NULL)	//kontrola ci aktualny prvok nie je prvy v zozname
		{
			tDLElemPtr toDelete;
			toDelete = L->Act->lptr;	//ukazatel na prvok ktory ma byt mazany
			L->Act->lptr = toDelete->lptr;	//lavy ukazatel aktualneho prvku bude ukazovat na prvok pred mazanym prvkom
			if (toDelete == L->First)	//ak sa ma mazat prvy prvok, prvy po prvom bude aktivny
				L->First = L->Act;
			else
				toDelete->lptr->rptr = L->Act;	//pravy ukazatel prvku pred mazanym prvkom bude ukazovat na aktualny prvok
			free(toDelete);
		}
	}
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL)	//kontrola ci je zoznam aktivny, ak nie nic sa nedeje
	{
		tDLElemPtr newNode = (tDLElemPtr)malloc(sizeof(struct tDLElem));	//alokacia pamate pre novy prvok
		if (newNode == NULL)	//kontrola ci sa podarila alokacia, ak nie vola sa funckia DLError()
			DLError();
		else
		{
			newNode->data = val;
			newNode->rptr = L->Act->rptr;	//pravy ukazatel noveho prvku ukazuje na prvok za aktualnym
			newNode->lptr = L->Act;	//lavy ukazatel noveho prvku ukazuje na aktualny prvok
			L->Act->rptr = newNode;	//pravy ukazatel prveho prvku ukazuje na novy prvok
		}
		if (L->Act == L->Last)	//ak je aktualny prvok posledny, novym poslednym prvkom bude novo pridany
			L->Last = newNode;
		else
			newNode->rptr->lptr = newNode;	//lavy ukazatel prvku za aktivnym bude ukazovat na novy prvok
	}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL)
	{
		tDLElemPtr newNode = (tDLElemPtr)malloc(sizeof(struct tDLElem));	//alokacia pamate pre novy prvok
		if (newNode == NULL)	//kontrola ci sa podarila alokacia, ak nie vola sa funckia DLError()
			DLError();
		else
		{
			newNode->data = val;
			newNode->rptr = L->Act;	//pravy ukazatel noveho prvku bude ukazovat na aktualny prvok
			newNode->lptr = L->Act->lptr;	//lavy ukazatel noveho prvku bude ukazovat na prvok za aktualnym
			L->Act->lptr = newNode;	//lavy ukazatel aktualneho prvku ukazuje na novy prvok
		}	
		if (L->Act == L->First)	//ak je aktualny prvok aj prvy, novym prvym prvkom bude novo pridany
			L->First = newNode;
		else
			newNode->lptr->rptr = newNode;	//pravy ukazatel prvku za aktualnym bude ukazovat na novy prvok
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if (L->Act == NULL)
		DLError();
	else
		*val = L->Act->data;	//*val bude obsahovat hodnotu aktivneho prvku
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act != NULL)
		L->Act->data = val;	//ak je zoznam aktivny prepise sa obsah aktivneho prvku na val
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL)	//kontrola ci je zoznam aktivny
	{
		if (L->Act->rptr == NULL)	//ak je aktivny prvok posledny v zozname, zoznam bude neaktivny
			L->Act = NULL;
		else
			L->Act = L->Act->rptr;	//posuniem aktivitu na nasledujuci prvok
	}
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL)	//kontrola ci je zoznam aktivny
	{
		if (L->Act->lptr == NULL)	//ak je aktivny prvok prvy v zozname, zoznam bude neaktivny
			L->Act = NULL;
		else
			L->Act = L->Act->lptr;	//posuniem aktivitu na predchadzajuci prvok
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return (L->Act) ? TRUE : FALSE;		//ak ze zoznam aktivny vracia TRUE, ak nie je aktivny vracia sa FALSE
}

/* Konec c206.c*/
