/*
 * led_driver.c
 *
 *  Created on: 26.07.2018
 *      Author: Kowalik
 */

#include "digital_out_driver.h"

DigitalOutDriver_Status_TypeDef DigitalOutDriver_init(DigitalOutDriver_TypeDef* pSelf, DigitalOutDriver_Port_TypeDef* port, DigitalOutDriver_Pin_TypeDef* pin,
		DigitalOutDriver_StartLevel_TypeDef startLevel){
	if (pSelf->state != DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_Errror;
	}

	pSelf->state = DigitalOutDriver_State_DuringInitalization;

	pSelf->port				= port;
	pSelf->pin				= pin;
	pSelf->onTimeMs			= 0;
	pSelf->offTimeMs		= 0;
	pSelf->onOffTimeCounter	= 0;

	DigitalOutDriver_Status_TypeDef ret;

	switch (startLevel) {
	case DigitalOutDriver_StartLevel_High:
		if ((ret = DigitalOutDriver_setHigh(pSelf)) != DigitalOutDriver_Status_OK){
			return ret;
		}
		break;
	case DigitalOutDriver_StartLevel_Low:
	default:
		if ((ret = DigitalOutDriver_setLow(pSelf)) != DigitalOutDriver_Status_OK){
			return ret;
		}
		break;
	}

	return DigitalOutDriver_Status_OK;

}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_getState(DigitalOutDriver_TypeDef* pSelf, DigitalOutDriver_State_TypeDef* pRetState){
	*pRetState = pSelf->state;

	return DigitalOutDriver_Status_OK;
}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_setHigh(DigitalOutDriver_TypeDef* pSelf){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_SET);

	pSelf->state = DigitalOutDriver_State_High_Stady;

	return DigitalOutDriver_Status_OK;

}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_setLow(DigitalOutDriver_TypeDef* pSelf){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_RESET);

	pSelf->state = DigitalOutDriver_State_Low;

	return DigitalOutDriver_Status_OK;
}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_setBlinking(DigitalOutDriver_TypeDef* pSelf, uint32_t onTimeMs, uint32_t offTimeMs){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	pSelf->onTimeMs			= onTimeMs;
	pSelf->offTimeMs		= offTimeMs;
	pSelf->onOffTimeCounter	= 0;


	HAL_GPIO_WritePin(pSelf->port, *(pSelf->pin), GPIO_PIN_RESET);

	pSelf->state			= DigitalOutDriver_State_Blinking;

	return DigitalOutDriver_Status_OK;

}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_toggle(DigitalOutDriver_TypeDef* pSelf){
	if (pSelf->state == DigitalOutDriver_State_UnInitialized){
		return DigitalOutDriver_Status_UnInitializedErrror;
	}

	DigitalOutDriver_State_TypeDef state;
	if (DigitalOutDriver_getState(pSelf, &state) != DigitalOutDriver_Status_OK){
		return DigitalOutDriver_Status_Errror;
	}

	if (state == DigitalOutDriver_State_High_Stady){
		return DigitalOutDriver_setLow(pSelf);
	} else if (state == DigitalOutDriver_State_Low){
		return DigitalOutDriver_setHigh(pSelf);
	} else {
		return DigitalOutDriver_Status_Errror;
	}
}

DigitalOutDriver_Status_TypeDef DigitalOutDriver_1msElapsedCallbackHandler(DigitalOutDriver_TypeDef* pSelf){

	if (pSelf->state != DigitalOutDriver_State_Blinking){
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
