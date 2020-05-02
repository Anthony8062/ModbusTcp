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
	unsigned char FunctionCode;		// Read Coils �Լ��ڵ� 0x01
	unsigned short StartngAddress;	// ���� �ַ� : ���� ���� ���� ù ��° �����Ʈ �ּ�
	unsigned short QuantityOutput;	// �����Ʈ ���� : ���� Ƽ���� �����Ʈ ���� �����Ѵ�. ������ 1~N
}Request_Read_Coils;

typedef struct Response_Read_Coils // Read Coils 0x01
{
	unsigned char FunctionCode;		// (�����Ʈ ���� + 7) / 8
	unsigned char ByteCode;         // �����Ʈ ���� LSB -> MSB �������� ǥ�� 0:OFF 1:ON   
	unsigned char OutputStatus;		// �����Ʈ ����
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
	unsigned char FunctionCode;		// (�����Ʈ ���� + 7) / 8
	unsigned short OutputAddress;		// ������ ��� ��Ʈ �ּ�
	unsigned short OutputValue;         // �����Ʈ ����
}Request_Write_Single_Coil;

typedef struct Response_Write_Single_Coil // Write Single Coil 0x05
{
	unsigned char FunctionCode;
	unsigned short OutputAddress;   // ������ ��� ��Ʈ �ּ� 
	unsigned short OutputValue;		// �����Ʈ ���� 0xFF00 : On  , 0x0000 : Off
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
	unsigned char ExceptionStatus;		// LSB->MSB ��ũ�� ������ �����Ʈ�� 1��Ʈ, �׷��� ���� ��Ʈ�� 0 ǥ��
}Response_Read_Exception_Status;

typedef struct Request_Multi_Coil_Controls // write multi coils 0x0F
{
	unsigned char FunctionCode;
	unsigned short StartngAddress;  // ���� �ּ�
	unsigned short QuantityOutput;  // ������ ������ ��Ʈ ���� ����  
	unsigned char ByteCode;         // (�����Ʈ ���� + 7) / 8 
	unsigned char OutputValue;		// �����Ʈ ���� LSB -> MSB �������� ǥ�� 0:OFF 1:ON   
}Request_Multi_Coil_Controls;

typedef struct Response_Multi_Coil_Controls // write multi coils 0x0F
{
	unsigned char FunctionCode;		// (�����Ʈ ���� + 7) / 8
	unsigned short StartngAddress;	// ���� �ּ�
	unsigned short QuantityOutput;  // �����Ʈ ����
}Response_Multi_Coil_Controls;

typedef struct Request_Multi_Registers // write multi register 0x10
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityRegister;	// ���� �� �������� �� 0x0001 write counter
	unsigned char ByteCode;				//�������ͼ� * 2
	unsigned short RegisterValue;
}Request_Multi_Registers;

typedef struct Response_Multi_Registers // write multi register 0x10
{
	unsigned char FunctionCode;
	unsigned short StartingAddress;
	unsigned short QuantityRegister;	// ���� �� �������� �� 0x0001 write counter
}Response_Multi_Registers;

typedef struct ModbusFrame			// Frame �ϳ� �ִ� ũ��� 260 byte �� ���ѵȴ�.
{
	unsigned short TransactionID;	// ���� �� ����.�� �ѽ��� �۾����� �����ϱ� ���� �����Ϳ� ���� ���� �ȴ�. 0x0000 ���� 1�� �����Ǹ� slave �� ���� �״�� �����Ѵ�.
	unsigned short ProtocalID;		// 0x0000 ���� ���� ��
	unsigned short Length;			// Length �ʵ� ���ĺ��� �ش� �������� ������ ������ ���̸� ��Ÿ���� byde �����̴�.
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
	unsigned char MEIType;					// MEI ���� 0x0D or 0x0E
	unsigned int SpecificData;
}Response_Encapsulated_Interface_Transport;

enum enumFucntionCode
{
	enumREAD_COILS = 1,							// ������ �����Ʈ ���� Ȯ�� 0x01
	enumREAD_DISCREATE_INPUTS = 2,				// ������ �Է���Ʈ ���� Ȯ�� 0x02
	enumWRITE_SINGLE_COIL,						// ������ �����Ʈ On / Off ���� 0x5 -> Output Address, Output Value  , 0xFF00 On
	enumWRITE_MULTIPLE_COILS,					// �������� ������ ������ �����Ʈ On / Off ���� 0x0F

	enumREAD_HOLDING_REGISTERS,					// ������/�Ƴ��α� �������Ʈ ����Ȯ�� 0x03
	enumREAD_INPUT_REGISTER,					// ������/ �Ƴ��α� �Է���Ʈ ����Ȯ�� 0x04	
	enumWRITE_SINGLE_REGISTER,					// �����Ʈ On / Off ���� 0x06
	enumWRITE_MULTIPLE_REGISTER,				//		0x10

	enumREAD_EXCEPTION_STATUS,					// ����������� �����ϸ� TCP �����Ʈ �� ��ũ�ΰ� ������ ��Ʈ�� Ȯ���Ѵ�.
	enumREAD_DEVICE_IDENTICATION_TRANSPORT,		// temp 0x2B
	enumDEVICE_EXIT = 999,
};