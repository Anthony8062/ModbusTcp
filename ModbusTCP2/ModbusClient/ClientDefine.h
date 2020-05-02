#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib, "WS2_32.LIB")

#define SERVER_PORT 502							
#define NE_IP "127.0.0.1"
#define NE_PORT 502

#define READ_COILS							0x01
#define READ_DISCREATE_INPUTS				0x02
#define WRITE_SINGLE_COIL					0x05
#define WRITE_MULTIPLE_COILS				0x0F

#define READ_HOLDING_REGISTERS				0x03
#define READ_INPUT_REGISTER					0x04
#define WRITE_SINGLE_REGISTER				0x06
#define WRITE_MULTIPLE_REGISTER				0x10

#define READ_EXCEPTION_STATUS				0x07
#define READ_DEVICE_IDENTICATION_TRANSPORT	0x2B


typedef struct Request_Read_Coils // Read Coils 0x01
{
	unsigned char FunctionCode;		// Read Coils 함수코드 0x01
	unsigned short StartngAddress;	// 시작 주로 : 상태 값을 읽을 첫 번째 출력포트 주소
	unsigned short QuantityOutput;	// 출력포트 개수 : 읽을 티지털 출력포트 수를 지정한다. 범위는 1~N
}Request_Read_Coils;

typedef struct Response_Read_Coils // Read Coils 0x01
{
	unsigned char FunctionCode;		// (출력포트 개수 + 7) / 8
	unsigned char ByteCode;         // 출력포트 상태 LSB -> MSB 방향으로 표시 0:OFF 1:ON   
	unsigned char OutputStatus;		// 출력포트 상태
}Response_Read_Coils;

typedef struct Request_Read_Discrete_Inputs // Read Discrete Inputs 0x02
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityInput;
}Request_Read_Discrete_Inputs;

typedef struct Response_Read_Discrete_Inputs // Read Discrete Inputs 0x02
{
	unsigned char FunctionCode;
	unsigned char ByteCode;
	unsigned char InputStatus;
}Response_Read_Discrete_Inputs;

typedef struct Request_Read_Holding_Register // Read Holding Register 0x03
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityRegister;
}Request_Read_Holding_Register;

typedef struct Reponse_Read_Holding_Register // Read Holding Register 0x03
{
	unsigned char FunctionCode;
	unsigned char ByteCode;
	unsigned short RegisterValue;
}Reponse_Read_Holding_Register;

typedef struct Request_Read_Input_Registers // Read Input Regusters 0x04
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityRegister;
}Request_Read_Input_Registers;

typedef struct Response_Read_Input_Registers // Read Input Regusters 0x04
{
	unsigned char FunctionCode;
	unsigned char ByteCode;
	unsigned short InputRegister;
}Response_Read_Input_Registers;

typedef struct Request_Write_Single_Coil // Write Single Coil 0x05
{
	unsigned char FunctionCode;		// (출력포트 개수 + 7) / 8
	unsigned short OutputAddress;		// 제어할 출력 포트 주소
	unsigned short OutputValue;         // 출력포트 개수
}Request_Write_Single_Coil;

typedef struct Response_Write_Single_Coil // Write Single Coil 0x05
{
	unsigned char FunctionCode;
	unsigned short OutputAddress;   // 제어할 출력 포트 주소 
	unsigned short OutputValue;		// 출력포트 개수 0xFF00 : On  , 0x0000 : Off
}Response_Write_Single_Coil;

typedef struct Request_Write_Single_Register // Write Single Register 0x06
{
	unsigned char FunctionCode;
	unsigned short RegisterAddress;
	unsigned short RegisterValue;
}Request_Write_Single_Register;

typedef struct Response_Write_Single_Register // Write Single Register 0x06
{
	unsigned char FunctionCode;
	unsigned short RegisterAddress;
	unsigned short RegisterValue;
}Response_Write_Single_Register;

typedef struct Request_Read_Exception_Status // Read Exception Status 0x07
{
	unsigned char FunctionCode;
}Request_Read_Exception_Status;

