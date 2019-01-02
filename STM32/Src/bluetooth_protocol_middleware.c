/*
 * bluetooth_protocol_middleware.c
 *
 *  Created on: 03.11.2018
 *      Author: Kowalik
 */

#include <string.h>
#include "bluetooth_protocol_middleware.h"

#define	__STATE_ERROR_IF_NOTEQ(__STATE__, __RETERROR__) 	\
		do {												\
			if (pSelf->state != (__STATE__)){ 				\
				return (__RETERROR__); 						\
			}												\
		} while (0u)

#define	__STATE_ERROR_IF_EQ(__STATE__, __RETERROR__) 		\
		do {												\
			if (pSelf->state == (__STATE__)){ 				\
				return (__RETERROR__); 						\
			}												\
		} while (0u)

#define __CALL_ERROR_IF_NOTEQ(__FOOCALL__, __FOO_RET__, __RET_ERROR__)	\
		do {															\
			if ((__FOOCALL__) != (__FOO_RET__)) {						\
				return __RET_ERROR__;									\
			}															\
		} while (0u)

static HC05Driver_State_TypeDef BluetoothProtocolMiddleware_interpretRequestInInterrupt(uint8_t byte);

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_init(BluetoothProtocolMiddleware_TypeDef* pSelf, HC05Driver_TypeDef* pHC05Driver){

	__STATE_ERROR_IF_NOTEQ(BluetoothProtocolMiddleware_State_UnInitialized,
			BluetoothProtocolMiddleware_Status_Errror);

	pSelf->pHC05Driver = pHC05Driver;
	pSelf->bufferIterator = 0;
	pSelf->bytesToRead = 0;
	pSelf->bytesToSend = 0;
	memset(pSelf->buffer, 0, BLUETOOTH_PROTOCOL_MIDDLEWARE_BUFFER_SIZE);

	__CALL_ERROR_IF_NOTEQ(	\
			HC05Driver_setReceiveDataCallback(pSelf->pHC05Driver, BluetoothProtocolMiddleware_receivedByteCallback, (void*)pSelf, NULL),
			HC05Driver_Status_OK,
			BluetoothProtocolMiddleware_Status_Errror);

	__CALL_ERROR_IF_NOTEQ(	\
			HC05Driver_setDataMode(pSelf->pHC05Driver),
			HC05Driver_Status_OK,
			BluetoothProtocolMiddleware_Status_Errror);

	return BluetoothProtocolMiddleware_Status_OK;

}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendACKResponse(BluetoothProtocolMiddleware_TypeDef* pSelf){
	HC05Driver_sendData(pSelf->pHC05Driver, (uint8_t*)BLUETOOTH_PROTOCOL_MIDDLEWARE_ACK_RESPONSE, BLUETOOTH_PROTOCOL_MIDDLEWARE_ACK_RESPONSE_LENGTH);
	pSelf->state = BluetoothProtocolMiddleware_State_Ready;
}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendListOfAllFiles(BluetoothProtocolMiddleware_TypeDef* pSelf){

}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendListOfLOGFiles(BluetoothProtocolMiddleware_TypeDef* pSelf){

}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendFile(BluetoothProtocolMiddleware_TypeDef* pSelf, char* filename){

}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_sendConfig(BluetoothProtocolMiddleware_TypeDef* pSelf){

}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_newConfig(BluetoothProtocolMiddleware_TypeDef* pSelf, uint8_t* byteStream){

}

BluetoothProtocolMiddleware_Status_TypeDef BluetoothProtocolMiddleware_worker(BluetoothProtocolMiddleware_TypeDef* pSelf){

	__STATE_ERROR_IF_EQ(BluetoothProtocolMiddleware_State_UnInitialized,
			BluetoothProtocolMiddleware_Status_UnInitializedErrror);

	switch(pSelf->state) {
		case BluetoothProtocolMiddleware_State_SendACKResponseRequest:			//0x00
			return BluetoothProtocolMiddleware_sendACKResponse(pSelf);
			break;
		case BluetoothProtocolMiddleware_State_Ready:
		case BluetoothProtocolMiddleware_State_SendFileCommand_CollectingName:	//0x03
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes1:			//0x05
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes2:
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes3:
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes4:
		case BluetoothProtocolMiddleware_State_NewConfig_Data:
		case BluetoothProtocolMiddleware_State_NewConfig_DataComplete:
		case BluetoothProtocolMiddleware_State_SendListOfAllFiles_Request:		//0x01
		case BluetoothProtocolMiddleware_State_SendListOfLogFiles_Request:		//0x02
		case BluetoothProtocolMiddleware_State_SendFileCommand_NameComplete:
		case BluetoothProtocolMiddleware_State_SendConfig_Request:
		default:
			//TODO Error handler
			break;
	}

}

