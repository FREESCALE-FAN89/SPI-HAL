
#ifndef SOURCES_SPI_HAL_H_
#define SOURCES_SPI_HAL_H_

#define SPI_HAL_DEFAULT_BAUDRATE 5000000

typedef SPI_Type *SPI_t; /* SPI 0-2 */
typedef uint8_t SPISelect_t;
typedef uint16_t SPIData_t;
typedef uint8_t SPISize_t;
typedef uint32_t SPIBaud_t;

void 	  SPI_HAL_Init 	  	  (void);

void 	  SPI_HAL_PCSIS		  (SPI_t, SPISelect_t);

void 	  SPI_HAL_SetSPE	  (SPI_t, bool);

void 	  SPI_HAL_SetBaudRate (SPI_t, SPIBaud_t);
void 	  SPI_HAL_SetDataSize (SPI_t, SPISize_t);

void 	  SPI_HAL_SetCPOL	  (SPI_t, bool);
void 	  SPI_HAL_SetCPHA	  (SPI_t, bool);
void 	  SPI_HAL_SetLSBFE	  (SPI_t, bool);

void 	  SPI_HAL_SetSPTIE	  (SPI_t, bool);
void 	  SPI_HAL_SetSPRIE	  (SPI_t, bool);
bool 	  SPI_HAL_GetSPTE	  (SPI_t);
void 	  SPI_HAL_ClearSPTE	  (SPI_t);
bool 	  SPI_HAL_GetSPRF	  (SPI_t);
void 	  SPI_HAL_ClearSPRF	  (SPI_t);

void 	  SPI_HAL_SetData	  (SPI_t, SPISelect_t, SPIData_t);
SPIData_t SPI_HAL_GetData	  (SPI_t);

#endif /* SOURCES_SPI_HAL_H_ */




