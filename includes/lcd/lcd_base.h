/*
 * File        : LCD_BASE.H
 * Author      : Joshua Fain
 * Host Target : ATMega1280
 * LCD         : Gravitech 20x4 LCD with built-in HD44780 controller
 * License     : MIT
 * Copyright (c) 2020, 2021
 *
 * Provides macros and basic function prototypes for interfacing with the LCD.
 * The functions here are used to implement the basic instructions available to
 * the HD44780. 'setting' arguments can be passed to the basic functions with
 * the corresponding function requires setting certain bits to specify 
 * features, settings, or operation. These settings are available as the MACRO 
 * flags defined here.
 */


#ifndef LCD_BASE_H
#define LCD_BASE_H

#include <avr/io.h>

/*
 ******************************************************************************
 *                                    MACROS
 ******************************************************************************
 */

#define DDR_INPUT            0x00           /* use to set DDRs to input */
#define DDR_OUTPUT           0xFF           /* use to set DDRs to output */


/*
 * ----------------------------------------------------------------------------
 *                                                                 CONTROL PORT
 * 
 * The control port contains the 3 control PINS - Register select, Read/Write,
 * and Enable.
 * 
 * RS : Determines whether to access the data or instruction register.
 *  0 - Data Register is selected
 *  1 - Instruction Register is selected
 * 
 * RW : Determines whether operating in Read or Write mode.
 *  0 = Write mode
 *  1 = Read mode
 * ----------------------------------------------------------------------------
 */

#define CTRL_DDR             DDRC         /* Control Port Direction Register */
#define CTRL_PORT            PORTC            
#define RS                   PC0                           /* Reg select */
#define RW                   PC1                           /* Read/Write */
#define EN                   PC2                           /* Enable */

// REGISTER SELECT
#define DATA_REG_SELECT      CTRL_PORT &= ~(1 << RS)       /* RS = 0 */
#define INSTR_REG_SELECT     CTRL_PORT |=  (1 << RS)       /* RS = 1 */

// READ/WRITE
#define WRITE_MODE           CTRL_PORT &= ~(1 << RW)       /* RW = 0 */
#define READ_MODE            CTRL_PORT |=  (1 << RW)       /* RW = 1 */

// ENABLE
#define ENABLE_LO            CTRL_PORT &= ~(1 << EN)       /* EN = 0 */
#define ENABLE_HI            CTRL_PORT |=  (1 << EN)       /* EN = 1 */


/*
 * ----------------------------------------------------------------------------
 *                                                                    DATA PORT
 * 
 * Macros to define the data port and pins.
 * 
 * Notes: To write values to PINS that are to be sent to the LCD's controller,
 *        then must write to the chosesn AVR PORT when the pins are configured
 *        for output. When reading in PIN values set by the LCD, then must read 
 *        the PIN register (not the PORT) when the chosen AVR port pins are
 *        configured for input. Therefore, from the macros below, use DATA_PORT
 *        when sending values and DATA_PIN when retrieving values. Use
 *        DATA_DDR to set whether the pins are input or output. Though the 
 *        data direction for each pin can be set individually, this should not
 *        be necessary in this implementation. To set all the port pins to 
 *        output, DATA_DDR = 0xFF and for input, DATA_DDR = 0.
 * ----------------------------------------------------------------------------
 */

#define DATA_DDR             DDRA           /* Data Port Direction Register */
#define DATA_PORT            PORTA          /* for sending OUT values */
#define DATA_PIN             PINA           /* for reading IN values */


/*
 * ----------------------------------------------------------------------------
 *                                                             LCD INSTRUCTIONS
 * 
 * These are the data port instructions that can be issued to the LCDs 
 * controller (HD44780U), meaning these are sent to the LCD on the DATA_PORT
 * pins. With the exception of CLEAR_DISPLAY and RETURN_HOME, when an 
 * instruction is sent to the LCD the setting of bits in positions less than
 * the instruction bit position are used to implement the specific settings 
 * available to that instruction.  Therefore, the instruction bit is the 
 * highest bit in the instruction byte.
 * 
 * Notes : (1) The specific settings available to each instruction are provided
 *             in the macro definitions immediately below this section, i.e. in
 *             the INSTRUCTION SETTINGS section.
 * 
 *         (2) There are three more instructions in addition to those here, but
 *             these are determined by the settings of RS and RW in the
 *             CTRL_PORT.
 * ----------------------------------------------------------------------------
 */

#define CLEAR_DISPLAY        0x01
#define RETURN_HOME          0x02
#define ENTRY_MODE_SET       0x04
#define DISPLAY_CTRL         0x08
#define CURSOR_DISPLAY_SHIFT 0x10
#define FUNCTION_SET         0x20
#define SET_CGRAM_ADDR       0x40
#define SET_DDRAM_ADDR       0x80


