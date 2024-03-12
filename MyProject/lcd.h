#ifndef __LCD_H__
#define __LCD_H__

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define BLACK 0x000000

#define LCD_PATH "/dev/fb0"

void lcd_Init(void);
void lcd_close(void);
void lcd_clear(void);
void lcd_DrawPixel(int x,int y,int color);
void lcd_test(void);

#endif
