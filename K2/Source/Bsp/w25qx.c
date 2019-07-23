/***************************************************************************************************
* FILE: w25qx.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/9, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "bsp.h"
#include "w25qx.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define W25Q80_ID                           0xEF13
#define W25Q80_FLASH_SIZE                   0x100000    /* 1MBytes */
#define W25Q80_BLOCK_SIZE                   0x10000     /* 64KBytes */
#define W25Q80_SECTOR_SIZE                  0x1000      /* 4kBytes */
#define W25Q80_PAGE_SIZE                    0x100       /* 256Bytes */

#define W25Q16_ID                           0xEF14
#define W25Q16_FLASH_SIZE                   0x100000    /* 1MBytes */
#define W25Q16_BLOCK_SIZE                   0x10000     /* 64KBytes */
#define W25Q16_SECTOR_SIZE                  0x1000      /* 4kBytes */
#define W25Q16_PAGE_SIZE                    0x100       /* 256Bytes */

#define W25Q32_ID                           0xEF15
#define W25Q32_FLASH_SIZE                   0x100000    /* 1MBytes */
#define W25Q32_BLOCK_SIZE                   0x10000     /* 64KBytes */
#define W25Q32_SECTOR_SIZE                  0x1000      /* 4kBytes */
#define W25Q32_PAGE_SIZE                    0x100       /* 256Bytes */

#define W25Q64_ID                           0xEF16
#define W25Q64_FLASH_SIZE                   0x100000    /* 1MBytes */
#define W25Q64_BLOCK_SIZE                   0x10000     /* 64KBytes */
#define W25Q64_SECTOR_SIZE                  0x1000      /* 4kBytes */
#define W25Q64_PAGE_SIZE                    0x100       /* 256Bytes */

#define W25Q128_ID                          0xEF17
#define W25Q128_FLASH_SIZE                  0x1000000   /* 16MBytes */
#define W25Q128_BLOCK_SIZE                  0x10000     /* 64KBytes */
#define W25Q128_SECTOR_SIZE                 0x1000      /* 4kBytes */
#define W25Q128_PAGE_SIZE                   0x100       /* 256Bytes */


#define W25Qx_BLOCK_ERASE_MAX_TIME          3000
#define W25Qx_SECTOR_ERASE_MAX_TIME         800
#define W25Qx_TIMEOUT_VALUE                 1000


/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* Flag Status Register */
#define W25Q128FV_FSR_BUSY                  ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                  ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                    ((uint8_t)0x02)    /*!< quad enable */


#define W25Qx_Enable()                      SPI1_CS_ENABLE()
#define W25Qx_Disable()                     SPI1_CS_DISABLE()



struct flash_info
{
    uint16_t id;
    uint32_t size;
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
};

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
static struct flash_info W25QxInfo;
/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/


/***************************************************************************************************
* Description:  
***************************************************************************************************/
static void	W25Qx_Reset(void)
{
	uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};
	
	W25Qx_Enable();
	/* Send the reset command */
	HAL_SPI_Transmit(&hspi1, cmd, 2, W25Qx_TIMEOUT_VALUE);	
	W25Qx_Disable();
}

static uint8_t W25Qx_CheckBusy(void)
{
	uint8_t cmd[] = {READ_STATUS_REG1_CMD};
	uint8_t status;
	
	W25Qx_Enable();
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25Qx_TIMEOUT_VALUE);	
	HAL_SPI_Receive(&hspi1,&status, 1, W25Qx_TIMEOUT_VALUE);
	W25Qx_Disable();
	
    if((status & W25Q128FV_FSR_BUSY) != 0)
    {
        return W25Qx_BUSY;
    }
	else
	{
		return W25Qx_OK;
	}		
}

/***************************************************************************************************
* Description:  ³õÊ¼»¯
***************************************************************************************************/
void W25Qx_Init(void)
{ 
	W25Qx_Reset();
	
	W25Qx_CheckBusy();
    
    W25QxInfo.id = W25Qx_ReadId();
    
    switch(W25QxInfo.id)
    {
        case W25Q80_ID:
        W25QxInfo.size = W25Q80_FLASH_SIZE;
        break;
        case W25Q16_ID:
        W25QxInfo.size = W25Q16_FLASH_SIZE;
        break;
        case W25Q32_ID:
        W25QxInfo.size = W25Q32_FLASH_SIZE;
        break;
        case W25Q64_ID:
        W25QxInfo.size = W25Q64_FLASH_SIZE;
        break;
        case W25Q128_ID:
        W25QxInfo.size = W25Q128_FLASH_SIZE;
        break;
        default:
        W25QxInfo.size = W25Q80_FLASH_SIZE;
        break;
    }
    
    W25QxInfo.block_size = W25Q80_BLOCK_SIZE;
    W25QxInfo.sector_size = W25Q80_SECTOR_SIZE;
    W25QxInfo.page_size = W25Q80_PAGE_SIZE;
}

