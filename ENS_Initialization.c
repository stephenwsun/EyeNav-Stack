/*
 * Initialization.c
 *
 *  Created on: Mar 13, 2016
 *  Author: Stephen Sun
 */

#include <msp430.h>
#include <stdio.h>
#include "ENS_Initialization.h"


void Init_EyeNav_Stack(void)
{
	Init_Clock();
	Init_Ports();

}



void Init_Ports(void)
{
	P10SEL = 0x30;	//P10.4 (Tx) and P10.5 (Rx) for Wifly UART
	printf("Wifly Ports Initialized");
}

void Init_Clock(void)
{
	volatile unsigned int i;
	// Increase Vcore setting to level3 to support fsystem=25MHz
	// NOTE: Change core voltage one level at a time..
	SetVcoreUp (0x01);
	SetVcoreUp (0x02);
	SetVcoreUp (0x03);
	UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
	UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
	__bis_SR_register(SCG0);	// Disable the FLL control loop
	UCSCTL0 = 0x0000;           // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_7;        // Select DCO range 50MHz operation
	UCSCTL2 = FLLD_1 + 762;     // Set DCO Multiplier for 25MHz

	__bic_SR_register(SCG0);    // Enable the FLL control loop

	__delay_cycles(782000);
	// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);	// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;                      	// Clear fault flags
	}
	while (SFRIFG1&OFIFG);                   	// Test oscillator fault flag
}

void SetVcoreUp (unsigned int level)
{
	PMMCTL0_H = PMMPW_H;  											// Open PMM registers for write
	SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;// Set SVS/SVM high side new level
	SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;  					// Set SVM low side to new level
	while ((PMMIFG & SVSMLDLYIFG) == 0);  							// Wait till SVM is settled
	PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);  							// Clear already set flags
	PMMCTL0_L = PMMCOREV0 * level;  								// Set VCore to new level
	if ((PMMIFG & SVMLIFG))  										// Wait till new level reached
		while ((PMMIFG & SVMLVLRIFG) == 0);
	SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;// Set SVS/SVM low side to new level
	PMMCTL0_H = 0x00;  												// Lock PMM registers for write access
}

