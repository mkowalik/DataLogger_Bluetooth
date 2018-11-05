/*
 * UARTDriver.h
 *
 *  Created on: 02.11.2018
 *      Author: Kowalik
 */

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define	UART_DRIVER_TIMEOUT				500
#define UART_DRIVER_MAX_CALLBACK_NUMBER	10
#define UART_DRIVER_BUFFER_SIZE			128

typedef enum {
	UartDriver_Status_OK = 0,
	UartDriver_Status_UnInitializedErrror,
	UartDriver_Status_BigDataSizeError,
	UartDriver_Status_TimeoutError,
	UartDriver_Status_BufferOverflowError,
	UartDriver_Status_NotReceivingErrror,
	UartDriver_Status_TooManyCallbacksError,
	UartDriver_Status_Error
} UartDriver_Status_TypeDef;

typedef enum {
	UartDriver_State_UnInitialized = 0,
	UartDriver_State_Ready,
	UartDriver_State_ChangingSettings,
	UartDriver_State_Receiving
} UartDriver_State_TypeDef;

typedef struct  {
	UartDriver_State_TypeDef	state;
	UART_HandleTypeDef*			pUartHandler;
	uint32_t					baudRate;
	uint16_t					writeIterator;
	uint8_t						receiveBuffer[UART_DRIVER_BUFFER_SIZE];
	void (*callbacks[UART_DRIVER_MAX_CALLBACK_NUMBER])(uint8_t byte, void* pArgs);
	void*						callbackArgs[UART_DRIVER_MAX_CALLBACK_NUMBER];
	uint16_t					readIterators[UART_DRIVER_MAX_CALLBACK_NUMBER];
} UartDriver_TypeDef;

/*typedef enum {
	UartDriver_Parity_None = 0,
	UartDriver_Parity_OddPairty,
	UartDriver_Parity_EvenParity
} UartDriver_Parity_TypeDef;

typedef enum {
	UartDriver_StopBits_1bit = 0,
	UartDriver_StopBits_2bits
} UartDriver_StopBits_TypeDef;*/

typedef uint16_t UartDriver_CallbackIterator_TypeDef;

UartDriver_Status_TypeDef UartDriver_init(UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, uint32_t baudRate);

UartDriver_Status_TypeDef UartDriver_getBaudRate(UartDriver_TypeDef* pSelf, uint32_t* pRetBaudRate);
UartDriver_Status_TypeDef UartDriver_setBaudRate(UartDriver_TypeDef* pSelf, uint32_t baudRate);

UartDriver_Status_TypeDef UartDriver_sendBytes(UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes);
UartDriver_Status_TypeDef UartDriver_receiveBytes(UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign);

UartDriver_Status_TypeDef UartDriver_sendAndReceive(UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign);

UartDriver_Status_TypeDef UartDriver_setReceiveDataCallback(UartDriver_TypeDef* pSelf, void (*foo)(uint8_t byte, void* pArgs), void* pArgs, UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator);
UartDriver_Status_TypeDef UartDriver_removeReceiveDataCallback(UartDriver_TypeDef* pSelf, UartDriver_CallbackIterator_TypeDef callbackIterator);

UartDriver_Status_TypeDef UartDriver_receivedBytesCallback(UartDriver_TypeDef* pSelf);

#endif /* UART_DRIVER_H_ */
