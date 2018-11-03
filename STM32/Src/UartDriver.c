/*
 * UARTDriver.c
 *
 *  Created on: 02.11.2018
 *      Author: Kowalik
 */

#include "UartDriver.h"

UartDriver_Status_TypeDef UartDriver_init(UartDriver_TypeDef* pSelf, UART_HandleTypeDef* pUartHandler, uint32_t baudRate){

	if (pSelf->state != UartDriver_State_UnInitialized){
		return UartDriver_Status_Errror;
	}

	pSelf->pUartHandler = pUartHandler;
	pSelf->callbacksCounter = 0;
	pSelf->receiveBuffer = 0;

	if (pSelf->pUartHandler->gState == HAL_UART_STATE_RESET){
		return UartDriver_Status_Errror;
	}

	pSelf->state = UartDriver_State_Initialized; //bacause UartDriver_changeBaudRate() needs state to be UartDriver_State_Initialized.

	if (pSelf->pUartHandler->Init.BaudRate != baudRate){
		UartDriver_changeBaudRate(pSelf, baudRate);
	}

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_changeBaudRate(UartDriver_TypeDef* pSelf, uint32_t baudRate){

	if (pSelf->state != UartDriver_State_Initialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	pSelf->state = UartDriver_State_ChangeSettings;

	if (HAL_UART_DeInit(pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Errror;
	}

	pSelf->pUartHandler->Init.BaudRate = baudRate;

	if (HAL_UART_Init(pSelf->pUartHandler) != HAL_OK){
		return UartDriver_Status_Errror;
	}

	pSelf->state = UartDriver_State_Initialized;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_sendBytes(UartDriver_TypeDef* pSelf, uint8_t* pBuffer, uint32_t bytes){

	if (pSelf->state != UartDriver_State_Initialized && pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (HAL_UART_Transmit(pSelf->pUartHandler, pBuffer, bytes, UART_TIMEOUT) != HAL_OK){
		return UartDriver_Status_Errror;
	}

	return UartDriver_Status_OK;
}

static UartDriver_Status_TypeDef UartDriver_receiveByte(UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (HAL_UART_Receive_IT(pSelf->pUartHandler, &(pSelf->receiveBuffer), 1) != HAL_OK){
		return UartDriver_Status_Errror;
	}

	return UartDriver_Status_OK;

}

UartDriver_Status_TypeDef UartDriver_startReceiver(UartDriver_TypeDef* pSelf){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state != UartDriver_State_Initialized){
		return UartDriver_Status_ReceivingError;
	}

	pSelf->state = UartDriver_State_Receiving;

	return UartDriver_receiveByte(pSelf);
}

UartDriver_Status_TypeDef UartDriver_stopReceiver(UartDriver_TypeDef* pSelf){

	if (pSelf->state != UartDriver_State_Receiving){
		return UartDriver_Status_UnInitializedErrror;
	}

	pSelf->state = UartDriver_State_Initialized;

	return UartDriver_Status_OK;
}

UartDriver_Status_TypeDef UartDriver_receivedBytesCallback(UartDriver_TypeDef* pSelf){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state == UartDriver_State_Initialized || pSelf->state == UartDriver_State_ChangeSettings){
		return UartDriver_Status_OK;
	}

	// must be: pSelf->state == UartDriver_State_Receiving

	for (uint16_t i=0; i<pSelf->callbacksCounter; i++){
		pSelf->callbacks[i](pSelf->receiveBuffer);
	}

	return UartDriver_receiveByte(pSelf);
}

UartDriver_Status_TypeDef UartDriver_setReceiveDataCallback(UartDriver_TypeDef* pSelf, void (*foo)(uint8_t byte)){

	if (pSelf->state == UartDriver_State_UnInitialized){
		return UartDriver_Status_UnInitializedErrror;
	}

	if (pSelf->state == UartDriver_State_Receiving){
		return UartDriver_Status_ReceivingError;
	}

	pSelf->callbacks[pSelf->callbacksCounter++] = foo;

	return UartDriver_Status_OK;

}
