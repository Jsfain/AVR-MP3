/*
 * File        : LCD_SF.H
 * Author      : Joshua Fain
 * Host Target : ATMega1280
 * LCD         : Gravitech 20x4 LCD with built-in HD44780 controller
 * License     : MIT
 * Copyright (c) 2020, 2021
 *
 * Prototypes of special functions for operating the LCD. These functions 
 * are implemented as special cases of some of the instructions available in
 * LCD_BASE and thus require the functions in LCD_BASE. These functions are
 * non-mode setting, and execute single one-time actions, such as shifting the
 * cursor to the right or left and/or reading the value of the address counter.
 * These are implemented in order to simplify some of the instruction calls
 * in LCD_BASE without needing to supply the specific settings.
 */

#ifndef LCD_SF_H
#define LCD_SF_H

#include <avr/io.h>


/*
 ******************************************************************************
 *                                FUNCTION PROTOTYPES
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

uint8_t lcd_readAddr(void);


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

void lcd_rightShiftCursor(void);
void lcd_leftShiftCursor(void);


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

void lcd_rightShiftDisplay(void);
void lcd_leftShiftDisplay(void);



#endif // LCD_SF_H