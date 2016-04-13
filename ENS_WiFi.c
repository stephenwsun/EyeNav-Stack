/*
 * ENS_UART.c
 *
 *  Created on: Mar 13, 2016
 *      Author: steph
 */


#include <msp430x54xA.h>
#include <string.h>
#include <stdio.h>

#include "ENS_WiFi.h"
//#include "Keypad.h"
//#include "Delay.h"
#include "UART.h"

//#include "HAL_SDCard.h"
//#include "diskio.h"
//#include "ff.h"

const char keyword1[12] = "Associated!";
const char keyword2[28] = "Connect to 165.91.61.7:6080";
const char keyword3[22] = "min_landmark_notfound";
const char keyword4[17] = "telecommcorridor";
const char keyword5[19] = "ETIDbusinessoffice";
const char keyword6[18] = "MMETprogramoffice";
const char keyword7[19] = "majorintersection1";
const char keyword8[15] = "womensrestroom";
const char keyword9[13] = "mensrestroom";
const char keyword10[19] = "majorintersection2";
const char keyword11[18] = "eastentrance/exit";
const char keyword12[18] = "westentrance/exit";
const char keyword13[9] = "elevator";
const char keyword14[10] = "stairwell";
const char keyword15[13] = "EETstairwell";

char *JoinKeyword;
char *TCPKeyword;
char *NoLandmarkKeyword;
char *TelecommKeyword;
char *ETIDKeyword;
char *MMETKeyword;
char *Major1Keyword;
char *WomenKeyword;
char *MenKeyword;
char *Major2Keyword;
char *EastKeyword;
char *WestKeyword;
char *ElevatorKeyword;
char *StairwellKeyword;
char *EETKeyword;

const char *Landmark = "";
int LandmarkCompare;
char audio[1000];
unsigned int i;
int x;

//FIL file;                           /* Opened file object */
//FATFS fatfs;                        /* File system object */
//DIRS dir;                           /* Directory object   */
//FRESULT errCode;                    /* Error code object  */
//FRESULT res;                        /* Result object      */
//UINT bytesRead;                     /* Bytes read object  */
//UINT read;                   		/* Read bytes object  */


/* Port Information List so user isn't forced to pass information all the time */
UARTConfig * prtInfList[5];
UARTConfig * prtInf;

/* Define UART buffers for transmit and receive */
#define MAXBUFFSIZE 1600
unsigned char uartTxBuf[MAXBUFFSIZE];
unsigned char uartRxBuf[MAXBUFFSIZE];
unsigned char KeypadBuf[3];


void JoinTAMUNetwork()
{
	_delay_cycles(100000000);

	uartSendDataInt(&cnf,(unsigned char *)"set uart mode 0x00\r", strlen("set uart mode 0x00\r"));

	_delay_cycles(100000000);

	uartSendDataInt(&cnf,(unsigned char *)"$$$", strlen("$$$"));

	_delay_cycles(100000000);

	uartSendDataInt(&cnf,(unsigned char *)"join tamulink-guest\r", strlen("join tamulink-guest\r"));

	_delay_cycles(100000000);

	if(JoinKeyword = strstr(uartRxBuf, keyword1))
	{
		printf("Connected to TAMU Network\n");
	}

	else
	{
		for(x=0;x<3;x++)
		{
			printf("Failed to Connect\n");

			uartSendDataInt(&cnf,(unsigned char *)"join tamulink-guest\r", strlen("join tamulink-guest\r"));

			_delay_cycles(100000000);

			if(JoinKeyword = strstr(uartRxBuf, keyword2))
			{
				printf("Connected to TAMU Network\n");

				break;
			}
		}
	}
}

