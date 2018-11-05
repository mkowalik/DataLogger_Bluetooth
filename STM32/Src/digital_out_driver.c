/*
 * led_driver.c
 *
 *  Created on: 26.07.2018
 *      Author: Kowalik
 */

#include "digital_out_driver.h"

DigitalOutDriver_Status_TypeDef DigitalOutDriver_init(DigitalOutDriver_TypeDef* pSelf, DigitalOutDriver_Port_TypeDef* port, DigitalOutDriver_Pin_TypeDef* pin){
	if (pSelf->state != DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_Errror;
	}

	pSelf->port				= port;
	pSelf->pin				= pin;
	pSelf->onTimeMs			= 0;
	pSelf->offTimeMs		= 0;
	pSelf->onOffTimeCounter	= 0;

	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_SET);

	pSelf->state = DigitalOutDriver_State_Off;

	return DigitalOutDriver_Status_OK;

}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_getState(DigitalOutDriver_TypeDef* pSelf, DigitalOutDriver_State_TypeDef* pRetState){
	*pRetState = pSelf->state;
}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_On(DigitalOutDriver_TypeDef* pSelf){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_SET);

	pSelf->state = DigitalOutDriver_State_OnStady;

	return DigitalOutDriver_Status_OK;

}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_Off(DigitalOutDriver_TypeDef* pSelf){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_RESET);

	pSelf->state = DigitalOutDriver_State_Off;

	return DigitalOutDriver_Status_OK;
}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_BlinkingLed(DigitalOutDriver_TypeDef* pSelf, uint32_t onTimeMs, uint32_t offTimeMs){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	pSelf->onTimeMs			= onTimeMs;
	pSelf->offTimeMs		= offTimeMs;
	pSelf->onOffTimeCounter	= 0;


	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_RESET);

	pSelf->state			= DigitalOutDriver_State_OnBlinking;

	return DigitalOutDriver_Status_OK;

}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_1msElapsedCallbackHandler(DigitalOutDriver_TypeDef* pSelf){

	if (pSelf->state != DigitalOutDriver_State_OnBlinking){
		return DigitalOutDriver_Status_OK;
	}

	pSelf->onOffTimeCounter++;

	if (pSelf->onOffTimeCounter == pSelf->onTimeMs){

		HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_SET);

	} else if (pSelf->onOffTimeCounter == pSelf->onTimeMs + pSelf->offTimeMs){

		HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_RESET);
		pSelf->onOffTimeCounter = 0;

	}

	return DigitalOutDriver_Status_OK;

}
