#include "BreaderConfig.h"
#include "BreaderTypes.h"

U32 u32SetSsiErrorCnt;

U8 au8TableCRC6[64] = {
				   0x00, 0x03, 0x06, 0x05, 0x0C, 0x0F, 0x0A, 0x09,
				   0x18, 0x1B, 0x1E, 0x1D, 0x14, 0x17, 0x12, 0x11,
				   0x30, 0x33, 0x36, 0x35, 0x3C, 0x3F, 0x3A, 0x39,
				   0x28, 0x2B, 0x2E, 0x2D, 0x24, 0x27, 0x22, 0x21,
				   0x23, 0x20, 0x25, 0x26, 0x2F, 0x2C, 0x29, 0x2A,
				   0x3B, 0x38, 0x3D, 0x3E, 0x37, 0x34, 0x31, 0x32,
				   0x13, 0x10, 0x15, 0x16, 0x1F, 0x1C, 0x19, 0x1A,
				   0x0B, 0x08, 0x0D, 0x0E, 0x07, 0x04, 0x01, 0x02
				   };

U32 au32BitResMapping[5] = {
	RESOLUTION_17BIT,
	RESOLUTION_18BIT,
	RESOLUTION_19BIT,
	RESOLUTION_20BIT,
	RESOLUTION_21BIT
};

extern structBreaderData gstBreaderData;


U8 Encoder_BiSS_CRC_Calculate(U32 u32Data)
{
	//The CRC polynomial for position, error and warning data is: x6 + x1 + x0 (0x43). 

	U8 crc;
	U32 tmp;
		
	tmp = (u32Data >> 30) & 0x00000003;
	
	crc = ((u32Data >> 24) & 0x0000003F);
	tmp = crc ^ au8TableCRC6[tmp];
	
	crc = ((u32Data >> 18) & 0x0000003F);
	tmp = crc ^ au8TableCRC6[tmp];
	
	crc = ((u32Data >> 12) & 0x0000003F);
	tmp = crc ^ au8TableCRC6[tmp];
	
	crc = ((u32Data >> 6) & 0x0000003F);
	tmp = crc ^ au8TableCRC6[tmp];
	
	crc = (u32Data & 0x0000003F);
	tmp = crc ^ au8TableCRC6[tmp];

	crc = au8TableCRC6[tmp];

	crc= (~crc & 0x3F);	
	
	return crc;

}

U32 GetAngle_FromBiSS(U32 ulVal,U32 ulBits )
{
	U32 u32BitShiftData,u32crcData,u32AngleBits;
    U8 u8CRC,u8CRCCalc,u8flagWarn,u8flagError;
    U32 u32Angle;

	u32BitShiftData = ulVal;
	
	u8CRC = (U8)(u32BitShiftData & 0x3f);
	u32BitShiftData = u32BitShiftData >> 6;

	u8flagWarn = (U8)(u32BitShiftData & 0x1);
	u32BitShiftData = u32BitShiftData >> 1;
	
	
	u8flagError = (U8)(u32BitShiftData & 0x1);
	u32BitShiftData = u32BitShiftData >> 1;

	switch (ulBits)
	{
		case PROTOCOL_BIT_17:
			u32AngleBits = (u32BitShiftData & 0x1FFFF);
			break;
		case PROTOCOL_BIT_18:
			u32AngleBits = (u32BitShiftData & 0x3FFFF);		
			break;
		case PROTOCOL_BIT_19:
			u32AngleBits = (u32BitShiftData & 0x7FFFF);				
			break;
		case PROTOCOL_BIT_20:
			u32AngleBits = (u32BitShiftData & 0xFFFFF);						
			break;
		default:
			break;
	}

	u32crcData = (u32AngleBits << 2) |(u8flagError << 1) | (u8flagWarn);		// position, error and warning  data
	u8CRCCalc = Encoder_BiSS_CRC_Calculate(u32crcData);									// get CRC

	if(u8CRC == u8CRCCalc)
    {
		u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / au32BitResMapping[ulBits]))*100000);
#if 0		
		switch (ulBits)
		{
			case PROTOCOL_BIT_17:
				u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / au32BitResMapping[ulBits]))*100000);
				break;
			case PROTOCOL_BIT_18:
				u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_18BIT))*100000);
				break;
			case PROTOCOL_BIT_19:
				u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_19BIT))*100000);
				break;
			case PROTOCOL_BIT_20:
				u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_20BIT))*100000);
				break;
			default:
				break;
		}
#endif

	}
	else
	{
		gstBreaderData.u32CrcErrCnt++;
		gstBreaderData.u32CommError = (gstBreaderData.u32CrcErrCnt > 2) ? COMM_ERROR:NO_ERROR;
		u32Angle =0;
	}

	return u32Angle;
}

U32 GetAngle_FromSSI(U32 ulVal,U32 ulBits )
{
	U32 u32BitShiftData,u32AngleBits;
    U32 u32Angle;

	u32AngleBits = ulVal;

	if(u32AngleBits == (au32BitResMapping[ulBits]-1))
	{
		u32SetSsiErrorCnt++;
		if(u32SetSsiErrorCnt > 1000)
		{
			gstBreaderData.u32CommError = COMM_ERROR;
		}
	}
	else
	{
		u32SetSsiErrorCnt=0;
	}

	u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / au32BitResMapping[ulBits]))*100000);

#if 0	
	switch (ulBits)
	{
		case PROTOCOL_BIT_17:
    		u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_17BIT))*100000);
			break;
		case PROTOCOL_BIT_18:
    		u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_18BIT))*100000);
			break;
		case PROTOCOL_BIT_19:
    		u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_19BIT))*100000);
			break;
		case PROTOCOL_BIT_20:
    		u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_20BIT))*100000);
			break;
		case PROTOCOL_BIT_21:
    		u32Angle = (U32)(((F64)u32AngleBits * (F64)(360.0 / RESOLUTION_21BIT))*100000);
			break;

		default:
			break;
	}
#endif

	return u32Angle;
}
