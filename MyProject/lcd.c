// 标准输入输出库
#include <stdio.h> 
#include <stdlib.h>
// 系统IO库
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
// 内存映射
#include <sys/mman.h>

#include "lcd.h"

int lcd_id = -1;
int *lcd_point = NULL;

/*
 *@brief 初始化LCD屏幕读写
 *@param 无
 *@retval 无
*/
void lcd_Init(void){
    lcd_id = open(LCD_PATH, O_RDWR);
    if(lcd_id == -1) {return ;}

    lcd_point = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_id,0);
    if(lcd_point == MAP_FAILED)
    {
        perror("mmap failed");
        close(lcd_id);
        return ;
    }
}

/*
 *@brief 在LCD屏幕上画一个像素点的颜色
 *@param 像素点x坐标
 *@param 像素点y坐标
 *@param 要画的颜色
 *@retval 无
*/
void lcd_DrawPixel(int x,int y,int color)
{
    if (x >= 0&& x < 800&& y >= 0&& y < 480)
    {
	    *(lcd_point+(y*800+x)) = color;
    }
}

/*
 *@brief 清空LCD屏幕
 *@param 无
 *@retval 无
*/
void lcd_clear(void){
    for(int h=0;h<480;h++){
        for(int w=0;w<800;w++){
            lcd_DrawPixel(w, h, WHITE);
        }
    }
}

/*
 *@brief 关闭LCD屏幕读写
 *@param 无
 *@retval 无
*/
void lcd_close(void){
    munmap(lcd_point, 480 * 800 * 4);
    close(lcd_id);
}

/*
 *@brief LCD屏测试程序，使全屏为红色
 *@param 无
 *@retval 无
*/
void lcd_test(void){
    for(int h=0;h<480;h++){
        for(int w=0;w<800;w++){
            lcd_DrawPixel(w, h, RED);
        }
    }
}