void ConnectToServer()
{
	setUartRxBuffer(&cnf, uartRxBuf, MAXBUFFSIZE);

	_delay_cycles(100000000);

	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);

	uartSendDataInt(&cnf,(unsigned char *)"scan\r", strlen("scan\r"));

	_delay_cycles(100000000);

	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);

	/*printf("\nXXXXXXXXXUARTRXBUFXXXXXXXX\n");
	printf("%s",cnf.rxBuf);
	printf("\nXXXXXXXXX\n");
	printf("\nXXXXXXXXXUARTRXBUFXXXXXXXX\n");
    printf("%s",uartTxBuf);
	printf("\nXXXXXXXXX\n");*/

	int offset=0;

	uartSendDataInt(&cnf,(unsigned char *)"set comm remote 0\r", strlen("set comm remote 0\r"));
	offset+=strlen("set comm remote 0\r");

	_delay_cycles(100000000);

	//Send string "open 165.91.61.7 5510" to open network socket with server
	uartSendDataInt(&cnf,(unsigned char *)"open 165.91.61.7 6080\r", strlen("open 165.91.61.7 6080\r"));

	offset+=strlen("open 165.91.61.7 6080\r");

	_delay_cycles(100000000);

	if(TCPKeyword = strstr(uartRxBuf, keyword2))
	{
		printf("Connected to Server\n");
	}

	else
	{
		for(x=0;x<3;x++)
		{
			printf("Failed to Connect\n");

			uartSendDataInt(&cnf,(unsigned char *)"open 165.91.61.7 6080\r", strlen("open 165.91.61.7 6080\r"));

			offset+=strlen("open 165.91.61.7 6080\r");

			_delay_cycles(100000000);

			if(TCPKeyword = strstr(uartRxBuf, keyword2))
			{
				printf("Connected to Server\n");

				break;
			}
		}
	}

	uartSendDataInt(&cnf,uartTxBuf,MAXBUFFSIZE);

	//Delay added to account for 250 microseconds before and after commands
	_delay_cycles(100000000);

	LandmarkComparison();

	//Flush the received data from Server
	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);

	_delay_cycles(100000000);

	//Send string "$$$" to initiate command mode on wifi module
	uartSendDataInt(&cnf,(unsigned char *)"$$$", strlen("$$$"));

	//Delay added to account for 250 microseconds before and after commands
	_delay_cycles(100000000);

	uartSendDataInt(&cnf,(unsigned char *)"close\r", strlen("close\r"));

	_delay_cycles(100000000);

	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);
}

