#ifndef INC_TYPES_H_
#define INC_TYPES_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_conf.h"

typedef int8_t                  S8 ;  //!< 8-bit signed integer.
typedef uint8_t                 U8 ;  //!< 8-bit unsigned integer.
typedef int16_t                 S16;  //!< 16-bit signed integer.
typedef uint16_t                U16;  //!< 16-bit unsigned integer.
typedef uint16_t                le16_t;
typedef uint16_t                be16_t;
typedef int32_t                 S32;  //!< 32-bit signed integer.
typedef uint32_t                U32;  //!< 32-bit unsigned integer.
typedef uint32_t                le32_t;
typedef uint32_t                be32_t;
typedef int64_t                 S64;  //!< 64-bit signed integer.
typedef uint64_t                U64;  //!< 64-bit unsigned integer.
typedef float                   F32;  //!< 32-bit floating-point number.
typedef double                  F64;  //!< 64-bit floating-point number.
typedef uint32_t                iram_size_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Config */
typedef enum
{
	UART_RX_BUFFER_MAX = 256,
	UART_TX_BUFFER_MAX = 200
}Uart_Buffer_Length_Config;

/* Enum definition */

enum PROTOCOL_TYPE_ENUM
{
	PROTOCOL_SSI  = 0,
	PROTOCOL_BISS = 1
};

enum PROTOCOL_BITS_ENUM
{
	PROTOCOL_BIT_17  = 0,
	PROTOCOL_BIT_18  = 1,
	PROTOCOL_BIT_19  = 2,
	PROTOCOL_BIT_20  = 3,
	PROTOCOL_BIT_21  = 4	
};

enum BITS_RESOLUTION_ENUM
{
	RESOLUTION_17BIT  = 131072,
	RESOLUTION_18BIT  = 262144,
	RESOLUTION_19BIT  = 524288,
	RESOLUTION_20BIT  = 1048576,
	RESOLUTION_21BIT  = 2097152	
};

enum FLAG_SET_RESET_ENUM
{
	FLAG_0          = 0,
	FLAG_1          = 1
};

enum BISS_READ_ERROR_ENUM
{
	NO_ERROR         = 0,
	COMM_ERROR       = 1,	
};



enum LED_TYPE_ENUM
{
	LED0 = 0,
	LED1 = 1,
	LEDn
};


/* Type definition */
typedef struct
{
	U8 aubTxBuf[4];	
	U8 aubRxBuf[4];
	//__attribute__((aligned(4))) U16 auwTxBuf[2];
	//__attribute__((aligned(4))) U16 auwRxBuf[2];
}structSpiTxRxBuf;


typedef struct{
	U32 u32ReceivedData;
	U32 u32BitData;
	U32 u32AngleE5;
	U32 u32AngleE5Prev;
	S32 s32AngleDiffE5;
	U32 u32Ptype;
	U32 u32Pbit;	
	U32 u32CommError;           // 0 : OK,  1 : Error
	U32 u32CrcErrCnt;
	U32 u32DispCnt;
}structBreaderData;


typedef struct {
    I2C_HandleTypeDef *hi2c;     // I2C handler for communication
    uint8_t address;            // I2C address of the LCD
} I2C_LCD_HandleTypeDef;

#endif /* INC_TYPES_H_ */
