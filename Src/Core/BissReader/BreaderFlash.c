#include "BreaderConfig.h"
#include "BreaderTypes.h"

#define FLASH_START_ADDRESS   0x08000000
#define FLASH_END_ADDRESS     0x08020000
#define FLASH_PAGE_SIZE       0x800
#define FLASH_62_START_ADDR   (0x08020000 - 0x800 - 0x800)
#define FLASH_63_START_ADDR   (0x08020000 - 0x800)

/*  G431KB */
//  Start 0x08000000 - 0x0801FFFF
//  Size  128KB (0x20000)   
//  Page  2KB (0x800) x 64 
//  Program Unit  64-bit

extern structUserData gstUserData;

void WriteDataPage63(void)
{
  	HAL_StatusTypeDef status = HAL_OK;
    FLASH_EraseInitTypeDef eraseInit;
    U32 pageError = 0;
	U8 *puData;
	U32 u32Size = 2048; // 2K

	puData = (U8*)&gstUserData;

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        return status;
    }

    eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInit.Page        = 63;
    eraseInit.NbPages     = 1;
    eraseInit.Banks       = FLASH_BANK_1;

    status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    for (U32 offset = 0; offset <u32Size ; offset += 8)
    {
        U64 u64Data;
        memcpy(&u64Data, &puData[offset], sizeof(U64));

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                    FLASH_63_START_ADDR + offset,
                                    u64Data);
        if (status != HAL_OK)
        {
            break;
        }
    }

    HAL_FLASH_Lock();

    return status;
}

void ReadDataPage63(void)
{
	U8 *puData;

	puData = (U8*)&gstUserData;
	memcpy(puData,FLASH_63_START_ADDR,sizeof(gstUserData));
}
