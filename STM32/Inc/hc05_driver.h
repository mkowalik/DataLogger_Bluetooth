/*
 * HC05Driver_driver.h
 *
 *  Created on: 03.11.2018
 *      Author: Kowalik
 */

#ifndef HC05Driver_DRIVER_H_
#define HC05Driver_DRIVER_H_

#include "digital_out_driver.h"
#include "uart_driver.h"

/**********		SET UP PART			**********/

#define	HC05_PASSWORD			2688
#define	HC05_DEVICE_NAME		"AGH_LOGGER_GRAZYNA2"
#define HC05_DATA_BAUDRATE		115200

/********** 	END OF SET UP PART		**********/


#define	HC05_DRIVER_MAX_CALLBACK_NUMBER		UART_DRIVER_MAX_CALLBACK_NUMBER
#define HC05_BUFFER_SIZE		128


typedef int16_t HC05Driver_CallbackIterator_TypeDef;

typedef enum {
	HC05Driver_State_UnInitialized 		= 0,
	HC05Driver_State_HardAT,
	HC05Driver_State_Data,
	HC05Driver_State_Initialized,
	HC05Driver_State_Ready,
	HC05Driver_State_Pairable,
	HC05Driver_State_Paired,
	HC05Driver_State_Inquiring,
	HC05Driver_State_Connecting,
	HC05Driver_State_Connected,
	HC05Driver_State_Disconnected
} HC05Driver_State_TypeDef;

typedef enum {
	HC05Driver_Role_Slave = 0,
	HC05Driver_Role_Master = 1,
	HC05Driver_Role_SlaveLoop = 2,
} HC05Driver_Role_TypeDef;

typedef enum {
	HC05Driver_Status_OK = 0,
	HC05Driver_Status_UnInitializedError,
	HC05Driver_Status_TimeoutError,
	HC05Driver_Status_NotConnectedError,
	HC05Driver_Status_UnsuportedError,
	HC05Driver_Status_Error,
} HC05Driver_Status_TypeDef;

typedef struct {
	UartDriver_TypeDef*					pUartDriver;
	DigitalOutDriver_TypeDef* 			pKeyPinDriver;
	HC05Driver_State_TypeDef			state;
	uint32_t							dataBaudRate;
	uint8_t								buffer[HC05_BUFFER_SIZE];
	UartDriver_CallbackIterator_TypeDef	callbacksIterators[HC05_DRIVER_MAX_CALLBACK_NUMBER];
	void (*callbacks[HC05_DRIVER_MAX_CALLBACK_NUMBER])(uint8_t byte, void* pArgs);
	void*								callbackArgs[HC05_DRIVER_MAX_CALLBACK_NUMBER];
} HC05Driver_TypeDef;

/**
 * 	 Possible BaudRates: 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1382400
 */
HC05Driver_State_TypeDef HC05Driver_init(HC05Driver_TypeDef* pSelf, HC05Driver_Role_TypeDef role, \
		UartDriver_TypeDef* pUartDriver, DigitalOutDriver_TypeDef* pKeyPinDriver, uint32_t baudRate,
		char* name, uint16_t password);

HC05Driver_Status_TypeDef HC05Driver_sendTestATCommand(HC05Driver_TypeDef* pSelf);
HC05Driver_Status_TypeDef HC05Driver_sendRestoreDefualtCommand(HC05Driver_TypeDef* pSelf);

HC05Driver_Status_TypeDef HC05Driver_getBaudRate(HC05Driver_TypeDef* pSelf, uint32_t* pRetBaudRate);
HC05Driver_Status_TypeDef HC05Driver_setBaudRate(HC05Driver_TypeDef* pSelf, uint32_t baudRate);

HC05Driver_Status_TypeDef HC05Driver_getPassword(HC05Driver_TypeDef* pSelf, uint32_t* pRetPassword);
HC05Driver_Status_TypeDef HC05Driver_setPassword(HC05Driver_TypeDef* pSelf, uint32_t password);

HC05Driver_Status_TypeDef HC05Driver_getDeviceName(HC05Driver_TypeDef* pSelf, char* pRetDeviceName);
HC05Driver_Status_TypeDef HC05Driver_setDeviceName(HC05Driver_TypeDef* pSelf, char* deviceName);

HC05Driver_Status_TypeDef HC05Driver_getDeviceRole(HC05Driver_TypeDef* pSelf, HC05Driver_Role_TypeDef* pRetRole);
HC05Driver_Status_TypeDef HC05Driver_setDeviceRole(HC05Driver_TypeDef* pSelf, HC05Driver_Role_TypeDef role);

HC05Driver_Status_TypeDef HC05Driver_getState(HC05Driver_TypeDef* pSelf, HC05Driver_State_TypeDef* pRetState);

HC05Driver_Status_TypeDef HC05Driver_sendData(HC05Driver_TypeDef* pSelf, uint8_t* data, uint16_t bytes);
HC05Driver_Status_TypeDef HC05Driver_sendAndReceiveDataTerminationSign(HC05Driver_TypeDef* pSelf, uint8_t* pSendData,
		uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bufferSize, uint8_t terminationSign);
HC05Driver_Status_TypeDef HC05Driver_sendAndReceiveDataNBytes(HC05Driver_TypeDef* pSelf, uint8_t* pSendData,
		uint16_t bytesToSend, uint8_t* pReceiveBuffer, uint16_t bytesToReceive);

HC05Driver_Status_TypeDef HC05Driver_setReceiveDataCallback(HC05Driver_TypeDef* pSelf,
		void (*foo)(uint8_t byte, void* pArgs), void* pArgs, HC05Driver_CallbackIterator_TypeDef* pRetCallbackIterator);
HC05Driver_Status_TypeDef HC05Driver_removeReceiveDataCallback(HC05Driver_TypeDef* pSelf,
		UartDriver_CallbackIterator_TypeDef callbackIterator);

#endif /* HC05Driver_DRIVER_H_ */
