/*
 * UARTDriver.h
 *
 *  Created on: 02.11.2018
 *      Author: Kowalik
 */

#ifndef UARTDRIVER_H_
#define UARTDRIVER_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define	UART_TIMEOUT				100
#define MAX_UART_CALLBACK_NUMBER	10

typedef enum {
	UartDriver_Status_OK = 0,
	UartDriver_Status_UnInitializedErrror,
	UartDriver_Status_Errror
} UartDriver_Status_TypeDef;

typedef enum {
	UartDriver_State_UnInitialized = 0,
	UartDriver_State_Initialized,
	UartDriver_State_ChangeSettings,
} UartDriver_State_TypeDef;

typedef struct  {
	UartDriver_State_TypeDef	state;
	UART_HandleTypeDef*			pUartHandler;
	uint16_t	 				callbacksCounter;
	void (*callbacks[MAX_UART_CALLBACK_NUMBER])(uint8_t* pData, uint32_t bytes);
} UartDriver_TypeDef;

UartDriver_Status_TypeDef UartDriver_init(UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, uint32_t baudRate);
UartDriver_Status_TypeDef UartDriver_changeBaudRate(UartDriver_TypeDef* pSelf, uint32_t baudRate);
UartDriver_Status_TypeDef UartDriver_sendBytes(UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint32_t bytes);
UartDriver_Status_TypeDef UartDriver_setReceiveDataCallback(UartDriver_TypeDef* pSelf, void (*foo)(uint8_t* pData, uint32_t bytes));
#endif /* UARTDRIVER_H_ */
