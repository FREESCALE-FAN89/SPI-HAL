
#ifndef SOURCES_SPI_H_
#define SOURCES_SPI_H_

#include "Template/LDM.h"
#include "SPI_HAL.h"

typedef enum
{
	SPI_mPolling,
	SPI_mInterrupt,
} SPIMode_t;

typedef enum
{
	SPI_sIdle,
	SPI_sRxTx,

	SPI_sError,
} SPIStatus_t;


void Spi_Port_Init (void);
void SPI_Init (void);

SPIStatus_t SPI_Status (SPI_t);
SPIStatus_t SPI_Mode   (SPI_t, SPIMode_t);

SPIStatus_t SPI_RxTx   (SPI_t, SPISelect_t, SPIData_t*, SPISize_t);

void my_SPI_Send(uint16_t data);
uint16_t my_SPI_Recieve(void);

#endif /* SOURCES_SPI_H_ */
