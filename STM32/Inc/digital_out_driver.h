/*
 * led_driver.h
 *
 *  Created on: 26.07.2018
 *      Author: Kowalik
 */

#ifndef DIGITAL_OUT_DRIVER_H_
#define DIGITAL_OUT_DRIVER_H_

#include "stdint.h"
#include "gpio.h"

typedef enum {
	DigitalOutDriver_State_UnInitialized = 0,
	DigitalOutDriver_State_Off,
	DigitalOutDriver_State_OnBlinking,
	DigitalOutDriver_State_OnStady
} DigitalOutDriver_State_TypeDef;

typedef enum {
	DigitalOutDriver_Status_OK = 0,
	DigitalOutDriver_Status_UnInitializedErrror,
	DigitalOutDriver_Status_Errror
} DigitalOutDriver_Status_TypeDef;

typedef	GPIO_TypeDef	DigitalOutDriver_Port_TypeDef;
typedef	uint16_t		DigitalOutDriver_Pin_TypeDef;

typedef struct {
	DigitalOutDriver_State_TypeDef			state;
	DigitalOutDriver_Port_TypeDef*	port;
	DigitalOutDriver_Pin_TypeDef*	pin;
	uint32_t						onTimeMs;
	uint32_t						offTimeMs;
	uint32_t						onOffTimeCounter;
} DigitalOutDriver_TypeDef;

DigitalOutDriver_Status_TypeDef DigitalOutDriver_init(DigitalOutDriver_TypeDef* pSelf, DigitalOutDriver_Port_TypeDef* port, DigitalOutDriver_Pin_TypeDef* pin);
DigitalOutDriver_Status_TypeDef DigitalOutDriver_getState(DigitalOutDriver_TypeDef* pSelf, DigitalOutDriver_State_TypeDef* pRetState);
DigitalOutDriver_Status_TypeDef DigitalOutDriver_On(DigitalOutDriver_TypeDef* pSelf);
DigitalOutDriver_Status_TypeDef DigitalOutDriver_Off(DigitalOutDriver_TypeDef* pSelf);
DigitalOutDriver_Status_TypeDef DigitalOutDriver_BlinkingLed(DigitalOutDriver_TypeDef* pSelf, uint32_t onTimeMs, uint32_t offTimeMs);
DigitalOutDriver_Status_TypeDef DigitalOutDriver_1msElapsedCallbackHandler(DigitalOutDriver_TypeDef* pSelf);



#endif /* DIGITAL_OUT_DRIVER_H_ */
