/*
 * ENS_UART.h
 *
 *  Created on: Mar 13, 2016
 *      Author: steph
 */

#include "UART.h"

#ifndef ENS_WIFI_H_
#define ENS_WIFI_H_

/* Configurations for UART port parameters and registers */
UARTConfig cnf;
USCIUARTRegs uartUsciRegs;
USARTUARTRegs uartUsartRegs;



/* Wi-Fi Module Functions */
void JoinTAMUNetwork();
void ConnectToServer();
void FirstConnectToServer();
void LandmarkComparison();

#endif /* ENS_WIFI_H_ */
