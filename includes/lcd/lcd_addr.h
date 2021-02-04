/*
 * File        : LCD_ADDR.H
 * Author      : Joshua Fain
 * Host Target : ATMega1280
 * LCD         : Gravitech 20x4 LCD with built-in HD44780 controller
 * License     : MIT
 * Copyright (c) 2020, 2021
 *
 * Provides some macro definitions for DDRAM addresses
 */


#ifndef LCD_ADDR_H
#define LCD_ADDR_H

// Addresses for beginning and ending display line positions.

// Display Line 1
#define LINE_1_BEG     0x00
#define LINE_1_END     0x13

// Display Line 2
#define LINE_2_BEG     0x40
#define LINE_2_END     0x53

// Display Line 3
#define LINE_3_BEG     0x14
#define LINE_3_END     0x27

// Display Line 4
#define LINE_4_BEG     0x54
#define LINE_4_END     0x67

#endif // LCD_ADDR_H