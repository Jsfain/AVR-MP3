/*
 * File         : LCD_BASE.C
 * Author       : Joshua Fain
 * Host Target  : ATMega1280
 * License      : MIT
 * LCD          : Gravitech 20x4 LCD using HD44780 LCD controller
 * Copyright (c) 2020
 *
 * Implementation of LCD_BASE.H
 */

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd_base.h"
#include "prints.h"


/*
 ******************************************************************************
 *                            "PRIVATE" FUNCTION
 ******************************************************************************
 */

//
// Called by all of the data port instruction functions to ensure the LCD's
// controller is not busy and to set the control port to 'write mode' and
// ensure the 'data register' has been selected.
//
void pvt_instrPreset (void)
{
  // ensure busy flag not set before proceeding
  lcd_waitClearBusy();

  // Set ctrl port pins
  DATA_REG_SELECT;
  WRITE_MODE;
}


/*
 ******************************************************************************
 *                                 FUNCTIONS
 ******************************************************************************
 */

/* 
  * ---------------------------------------------------------------------------
  *                                                          INITIALIZE THE LCD
  * 
  * Description : The 'Initializing by Instruction' routine - 8-bit mode 
  *               version. This must be executed if the power supply conditions
  *               for operating the internal reset circuit are not met when 
  *               powering up.
  * 
  * Arguments   : void
  * 
  * Returns     : void
  * ---------------------------------------------------------------------------
  */

void lcd_init (void)
{
  // ensure enable is low
  ENABLE_LO;
  
  // Set Data and Control port data direction to output 
  DATA_DDR = DDR_OUTPUT;
  CTRL_DDR = DDR_OUTPUT;

  // Set ctrl port pins to necessary values
  DATA_REG_SELECT;
  WRITE_MODE;

  // Busy flag should not be checked until after these
  // three FUNCTION_SET instructions have been sent.
  _delay_ms(16);
  lcd_sendInstruction (FUNCTION_SET | DATA_LENGTH_8_BITS);
  _delay_ms(5);
  lcd_sendInstruction (FUNCTION_SET | DATA_LENGTH_8_BITS);
  _delay_ms(1);
  lcd_sendInstruction (FUNCTION_SET | DATA_LENGTH_8_BITS);

  // Busy flag can be checked, so now the instruction functions can be used.
  lcd_functionSet (DATA_LENGTH_8_BITS | TWO_LINES | FONT_5x8);
  lcd_displayCtrl (DISPLAY_OFF | CURSOR_OFF | BLINKING_OFF);
  lcd_clearDisplay();
  lcd_entryModeSet (INCREMENT);  
}


// *******************   LCD Data Port Instruction Functions   ****************

/* 
 * ----------------------------------------------------------------------------
 *                                                            CLEAR LCD DISPLAY
 * 
 * Description : Clears the display and sets DDRAM address to 0 in the address 
 *               counter.
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_clearDisplay (void)
{
  pvt_instrPreset();
  lcd_sendInstruction (CLEAR_DISPLAY);
}


/* 
 * ----------------------------------------------------------------------------
 *                                                                  RETURN HOME
 * 
 * Description : Sets DDRAM address to 0 in the address counter, returning the 
 *               dispaly cursor to the 0 position. Display is not changed.
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_returnHome (void)
{
  pvt_instrPreset();
  lcd_sendInstruction (RETURN_HOME);
}


/* 
 * ----------------------------------------------------------------------------
 *                                                               ENTRY MODE SET
 * 
 * Description : The ENTRY MODE SET instruction is used to set the direction
 *               the cursor moves and specifies whether the display will shift.
 *               These will be performed during data writes and reads.
 * 
 * Argument    : setting     The settings of the instruction-specific bits to 
 *                           send with the ENTRY_MODE_SET instruction. The 
 *                           available settings are listed below. The setting
 *                           flags should be OR'd.
 * 
 * Settings    : INCREMENT or           Specifies whether the cursor will
 *               DECREMENT              increment or decrement by 1 upon a
 *                                      data read or write.
 * 
 *               DISPLAY_SHIFT_DATA     Setting this flag will cause the 
 *                                      display to shift upon data read or 
 *                                      write.
 * 
 * Returns     : LCD Error code. INVALID_ARG will be returned if the value of 
 *               settings value is >= ENTRY_MODE_SET. Otherwise 
 *               LCD_INSTR_SUCCESS is returned.
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_entryModeSet (uint8_t setting)
{
  if (setting >= ENTRY_MODE_SET)
    return INVALID_ARG;

  pvt_instrPreset();
  lcd_sendInstruction (ENTRY_MODE_SET | setting);
  return LCD_INSTR_SUCCESS;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                       DISPLAY ON/OFF CONTROL
 * 
 * Description : The display ON or OFF control instruction (DISPLAY_CTRL) is 
 *               used to turn the display ON or OFF, to turn the cursor ON or 
 *               OFF, and to specify whether cursor blinking will be ON or OFF.
 * 
 * Argument    : setting     The settings of the instruction-specific bits to 
 *                           send with the DISPLAY_CTRL instruction. The 
 *                           available settings are listed below. The setting
 *                           flags should be OR'd.
 * 
 * Settings    : DISPLAY_ON or      Specify whether display will be ON or OFF.
 *               DISPLAY_OFF
 * 
 *               CURSOR_ON or       Specify whether cursor will be ON or OFF. 
 *               CURSOR_OFF
 * 
 *               BLINKING_ON or     Specify if cursor blinking is ON or OFF.
 *               BLINKING_OFF                       
 * 
 * Returns     : LCD Error code. INVALID_ARG will be returned if the value of 
 *               settings value is >= DISPLAY_CTRL. Otherwise LCD_INSTR_SUCCESS
 *               is returned.
 * ----------------------------------------------------------------------------
 */

