/************************************************************************************
 * Keypad.h.c
 * Author: Stephen Sun
 ************************************************************************************/

#ifndef EYENAV_SYSTEM_KEYPAD_H_
#define EYENAV_SYSTEM_KEYPAD_H_

//Define keypad port and pins
#define keyport P4OUT
#define COL1 (0x10 & P1IN)
#define COL2 (0x20 & P1IN)
#define COL3 (0x40 & P1IN)
#define COL4 (0x80 & P1IN)

unsigned char get_key(void);
void DelayMs(int Ms);
void EnterUserDestination();

#endif /* EYENAV_SYSTEM_KEYPAD_H_ */
