/*
 * File        : LCD_SF.C
 * Author      : Joshua Fain
 * Host Target : ATMega1280
 * LCD         : Gravitech 20x4 LCD with built-in HD44780 controller
 * License     : MIT
 * Copyright (c) 2020, 2021
 *
 * Implementation of LCD_SF.H
 */

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd_base.h"
#include "lcd_sf.h"
#include "prints.h"


/*
 ******************************************************************************
 *                                FUNCTIONS
 ******************************************************************************
 */

/* 
 * ----------------------------------------------------------------------------
 *                                                         READ ADDRESS COUNTER
 * 
 * Description : Gets the value in the address counter. It does this by calling 
 *               lcd_readBusyAndAddr() and clearing the busy flag from its 
 *               returned value, leaving only the address counter value.
 * 
 * Arguments   : None
 * 
 * Returns     : Current value in the address counter.
 * -----------------------------------------------------------------------------
 */

uint8_t lcd_readAddr (void)
{
  // extract and return address.
  return (ADDRESS_MASK & lcd_readBusyAndAddr());
}


/* 
 * ----------------------------------------------------------------------------
 *                                                   CURSOR RIGHT or LEFT SHIFT
 * 
 * Description : Shift the cursor one position to the right or left, depending 
 *               on which function is called. These functions will call
 *               lcd_cursorDisplayShift() with the appropriate arguments. 
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_rightShiftCursor (void)
{
  lcd_cursorDisplayShift (CURSOR_SHIFT | RIGHT_SHIFT);
}

void lcd_leftShiftCursor (void)
{
  lcd_cursorDisplayShift (CURSOR_SHIFT | LEFT_SHIFT);
}


/* 
 * ----------------------------------------------------------------------------
 *                                                  DISPLAY RIGHT or LEFT SHIFT
 * 
 * Description : Shift the entire display one position to the right or left, 
 *               depending on which function is called. These functions will 
 *               call lcd_cursorDisplayShift() with the appropriate arguments. 
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_rightShiftDisplay (void)
{
  lcd_cursorDisplayShift (DISPLAY_SHIFT | RIGHT_SHIFT);
}

void lcd_leftShiftDisplay (void)
{
  lcd_cursorDisplayShift (DISPLAY_SHIFT | LEFT_SHIFT);
}



