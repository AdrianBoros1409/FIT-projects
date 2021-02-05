/************************************************************************************/
/*                                                                                  */
/*  Projekt IMP - Testovanie mikrokontroleru                                        */
/*                                                                                  */
/*  Veduci: Strnadel Josef, Ing., Ph.D.                                             */
/*                                                                                  */
/*  Rieseni vytvoril a odovzdava: (Adrian Boros, xboros03)                          */
/*   																			    */
/*	Datum: 15.12.2019																*/
/*                                                                                  */
/************************************************************************************/

#include "MK60D10.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/** Mapovanie LED na specificke piny portov: **/
#define LED_D9  0x20      // Port B, bit 5
#define LED_D10 0x10      // Port B, bit 4
#define LED_D11 0x8       // Port B, bit 3
#define LED_D12 0x4       // Port B, bit 2
#define LED_R_MASK 0x3C

/** Funkcia na spozdenie **/
void delay(long long bound) {
  long long i;
  for(i=0;i<bound;i++);
}

/** Inicializacia MCU - zakladne nastavenie hodin, vypnutie watchdogu **/
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/** Pipnutie cez bzuciak **/
void Beep() {
    for (unsigned int q=0; q<500; q++) {
        PTA->PDOR = GPIO_PDOR_PDO(0x0010);
        delay(500);
        PTA->PDOR = GPIO_PDOR_PDO(0x0000);
        delay(500);
    }
}

/** Inicializacia portov **/
void PortsInit() {

    /** Povolenie hodin pre PORTA a PORTB **/
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK;

    /** PORT A - pripojeny bzuciak **/
    PORTA->PCR[4] = PORT_PCR_MUX(0x01);

    /** PORT B - pripojene LEDky **/
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); /** D9 LED **/
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); /** D10 LED **/
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); /** D11 LED **/
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); /** D12 LED **/

    /** Nastavenie portov ako vystup **/
    PTA->PDDR =  GPIO_PDDR_PDD(0x0010);
    PTB->PDDR =  GPIO_PDDR_PDD(LED_R_MASK);
    PTB->PDOR |= GPIO_PDOR_PDO(LED_R_MASK); /** Vypnutie vsetkych LEDiek **/
}

/** Funkcia realizujica kratke bliknutie **/
void flash() {
	PTB->PDOR &= ~GPIO_PDOR_PDO(LED_R_MASK);
	delay(300000);
	PTB->PDOR |= GPIO_PDOR_PDO(LED_R_MASK);
	delay(300000);
}

static int i = 0;

/** Funkcia realizujuca MARCH X test RAM pamate **/
bool memTestDevice(){
	int backup[9][7];					/** Pole pre zalohu hodnot **/
	volatile char* start_addr = (char*)0x20000120;		/** Pociatocna adresa **/
	memset(backup, 0, sizeof(backup[0][0]) * 9 * 7);
	volatile char *address = (char *)start_addr;
	unsigned long *backup_addr = (long*)0x20000120;
	for(int i = 10; i > 0; i--)
	{
		for(int k = 0; k < 9; k++)
		{
			for(int j = 0; j < 7; j++)
			{
				backup[k][j] = *backup_addr;		/** Zaloha hodnot **/
				*backup_addr = 0x00000000;			/** Vynulovanie oblasti **/
				backup_addr+=1;
			}
		}

		address = (char *)start_addr;
		for(int k = 0; k < 9; k++)
		{
			for(int j = 0; j < 7; j++)			/** Vzostupne kontrola hodnoty 0x00 a zapis hodnoty 0x11 **/
			{
				for (int offset = 0; offset < 4; offset++)
				{
					if (address[offset] != 0x00)
						return false;
					else
						address[offset] = 0x11;
				}
				address+=4;
			}
		}

		address = (char *)start_addr + 248;		/** Zostupne kontrola hodnoty 0x11 a zapis hodnoty 0x00 **/
		for(int k = 0; k < 9; k++)
		{
			for(int j = 0; j < 7; j++)
			{
				for (int offset = 0; offset < 4; offset++)
				{
					if (address[offset] != 0x11)
						return false;
					else
						address[offset] = 0x00;
				}
				address-=4;
			}
		}

		backup_addr = (long *)start_addr;		/** Kontrola ci je cela oblast rovna 0x00000000 **/
		for(int k = 0; k < 9; k++)
		{
			for(int j = 0; j < 7; j++)
			{
				if(*backup_addr != 0x00000000)
					return false;
				backup_addr+=1;
			}
		}
		backup_addr = (long *)start_addr;	/** Navratenie zalohy do povodneho stavu **/
		for(int k = 0; k < 9; k++)
		{
			for(int j = 0; j < 7; j++)
			{
				*backup_addr = backup[k][j];
				backup_addr+=1;
			}
		}
		start_addr +=252;				/** Posun na dalsiu oblast na otestovanie **/
	}
	return true;
}

