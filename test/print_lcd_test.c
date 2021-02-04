/*
 *                             TESTING FOR AVR-LCD
 *
 * File        : LCD_TEST.C
 * Author      : Joshua Fain
 * Host Target : ATMega1280
 * LCD         : Gravitech 20x4 LCD using HD44780 LCD controller
 * License     : MIT
 * Copyright (c) 2020, 2021
 *
 * Contains main(). Used to test the AVR-LCD Module. This test implements an 
 * interface that allows direct typing from a keyboard via the AVRs USART0 port
 * and redirects the characters entered to the LCD screen.
 *
 * NOTE:
 * Before simply sending a character to the LCD, the test program performs
 * several checks, such as checking to see wheter control or display 
 * character(s) were entered. It also tests to see if the address counter
 * should be adjusted. The way the LCD itself works, when the address counter
 * is pointing at the last address of a line, the next sequential address would
 * actually point to the first position 2 lines down, with the exception of the
 * last line. Thus, when at the end of a line and navigating to the next 
 * position, either by entering a character or pressing the right arrow key,
 * then the address counter will be updated by software to point to the first 
 * DDRAM address of the next line, rather than the next sequential DDRAM 
 * address. Similarly, for when moving backwards/decrementing. The opposite
 * should occur. 
 * 
 * The display lines map to the DDRAM addresses as follows:
 *
 * Line 1: DDRAM Address - 0x00 - 0x13
 * Line 2: DDRAM Address - 0x40 - 0x53
 * Line 3: DDRAM Address - 0x14 - 0x27
 * Line 4: DDRAM Address - 0x54 - 0x67
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "prints.h"
#include "usart0.h"
#include "spi.h"
#include "sd_spi_base.h"
#include "sd_spi_rwe.h"
#include "fat_bpb.h"
#include "fat.h"
#include "fat_to_disk.h"
#include "lcd_addr.h"
#include "lcd_base.h"
#include "lcd_sf.h"


// 127 = backspace/delete for my apple keyboard
#define BACK_SPACE           127

// some control operations using apple keyboard
#define HOME                 0x08      // ctrl + h
#define CLEAR                0x03      // ctrl + c
#define R_DISP_SHIFT         0x08      // ctrl + d

// for left and right arrows
/*#define ARROW_CTRL_2         0x1B
#define ARROW_CTRL_1         0x5B
#define L_ARROW              0x44
#define R_ARROW              0x43
*/
void PrintToLCD(char * ln);

int main(void)
{
  // --------------------------------------------------------------------------
  //                                 INITIALIZATIONS - USART, SPI, LCD, SD CARD
  
  // usart required for character entry
  usart_init();
  spi_masterInit();

  // Ensure LCD is initialized.
  lcd_init();

  // Turn display and cursor on and set cursor to blink 
  lcd_displayCtrl (DISPLAY_ON | CURSOR_ON | BLINKING_ON);

  // SD card initialization
  CTV *ctvPtr = malloc(sizeof(CTV));             // SD card type & version
  uint32_t sdInitResp;                           // SD card init error response

  // Attempt SD card init up to 5 times.
  for (uint8_t i = 0; i < 5; i++)
  {
    print_str ("\n\n\r >> SD Card Initialization Attempt "); 
    print_dec(i);
    sdInitResp = sd_spiModeInit (ctvPtr);        // init SD card into SPI mode

    if (sdInitResp != 0)
    {    
      print_str (": FAILED TO INITIALIZE SD CARD.");
      print_str (" Initialization Error Response: "); 
      sd_printInitError (sdInitResp);
      print_str (", R1 Response: "); 
      sd_printR1 (sdInitResp);
    }
    else
    {   
      print_str (": SD CARD INITIALIZATION SUCCESSFUL");
      break;
    }
  }
  
  if (sdInitResp == 0)
  {
    uint8_t err;                                 // for returned errors
    

    //
    // Create and set Bios Parameter Block instance. Members of this instance
    // are used to calculate where on the physical disk volume, the FAT 
    // sectors/blocks are located. This should only be set once here.
    //
    BPB *bpbPtr = malloc(sizeof(BPB));
    err = fat_setBPB (bpbPtr);
    if (err != BOOT_SECTOR_VALID)
    {
      print_str("\n\r fat_setBPB() returned ");
      fat_printBootSectorError(err);
    }
    

    //
    // Create and set a FatDir instance. Members of this instance are used for
    // holding parameters of a FAT directory. This instance can be treated as
    // the current working directory. The instance should be initialized to 
    // the root directory with fat_setDirToRoot() prior to using anywhere else.
    //
    FatDir *cwdPtr = malloc(sizeof(FatDir));
    fat_setDirToRoot (cwdPtr, bpbPtr);


    // 
    // Create and initialize a FatEntry instance. This will set the snEntClusIndx
    // member to the root directory and all other members to 0 or null strings.
    // Afterwards, update the snEntClusIndx to point to the first cluster index of 
    // the FatDir instance (dir).
    //
    FatEntry * entPtr = malloc(sizeof(FatEntry));
    fat_initEntry (entPtr, bpbPtr);
    usart_transmit('\n');
    usart_transmit('\r');


    uint8_t fatErr;

    while(1)
    {
      fatErr = fat_setNextEntry (cwdPtr, entPtr, bpbPtr);
      if (fatErr != END_OF_DIRECTORY)
      {     
        // only print hidden entries if the HIDDEN filter flag is been set.
        if (!(entPtr->snEnt[11] & HIDDEN_ATTR))
        {
          // if entry not pointing to current director or parent directory entry.
          if (strcmp(entPtr->snStr, ".") && strcmp(entPtr->snStr, ".."))
          {
            PrintToLCD(entPtr->lnStr);
            usart_transmit('\n');
            usart_transmit('\r');
            if (usart_receive() == 'o' && entPtr->snEnt[11] & DIR_ENTRY_ATTR)
            {
              fat_setDir (cwdPtr, entPtr->lnStr, bpbPtr);
              // set fat entry to first entry of new directory
              fat_initEntry (entPtr, bpbPtr);
              entPtr->snEntClusIndx = cwdPtr->fstClusIndx;
            }
          }
        }
      }
      else
      {
        fat_initEntry (entPtr, bpbPtr);
        entPtr->snEntClusIndx = cwdPtr->fstClusIndx;
      }
    }
  }
  return 0;
}


void PrintToLCD(char *ln)
{
  lcd_clearDisplay();
  lcd_returnHome();

  print_str(ln);
 
  int ndx = 0;
  
  while (ln[ndx] != '\0')
  {
    lcd_writeData(ln[ndx]);
    ndx++;
  }
}
