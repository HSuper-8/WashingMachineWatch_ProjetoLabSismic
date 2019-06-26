#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "i2c.h"

void LCD_BL_on(void);
void LCD_BL_off(void);
void LCD_00(void);
void ClearDisplay_LCD();

void Write_letter_LCD(char data);
void Write_string_LCD(char* string, int line);
void ChangeCursor_LCD(uint8_t size, uint8_t isCursor, uint8_t direction);
void CursorDisplayShift_nibble_LCD(uint8_t nibble);
void Pulse_LCD(uint8_t byte, int RS, int RW);
void CursorPosition_LCD(uint8_t position);
void Write_floatValue_LCD(float value);
void Write_8hexValue_LCD(int value);
void Write_toHexValue_LCD(int value);
void Write_8DecValue_LCD(int value);
void Write_16DecValue_LCD(int value);