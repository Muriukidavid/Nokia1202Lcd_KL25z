/*
 * nokia1202_lcd.h
 *
 *  Created on: Jul 20, 2014
 *      Author: karibe
 */

#ifndef nokia1202_lcd_H_
#define nokia1202_lcd_H_

#define LCD_DATA 0x100 //for 9 bit transmission
#define LCD_COMMAND 0x000

#include "font.h"
#include "uon_logo.h"


// Whether using hardware SPI or bit-banging
#define USEHW 0

// Function prototypes
void lcd_init(void);
void lcd_clr(void);
void lcd_home(void);
void lcd_start_line(unsigned char line);
void lcd_delay_ms(unsigned int ms);
void lcd_init_gpio(void);
void lcd_reset(void);
void lcd_goto_xy(unsigned char x, unsigned char y);
//void lcd_reverse(unsigned char inv);
void lcd_putchar (char *s, int line);
void lcd_send(unsigned int d);
void show_uon_logo(void);

// Function implementations
void lcd_init(void)
{
    lcd_init_gpio();
    lcd_reset(); //hard reset by holding reset pin low, then high
    lcd_delay_ms(2);
	lcd_send(LCD_COMMAND | 0xE2); // Software Reset
	lcd_delay_ms(2);
	lcd_send(LCD_COMMAND | 0xAE);

	lcd_send(LCD_COMMAND | 0x24); // V0R = 8.12 V (Default)
	lcd_send(LCD_COMMAND | 0x9F); // Electronic volume VLCD MAX
	lcd_send(LCD_COMMAND | 0xA4); // Power saver off
	lcd_send(LCD_COMMAND | 0x2F); // Power control set
	//lcd_send(LCD_COMMAND | 0xC8); // Common Dir Reverse
	//lcd_send(LCD_COMMAND | 0xA1); // Segment driver direction select reverse
	//lcd_send(LCD_COMMAND | 0xA7); // Inverse video display
	lcd_send(LCD_COMMAND | 0xAF); // LCD display on
	//testig different settings
	/*lcd_delay_ms(2000);
	lcd_send(LCD_COMMAND | 0xAE);
	lcd_send(LCD_COMMAND | 0xA5);
	lcd_delay_ms(2000);
	lcd_send(LCD_COMMAND | 0xA4); // Power saver off
	lcd_send(LCD_COMMAND | 0xAF); // LCD display on
	lcd_send(LCD_COMMAND | 0xA0); // Segment driver direction select Normal
	lcd_send(LCD_COMMAND | 0xA6); // Normal Video display
	lcd_send(LCD_COMMAND | 0xC0); // Common Dir Normal
    lcd_clr(); // clear all pixels
    lcd_home();// take cursor to row 0 col 0*/
}

// the gpio are autoinitialized in Processor Expert settings :)
void lcd_init_gpio(void){
	//disable the lcd initially
	CS_SetVal();
	MOSI_ClrVal();
	SCK_SetVal();
}

void lcd_clr(void)
{
    int i;
    lcd_home();
    for(i=0; i<16*6*9; i++)
    {
    	lcd_send(LCD_DATA | 0x00);
    }
}

void show_uon_logo(void){
	lcd_home();
	int i;
	for (i=0;i<(96*68/8);i++){
		lcd_send(LCD_DATA | uon_logo[i]);
	}
	lcd_delay_ms(2000);
}

void lcd_home(void)
{
	lcd_goto_xy(0,0);
}

void lcd_reverse(unsigned char inv)
{
    if(inv)
    	lcd_send(LCD_COMMAND | 0xA7); // reverse display
    else
    	lcd_send(LCD_COMMAND | 0xA6); // normal display
}

void lcd_start_line(unsigned char line)
{
	lcd_send(LCD_COMMAND | 0x40 | (line & 0x3F));
}

void lcd_delay_ms(unsigned int ms)
{
    WAIT1_Waitms(ms);
}

void lcd_reset(void)
{
    RESET_ClrVal();
    WAIT1_Waitms(20);
    RESET_SetVal();
    WAIT1_Waitms(20);
}

void lcd_goto_xy(unsigned char x, unsigned char y)
{
	lcd_send(LCD_COMMAND | 0xB0 | (x & 0x0F)); // Page address set
	lcd_send(LCD_COMMAND | 0x10 | (y >> 4)); // Sets the DDRAM column address - upper 3-bit
	lcd_send(LCD_COMMAND | 0x00 | (y & 0x0F)); // lower 4-bit
}

//write a single character
void lcd_putchar (char *s, int line)
{
   int i;
   int d;

   lcd_goto_xy (line, 0);

   for ( ; *s; s++) {
      d = (*s - ' ') * 5;

      for (i = 0; i < 5; i++) {
    	  lcd_send(LCD_DATA | font_lookup[d++]);
      }

      lcd_send(LCD_DATA | 0); // 1px gap after each letter
   }
}


// bit-banging SPI
void lcd_send(unsigned int d){
	CS_ClrVal();//hold CS low to enable write
	   char i;
	   for (i = 0; i < 9; i++) {//write MSB 1 and 8 data bits
	      MOSI_ClrVal(); // clear bit by default
	      if (d & 0x100){
	         MOSI_SetVal(); // if bit is set, send a high
	      }
	      SCK_ClrVal(); // clock low, bit transfered on rising edge
	      Green_SetVal(); // status LED on
	      SCK_SetVal(); //clock high to transfer bit
	      Green_ClrVal(); // status LED off

	      d <<= 1; //shift to next bit
	   }
	   CS_SetVal(); //disable write
}

#endif /* nokia1202_lcd_H_ */