typedef struct Response_Read_Exception_Status // Read Exception Status 0x07
{
	unsigned char FunctionCode;
	unsigned char ExceptionStatus;		// LSB->MSB 매크로 설정된 출력포트는 1비트, 그렇지 않은 비트는 0 표시
}Response_Read_Exception_Status;

typedef struct Request_Multi_Coil_Controls // write multi coils 0x0F
{
	unsigned char FunctionCode;
	unsigned short StartngAddress;  // 시작 주소
	unsigned short QuantityOutput;  // 제어할 디지털 포트 범위 지정  
	unsigned char ByteCode;         // (출력포트 개수 + 7) / 8 
	unsigned char OutputValue;		// 출력포트 상태 LSB -> MSB 방향으로 표시 0:OFF 1:ON   
}Request_Multi_Coil_Controls;

typedef struct Response_Multi_Coil_Controls // write multi coils 0x0F
{
	unsigned char FunctionCode;		// (출력포트 개수 + 7) / 8
	unsigned short StartngAddress;	// 시작 주소
	unsigned short QuantityOutput;  // 출력포트 개수
}Response_Multi_Coil_Controls;

typedef struct Request_Multi_Registers // write multi register 0x10
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityRegister;	// 값을 쓸 레지스터 수 0x0001 write counter
	unsigned char ByteCode;				//레지스터수 * 2
	unsigned short RegisterValue;
}Request_Multi_Registers;

typedef struct Response_Multi_Registers // write multi register 0x10
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityRegister;	// 값을 쓸 레지스터 수 0x0001 write counter
}Response_Multi_Registers;

typedef struct ModbusFrame			// Frame 하나 최대 크기는 260 byte 로 제한된다.
{
	unsigned short TransactionID;	// 쿼리 및 응답.을 한쌍의 작업으로 구분하기 위함 마스터에 의해 설정 된다. 0x0000 부터 1씩 증가되며 slave 는 값을 그대로 복사한다.
	unsigned short ProtocalID;		// 0x0000 으로 고정 값
	unsigned short Length;			// Length 필드 이후부터 해당 프레임의 마지막 까지의 길이를 나타내며 byde 단위이다.
	unsigned char UnitID;
}ModbusFrame;

typedef struct Request_Encapsulated_Interface_Transport // Encapsulated interface Transport 0x2B
{
	unsigned char FunctionCode;
	unsigned char MEIType;
	unsigned int SpecificData;
}Request_Encapsulated_Interface_Transport;

typedef struct Response_Encapsulated_Interface_Transport // Encapsulated interface Transport 0x2B
{
	unsigned char FunctionCode;
	unsigned char MEIType;					// MEI 종류 0x0D or 0x0E
	unsigned int SpecificData;
}Response_Encapsulated_Interface_Transport;

enum enumFucntionCode
{
	enumREAD_COILS = 1,							// 디지털 출력포트 상태 확인 0x01
	enumREAD_DISCREATE_INPUTS = 2,				// 디지털 입력포트 상태 확인 0x02
	enumWRITE_SINGLE_COIL,						// 디지털 출력포트 On / Off 제어 0x5 -> Output Address, Output Value  , 0xFF00 On
	enumWRITE_MULTIPLE_COILS,					// 연속적인 여러개 디지털 출력포트 On / Off 제어 0x0F

	enumREAD_HOLDING_REGISTERS,					// 디지털/아날로그 입출력포트 상태확인 0x03
	enumREAD_INPUT_REGISTER,					// 다지털/ 아날로그 입력포트 상태확인 0x04	
	enumWRITE_SINGLE_REGISTER,					// 출력포트 On / Off 제어 0x06
	enumWRITE_MULTIPLE_REGISTER,				//		0x10

	enumREAD_EXCEPTION_STATUS,					// 예외응답과는 무관하며 TCP 출력포트 중 매크로가 설정된 포트를 확인한다.
	enumREAD_DEVICE_IDENTICATION_TRANSPORT,		// temp 0x2B
	enumDEVICE_EXIT = 999,
};