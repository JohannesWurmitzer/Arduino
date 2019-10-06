/*
	Copyright (c) 2003, Maximilian Johannes Wurmitzer.
	All rights reserved.
*/
/*	Function description

	Header File for bldc-2_telegram.c

	typedefs for the protocols

*/
/*
	File Name:		bldc-2_protocols.h
	Target Device:
	Created by:		Markus Emanuel Wurmitzer
	Created on:		2005-02-02
	derived from:	TemplateHeader.h	V1.00
*/
/*	History

	2015-12-18	V4		JoWu
		- standard cyclic protocol (receive and write) for BLDC_V7
		- S_PROT10W s_Prot10W;

	2015-12-17	V3		JoWu
		- added PROTNR_FW_WR - S_PROT64R, S_PROT64W
		- added PROTNR_FW_RD - S_PROTE4R, S_PROTE4W
		- increased PROT_LEN_MAX

	2006-05-25	V2		TI
		- add S_PROT40R, S_PROT40W

	2005-02-02	V1		MEW	Creation
		- Creation of file
*/
/*	todo-list
	2015-12-17, JoWu: Check PROT_LEN_MAX, I think it's to big, should be 36
	-	do not know...

*/
#define ARDUINO_TEST
/*
	includes
*/
#ifdef ARDUINO_TEST
 #include "bldc-2_types.h"        // renamed Types.h from BLDC project to bldc-2_type.h
#else
 #include "..\Basic\Types.h"
#endif
/*
	defines
*/
#define PROT_LEN_MAX 38		// maximum Bytes for protocol

/*
	typedefs
*/
// standard cyclic protocol (receive and write) for BLDC_V7
typedef struct{							
	T_U8	u8_ParamNr;					// should be 00 in this case
	T_U16	u16_RevolutionsPerMinute;	// Bit 15: 0 ... right, 1 ... left rotation
	T_U8	u8_TorqueMax;				// Torque max Value
	T_U8	u8_WriteValue;				// Value to write in choosed Variable
	T_U8	u8_CommandOrIndex;			// Number of command or index for eeprom
										// Bit 7: 0 ... read  or status
										//		  1 ... write or command
	T_U8	u8_Index2;					// Index of 16 Bit Read Data, if 0 then Status is sent within the Answer-High-Byte
	T_U8	u8_CRC8;					// crc8
}S_PROT10R;		// 

typedef struct{
	T_U8	u8_ParamNr;						// should be 00 in this case
	T_U16	u16_RevolutionsPerMinuteMeas;	// Bit 15: 0 ... right, 1 ... left rotation
	T_U8	u8_TorqueMeas;
	T_U8	u8_ReadData;					// Read data if read was asked or error code
	T_U16	u16_Index2Value;				// Status of BLDC (ErrorByte), of high-byte of 16 Bit Value
	//T_U8	u8_Index2Low;					// 16 Bit Value low-byte
	T_U8	u8_CRC8;						// Sum of all bytes modulo 256
}S_PROT10W; 	// standard protocol write

// standard cyclic protocol (receive and write) for BLDC_V5.0.1.1
typedef struct{							
	T_U8	u8_ParamNr;					// should be 00 in this case
	T_U16	u16_RevolutionsPerMinute;	// Bit 15: 0 ... right, 1 ... left rotation
	T_U8	u8_TorqueMax;				// Torque max Value
	T_U8	u8_WriteValue;				// Value to write in choosed Variable
	T_U8	u8_CommandOrIndex;			// Number of command or index for eeprom
										// Bit 7: 0 ... read  or status
										//		  1 ... write or command
	T_U8	u8_Index2;					// Index of 16 Bit Read Data, if 0 then Status is sent within the Answer-High-Byte
	T_U8	u8_CRC8;					// crc8
}S_PROT40R;		// 

typedef struct{
	T_U8	u8_ParamNr;						// should be 00 in this case
	T_U16	u16_RevolutionsPerMinuteMeas;	// Bit 15: 0 ... right, 1 ... left rotation
	T_U8	u8_TorqueMeas;
	T_U8	u8_ReadData;					// Read data if read was asked or error code
	T_U8	u8_Status_Error_Index2High;		// Status of BLDC (ErrorByte), of high-byte of 16 Bit Value
	T_U8	u8_Index2Low;					// 16 Bit Value low-byte
	T_U8	u8_CRC8;						// Sum of all bytes modulo 256
}S_PROT40W; 	// standard protocol write