/** Funkcia realizujuca testovanie CPU registrov R2 - R12 **/
int registerTest(){

	/** Register R2 **/
	__asm("LDR R2, =0xAAAAAAAA");
	__asm("CMP R2, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R2, =0x55555555");
	__asm("CMP R2, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R3 **/
	__asm("LDR R3, =0xAAAAAAAA");
	__asm("CMP R3, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R3, =0x55555555");
	__asm("CMP R3, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R4 **/
	__asm("LDR R4, =0xAAAAAAAA");
	__asm("CMP R4, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R4, =0x55555555");
	__asm("CMP R4, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R5 **/
	__asm("LDR R5, =0xAAAAAAAA");
	__asm("CMP R5, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R5, =0x55555555");
	__asm("CMP R5, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R6 **/
	__asm("LDR R6, =0xAAAAAAAA");
	__asm("CMP R6, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R6, =0x55555555");
	__asm("CMP R6, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R8 **/
	__asm("LDR R8, =0xAAAAAAAA");
	__asm("CMP R8, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R8, =0x55555555");
	__asm("CMP R8, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R9 **/
	__asm("LDR R9, =0xAAAAAAAA");
	__asm("CMP R9, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R9, =0x55555555");
	__asm("CMP R9, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R10 **/
	__asm("LDR R10, =0xAAAAAAAA");
	__asm("CMP R10, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R10, =0x55555555");
	__asm("CMP R10, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R11 **/
	__asm("LDR R11, =0xAAAAAAAA");
	__asm("CMP R11, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R11, =0x55555555");
	__asm("CMP R11, R1");
	__asm("BNE ERROR_LABEL");

	/** Register R12 **/
	__asm("LDR R12, =0xAAAAAAAA");
	__asm("CMP R12, R0");
	__asm("BNE ERROR_LABEL");
	__asm("LDR R12, =0x55555555");
	__asm("CMP R12, R1");
	__asm("BNE ERROR_LABEL");
	__asm("B TEST_OK");

	int status = 0;
	__asm("ERROR_LABEL:");
	__asm ("MOV %0, $1;" /** Ak test nepresiel vracia sa hodnota 1 **/
	    :"=r"(status)
	    :);
	__asm("B TEST_END");

	__asm("TEST_OK:");
	__asm ("MOV %0, $0;" /** Ak test presiel vracia sa hodnota 0 **/
		  :"=r"(status)
		  :);

	__asm("TEST_END:");
	return status;
}

int main(void)
{
    /* Write your code here */
	__asm("PUSH {r0-r12}");			/** Zaloha obsahu registrov R0 - R12 **/
    MCUInit();
    PortsInit();
	__asm("LDR R0, =0xAAAAAAAA");	/** Prvy testovaci vzor **/
	__asm("LDR R1, =0x55555555");	/** Druhy testovaci vzor **/
	int return_code = registerTest();
	__asm("POP {r0-r12}");			/** Obnova stavu registrov **/
	if (return_code == 0) {			/** Ak uspel test na registre, 5-krat bzuciak pipne **/
	    for(unsigned int i=0; i<5; i++) {
	    	Beep();
	        delay(70000);
	    }
	}
	else {							/** V opacnom pripade zacnu blikat LEDky **/
		for(unsigned int i=0; i<5; i++) {
		    flash();
		    delay(3000000);
		}
	}
	delay(3000000);
	if(memTestDevice() == true) {	/** Ak presiel test na RAM, 5-krat sa pipne **/
		for(unsigned int i=0; i<5; i++) {
			Beep();
			delay(70000);
		}
	}
	else {
		for(unsigned int i=0; i<5; i++) {	/** V opacnom pripade zacnu blikat LED **/
			flash();
			delay(3000000);
		}
	}

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
