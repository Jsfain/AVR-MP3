#include <avr/io.h>
#include <util/delay.h>
#include "mp3.h"
#include "prints.h"


void VSReset(void)
{
	/*
  Clrb(VS_RST_PRTC,VS_RST_PIN);
	delay_1ms(500);
	Setb(VS_RST_PRTS,VS_RST_PIN);
	delay_1ms(100);
  */
         print_Str("\n\rIN RESET \n\rDREQ = 0x");
    print_Hex(PIND & (1 << DREQ));
  HW_RST_ASSERT
  _delay_ms(500);
          print_Str("\n\rDREQ = 0x");
    print_Hex(PIND & (1 << DREQ));
  HW_RST_DEASSERT
  _delay_ms(500);
          print_Str("\n\rDREQ = 0x");
    print_Hex(PIND & (1 << DREQ));
           print_Str("\n\rEXIT RESET");
}

/*
void VSSCIWrite(unsigned char ad, unsigned short data)
{
	unsigned char spi_buf[4];
	spi_buf[0] = VS_INS_WRITE;
	spi_buf[1] = ad;
	spi_buf[2] = data >> 8;
	spi_buf[3] = data;

	VSSCITransfer(4,spi_buf);
}

void VSSDITransfer(unsigned int len, unsigned char *spi_buf)
{
	//VSSPIInit();
	
	Clrb(VS_DCS_PRTC,VS_DCS_PIN);
	
	SPIMasterTransfer2(VS_SPI,len,spi_buf);
	
	Setb(VS_DCS_PRTS,VS_DCS_PIN);
	
	SPIMasterDisable(VS_SPI);
}
*/
