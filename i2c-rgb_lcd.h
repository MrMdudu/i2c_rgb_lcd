/*
 * i2c-rgb_lcd.h
 *
 *  Created on: Sep 6, 2023
 *      Author: Max
 */

#ifndef I2C_RGB_LCD_H_
#define I2C_RGB_LCD_H_

#define LCD_ADDRESS_DEFAULT (0x7c>>1)   // change this according to ur setup
#define RGB_ADDRESS_DEFAULT (0xc0>>1)   // change this according to ur setup

typedef struct{
	uint8_t lcdAddr;
	uint8_t rgbAddr;
	I2C_HandleTypeDef* hi2c_ptr;
	uint8_t _showFunction;
	uint8_t _showControl;
	uint8_t _showMode;
}rgb_lcd_HandleTypeDef;


void lcd_init(rgb_lcd_HandleTypeDef *lcd, I2C_HandleTypeDef* hi2c_ptr, uint8_t lcdAddr, uint8_t rgbAddr);
void lcd_clear(rgb_lcd_HandleTypeDef *lcd);
void lcd_home(rgb_lcd_HandleTypeDef *lcd);
void lcd_display(rgb_lcd_HandleTypeDef *lcd);
void lcd_noDisplay(rgb_lcd_HandleTypeDef *lcd);
void lcd_blink(rgb_lcd_HandleTypeDef *lcd);
void lcd_stopBlink(rgb_lcd_HandleTypeDef *lcd);
void lcd_cursor(rgb_lcd_HandleTypeDef *lcd);
void lcd_noCursor(rgb_lcd_HandleTypeDef *lcd);
void lcd_scrollDisplayLeft(rgb_lcd_HandleTypeDef *lcd);
void lcd_scrollDisplayRight(rgb_lcd_HandleTypeDef *lcd);
void lcd_leftToRight(rgb_lcd_HandleTypeDef *lcd);
void lcd_rightToLeft(rgb_lcd_HandleTypeDef *lcd);
void lcd_autoscroll(rgb_lcd_HandleTypeDef *lcd);
void lcd_noAutoscroll(rgb_lcd_HandleTypeDef *lcd);
void lcd_customSymbol(rgb_lcd_HandleTypeDef *lcd, uint8_t location, uint8_t charmap[]);
void lcd_write(rgb_lcd_HandleTypeDef *lcd, uint8_t value);
void lcd_setReg(rgb_lcd_HandleTypeDef *lcd, uint8_t addr, uint8_t data);
void lcd_setRGB(rgb_lcd_HandleTypeDef *lcd, uint8_t r, uint8_t g, uint8_t b);

#endif /* I2C_RGB_LCD_H_ */
