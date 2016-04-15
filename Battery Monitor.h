/************************************************************************************
 * Battery Monitor.h
 * Author: Stephen Sun
 ************************************************************************************/

#ifndef BATTERY_MONITOR_DS2438_H_
#define BATTERY_MONITOR_DS2438_H_

#define DS2438_OUT 					P7OUT
#define DS2438_DIR 					P7DIR
#define DS2438_SEL					P7SEL
#define DS2438_IN					P7IN
#define DS2438_DATA_IN_PIN          BIT4
#define DS2438_VCC			        BIT3
#define DS2438_GND          		BIT1

//DS2438 ROM Function Commands
#define DS2438_SKIP_ROM             0xCC

//DS2438 Memory Function Commands
#define DS2438_RECALL_MEMORY		0xB8
#define DS2438_READ_SCRATCHPAD      0xBE

//DS2438 Register Commands
#define DS2438_CONVERT_VOLTAGE      0xB4

void BatteryMonitorInitialize();
unsigned int ResetDS2438 ( void );
void DS2438_HI(void);
void DS2438_LO(void);
void WriteZero(void);
void WriteOne(void);
unsigned int ReadBit(void);
void WriteDS2438 (unsigned char);
unsigned int ReadDS2438 ( void );
float BatteryMonitor(void);
#endif /*BATTERY_MONITOR_DS2438_H_*/
