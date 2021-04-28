/*
 *
 * File       : MP3_TEST.H
 * Author     : Joshua Fain
 * Target     : ATMega1280
 * Compiler   : AVR-GCC 9.3.0
 * Downloader : AVRDUDE 6.3
 * License    : GNU GPLv3
 * Author     : Joshua Fain
 * Copyright (c) 2020, 2021
 * 
 * DESCRIPTION: 
 *
 */

#ifndef MP3_H
#define MP3_H

/* 
 * ----------------------------------------------------------------------------
 *                                                                  CHIP SELECT
 *
 * Description : defines the SPI port's chip select pin for the SD card.
 * 
 * Notes       : 1) Assert by setting CS low. De-Assert by setting CS high.
 *               2) SSO, defined in SPI.H, is used here as the CS pin.
 *               3) SPI.H also defines a second SS pin (SS1) for connecting 
 *                  multiple devices to the SPI port of the AVR. Therefore, 
 *                  this MACRO must also ensure the SS1 pin is de-asserted 
 *                  (set high) when SS0 is asserted (set low).
 * ----------------------------------------------------------------------------
 */
#define XCS_ASSERT      SPI_PORT = ((SPI_PORT & ~(1 << SS1)) | (1 << SS0)      \
                                   | (1 << SS2));
#define XCS_DEASSERT    SPI_PORT |= (1 << SS1);

#define XDCS_ASSERT     SPI_PORT = ((SPI_PORT & ~(1 << SS2)) | (1 << SS0)      \
                                   | (1 << SS1));
#define XDCS_DEASSERT   SPI_PORT |= (1 << SS2);


#define XRESET           PD0
#define HW_RST_ASSERT    PORTD = (PORTD & ~(1 << XRESET));
#define HW_RST_DEASSERT  PORTD |= (1 << XRESET);

#define DREQ     PIND1 





//SCI Registers
#define VS_SCI_MODE		  0x00	//RW	Mode control
#define VS_SCI_STATUS	  0x01	//RW	Status
#define VS_SCI_BASS		  0x02	//RW	Built-in bass enhancer
#define VS_SCI_CLOCKF	  0x03	//RW	Clock freq+doubler
#define VS_SCI_DEC_TIME	0x04	//R		Decode time in seconds 
#define VS_SCI_AUDATA	  0x05	//RW	Misc. audio data
#define VS_SCI_WRAM		  0x06	//RW	RAM write
#define VS_SCI_WRAMADDR	0x07	//RW	Base address for RAM write
#define VS_SCI_HDAT0	  0x08	//R		Stream header data 0
#define VS_SCI_HDAT1	  0x09	//R		Stream header data 1
#define VS_SCI_AIADDR	  0x0A	//RW	Start address of application
#define VS_SCI_VOL	  	0x0B	//RW	Volume control
#define VS_SCI_AICTRL0	0x0C	//RW	Application control register 0
#define VS_SCI_AICTR11	0x0D	//RW	Application control register 1
#define VS_SCI_AICTRL2	0x0E	//RW	Application control register 2
#define VS_SCI_AICTRL3	0x0F	//RW	Application control register 3


// SCI_MODE
#define SM_DIFF         0x0001    // Differential
#define SM_LAYER12      0x0002    // Allow MPEG layers I & II
#define SM_RESET        0x0004    // Soft reset
#define SM_OUTOFWAV     0x0008    // Jump out of WAV decoding
#define SM_SETTOZERO1   0x0010    // Set to zero
#define SM_TESTS        0x0020    // Allow SDI tests
#define SM_STREAM       0x0040    // Stream mode
#define SM_SETTOZERO2   0x0080    // set to zero
#define SM_DACT         0x0100    // DCLK active edge
#define SM_SDIORD       0x0200    // SDI bit order
#define SM_SDISHARE     0x0400    // Share SPI chip select
#define SM_SDINEW       0x0800    // VS1002 native SPI modes
#define SM_SETTOZERO3   0x1000    // Set to zero
#define SM_SETTOZERO4   0x2000    // Set to zero
 

void VSReset(void);
void VSSCIWrite(unsigned char ad, unsigned short data);
void VSSDITransfer(unsigned int len, unsigned char *spi_buf);

#endif // MP3_H