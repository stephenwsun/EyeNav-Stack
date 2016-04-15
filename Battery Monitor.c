/************************************************************************************
 * Battery Monitor.c
 * Author: Stephen Sun
 ************************************************************************************/

#include <msp430x54xA.h>
#include <stdio.h>

#include "Battery Monitor.h"
#include "Delay.h"

#include "HAL_SDCard.h"
#include "diskio.h"
#include "ff.h"

char audio[1000];
unsigned int i;

FIL file;                           /* Opened file object */
FATFS fatfs;                        /* File system object */
DIRS dir;                           /* Directory object   */
FRESULT errCode;                    /* Error code object  */
FRESULT res;                        /* Result object      */
UINT bytesRead;                     /* Bytes read object  */
UINT read;                   		/* Read bytes object  */


void DS2438_HI()
{
	DS2438_DIR|=DS2438_DATA_IN_PIN; //set port as output
	DS2438_OUT|=DS2438_DATA_IN_PIN;	//set port high
}
void DS2438_LO()
{
	DS2438_DIR|=DS2438_DATA_IN_PIN; //set port as output
	DS2438_OUT&=~DS2438_DATA_IN_PIN;//set port low
}

unsigned int ResetDS2438 ( void )
{
  	/* Steps to reset one wire bus
  	 * Pull bus low 
  	 * hold condition for 480us
  	 * release bus
  	 * wait for 60us
  	 * read bus
  	 * if bus low then device present set / return var accordingly
  	 * wait for balance period (480-60)
  	 */
  	int device_present=0;
    DS2438_LO();         						// Drive bus low
    delay_us (480);                             // hold for 480us
    DS2438_DIR &= ~DS2438_DATA_IN_PIN;			//release bus. set port in input mode
    //delay_us (30);
    if(DS2438_IN & DS2438_DATA_IN_PIN)
	{
		device_present=0;
		printf("Device Present\n");
	}
    delay_us (480);								//wait for 480us
  	return device_present;
}
void WriteZero(void)		
{
	/*Steps for master to transmit logical zero to slave device on bus
	 * pull bus low
	 * hold for 60us
	 * release bus
	 * wait for 1us for recovery 
	 */ 
	
	DS2438_LO();         						// Drive bus low
	delay_us (60);								//sample time slot for the slave
	DS2438_DIR &= ~DS2438_DATA_IN_PIN;			//release bus. set port in input mode
    delay_us (5);								//recovery time slot
	
	
}
void WriteOne(void)			
{
	/*Steps for master to transmit logical one to slave device on bus
	 * pull bus low
	 * hold for 5us
	 * release bus
	 * wait for 1us for recovery 
	 */ 
	DS2438_LO();         						// Drive bus low
	delay_us (5);
	DS2438_DIR &= ~DS2438_DATA_IN_PIN;			//release bus. set port in input mode
    delay_us (80);								//sample time slot for the slave
    delay_us (5);								//recovery time slot
    
}


void WriteDS2438 (unsigned char data)
{
   	unsigned char i;
	for(i=8;i>0;i--)
    {
    	
        if(data & 0x01)
        {
            WriteOne();
        }
        else
        {
        	WriteZero();
        }
          	
		data >>=1;
	
    }
    /*if(power == 1)
    { 
    	DS2438_HI();
    	delay_ms(10);
    }*/

}

unsigned int ReadBit (void)
{
	
	/*Steps for master to issue a read request to slave device on bus aka milk slave device
	 * pull bus low
	 * hold for 5us
	 * release bus
	 * wait for 45us for recovery 
	 */ 
	int bit=0;
	DS2438_LO();         						// Drive bus low
	delay_us (5);  								//hold for 5us
	DS2438_DIR &= ~DS2438_DATA_IN_PIN;			//release bus. set port in input mode
    delay_us (10);								//wait for slave to drive port either high or low
    if(DS2438_IN & DS2438_DATA_IN_PIN)			//read bus
	{
		bit=1;									//if read high set bit high
	}
    delay_us (45);								//recovery time slot
	return bit;
	
	
}
unsigned int ReadDS2438 ( void )
{
		  
 	unsigned char i;
 	unsigned int data=0;
	DS2438_DIR &= ~DS2438_DATA_IN_PIN;			//release bus. set port in input mode
 	
 	for(i=8;i>0;i--)
 	{
		data>>=1;
		if(ReadBit())
		{
			data |=0x80;
		}
	}
	
	return(data);
}

float BatteryMonitor(void)
{
	unsigned int voltage;

	//Write Scratchpad
	ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(0x4E);
	WriteDS2438(0x00);
	WriteDS2438(0x07);

	ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(0x48);
	WriteDS2438(0x00);
	delay_ms(20);

	//Read Scratchpad
	ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(DS2438_READ_SCRATCHPAD);
	WriteDS2438(0x00);

	//Voltage Conversion
	ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(DS2438_CONVERT_VOLTAGE);

	//Recall Page 0 to Scratchpad
	ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(DS2438_RECALL_MEMORY);
	WriteDS2438(0x00);

	//Read Scratchpad
	ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(DS2438_READ_SCRATCHPAD);
	WriteDS2438(0x00);

	/*ResetDS2438();
	WriteDS2438(DS2438_SKIP_ROM);
	WriteDS2438(DS2438_READ_SCRATCHPAD);
	WriteDS2438(0x00);*/

	//Disregard
	ReadDS2438();
	ReadDS2438();
	ReadDS2438();

	//Voltage values
	voltage = ReadDS2438();
	printf("%d\n", voltage);
	ReadDS2438();

	if(voltage <= 100)
	{
		/* play error audio file */

		/* Register work area to the default drive */
		f_mount(0, &fatfs);

		/* Open audio file */
		f_open(&file,"error.raw", FA_OPEN_EXISTING|FA_READ);

		i = 0;

		do
		{
			f_read(&file,&audio[i],sizeof("error.raw"),&bytesRead);
			i++;
		} while(audio[i-1]!= 0x00);

		f_close(&file);
	}

	return 0;
}

