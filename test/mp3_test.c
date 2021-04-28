/*
 *                           Test file for MP3 Module
 *
 * File       : MP3_TEST.C
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

#include <string.h>
#include <stdint.h>
#include <util/delay.h>
#include "usart0.h"
#include "spi.h"
#include "prints.h"
#include "sd_spi_base.h"
#include "sd_spi_rwe.h"
#include "fat_bpb.h"
#include "fat.h"
#include "fat_to_disk_if.h"
#include "mp3.h"

#define SD_CARD_INIT_ATTEMPTS_MAX      5
#define CMD_LINE_MAX_CHAR              100  // max num of chars of a cmd/arg
#define MAX_ARG_CNT                    10   // max num of CL arguments
#define BACKSPACE                      127  // used for keyboard backspace here


int main(void)
{
  // Initializat usart and spi ports.
  usart_Init();
  spi_MasterInit();

  //
  // SD card initialization
  //
  CTV ctv;          
  uint32_t sdInitResp;

  // Loop will continue until SD card init succeeds or max attempts reached.
  for (uint8_t att = 0; att < SD_CARD_INIT_ATTEMPTS_MAX; ++att)
  {
    print_Str("\n\n\r >> SD Card Initialization Attempt "); 
    print_Dec(att);
    sdInitResp = sd_InitModeSPI(&ctv);      // init SD Card

    if (sdInitResp != OUT_OF_IDLE)          // Fail to init if not OUT_OF_IDLE
    {    
      print_Str(": FAILED TO INITIALIZE SD CARD."
                " Initialization Error Response: "); 
      sd_PrintInitError(sdInitResp);
      print_Str(" R1 Response: "); 
      sd_PrintR1(sdInitResp);
    }
    else
    {   
      print_Str(": SD CARD INITIALIZATION SUCCESSFUL");
      break;
    }
  }
    print_Str("\n\rDDRD = 0x");
    print_Hex(DDRD);
  DDRD = (1 << DDD0); // XRESET output
  DDRD = DDRD & ~(1 << DDD1); // DREQ is input
    print_Str("\n\rDDRD = 0x");
    print_Hex(DDRD);
    print_Str("\n\rDREQ = 0x");
    print_Hex(PIND & (1 << DREQ));
  VSReset();
      print_Str("\n\rDREQ = 0x");
    print_Hex(PIND & (1 << DREQ));
  spi_MasterTransmit(VS_SCI_MODE);
  spi_MasterTransmit((uint8_t)(SM_SDINEW >> 8));
  spi_MasterTransmit(SM_RESET);
   // print_Str("\n\rDREQ = 0x");
   // print_Hex(PIND);
//  VSSCIWrite(VS_SCI_MODE, SM_SDINEW | SM_RESET); //soft reset, set to new mode and disable layer 1-2
	//_delay_ms(1);
	while ( !(PIND & (1 << DREQ))) {
    //print_Str("DREQ = 0x");
    print_Hex(PIND);
  }
      print_Str("\n\rDREQ = 0x");
    print_Hex(PIND & (1 << DREQ));


/*
  for(;;) {
    usart_Transmit(usart_Receive());
    XCS_ASSERT;
    usart_Transmit(usart_Receive());
    XDCS_ASSERT;
    usart_Transmit(usart_Receive());
    CS_SD_LOW;
    usart_Transmit(usart_Receive());
  }
*/
  return 0;
}