// standard cyclic protocol (receive and write)
typedef struct{							
	T_U8	u8_ParamNr;					// should be 00 in this case
	T_U16	u16_RevolutionsPerMinute;	// Bit 15: 0 ... right, 1 ... left rotation
	T_U8	u8_TorqueMaxOrWriteValue;	// if bit 7 of CommandOrIndex == 1 then write value will be
										// sent instead of torque max
	T_U8	u8_CommandOrIndex;			// Number of command or index for eeprom
										// Bit 7: 0 ... read  or status
										//		  1 ... write or command
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROT00R;		// 

typedef struct{
	T_U8	u8_ParamNr;						// should be 00 in this case
	T_U16	u16_RevolutionsPerMinuteMeas;	// Bit 15: 0 ... right, 1 ... left rotation
	T_U8	u8_TorqueMeas;
	T_U8	u8_ReadData;					// Read data if read was asked or error code
	T_U8	u8_CheckSum;					// Sum of all bytes modulo 256
}S_PROT00W; 	// standard protocol write


// EEPROM write (in the old program) (receive and write)
typedef struct{
	T_U8	u8_ParamNr;		// should be 01 in this case
	T_U8	u8_AddressH;	// Adress High (Format) 0 ... 8 Bit, 1 ... 16 Bit
	T_U8	u8_AddressL;	// Adress Low
	T_U8	u8_ValueH;
	T_U8	u8_ValueL;
	T_U8	u8_CheckSum;	// Sum of all bytes modulo 256
}S_PROT01R;  	// )

typedef struct{
	T_U8	u8_ParamNr;		// should be 01 in this case
	T_U8	u8_AddressH;	// Adress High (Format) 0 ... 8 Bit, 1 ... 16 Bit
	T_U8	u8_AddressL;	// Adress Low
	T_U8	u8_ValueH;
	T_U8	u8_ValueL;
	T_U8	u8_CheckSum;	// Sum of all bytes modulo 256
}S_PROT01W;


// EEPROM read (in the old program) (receive and write)
typedef struct{
	T_U8	u8_ParamNr;					// should be 02 in this case
	T_U8	u8_AddressH;				// Adress High (Format) 0 ... 8 Bit, 1 ... 16 Bit
	T_U8	u8_AddressL;				// Adress Low
	T_U8	u8_ValueH;					// not used
	T_U8	u8_ValueL;					// not used
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROT02R;		//

typedef struct{
	T_U8	u8_ParamNr;					// should be 02 in this case
	T_U8	u8_AddressH;				// Adress High (Format) 0 ... 8 Bit, 1 ... 16 Bit
	T_U8	u8_AddressL;				// Adress Low
	T_U8	u8_ValueH;					// read value of eeprom
	T_U8	u8_ValueL;					// read value of eeprom
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROT02W;

// PROTNR_FW_WR protocol # for writing firmware to bldc mirror memory
typedef struct{
	T_U8	u8_ParamNr;					// should be 02 in this case
	T_U16	u16_Address;				// Adress, where the data has to be put
	T_U8	u8_Data[16+16];
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROT64R;

typedef struct{
	T_U8	u8_ParamNr;					// should be 02 in this case
	T_U16	u16_Address;				// Adress, where the data has to been put (FFFF if error)
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROT64W;

// PROTNR_FW_RD protocol # for reading firmware from bldc mirror memory
typedef struct{
	T_U8	u8_ParamNr;					// should be 02 in this case
	T_U16	u16_Address;				// Adress, where the data has to been put (FFFF if error)
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTE4R;

typedef struct{
	T_U8	u8_ParamNr;					// should be 02 in this case
	T_U16	u16_Address;				// Adress, where the data has to be put
	T_U8	u8_Data[16+16];
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTE4W;

// Analogue values read (receive [0] and write [16])
typedef struct{
	T_U8	u8_ParamNr;					// should be 03 in this case
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTF0R;		//

// current analogue values 10 bit [16]
typedef struct{
	T_U8	u8_ParamNr;					// should be 0xF0 in this case
	T_U16	u16_aiP70;					// P7.0 analog input DC Netz
	T_U16	u16_aiP71;					// P7.1 analog input DC ZK
	T_U16	u16_aiP72;					// P7.2 analog input Stomistwert ZK
	T_U16	u16_aiP73;					// P7.3 analog input Spannung am Thyristor f�r die �berbr�ckung
	T_U16	u16_aiP74;					// P7.4 analog input Motortemperatur KTY 11-7
	T_U16	u16_aiP75;					// P7.5 analog input Motortemperatur Schalter PTC (+155�C)
	T_U16	u16_aiP76;					// P7.6 analog input Temperatur Geh�use KTY 11-7
	T_U16	u16_aiP77;					// P7.7 analog input Phasenstrom
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTF0W;

// current analogue values scaled [16]
typedef struct{
	T_U8	u8_ParamNr;					// should be 0xF1 in this case
	T_U16	u16_aiP70;					// P7.0 analog input DC Netz
	T_U16	u16_aiP71;					// P7.1 analog input DC ZK
	T_U16	u16_aiP72;					// P7.2 analog input Stomistwert ZK
	T_U16	u16_aiP73;					// P7.3 analog input Spannung am Thyristor f�r die �berbr�ckung
	T_U16	u16_aiP74;					// P7.4 analog input Motortemperatur KTY 11-7
	T_U16	u16_aiP75;					// P7.5 analog input Motortemperatur Schalter PTC (+155�C)
	T_U16	u16_aiP76;					// P7.6 analog input Temperatur Geh�use KTY 11-7
	T_U16	u16_aiP77;					// P7.7 analog input Phasenstrom
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTF1W;

// current aux values [16]
typedef struct{
	T_U8	u8_ParamNr;					// should be 0xF7 in this case
	T_U16	u16_AuxValue0;				// Auxiliary Value 0
	T_U16	u16_AuxValue1;				// Auxiliary Value 1
	T_U16	u16_AuxValue2;				// Auxiliary Value 2
	T_U16	u16_AuxValue3;				// Auxiliary Value 3
	T_U16	u16_AuxValue4;				// Auxiliary Value 4
	T_U16	u16_AuxValue5;				// Auxiliary Value 5
	T_U16	u16_AuxValue6;				// Auxiliary Value 6
	T_U16	u16_AuxValue7;				// Auxiliary Value 7
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTF7W;

// current hal information [16]
typedef struct{
	T_U8	u8_ParamNr;					// should be 0xF9 in this case
	T_U16	u16_HS1_ON;					// Time at hallsensor 1 ON
	T_U16	u16_HS1_OFF;				// Time at hallsensor 1 OFF
	T_U16	u16_HS2_ON;					// Time at hallsensor 2 ON
	T_U16	u16_HS2_OFF;				// Time at hallsensor 2 OFF
	T_U16	u16_HS3_ON;					// Time at hallsensor 3 ON
	T_U16	u16_HS3_OFF;				// Time at hallsensor 3 OFF
	T_U8	u8_HS_PStatus;				// Hallsensor Portstatus
	T_U8	u8_res1;					// reserve
	T_U16	u16_res3;					// reserve
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTF9W;

// sinus points ??? [16]
typedef struct{
	T_U8	u8_ParamNr;					// should be 0xFA in this case
	T_U8	rau8_SinPoint[16];			// ???
	T_U8	u8_CheckSum;				// Sum of all bytes modulo 256
}S_PROTFEW;


// union for the protocol
typedef union{
	T_U8 au8_ProtData[PROT_LEN_MAX];
	S_PROT10W s_Prot10W;

	S_PROT40R s_Prot40R;
	S_PROT40W s_Prot40W;
	S_PROT00R s_Prot00R;
	S_PROT00W s_Prot00W;
	S_PROT01R s_Prot01R;
	S_PROT01W s_Prot01W;
	S_PROT02R s_Prot02R;
	S_PROT02W s_Prot02W;

	S_PROT64R	s_Prot64R;	// PROTNR_FW_WR protocol # for writing firmware to bldc mirror memory
	S_PROT64W	s_Prot64W;	// PROTNR_FW_WR protocol # for writing firmware to bldc mirror memory

	S_PROTE4R	s_ProtE4R;	// PROTNR_FW_RD protocol # for reading firmware from bldc mirror memory
	S_PROTE4W	s_ProtE4W;	// PROTNR_FW_RD protocol # for reading firmware from bldc mirror memory

	S_PROTF0R s_ProtF0R;
	S_PROTF0W s_ProtF0W;
	S_PROTF1W s_ProtF1W;

	S_PROTF7W s_ProtF7W;

	S_PROTF9W s_ProtF9W;

	S_PROTFEW s_ProtFEW;
}U_PROTOCOL;

// public data
#ifdef ARDUINO_TEST
#else
U_PROTOCOL gu_ProtData;		// union for the protocols
#endif
/*
	Function Header Prototypes
*/
//void function(void);