/*
 * ----------------------------------------------------------------------------
 *                                                     LCD INSTRUCTION SETTINGS
 * 
 * These macro definitions are the options available to the DATA PORT 
 * instructions defined above.
 *  
 * Notes : Only use the settings available to a specific instruction, as
 *         indicated by the comment above each settings grouping.
 * ----------------------------------------------------------------------------
 */

// ENTRY_MODE_SET
#define INCREMENT            0x02
#define DECREMENT            0x00
#define DISPLAY_SHIFT_DATA   0x01

// DISPLAY_ON_OFF_CTRL
#define DISPLAY_ON           0x04
#define DISPLAY_OFF          0x00
#define CURSOR_ON            0x02
#define CURSOR_OFF           0x00
#define BLINKING_ON          0x01
#define BLINKING_OFF         0x00

// CURSOR_DISPLAY_SHIFT
#define DISPLAY_SHIFT        0x08
#define CURSOR_SHIFT         0x00
#define RIGHT_SHIFT          0x04
#define LEFT_SHIFT           0x00

// FUNCTION_SET
#define DATA_LENGTH_8_BITS   0x10
#define DATA_LENGTH_4_BITS   0x00
#define TWO_LINES            0x08
#define ONE_LINE             0x00
#define FONT_5x10            0x04
#define FONT_5x8             0x00


/*
 * ----------------------------------------------------------------------------
 *                                                      INSTRUCTION ERROR FLAGS
 * 
 * Errors flags returned by the instruction functions
 * ----------------------------------------------------------------------------
 */

#define LCD_INSTR_SUCCESS    0x00
#define INVALID_ARG          0x01


/*
 * ----------------------------------------------------------------------------
 *                                                        BUSY and ADDRESS MASK
 * 
 * These are used to filter the busy flag and the address counter value from 
 * the byte returned by lcd_readBusyAndAddr().
 * ----------------------------------------------------------------------------
 */

#define ADDRESS_MASK    0x7F      
#define BUSY_MASK       0x80


/*
 * ----------------------------------------------------------------------------
 *                                                             BUSY ERROR FLAGS
 * 
 * Errors flags returned by lcd_waitBusy(). These are used in conjuction with
 * the instruction error flags above.
 * ----------------------------------------------------------------------------
 */

#define BUSY_RESET_SUCCESS   0x02
#define BUSY_RESET_TIMEOUT   0x04



/*
 ******************************************************************************
 *                              FUNCTION PROTOTYPES
 ******************************************************************************
 */

/* 
 * ----------------------------------------------------------------------------
 *                                                           INITIALIZE THE LCD
 * 
 * Description : The 'Initializing by Instruction' routine - 8-bit mode
 *               version. This must be executed if the power supply conditions
 *               for operating the internal reset circuit are not met when 
 *               powering up.
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_init (void);


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

void lcd_clearDisplay (void);


/* 
 * ----------------------------------------------------------------------------
 *                                                                  RETURN HOME
 * 
 * Description : Sets DDRAM address to 0 in the address counter. Returns 
 *               dispaly to original position. DDRAM contents are not changed.
 * 
 * Arguments   : void
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_returnHome (void);


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

uint8_t lcd_entryModeSet (uint8_t setting);


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

uint8_t lcd_displayCtrl (uint8_t setting);


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

uint8_t lcd_cursorDisplayShift (uint8_t setting);


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

uint8_t lcd_functionSet (uint8_t setting);


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

uint8_t lcd_setAddrCGRAM (uint8_t acg);


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

uint8_t lcd_setAddrDDRAM (uint8_t add);



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

uint8_t lcd_readBusyAndAddr (void);


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

void lcd_writeData (uint8_t data);


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

uint8_t lcd_readData (void);



// ************************   Some helper functions   *************************

/* 
 * ----------------------------------------------------------------------------
 *                                                  WAIT FOR BUSY FLAG TO RESET
 * 
 * Description : Use this function to poll the busy flag. This function will
 *               return once it detects the busy flag is no longer set.
 * 
 * Arguments   : void
 * 
 * Returns     : Busy Error Flag. BUSY_RESET_SUCCESS if the busy flag was found
 *               to be reset and the LCD's controller is ready to receive the 
 *               next command. BUSY_RESET_TIMEOUT if the flag does not reset 
 *               after a set timeout period.
 * ----------------------------------------------------------------------------
*/

uint8_t lcd_waitClearBusy (void);


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

void lcd_pulseEnable (void);


/* 
 * ----------------------------------------------------------------------------
 *                                                      SEND INSTRUCTION TO LCD
 * 
 * Description : This function sets the necessary port pins for executing the
 *               instructions and their settings. This function is called by 
 *               all of the instruction functions.
 * 
 * Arguments   : cmd     instruction and settings that are to be executed by
 *                       the LCDs controller.
 * 
 * Returns     : void
 * ----------------------------------------------------------------------------
 */

void lcd_sendInstruction (uint8_t cmd);


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

void lcd_printError(uint8_t err);


#endif // LCD_BASE_H