/************************************************************************************
 * Keypad.c
 * Author: Stephen Sun
 ************************************************************************************/

#include <msp430x54xA.h>
#include <stdio.h>

#include "Wifi Module.h"
#include "Keypad.h"
#include "delay.h"

unsigned char Value1=0;
unsigned char Value2=0;
unsigned char Value3=0;
unsigned int count=0;
int x;

unsigned char i,k,key=0;
unsigned char Key_Val[] = {' ','C','1','2','3','D','4','5','6','E','7','8','9','F','*','0','#'};

void EnterUserDestination()
{
	/* Set Variables */
	unsigned int count=0;
	unsigned char value=0;
	unsigned char destination[3],x;

	for(x=0;x<3;x++)
	{
		printf("Enter a number");
		while((count = get_key())==0);
		value = Key_Val[count],&destination[x];
		printf("Value = %c\n", value);

		delay_ms(100);
	}

	memcpy(destination,KeypadBuf,3);
}

//This function scans for key presses
unsigned char get_key(void)
{
    k=1;
    for(i=0;i<4;i++)
    {
    	//Scans for a key by sending '0' on rows
        keyport = ((0x01<<i) ^ 0xff);

        //When a key is pressed, the number/character is returned
        if(!COL1)
        {
            key = k+0;
            while(!COL1);
            return key;
        }

        if(!COL2)
        {
            key = k+1;
            while(!COL2);
            return key;
        }

        if(!COL3)
        {
            key = k+2;
            while(!COL3);
            return key;
        }

        if(!COL4)
        {
            key = k+3;
            while(!COL4);
            return key;
        }

        k+=4;
        keyport |= (0x01<<i);
    }

    return 0;
}

//Delay function to account for mechanical delay
void DelayMs(int Ms)
{
    int k;
    while(Ms>0)
    {
        for(k=0;k<104;k++);
        {
            Ms--;
        }
    }
}



