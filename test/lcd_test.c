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
#include <avr/io.h>
#include <util/delay.h>
#include "prints.h"
#include "usart0.h"
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
#define ARROW_CTRL_2         0x1B
#define ARROW_CTRL_1         0x5B
#define L_ARROW              0x44
#define R_ARROW              0x43


int main(void)
{
  // usart required for character entry
  usart_init();

  // Ensure LCD is initialized.
  lcd_init();

  // Turn display and cursor on and set cursor to blink 
  lcd_displayCtrl (DISPLAY_ON | CURSOR_ON | BLINKING_ON);

  char c;                              // for input characters
  uint8_t addr;                        // ddram address

  do
  {
    // get input character
    c = usart_receive();      

    //
    // If "backspace" is entered, this section will backspace, delete
    // character, backspace. Note this is set for use with a mac keyboard
    // where the backspace/delete key = 127 (delete).
    //
    if (c == BACK_SPACE)
    {        
      // set display to decrement address counter (AC)
      lcd_entryModeSet (DECREMENT);

      // get current value of AC
      addr = lcd_readAddr();

      // If AC adjust is needed, set it accordingly, else LEFT_SHIFT.

      if (LINE_2_BEG == addr)
        lcd_setAddrDDRAM (LINE_1_END);
      else if (LINE_3_BEG == addr)
        lcd_setAddrDDRAM (LINE_2_END);
      else if (LINE_4_BEG == addr)
        lcd_setAddrDDRAM (LINE_3_END);
      else
        lcd_leftShiftCursor();

      // Write space to display to clear value, and reset to INCREMENT mode.
      lcd_writeData (' ');
      lcd_rightShiftCursor();
      lcd_entryModeSet(INCREMENT);
    }

    // if 'ENTER' is pressed, point AC to the first address of the next line.
    else if (c == '\r')
    {
      addr = lcd_readAddr();

      if (addr >= LINE_1_BEG && addr <= LINE_1_END)
        lcd_setAddrDDRAM (LINE_2_BEG);
      else if (addr >= LINE_3_BEG && addr <= LINE_3_END)
        lcd_setAddrDDRAM (LINE_4_BEG);
      else if (addr >= LINE_2_BEG && addr <= LINE_2_END)
        lcd_setAddrDDRAM (LINE_3_BEG);
      else if (addr >= LINE_4_BEG && addr <= LINE_4_END)
        lcd_setAddrDDRAM (LINE_1_BEG);
    }

    // if ctrl + 'h', return home. 
    else if (c == HOME)
      lcd_returnHome();
    
    // if ctrl + 'c', clear screen.
    else if (c == CLEAR) 
      lcd_clearDisplay();

    // if ctrl + 'd', shift display.
    else if (c == R_DISP_SHIFT) 
      lcd_rightShiftDisplay ();

    //
    // If right or left arrow is pressed then move cursor in that direction.
    // Note on mac keyboard, right and left arrows are 3 characters long and 
    // only differ in the third character. Right = 0x1B5B43, Left = 0x1B5B44.
    //
    else if (c == ARROW_CTRL_2)
    {
      c = usart_receive();
      if (c == ARROW_CTRL_1)
      {
        addr = lcd_readAddr();
        c = usart_receive();

        // Left arrow
        if (c == L_ARROW)
        {
          if (LINE_2_BEG == addr)
            lcd_setAddrDDRAM (LINE_1_END);
          else if (LINE_3_BEG == addr)
            lcd_setAddrDDRAM(LINE_2_END);
          else if (LINE_4_BEG == addr)
            lcd_setAddrDDRAM(LINE_3_END);
          else
            lcd_leftShiftCursor();
        }
        // right arrow
        else if (c == R_ARROW)
        {
          if (addr == LINE_1_END)
            lcd_setAddrDDRAM(LINE_2_BEG);
          else if (addr == LINE_2_END)
            lcd_setAddrDDRAM(LINE_3_BEG);
          else if (addr == LINE_3_END)
            lcd_setAddrDDRAM(LINE_4_BEG);
          else
            lcd_rightShiftCursor();
        }
      }
    } 

    // print character to LCD display
    else 
    {
      lcd_writeData(c);

      // AC adjustments if at the start of a display line.
      addr = lcd_readAddr();

      if (addr == LINE_3_BEG)
        lcd_setAddrDDRAM (LINE_2_BEG);
      else if (addr == LINE_2_BEG)
        lcd_setAddrDDRAM (LINE_4_BEG);
      else if (addr == LINE_4_BEG)
        lcd_setAddrDDRAM (LINE_3_BEG);
    }
  } 
  while (1);

  return 1;
}