/***************************************************************************************************
* Description:  ¶ÁÈ¡FLASH ID
***************************************************************************************************/
uint16_t W25Qx_ReadId(void)
{
	uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
    uint8_t id_buf[2] = {0};
    uint16_t id;
	
	W25Qx_Enable();
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25Qx_TIMEOUT_VALUE);	
	HAL_SPI_Receive(&hspi1, id_buf, 2, W25Qx_TIMEOUT_VALUE);
	W25Qx_Disable();
    
    id = (id_buf[0] << 8) + id_buf[1];
    
    return id; 
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint8_t W25Qx_WriteEnable(void)
{
	uint8_t cmd[] = {WRITE_ENABLE_CMD};
	uint32_t tickstart = HAL_GetTick();
    
	/*Select the FLASH: Chip Select low */
	W25Qx_Enable();
	/* Send the read ID command */
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25Qx_TIMEOUT_VALUE);	
	/*Deselect the FLASH: Chip Select high */
	W25Qx_Disable();
	
	/* Wait the end of Flash writing */
	while(W25Qx_CheckBusy() == W25Qx_BUSY);
	{
		/* Check for the Timeout */
        if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
        {        
			return W25Qx_TIMEOUT;
        }
	}
	
	return W25Qx_OK;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint8_t W25Qx_Read(uint32_t ReadAddr, uint8_t* pData, uint32_t Size)
{
	uint8_t cmd[4];
    
	/* Configure the command */
	cmd[0] = READ_CMD;
	cmd[1] = (uint8_t)(ReadAddr >> 16);
	cmd[2] = (uint8_t)(ReadAddr >> 8);
	cmd[3] = (uint8_t)(ReadAddr);
	
	W25Qx_Enable();
	/* Send the read ID command */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25Qx_TIMEOUT_VALUE);	
	/* Reception of the data */
	if (HAL_SPI_Receive(&hspi1, pData,Size,W25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
        return W25Qx_ERROR;
    }
	W25Qx_Disable();
	return W25Qx_OK;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint8_t W25Qx_Write(uint32_t WriteAddr, uint8_t* pData, uint32_t Size)
{
	uint8_t cmd[4];
	uint32_t end_addr, current_size, current_addr;
	uint32_t tickstart = HAL_GetTick();
	
	/* Calculation of the size between the write address and the end of the page */
    current_addr = 0;
    
    while (current_addr <= WriteAddr)
    {
        current_addr += W25QxInfo.page_size;
    }
    current_size = current_addr - WriteAddr;
    
    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
        current_size = Size;
    }
    
    /* Initialize the adress variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;
	
    /* Perform the write page by page */
    do
    {
		/* Configure the command */
		cmd[0] = PAGE_PROG_CMD;
		cmd[1] = (uint8_t)(current_addr >> 16);
		cmd[2] = (uint8_t)(current_addr >> 8);
		cmd[3] = (uint8_t)(current_addr);
        
		/* Enable write operations */
		W25Qx_WriteEnable();
        
		W25Qx_Enable();
        /* Send the command */
        if (HAL_SPI_Transmit(&hspi1,cmd, 4, W25Qx_TIMEOUT_VALUE) != HAL_OK)
        {
            return W25Qx_ERROR;
        }
        
        /* Transmission of the data */
        if (HAL_SPI_Transmit(&hspi1, pData,current_size, W25Qx_TIMEOUT_VALUE) != HAL_OK)
        {
            return W25Qx_ERROR;
        }
        W25Qx_Disable();
    	/* Wait the end of Flash writing */
		while(W25Qx_CheckBusy() == W25Qx_BUSY);
		{
			/* Check for the Timeout */
			if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
			{        
				return W25Qx_TIMEOUT;
			}
		}
        
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + W25QxInfo.page_size) > end_addr) ? (end_addr - current_addr) : W25QxInfo.page_size;
    } while (current_addr < end_addr);
    
	return W25Qx_OK;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint8_t W25Qx_EraseBlock(uint32_t Address)
{
	uint8_t cmd[4];
	uint32_t tickstart = HAL_GetTick();
	cmd[0] = SECTOR_ERASE_CMD;
	cmd[1] = (uint8_t)(Address >> 16);
	cmd[2] = (uint8_t)(Address >> 8);
	cmd[3] = (uint8_t)(Address);
	
	/* Enable write operations */
	W25Qx_WriteEnable();
	
	/*Select the FLASH: Chip Select low */
	W25Qx_Enable();
	/* Send the read ID command */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25Qx_TIMEOUT_VALUE);	
	/*Deselect the FLASH: Chip Select high */
	W25Qx_Disable();
	
	/* Wait the end of Flash writing */
	while(W25Qx_CheckBusy() == W25Qx_BUSY);
	{
		/* Check for the Timeout */
        if((HAL_GetTick() - tickstart) > W25Qx_SECTOR_ERASE_MAX_TIME)
        {        
			return W25Qx_TIMEOUT;
        }
	}
	return W25Qx_OK;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint8_t W25Qx_EraseChip(void)
{
	uint8_t cmd[4];
	uint32_t tickstart = HAL_GetTick();
	cmd[0] = SECTOR_ERASE_CMD;
	
	/* Enable write operations */
	W25Qx_WriteEnable();
	
	/*Select the FLASH: Chip Select low */
	W25Qx_Enable();
	/* Send the read ID command */
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25Qx_TIMEOUT_VALUE);	
	/*Deselect the FLASH: Chip Select high */
	W25Qx_Disable();
	
	/* Wait the end of Flash writing */
	while(W25Qx_CheckBusy() != W25Qx_BUSY);
	{
		/* Check for the Timeout */
//        if((HAL_GetTick() - tickstart) > W25Q128FV_BULK_ERASE_MAX_TIME)
//        {
//			return W25Qx_TIMEOUT;
//        }
	}
	return W25Qx_OK;
}

/****************************************** END OF FILE *******************************************/

