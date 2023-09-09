/*
 * i2c-rgb_lcd.c
 *
 *  Created on: Sep 6, 2023
 *      Author: Max
 */

#include "stm32g4xx_hal.h"
#include "i2c-rgb_lcd.h"

#define REG_RED         0x04        // pwm2
#define REG_GREEN       0x03        // pwm1
#define REG_BLUE        0x02        // pwm0
#define REG_ONLY        0x02

#define WHITE           0
#define RED             1
#define GREEN           2
#define BLUE            3

#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00


void lcd_send_cmd (rgb_lcd_HandleTypeDef *lcd, char cmd)
{
	uint8_t data_t[2] = {0x80, cmd};
	HAL_I2C_Master_Transmit (lcd->hi2c_ptr, lcd->lcdAddr<<1,(uint8_t *) data_t, 2, 100);
	HAL_Delay(100);
}

void lcd_write(rgb_lcd_HandleTypeDef *lcd, uint8_t value)
{
    uint8_t data[2] = {0x40, value};
	HAL_I2C_Master_Transmit (lcd->hi2c_ptr, lcd->lcdAddr<<1,(uint8_t *) data, 2, 100);
}

void lcd_init(rgb_lcd_HandleTypeDef *lcd, I2C_HandleTypeDef* hi2c_ptr, uint8_t lcdAddr, uint8_t rgbAddr){

	lcd->lcdAddr = lcdAddr;
	lcd->rgbAddr = rgbAddr;
	lcd->hi2c_ptr = hi2c_ptr;
	lcd->_showFunction = 0;
	lcd->_showControl = 0;
	lcd->_showMode = 0;

	lcd->_showFunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	HAL_Delay(50);

	///< this is according to the hitachi HD44780 datasheet
	///< page 45 figure 23

	///< Send function set command sequence
	lcd_send_cmd(lcd, LCD_FUNCTIONSET | lcd->_showFunction);
	HAL_Delay(5);  // wait more than 4.1ms

	///< second try
	lcd_send_cmd(lcd, LCD_FUNCTIONSET | lcd->_showFunction);
	HAL_Delay(5);

	///< third go
	lcd_send_cmd(lcd, LCD_FUNCTIONSET | lcd->_showFunction);

	///< turn the display on with no cursor or blinking default
	lcd->_showControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

	lcd_display(lcd);

	///< clear it off
	lcd_clear(lcd);

	///< Initialize to default text direction (for romance languages)
	lcd->_showMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	///< set the entry mode
	lcd_send_cmd(lcd, LCD_ENTRYMODESET | lcd->_showMode);

    ///< backlight init
	lcd_setReg(lcd, REG_MODE1, 0);
    ///< set LEDs controllable by both PWM and GRPPWM registers
	lcd_setReg(lcd, REG_OUTPUT, 0xFF);
    ///< set MODE2 values
    ///< 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
	lcd_setReg(lcd, REG_MODE2, 0x20);
}

void lcd_clear(rgb_lcd_HandleTypeDef *lcd){
	lcd_send_cmd(lcd, LCD_CLEARDISPLAY);        // clear display, set cursor position to zero
    HAL_Delay(2);          // this command takes a long time!
}

void lcd_home(rgb_lcd_HandleTypeDef *lcd)
{
	lcd_send_cmd(lcd, LCD_RETURNHOME);        // set cursor position to zero
	HAL_Delay(2);         // this command takes a long time!
}

void lcd_display(rgb_lcd_HandleTypeDef *lcd) {
    lcd->_showControl |= LCD_DISPLAYON;
    lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | lcd->_showControl);
}

void lcd_noDisplay(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showControl &= ~LCD_DISPLAYON;
    lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | lcd->_showControl);
}

void lcd_blink(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showControl |= LCD_BLINKON;
    lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | lcd->_showControl);
}

void lcd_stopBlink(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showControl &= ~LCD_BLINKON;
    lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | lcd->_showControl);
}

void lcd_cursor(rgb_lcd_HandleTypeDef *lcd) {
    lcd->_showControl |= LCD_CURSORON;
    lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | lcd->_showControl);
}

void lcd_noCursor(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showControl &= ~LCD_CURSORON;
    lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | lcd->_showControl);
}

void lcd_scrollDisplayLeft(rgb_lcd_HandleTypeDef *lcd)
{
	lcd_send_cmd(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scrollDisplayRight(rgb_lcd_HandleTypeDef *lcd)
{
	lcd_send_cmd(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_leftToRight(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showMode |= LCD_ENTRYLEFT;
    lcd_send_cmd(lcd, LCD_ENTRYMODESET | lcd->_showMode);
}

void lcd_rightToLeft(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showMode &= ~LCD_ENTRYLEFT;
    lcd_send_cmd(lcd, LCD_ENTRYMODESET | lcd->_showMode);
}

void lcd_autoscroll(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showMode |= LCD_ENTRYSHIFTINCREMENT;
    lcd_send_cmd(lcd, LCD_ENTRYMODESET | lcd->_showMode);
}

void lcd_noAutoscroll(rgb_lcd_HandleTypeDef *lcd)
{
    lcd->_showMode &= ~LCD_ENTRYSHIFTINCREMENT;
    lcd_send_cmd(lcd, LCD_ENTRYMODESET | lcd->_showMode);
}

void lcd_customSymbol(rgb_lcd_HandleTypeDef *lcd, uint8_t location, uint8_t charmap[])
{

    location &= 0x7; // we only have 8 locations 0-7
    lcd_send_cmd(lcd, LCD_SETCGRAMADDR | (location << 3));

    uint8_t data[9];
    data[0] = 0x40;
    for(int i=0; i<8; i++)
    {
        data[i+1] = charmap[i];
    }

    HAL_I2C_Master_Transmit (lcd->hi2c_ptr, lcd->lcdAddr<<1,(uint8_t *) data, 9, 100);
}

void lcd_setReg(rgb_lcd_HandleTypeDef *lcd, uint8_t addr, uint8_t data)
{
	uint8_t data_t[2] = { addr,data};
	HAL_I2C_Master_Transmit (lcd->hi2c_ptr, lcd->rgbAddr<<1,(uint8_t *) data_t, 2, 100);
}

void lcd_setRGB(rgb_lcd_HandleTypeDef *lcd, uint8_t r, uint8_t g, uint8_t b){
	uint16_t temp_r,temp_g,temp_b;
	if(lcd->rgbAddr == 0x60>>1){
		temp_r = (uint16_t)r*192/255;
		temp_g = (uint16_t)g*192/255;
		temp_b = (uint16_t)b*192/255;
		lcd_setReg(lcd, REG_RED, temp_r);
		lcd_setReg(lcd, REG_GREEN, temp_g);
		lcd_setReg(lcd, REG_BLUE, temp_b);
	} else {
	  lcd_setReg(lcd, REG_RED, r);
	  lcd_setReg(lcd, REG_GREEN, g);
	  lcd_setReg(lcd, REG_BLUE, b);
	  if(lcd->rgbAddr == 0x6B){
			lcd_setReg(lcd, 0x07, 0xFF);
		}
	}
}
