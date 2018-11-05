/*
 * BluetoothMiddleware.h
 *
 *  Created on: 02.11.2018
 *      Author: Kowalik
 */

#ifndef BLUETOOTH_MIDDLEWARE_H_
#define BLUETOOTH_MIDDLEWARE_H_

#include "uart_driver.h"
/*
typedef enum {
	BluetoothMiddleware_Status_OK = 0,
	BluetoothMiddleware_Status_UnInitializedErrror,
	BluetoothMiddleware_Status_Errror
} BluetoothMiddleware_Status_TypeDef;

typedef enum {
	BluetoothMiddleware_State_UnInitialized = 0,
	BluetoothMiddleware_State_Ready_DataMode,
	BluetoothMiddleware_State_Ready_ATMode
} BluetoothMiddleware_State_TypeDef;

typedef struct {
	UartDriver_TypeDef*			pUartDriver;
	UartDriver_State_TypeDef	state;
} BluetoothMiddleware_State_TypeDef;

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_init(BluetoothMiddleware_TypeDef* pSelf, UartDriver_TypeDef* pUartDriver);

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_setDeviceName(BluetoothMiddleware_TypeDef* pSelf, char* pDeviceName);
BluetoothMiddleware_Status_TypeDef BluetootMiddleware_getDeviceName(BluetoothMiddleware_TypeDef* pSelf, char* pRetDeviceName);
BluetoothMiddleware_Status_TypeDef BluetootMiddleware_getDeviceName(BluetoothMiddleware_TypeDef* pSelf, char* pRetDeviceName);

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_getState(BluetoothMiddleware_TypeDef* pSelf, BluetoothMiddleware_State_TypeDef* pRetState);

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_setPinCode(BluetoothMiddleware_TypeDef* pSelf, uin16_t pinCode);
BluetoothMiddleware_Status_TypeDef BluetootMiddleware_getPinCode(BluetoothMiddleware_TypeDef* pSelf, uin16_t* pRetPinCode);

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_setDataMode(BluetoothMiddleware_TypeDef* pSelf);

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_sendData(BluetoothMiddleware_TypeDef* pSelf, uint8_t* pData, uint32_t bytes);

BluetoothMiddleware_Status_TypeDef BluetootMiddleware_setReceiveDataCallback(BluetoothMiddleware_TypeDef* pSelf, void (*foo)(uint8_t* pData, uint32_t bytes));

*/
#endif /* BLUETOOTH_MIDDLEWARE_H_ */