uint8_t
lcd_displayCtrl (uint8_t setting)
{
  if (setting >= DISPLAY_CTRL)
    return INVALID_ARG;

  pvt_instrPreset();
  lcd_sendInstruction (DISPLAY_CTRL | setting);
  return LCD_INSTR_SUCCESS;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                      CURSOR or DISPLAY SHIFT
 * 
 * Description : Immediately moves the cursor or display to the right or left 
 *               left depending on the settings passed as the argument. It does
 *               not require data read or write in order to move the cursor or
 *               display, and it does not matter what the ENTRY_MODE_SET 
 *               settings are.
 * 
 * Argument    : setting     The settings of the instruction-specific bits to 
 *                           send with the CURSOR_DISPLAY_SHIFT instruction.
 *                           The available settings are listed below. The
 *                           setting flags should be OR'd.
 * 
 * Settings    : CURSOR_SHIFT or     Specify display or cursor shift.
 *               DISPLAY_SHIFT
 * 
 *             : RIGHT_SHIFT or      Specify direction of shift.
 *               LEFT_SHIFT          
 *
 * Returns     : LCD Error code. INVALID_ARG will be returned if the value of 
 *               settings value is >= CURSOR_DISPLAY_SHIFT. Otherwise 
 *               LCD_INSTR_SUCCESS is returned.
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_cursorDisplayShift (uint8_t setting)
{
  if (setting >= CURSOR_DISPLAY_SHIFT)
    return INVALID_ARG;

  pvt_instrPreset();
  lcd_sendInstruction (CURSOR_DISPLAY_SHIFT | setting);
  return LCD_INSTR_SUCCESS;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                                 FUNCTION SET
 * 
 * Description : The FUNCTION SET instruction is used to set the interface 
 *               data length (4 or 8 bits), to set the number of display lines,
 *               and set the character font.
 * 
 * Argument    : setting     The settings of the instruction-specific bits to 
 *                           send with the FUNCTION_SET instruction. The 
 *                           available settings are listed below. The setting
 *                           flags should be OR'd.
 * 
 * Settings    : DATA_LENTH_8_BITS or     Operate in 8-bit or 4-bit mode.
 *               DATA_LENTH_4_BITS                      
 *  
 *               TWO_LINES or             Specify number of display lines.
 *               ONE_LINE 
 * 
 *               FONT_5x10 or             Specify 5x10 or 5x8 dot characters.
 *               FONT_5x8                
 * 
 * Returns     : LCD Error code. INVALID_ARG will be returned if the value of 
 *               settings value is >= FUNCTION_SET. Otherwise LCD_INSTR_SUCCESS
 *               is returned.
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_functionSet (uint8_t setting)
{
  if (setting >= FUNCTION_SET)
    return INVALID_ARG;

  pvt_instrPreset();
  lcd_sendInstruction (FUNCTION_SET | setting);
  return LCD_INSTR_SUCCESS;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                            SET CGRAM ADDRESS
 * 
 * Description : The 'set CGRAM address' instruction is used to set the address
 *               of the Character Generator RAM. CGRAM data is sent and 
 *               received after sending this instruction.
 * 
 * Argument    : acg     byte specifying the address the CGRAM pointer will 
 *                       point to. The lowest 6 bits in the acg argument are
 *                       the address.
 * 
 * Returns     : LCD Error code. INVALID_ARG will be returned if the value of 
 *               settings value is >= SET_CGRAM_ADDR. Otherwise 
 *               LCD_INSTR_SUCCESS is returned.
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_setAddrCGRAM (uint8_t acg)
{
  if (acg >= SET_CGRAM_ADDR)
    return INVALID_ARG;

  pvt_instrPreset();
  lcd_sendInstruction (SET_CGRAM_ADDR | acg);
  return LCD_INSTR_SUCCESS;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                            SET DDRAM ADDRESS
 * 
 * Description : The 'set DDRAM address' instruction is used to set the address
 *               of the Display Data RAM. DDRAM data is sent and received after
 *               sending this instruction.
 *  
 * Argument    : add     byte specifying the address the DDRAM pointer will 
 *                       point to. The lowest 7 bits in the add argument are
 *                       the address.
 *
 * Returns     : LCD Error code. INVALID_ARG will be returned if the value of 
 *               settings value is >= SET_DDRAM_ADDR. Otherwise 
 *               LCD_INSTR_SUCCESS is returned.
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_setAddrDDRAM (uint8_t add)
{
  if (add >= SET_DDRAM_ADDR)
    return INVALID_ARG;

  pvt_instrPreset();
  lcd_sendInstruction (SET_DDRAM_ADDR | add);
  return LCD_INSTR_SUCCESS;
}


// *****************   LCD Control Port Instruction Functions   ***************

/* 
 * ----------------------------------------------------------------------------
 *                                                   READ BUSY FLAG and ADDRESS
 * 
 * Description : Reads & returns the busy flag setting and the address counter.
 * 
 * Arguments   : void
 * 
 * Returns     : Byte
 *               [0:6] = The current value in the address counter.
 *               [7]   = Busy Flag. Set to 1 if the controller is busy. 
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_readBusyAndAddr (void)
{
  uint8_t busy_addr;       

  // data pins set to input
  DATA_DDR = DDR_INPUT;

  //
  // Set control port pins. For control port instructions, these settings
  // are the instruction which is "sent" when the ENABLE pin goes high.  
  //
  DATA_REG_SELECT;
  READ_MODE;

  // "send" control port instruction
  ENABLE_HI;

  // wait then read pin values
  _delay_ms(1);
  busy_addr = DATA_PIN;
  _delay_ms(1);

  // reset data pins back to output before exiting
  DATA_DDR = DDR_OUTPUT;
  
  // return the current busy flag and address counter
  return busy_addr;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                 WRITE DATA TO DDRAM or CGRAM
 * 
 * Description : Write data to the DDRAM's or CGRAM's location pointed to by
 *               the address counter. Which RAM is written to will be 
 *               determined by the most recent "set address" instruction that 
 *               was sent (i.e. SET_DDRAM_ADDR or SET_CGRAM_ADDR).
 * 
 * Arguments   : data     data byte that will be written to the DDRAM or 
 *                        CGRAM at the location pointed to by the address 
 *                        counter.
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
*/

void lcd_writeData (uint8_t data)
{
  // ensure LCD controller is not busy
  lcd_waitClearBusy();

  // set control port pins
  INSTR_REG_SELECT;
  WRITE_MODE;

  // write to data port
  DATA_PORT = data;
  
  // wait and pulse enable pin to send the data to LCD.
  _delay_ms(1);
  lcd_pulseEnable();
}


/* 
 * ----------------------------------------------------------------------------
 *                                                READ DATA FROM DDRAM or CGRAM
 * 
 * Description : Read data from the DDRAM's or CGRAM's location pointed to by 
 *               the address counter. Which RAM is read from to will be 
 *               determined by the most recent "set address" instruction that 
 *               was sent (i.e. SET_DDRAM_ADDR or SET_CGRAM_ADDR).
 * 
 * Arguments  : void
 * 
 * Returns    : byte in either CGRAM or DDRAM at the address pointed at by
 *              the address counter.
 * ----------------------------------------------------------------------------
 */

uint8_t lcd_readData (void)
{
  uint8_t data;

  // ensure LCD controller is not busy
  lcd_waitClearBusy();

  DATA_DDR = DDR_INPUT;

  //
  // Set control port pins. For control port instructions, these settings
  // are the instruction which is "sent" when the ENABLE pin goes high.  
  //
  INSTR_REG_SELECT;
  READ_MODE;

  // 'send' the instruction
  _delay_ms(5);
  ENABLE_HI;

  // wait and read in pin values
  _delay_ms(1);
  data = DATA_PIN;
  _delay_ms(1);

  // set data pins back to output before exiting
  DATA_DDR = DDR_OUTPUT;

  // return the CGRAM or DDRAM data
  return data;
}


// ************************   Some helper functions   *************************

/* 
 * ----------------------------------------------------------------------------
 *                                                  WAIT FOR BUSY FLAG TO RESET
 * 
 * Description : Use this function to poll the busy flag. This function will
 *               return once it detects the busy flag is no longer set or 
 *               a timeout has been reached.
 * 
 * Arguments   : void
 * 
 * Returns     : On of the Busy Error Flags. BUSY_RESET_SUCCESS is returned if
 *               the busy flag was found to be reset and the LCD's controller 
 *               is ready to receive the next command. BUSY_RESET_TIMEOUT if 
 *               the flag does not reset after a set timeout period.
 * ----------------------------------------------------------------------------
*/

uint8_t lcd_waitClearBusy (void)
{
  // loop to poll the DATA_PIN to and check if busy flag has cleared
  for (uint8_t timeout = 0; timeout < 0xFE; timeout++)
  {
    //delay between loop iterations
    _delay_ms(1);
    if ( !(lcd_readBusyAndAddr() & BUSY_MASK))
      return BUSY_RESET_SUCCESS;
  }
  // busy flag NOT cleared
  return BUSY_RESET_TIMEOUT;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                             PULSE ENABLE PIN
 * 
 * Description : Pulses the enable pin.  
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * 
 * Notes       : In order to execute an instruction the enable pin must
 *               transition from high to low. This function performs this
 *               operation by setting the enable pin high and then low. This 
 *               function should be called once all the other necessary pins 
 *               have been set according to the desired instruction and 
 *               settings.
 * ----------------------------------------------------------------------------
 */

void lcd_pulseEnable (void)
{
  _delay_ms(0.5);
  ENABLE_HI;
  _delay_ms(0.5);
  ENABLE_LO;
}


/* 
 * ----------------------------------------------------------------------------
 *                                                      SEND INSTRUCTION TO LCD
 * 
 * Description : This function sets the necessary port pins for executing the
 *               instructions and their settings. This function is called by 
 *               all of the data port instruction functions.
 * 
 * Arguments   : instr     instruction and settings that are to be executed by
 *                         the LCDs controller.
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_sendInstruction (uint8_t inst)
{
  // set pins according to the instuction and settings
  DATA_PORT = inst;
  _delay_ms(0.2);
  // 'send' the instruction and settings
  lcd_pulseEnable();
}


/* 
 * ----------------------------------------------------------------------------
 *                                                              PRINT LCD ERROR
 * 
 * Description : Prints the lcd error passed as the argument.
 * 
 * Arguments   : lcd error 
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_printError(uint8_t err)
{
  switch (err)
  {
    case LCD_INSTR_SUCCESS:
      print_Str("\n\rLCD_INSTR_SUCCESS");
      break;
    case INVALID_ARG:
      print_Str("\n\rINVALID_ARGUMENT");
      break;
    case BUSY_RESET_SUCCESS:
      print_Str("\n\rBUSY_RESET_SUCCESS");
      break;
    case BUSY_RESET_TIMEOUT:
      print_Str("\n\rBUSY_RESET_TIMEOUT");
      break;
    default:
      print_Str("\n\rINVALID LCD ERROR");
      break;
  }
}
