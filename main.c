#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ENS_Initialization.h"

/*
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    printf("The EyeNav Stack will now begin initializing...");
	
    Init_EyeNav_Stack();

	return 0;
}


