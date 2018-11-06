/*
 * UARTDriver.c
 *
 *  Created on: 02.11.2018
 *      Author: Kowalik
 */

#include "uart_driver.h"
#include <string.h>

static UartDriver_Status_TypeDef UartDriver_TransmitBytes(UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes);
static UartDriver_Status_TypeDef UartDriver_startReceiver(UartDriver_TypeDef* pSelf);
static UartDriver_Status_TypeDef UartDriver_stopReceiver(UartDriver_TypeDef* pSelf);

UartDriver_Status_TypeDef UartDriver_init(UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, uint32_t baudRate){

	if (pSelf->state != UartDriver_State_UnInitialized){
		return UartDriver_Status_Error;
	}

	pSelf->pUartHandler = pUartHandler;
	pSelf->receiveBufferIterator = 0;
	pSelf->transmitInProgress = 0;
	memset((char*)pSelf->receiveBuffer, 0, UART_DRIVER_BUFFER_SIZE);

	for (uint16_t i=0; i<UART_DRIVER_MAX_CALLBACK_NUMBER; i++){
		pSelf->callbacks[i] = NULL;
		pSelf->callbackArgs[i] = NULL;
		pSelf->readIterators[i] = 0;
	}

	if (pSelf->pUartHandler->gState == HAL_UART_STATE_RESET){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_Ready; //Because UartDriver_setBaudRate() needs state to be UartDriver_State_Initialized.

	if (pSelf->pUartHandler->Init.BaudRate != baudRate){
		UartDriver_setBaudRate(pSelf, baudRate);
	}

	pSelf->baudRate = baudRate;

	return UartDriver_startReceiver(pSelf);
}

UartDriver_Status_TypeDef UartDriver_getBaudRate(UartDriver_TypeDef* pSelf, uint32_t* pRetBaudRate){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	*pRetBaudRate = pSelf->baudRate;

	return UartDriver_Status_OK;

}

UartDriver_Status_TypeDef UartDriver_setBaudRate(UartDriver_TypeDef* pSelf, uint32_t baudRate){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	UartDriver_Status_TypeDef ret;
	UartDriver_State_TypeDef prevState = pSelf->state;

	if (pSelf->state == UartDriver_State_Receiving){
		if ((ret = UartDriver_stopReceiver(pSelf)) != UartDriver_Status_OK){
			return ret;
		}
	}

	if (pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_ChangingSettings;

	if (HAL_UART_DeInit(pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->pUartHandler->Init.BaudRate = baudRate;

	if (HAL_UART_Init(pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->baudRate = baudRate;
	pSelf->state = UartDriver_State_Ready;

	if (prevState == UartDriver_State_Receiving){
		return UartDriver_startReceiver(pSelf);
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendBytes(UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	UartDriver_Status_TypeDef ret;

	if ((ret = UartDriver_TransmitBytes(pSelf, pBuffer, bytes)) != UartDriver_Status_OK){
		return ret;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receiveBytesTerminationSign(UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint16_t tmpIterator = pSelf->receiveBufferIterator;
	uint16_t charCounter = 0;

	while (charCounter < bufferSize){
		if (pSelf->receiveBufferIterator != tmpIterator){
			pReceiveBuffer[charCounter] = pSelf->receiveBuffer[tmpIterator];
			if (pReceiveBuffer[charCounter++] == terminationSign){
				break;
			} else {
				tmpIterator = (tmpIterator+1) % UART_DRIVER_BUFFER_SIZE;
			}
		}
	}

	if (charCounter == UART_DRIVER_BUFFER_SIZE && pReceiveBuffer[charCounter-1] != terminationSign){
		return UartDriver_Status_BufferOverflowError;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receiveNBytes(UartDriver_TypeDef* pSelf, uint8_t* pReceiveBuffer, uint16_t bytesToRead){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint16_t tmpIterator = pSelf->receiveBufferIterator;
	uint16_t charCounter = 0;

	while (charCounter < bytesToRead){
		if (pSelf->receiveBufferIterator != tmpIterator){
			pReceiveBuffer[charCounter++] = pSelf->receiveBuffer[tmpIterator];
			tmpIterator = (tmpIterator+1) % UART_DRIVER_BUFFER_SIZE;
		}
	}

	if (charCounter == UART_DRIVER_BUFFER_SIZE){
		return UartDriver_Status_BufferOverflowError;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendAndReceiveTerminationSign(UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, \
		uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;
	if ((ret = UartDriver_TransmitBytes(pSelf, pSendBuffer, bytesToSend)) != UartDriver_Status_OK){
		return ret;
	}

	return UartDriver_receiveBytesTerminationSign(pSelf, pReceiveBuffer, bufferSize, terminationSign);
}

UartDriver_Status_TypeDef UartDriver_sendAndReceiveNBytes(UartDriver_TypeDef* pSelf, uint8_t* pSendBuffer, uint16_t bytesToSend, \
		uint8_t* pReceiveBuffer, uint16_t bytesToRead){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	UartDriver_Status_TypeDef ret = UartDriver_Status_OK;
	if ((ret = UartDriver_TransmitBytes(pSelf, pSendBuffer, bytesToSend)) != UartDriver_Status_OK){
		return ret;
	}

	return UartDriver_receiveNBytes(pSelf, pReceiveBuffer, bytesToRead);
}

UartDriver_Status_TypeDef UartDriver_setReceiveDataCallback(UartDriver_TypeDef* pSelf, void (*foo)(uint8_t byte, void* pArgs), void* pArgs, UartDriver_CallbackIterator_TypeDef* pRetCallbackIterator){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	uint16_t i;
	for (i=0; i<UART_DRIVER_MAX_CALLBACK_NUMBER; i++){
		if (pSelf->callbacks[i] == NULL){
			pSelf->callbacks[i] = foo;
			pSelf->callbackArgs[i] = pArgs;
			pSelf->readIterators[i] = pSelf->receiveBufferIterator;
			break;
		}
	}

	if (i == UART_DRIVER_MAX_CALLBACK_NUMBER){
		return UartDriver_Status_TooManyCallbacksError;
	}

	if (pRetCallbackIterator != NULL){
		*pRetCallbackIterator = (UartDriver_CallbackIterator_TypeDef)i;
	}


	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_removeReceiveDataCallback(UartDriver_TypeDef* pSelf, UartDriver_CallbackIterator_TypeDef callbackIterator){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Ready && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_Error;
	}

	if (pSelf->callbacks[callbackIterator] == NULL){
		return UartDriver_Status_Error;
	}

	pSelf->callbacks[callbackIterator] = NULL;
	pSelf->callbackArgs[callbackIterator] = NULL;
	pSelf->readIterators[callbackIterator] = 0;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receivedBytesCallback(UartDriver_TypeDef* pSelf){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_NotReceivingErrror;
	}

	for (uint16_t fooIt=0; fooIt<UART_DRIVER_MAX_CALLBACK_NUMBER; fooIt++){
		if (pSelf->callbacks[fooIt] != NULL){
			uint16_t rdIt = pSelf->readIterators[fooIt];
			pSelf->readIterators[fooIt] = (pSelf->readIterators[fooIt] + 1) % UART_DRIVER_BUFFER_SIZE;
			pSelf->callbacks[fooIt](pSelf->receiveBuffer[rdIt], pSelf->callbackArgs[fooIt]);
		}
	}

	pSelf->receiveBufferIterator = (pSelf->receiveBufferIterator + 1) % UART_DRIVER_BUFFER_SIZE;

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, pSelf->receiveBuffer+pSelf->receiveBufferIterator, 1) != HAL_OK){
		return UartDriver_Status_Error;
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_transmitCompleteCallback(UartDriver_TypeDef* pSelf){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->transmitInProgress == 0){
		return UartDriver_Status_NotTransmitingErrror;
	}

	pSelf->transmitInProgress = 0;

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_TransmitBytes(UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint16_t bytes){

	while (pSelf->transmitInProgress != 0){ }

	if (HAL_UART_Transmit_IT(pSelf->pUartHandler, pBuffer, bytes) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->transmitInProgress = 1;

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_startReceiver(UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Ready){
		return UartDriver_Status_Error;
	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, pSelf->receiveBuffer+pSelf->receiveBufferIterator, 1) != HAL_OK){
		return UartDriver_Status_Error;
	}

	pSelf->state = UartDriver_State_Receiving;

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_stopReceiver(UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_NotReceivingErrror;
	}

	HAL_UART_IRQHandler(pSelf->pUartHandler);	//TODO do sprawdzenia

	pSelf->state = UartDriver_State_Ready;

	return UartDriver_Status_OK;
}
