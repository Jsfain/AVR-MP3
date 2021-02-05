/*
 * File        : PRINT_LCD_TEST.C
 * Author      : Joshua Fain
 * Host Target : ATMega1280
 * LCD         : Gravitech 20x4 LCD using HD44780 LCD controller
 * License     : MIT
 * Copyright (c) 2020, 2021
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

#define MODULE_INIT_SUCCESS  0
#define MODULE_INIT_FAILED   1

//navigation macros
#define NEXT   'n'               // next entry in the directory
#define SELECT 's'               // select current displayed item
#define UP     'u'               // up a directory level. e.g. songs --> albums


void PrintToLCD(char * ln);
uint8_t InitModules(void);

int main(void)
{
  // initialize usart, spi, lcd, and sd card and only continue if success.
  if (InitModules() == MODULE_INIT_SUCCESS)
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
//    FatDir *artistDirPtr = malloc(sizeof(FatDir));
//    FatDir *albumsDirPtr = malloc(sizeof(FatDir));
//    FatDir *songsDirPtr  = malloc(sizeof(FatDir));
    FatDir *cwdPtr  = malloc(sizeof(FatDir));
    //fat_setDirToRoot (cwdPtr, bpbPtr);


    // 
    // Create and initialize a FatEntry instance. This will set the snEntClusIndx
    // member to the root directory and all other members to 0 or null strings.
    // Afterwards, update the snEntClusIndx to point to the first cluster index of 
    // the FatDir instance (dir).
    //
    FatEntry * entPtr = malloc(sizeof(FatEntry));
    // fat_initEntry (entPtr, bpbPtr);
    usart_transmit('\n');
    usart_transmit('\r');

    uint8_t fatErr;
    char c;

    // artist set to root directory. Should never change.
    //fat_setDirToRoot (cwdPtr, bpbPtr);
    while (1)
    {
      
      //                                    ARTISTS
      //
      
      // artist set to root directory.
      fat_setDirToRoot (cwdPtr, bpbPtr);
      fat_initEntry (entPtr, bpbPtr);

      while (1)
      {
        print_str("\n\rARTISTS");
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
            }


            //                               ABLUMS
            //
            c = usart_receive();
            if (c == UP)
              break;
            else if (c == SELECT && entPtr->snEnt[11] & DIR_ENTRY_ATTR)
            {
              fat_setDir (cwdPtr, entPtr->lnStr, bpbPtr);
              // set fat entry to first entry of new directory
              fat_initEntry (entPtr, bpbPtr);
              entPtr->snEntClusIndx = cwdPtr->fstClusIndx;

              while (1)
              {
                print_str("\n\rALBUMS");
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
                    }

                    //                               SONGS
                    //
                    c = usart_receive();
                    if (c == UP)
                    {
                      fat_setDir (cwdPtr, "..", bpbPtr);
                      fat_initEntry (entPtr, bpbPtr);
                      entPtr->snEntClusIndx = cwdPtr->fstClusIndx;
                      break;
                    }
                    else if (c == SELECT && entPtr->snEnt[11] & DIR_ENTRY_ATTR)
                    {
                      fat_setDir (cwdPtr, entPtr->lnStr, bpbPtr);
                      // set fat entry to first entry of new directory
                      fat_initEntry (entPtr, bpbPtr);
                      entPtr->snEntClusIndx = cwdPtr->fstClusIndx;

                      while (1)
                      {
                        print_str("\n\rSONGS");
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
                            }
                            c = usart_receive();
                            if (c == UP)
                            {
                              fat_setDir (cwdPtr, "..", bpbPtr);
                              fat_initEntry (entPtr, bpbPtr);
                              entPtr->snEntClusIndx = cwdPtr->fstClusIndx;
                              break;                            
                            }
                            else if (c == SELECT && !(entPtr->snEnt[11] & DIR_ENTRY_ATTR))
                            {
                              print_str("Playing Song: ");
                              PrintToLCD(entPtr->lnStr);
                            }
                            // Songs else
                            else
                              continue;
                          }
                        }
                        // reset for song loop
                        else
                        {
                          fat_initEntry (entPtr, bpbPtr);
                          entPtr->snEntClusIndx = cwdPtr->fstClusIndx;
                        }
                      }
                    }
                    // Albums else
                    else
                      continue;
                  }
                }
                // reset for album loop
                else
                {
                  fat_initEntry (entPtr, bpbPtr);
                  entPtr->snEntClusIndx = cwdPtr->fstClusIndx;
                }
              }
            }
            // Artists else
            else
              continue;
          }
        }
        else
          break;
      }
    }
/*
    while(1)
    {
      // search artists
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
*/
  }
  else
    print_str("\n\rFailed to initialize");

  return 0;
}





uint8_t InitModules(void)
{
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
    sdInitResp = sd_spiModeInit (ctvPtr);        // init SD card into SPI mode
    if (sdInitResp != 0)   
      continue;
    else
      return MODULE_INIT_SUCCESS;
  }
  return MODULE_INIT_FAILED;
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
