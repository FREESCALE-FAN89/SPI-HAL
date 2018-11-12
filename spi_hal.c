
#include "SPI_HAL.h"

typedef struct
{
	uint16_t br;
} SPIHALScaler_t;

typedef struct
{
	uint16_t pbr;
} SPIHALPrescaler_t;

const SPIHALScaler_t SPI_HAL_ScalerTable[] =
{
	2,
	4,
	6,
	8,
	16,
	32,
	64,
	128,
	256,
	512,
	1024,
	2048,
	4096,
	8192,
	16384,
	32768,
};

const SPIHALPrescaler_t SPI_HAL_PrescalerTable[] =
{
	2,
	3,
	5,
	7,
};

void SPI_HAL_Init (void)
{
	SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;
	SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
	SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;

	NVIC_EnableIRQ(SPI0_IRQn);
	NVIC_EnableIRQ(SPI1_IRQn);
	NVIC_EnableIRQ(SPI2_IRQn);

	SPI0->MCR = SPI_MCR_HALT_MASK;
	SPI1->MCR = SPI_MCR_HALT_MASK;
	SPI2->MCR = SPI_MCR_HALT_MASK;

	SPI0->MCR = SPI_MCR_MSTR_MASK | SPI_MCR_DIS_TXF_MASK | SPI_MCR_DIS_RXF_MASK  | SPI_MCR_CLR_TXF_MASK  | SPI_MCR_CLR_RXF_MASK | SPI_MCR_HALT_MASK;
	SPI1->MCR = SPI_MCR_MSTR_MASK | SPI_MCR_DIS_TXF_MASK | SPI_MCR_DIS_RXF_MASK  | SPI_MCR_CLR_TXF_MASK  | SPI_MCR_CLR_RXF_MASK | SPI_MCR_HALT_MASK;
	SPI2->MCR = SPI_MCR_MSTR_MASK | SPI_MCR_DIS_TXF_MASK | SPI_MCR_DIS_RXF_MASK  | SPI_MCR_CLR_TXF_MASK  | SPI_MCR_CLR_RXF_MASK | SPI_MCR_HALT_MASK;
}






void SPI_HAL_SetSPE	(SPI_t spi, bool mode)
{
	spi->MCR = (spi->MCR & ~(SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK)) | SPI_MCR_MDIS(!mode) | SPI_MCR_HALT(!mode);
}

void SPI_HAL_SetBaudRate (SPI_t spi, SPIBaud_t baudrate)
{
	uint32_t pbr, br, currentPBR, computedRate, absError;
	uint32_t bestError = 0xFFFFFFFF;
	uint32_t bestPBR = 0;
	uint32_t bestBR = 0;
	uint32_t bestDBR = (baudrate > (__CORE_CLOCK__ >> 3))?(1):(0);
	uint8_t  pbrSize = (sizeof(SPI_HAL_PrescalerTable) / sizeof(SPIHALPrescaler_t));
	uint8_t  brSize = (sizeof(SPI_HAL_ScalerTable) / sizeof(SPIHALScaler_t));
	uint32_t clock = __CORE_CLOCK__ >> (1 - bestDBR);

	for (pbr = 0; (pbr < pbrSize) && (bestError != 0); pbr++)
	{
		currentPBR = SPI_HAL_PrescalerTable[pbr].pbr;

		for (br = 0; br < brSize; br++)
		{
			computedRate = clock / (currentPBR * SPI_HAL_ScalerTable[br].br);
			absError = (baudrate > computedRate)?(baudrate - computedRate):(computedRate - baudrate);

			if (absError < bestError)
			{
				bestPBR = pbr;
				bestBR = br;
				bestError = absError;

				if (!absError)
					break;
			}
		}
	}

	spi->CTAR[0] = (spi->CTAR[0] & ~(SPI_CTAR_DBR_MASK | SPI_CTAR_PBR_MASK | SPI_CTAR_BR_MASK)) | SPI_CTAR_BR(bestBR) | SPI_CTAR_PBR(bestPBR) | SPI_CTAR_DBR(bestDBR);
}


void SPI_HAL_SetDataSize (SPI_t spi, SPISize_t size)
{
	SPISize_t s = ((size >= 4) && (size <= 16))?(size - 1):((size < 4)?(4 - 1):(16 - 1));
	spi->CTAR[0] = (spi->CTAR[0] & ~SPI_CTAR_FMSZ_MASK) | SPI_CTAR_FMSZ(s);
}


/*

void SPI_HAL_PCSIS (SPI_t spi, SPISelect_t select)
{
	SPI0->MCR = (SPI0->MCR & ~SPI_MCR_PCSIS_MASK) | SPI_MCR_PCSIS(select);
}


*/

/*
 *
void SPI_HAL_SetCPOL (SPI_t spi, bool mode)
{
	spi->CTAR[0] = (spi->CTAR[0] & ~SPI_CTAR_CPOL_MASK) | SPI_CTAR_CPOL(mode);
}

void SPI_HAL_SetCPHA (SPI_t spi, bool mode)
{
	spi->CTAR[0] = (spi->CTAR[0] & ~SPI_CTAR_CPHA_MASK) | SPI_CTAR_CPHA(mode);
}

void SPI_HAL_SetLSBFE (SPI_t spi, bool mode)
{
	spi->CTAR[0] = (spi->CTAR[0] & ~SPI_CTAR_LSBFE_MASK) | SPI_CTAR_LSBFE(mode);
}

void SPI_HAL_SetSPTIE (SPI_t spi, bool mode)
{
	spi->RSER = (spi->RSER & ~SPI_RSER_TFFF_RE_MASK) | SPI_RSER_TFFF_RE(mode);
}

void SPI_HAL_SetSPRIE (SPI_t spi, bool mode)
{
	spi->RSER = (spi->RSER & ~SPI_RSER_RFDF_RE_MASK) | SPI_RSER_RFDF_RE(mode);
}

bool SPI_HAL_GetSPTE (SPI_t spi)
{
	return spi->SR & SPI_SR_TFFF_MASK;
}

void SPI_HAL_ClearSPTE (SPI_t spi)
{
	spi->SR = SPI_SR_TFFF_MASK;
}

bool SPI_HAL_GetSPRF (SPI_t spi)
{
	return spi->SR & SPI_SR_RFDF_MASK;
}

void SPI_HAL_ClearSPRF (SPI_t spi)
{
	spi->SR = SPI_SR_RFDF_MASK | SPI_SR_TCF_MASK;
}

void SPI_HAL_SetData (SPI_t spi, SPISelect_t select, SPIData_t data)
{
	spi->PUSHR = SPI_PUSHR_PCS(select) | SPI_PUSHR_TXDATA(data);
}

SPIData_t SPI_HAL_GetData (SPI_t spi)
{
	uint32_t mask = (1 << (spi->CTAR[0] & SPI_CTAR_FMSZ_MASK + 1)) - 1;
	mask = (mask < 0x000F)?(0x000F):(mask);

	return spi->POPR & mask;
}


*/