void BluetoothProtocolMiddleware_receivedByteCallback(uint8_t byte, void* pVoidSelf){

	BluetoothProtocolMiddleware_TypeDef* pSelf = (BluetoothProtocolMiddleware_TypeDef*) pVoidSelf;

	if (pSelf->state == BluetoothProtocolMiddleware_State_UnInitialized){
		//TODO Error handler
		return;
	}

	switch(pSelf->state) {
		case BluetoothProtocolMiddleware_State_Ready:
			pSelf->bufferIterator = 0;
			pSelf->bytesToRead = 0;
			pSelf->bytesToSend = 0;
			pSelf->state = BluetoothProtocolMiddleware_interpretRequestInInterrupt(byte);
			break;
		case BluetoothProtocolMiddleware_State_SendFileCommand_CollectingName:	//0x03
			pSelf->buffer[pSelf->bufferIterator++] = byte;
			if (byte == 0){
				pSelf->state = BluetoothProtocolMiddleware_State_SendFileCommand_NameComplete;
			}
			break;
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes1:			//0x05
			pSelf->bytesToRead = byte;
			pSelf->state = BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes2;
			break;
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes2:
			pSelf->bytesToRead |= (byte << 8);
			pSelf->state = BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes3;
			break;
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes3:
			pSelf->bytesToRead |= (byte << 16);
			pSelf->state = BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes4;
			break;
		case BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes4:
			pSelf->bytesToRead |= (byte << 24);
			pSelf->state = BluetoothProtocolMiddleware_State_NewConfig_Data;
			break;
		case BluetoothProtocolMiddleware_State_NewConfig_Data:
			pSelf->buffer[pSelf->bufferIterator] = byte;
			pSelf->bufferIterator = (pSelf->bufferIterator+1)%BLUETOOTH_PROTOCOL_MIDDLEWARE_BUFFER_SIZE;
			if (pSelf->bytesToRead == 0){
				pSelf->state = BluetoothProtocolMiddleware_State_NewConfig_DataComplete;
			}
			break;
		case BluetoothProtocolMiddleware_State_NewConfig_DataComplete:
		case BluetoothProtocolMiddleware_State_SendACKResponseRequest:			//0x00
		case BluetoothProtocolMiddleware_State_SendListOfAllFiles_Request:		//0x01
		case BluetoothProtocolMiddleware_State_SendListOfLogFiles_Request:		//0x02
		case BluetoothProtocolMiddleware_State_SendFileCommand_NameComplete:
		case BluetoothProtocolMiddleware_State_SendConfig_Request:
		default:
			pSelf->state = BluetoothProtocolMiddleware_State_NewConfig_UnknownError;
			//TODO Error handler
			break;
	}
}

static HC05Driver_State_TypeDef BluetoothProtocolMiddleware_interpretRequestInInterrupt(uint8_t byte){
	switch(byte){
	case BLUETOOTH_PROTOCOL_MIDDLEWARE_ACK_COMMAND:
		return BluetoothProtocolMiddleware_State_SendACKResponseRequest;
	case BLUETOOTH_PROTOCOL_MIDDLEWARE_LIST_ALL_FILES_COMMAND:
		return BluetoothProtocolMiddleware_State_SendListOfAllFiles_Request;
	case BLUETOOTH_PROTOCOL_MIDDLEWARE_LIST_LOG_FILES_COMMAND:
		return BluetoothProtocolMiddleware_State_SendListOfLogFiles_Request;
	case BLUETOOTH_PROTOCOL_MIDDLEWARE_SEND_FILE_COMMAND:
		return BluetoothProtocolMiddleware_State_SendFileCommand_CollectingName;
	case BLUETOOTH_PROTOCOL_MIDDLEWARE_SEND_CONFIG_COMMAND:
		return BluetoothProtocolMiddleware_State_SendConfig_Request;
	case BLUETOOTH_PROTOCOL_MIDDLEWARE_NEW_CONFIG_COMMAND:
		return BluetoothProtocolMiddleware_State_NewConfig_NumOfBytes1;
	default:
		return BluetoothProtocolMiddleware_State_NewConfig_UnknownError;
	}
	return BluetoothProtocolMiddleware_State_NewConfig_UnknownError;
}
