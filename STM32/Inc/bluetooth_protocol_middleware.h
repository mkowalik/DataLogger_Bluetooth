/*
 * BluetoothProtocolMiddleware.h
 *
 *  Created on: 02.11.2018
 *      Author: Kowalik
 */

#ifndef BLUETOOTH_PROTOCOL_MIDDLEWARE_H_
#define BLUETOOTH_PROTOCOL_MIDDLEWARE_H_

#include "uart_driver.h"
#include "hc05_driver.h"

#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_BUFFER_SIZE				128	//must be bigger than max name length
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_MAX_SENDING_PACKAGE_SIZE	64

#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_ACK_COMMAND				0x00
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_LIST_ALL_FILES_COMMAND	0x01
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_LIST_LOG_FILES_COMMAND	0x02
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_SEND_FILE_COMMAND			0x03
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_SEND_CONFIG_COMMAND		0x04
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_NEW_CONFIG_COMMAND		0x05

#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_ACK_RESPONSE				"\0"
#define	BLUETOOTH_PROTOCOL_MIDDLEWARE_ACK_RESPONSE_LENGTH		1


typedef enum {
	BluetoothProtocolMiddleware_Status_OK = 0,
	BluetoothProtocolMiddleware_Status_UnInitializedErrror,
	BluetoothProtocolMiddleware_Status_TimeoutError,
	BluetoothProtocolMiddleware_Status_NotConnectedError,
	BluetoothProtocolMiddleware_Status_Errror
} BluetoothProtocolMiddleware_Status_TypeDef;

typedef enum {
	BluetoothProtocolMiddleware_State_UnInitialized = 0,
	BluetoothProtocolMiddleware_State_Ready,
	BluetoothProtocolMiddleware_State_SendACKResponseRequest,			//0x00
	BluetoothProtocolMiddleware_State_SendListOfAllFiles_Request,		//0x01
	BluetoothProtocolMiddleware_State_SendListOfLogFiles_Request,		//0x02
	BluetoothProtocolMiddleware_State_SendFileCommand_CollectingName,	//0x03
	BluetoothProtocolMiddleware_State_SendFileCommand_NameComplete,
	BluetoothProtocolMiddleware_State_SendConfig_Request,				//0x04
	BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes1,			//0x05
	BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes2,
	BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes3,
	BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes4,
	BluetoothProtocolMiddleware_State_NewConfig_NumOfBytesMSB,
	BluetoothProtocolMiddleware_State_NewConfig_Data,
	BluetoothProtocolMiddleware_State_NewConfig_DataComplete,
	BluetoothProtocolMiddleware_State_NewConfig_UnknownError
} BluetoothProtocolMiddleware_State_TypeDef;

typedef struct {
	HC05Driver_TypeDef*									pHC05Driver;
	volatile BluetoothProtocolMiddleware_State_TypeDef	state;
	volatile uint32_t									bufferIterator;
	volatile uint32_t									bytesToRead;
	volatile uint32_t									bytesToSend;
	uint8_t volatile									buffer[BLUETOOTH_PROTOCOL_MIDDLEWARE_BUFFER_SIZE];
} BluetoothProtocolMiddleware_TypeDef;

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_init(BluetoothProtocolMiddleware_TypeDef* pSelf, HC05Driver_TypeDef* pHC05Driver);

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendACKResponse(BluetoothProtocolMiddleware_TypeDef* pSelf);					//0x00
BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendListOfAllFiles(BluetoothProtocolMiddleware_TypeDef* pSelf);				//0x01
BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendListOfLOGFiles(BluetoothProtocolMiddleware_TypeDef* pSelf);				//0x02
BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendFile(BluetoothProtocolMiddleware_TypeDef* pSelf, char* filename);		//0x03
BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendConfig(BluetoothProtocolMiddleware_TypeDef* pSelf);						//0x04
BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_newConfig(BluetoothProtocolMiddleware_TypeDef* pSelf, uint8_t* byteStream);	//0x05

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_worker(BluetoothProtocolMiddleware_TypeDef* pSelf);

void BluetoothProtocolMiddleware_receivedByteCallback(uint8_t byte, void* pSelf);

#endif /* BLUETOOTH_PROTOCOL_MIDDLEWARE_H_ */