void LandmarkComparison()
{
	if(NoLandmarkKeyword = strstr(uartRxBuf, keyword3))
	{
		printf("Landmark was not found\n");

		//strcpy(Landmark,keyword4);
		Landmark = keyword3;

		printf("New Landmark: %s\n",Landmark);
	}

	else if(TelecommKeyword = strstr(uartRxBuf, keyword4))
	{
		printf("Telecomm Corridor landmark found\n");

		LandmarkCompare = strcmp(keyword4,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword4);
			Landmark = keyword4;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(ETIDKeyword = strstr(uartRxBuf, keyword5))
	{
		printf("ETID Business Office landmark found\n");

		LandmarkCompare = strcmp(keyword5,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword5);
			Landmark = keyword5;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(MMETKeyword = strstr(uartRxBuf, keyword6))
	{
		printf("MMET Program Office landmark found\n");

		LandmarkCompare = strcmp(keyword6,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword6);
			Landmark = keyword6;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(Major1Keyword = strstr(uartRxBuf, keyword7))
	{
		printf("Major Intersection 1 landmark found\n");

		LandmarkCompare = strcmp(keyword7,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword7);
			Landmark = keyword7;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(WomenKeyword = strstr(uartRxBuf, keyword8))
	{
		printf("Women's Restroom landmark found\n");

		LandmarkCompare = strcmp(keyword8,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword8);
			Landmark = keyword8;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(MenKeyword = strstr(uartRxBuf, keyword9))
	{
		printf("Men's Restroom landmark found\n");

		LandmarkCompare = strcmp(keyword9,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword9);
			Landmark = keyword9;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(Major2Keyword = strstr(uartRxBuf, keyword10))
	{
		printf("Major Intersection 2 landmark found\n");
		LandmarkCompare = strcmp(keyword10,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword10);
			Landmark = keyword10;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(EastKeyword = strstr(uartRxBuf, keyword11))
	{
		printf("East Entrance/Exit landmark found\n");

		LandmarkCompare = strcmp(keyword11,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword11);
			Landmark = keyword11;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(WestKeyword = strstr(uartRxBuf, keyword12))
	{
		printf("West Entrance/Exit landmark found\n");

		LandmarkCompare = strcmp(keyword12,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword12);
			Landmark = keyword12;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(ElevatorKeyword = strstr(uartRxBuf, keyword13))
	{
		printf("Elevator landmark found\n");

		LandmarkCompare = strcmp(keyword13,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword13);
			Landmark = keyword13;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(StairwellKeyword = strstr(uartRxBuf, keyword14))
	{
		printf("Stairwell landmark found\n");

		LandmarkCompare = strcmp(keyword14,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword14);
			Landmark = keyword14;
		}

		printf("New Landmark: %s\n",Landmark);
	}

	else if(EETKeyword = strstr(uartRxBuf, keyword15))
	{
		printf("EET Stairwell landmark found\n");

		LandmarkCompare = strcmp(keyword15,Landmark);

		if(LandmarkCompare == 0)
		{
			printf("Same Landmark\n");
		}

		else
		{
			printf("Landmark has Changed\n");

			//strcpy(Landmark,keyword15);
			Landmark = keyword15;
		}

		printf("New Landmark: %s\n",Landmark);
	}
}

/* User Destination Input (FOR FUTURE USE) */

/*
void FirstConnectToServer()
{
	setUartRxBuffer(&cnf, uartRxBuf, MAXBUFFSIZE);

	_delay_cycles(100000000);
	//delay_ms(260);

	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);

	uartSendDataInt(&cnf,(unsigned char *)"scan\r", strlen("scan\r"));

	_delay_cycles(100000000);
	//delay_ms(260);

	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);

	printf("\nXXXXXXXXXUARTRXBUFXXXXXXXX\n");
	printf("%s",cnf.rxBuf);
	printf("\nXXXXXXXXX\n");
	printf("\nXXXXXXXXXUARTRXBUFXXXXXXXX\n");
    printf("%s",uartTxBuf);
	printf("\nXXXXXXXXX\n");

	int offset=0;

	uartSendDataInt(&cnf,(unsigned char *)"set comm remote 0\r", strlen("set comm remote 0\r"));
	offset+=strlen("set comm remote 0\r");

	_delay_cycles(100000000);
	//delay_ms(260);

	//Send string "open 165.91.61.7 5510" to open network socket with server
	uartSendDataInt(&cnf,(unsigned char *)"open 165.91.61.7 6090\r", strlen("open 165.91.61.7 6090\r"));

	offset+=strlen("open 165.91.61.7 6090\r");

	_delay_cycles(100000000);
	//delay_ms(260);

	printf("Offset is %d\n",offset);

	uartSendDataInt(&cnf,(unsigned char *)"KEYPAD,\r", strlen("KEYPAD,\r"));
	uartSendDataInt(&cnf,KeypadBuf,MAXBUFFSIZE);
	uartSendDataInt(&cnf,uartTxBuf,MAXBUFFSIZE);


	//Delay added to account for 250 microseconds before and after commands
	_delay_cycles(100000000);
	//delay_ms(260);

	//Flush the received data from Server
	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);

	_delay_cycles(100000000);
	//delay_ms(260);

	//Send string "$$$" to initiate command mode on wifi module
	uartSendDataInt(&cnf,(unsigned char *)"$$$", strlen("$$$"));

	//Delay added to account for 250 microseconds before and after commands
	_delay_cycles(100000000);
	//delay_ms(260);

	uartSendDataInt(&cnf,(unsigned char *)"close\r", strlen("close\r"));

	_delay_cycles(100000000);
	//delay_ms(260);

	readRxBytes(&cnf,uartTxBuf,MAXBUFFSIZE,0);
}*/

