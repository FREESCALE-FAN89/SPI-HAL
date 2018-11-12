
#include "SPI.h"
#include "PORT.h"

#define TX_FIFO_IS_FULL  0
#define TX_FIFO_IS_EMPTY 1

#define RX_FIFO_IS_FULL  1
#define RX_FIFO_IS_EMPTY 0


#define SPI_ENABLE  1
#define SPI_DISABLE 0

volatile uint16_t gdato;

void SPI_Init (void)
{
	SPI_HAL_Init();

	SPI_HAL_SetBaudRate(SPI0, 100000);
	SPI_HAL_SetDataSize(SPI0, 8);

	SPI0->RSER = (SPI0->RSER & ~SPI_RSER_RFDF_RE_MASK) | SPI_RSER_RFDF_RE(1); // Irq on/off

	SPI_HAL_SetSPE(SPI0, true);  //SPi Enable
	Spi_Port_Init();
}



void Spi_Port_Init (void)
{

		PCRstr UserPCR;

		UserPCR.PCR=false;					// Default All false, Set only those needed

		UserPCR.FIELD.DSE=true;				// Drive strength
		UserPCR.FIELD.MUX=PORT_mAlt2;		// SPI0
		UserPCR.FIELD.IRQC=PORT_eDisabled;  // No irqs

		PORT_Configure2 (PORTD,0,UserPCR);  // SPI0 PCS0
		PORT_Configure2 (PORTD,1,UserPCR);  // SPI0 SCK
		PORT_Configure2 (PORTD,2,UserPCR);  // SPI0 Sout (MOSI)
		PORT_Configure2 (PORTD,3,UserPCR);  // SPI0 Sin  (MISO)

		//	PORTD_GPCLR=0;  // TODO !!!!
		//	PORTD_GPCHR=0;


}



void my_SPI_Send(uint16_t data)
{

	while ((SPI0->SR & SPI_SR_TFFF_MASK)==TX_FIFO_IS_FULL); 		// Wait until TXmitter is ready

	SPI0->SR = SPI_SR_TFFF_MASK;   								// Clear flag
	SPI0->PUSHR=SPI_PUSHR_PCS(0x01) | SPI_PUSHR_TXDATA(data); 	// Activo el PCS0 (0x01) y envio data


}




uint16_t my_SPI_Recieve(void)
{
	volatile uint32_t mask,nshift,tmp;
	volatile uint32_t SRR;

	while ((SPI0->SR & SPI_SR_RFDF_MASK)==RX_FIFO_IS_EMPTY); /// Wait until Rcvr is ready

	SRR=SPI0->SR;
	// Since Frame size is variable we need
	// Mask out from received data those bits that are not part of Frame size
	// Eg.: given Frame size=4 then mask is 1111

	// Algorithm
	// 1-Isolate Frame size from CTAR  xfff fxxx xxxx xxxx   xxxx xxxx xxxx xxxx -->  ffff (Frame size=nshift)
    // In our case it is 4
	nshift = (SPI0->CTAR[0] & SPI_CTAR_FMSZ_MASK) >> SPI_CTAR_FMSZ_SHIFT ;

	// 2- Create 2^(Framesize)  In our case 0000 0000 0000 0000 - 0000 0000 0000 1000

	tmp = 1 << nshift;

	// 3 - mask = (2^Framesize-1) | 2^Framesize   In our case it is: 0111 | 1000 = 1111 =mask

	mask=( tmp-1) | tmp;

    // Limit Lower bound in case frame size was wrong chosen

	mask = (mask < 0x000F)?(0x000F):(mask);


//	SPI0->SR = SPI_SR_RFDF_MASK ;    // Clear flag NO here

	tmp=SPI0->POPR & mask;  // Mask out received data to frame size

	SPI0->SR = SPI_SR_RFDF_MASK ;    // Clear flag OK here

	SRR=SPI0->SR;

	return tmp;  // Mask out received data to frame size

}




__ISR__ SPI0_IRQHandler (void)

{
	volatile uint16_t dato;

	dato=my_SPI_Recieve();

    gdato=dato;
}


