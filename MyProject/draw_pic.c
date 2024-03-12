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

#include "draw_pic.h"
#include "lcd.h"

int picture_id = -1;

/*
 *@brief 显示一张BMP图像
 *@param 图像左上方x坐标
 *@param 图像左上方y坐标
 *@param 图像路径
 *@param 打开方式(Mediate/Custom)
 *@retval 无
*/
void Draw_Picture(int posx,int posy,const char *pathname, int status)
{
    picture_id = open(pathname, O_RDONLY);
    if(picture_id == -1)
    {
        perror("picture open failed");
        return ;
    }
    // 读取BMP图片的一些属性信息
    // 读取文件魔数，用于描述该文件是什么类型的文件
    unsigned char data[4]={0};
    read(picture_id, data, 2);
    if(data[0]!='B'||data[1]!='M')
    {
        printf("this picture not bmp\n");
        close(picture_id);
        return ;
    }

    // 读取像素数组的偏移量
    lseek(picture_id,0x0a,SEEK_SET); // 偏移到像素数组偏移量数据的起始位置
    read(picture_id,data,4);
    int offset = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    // 读取宽高
    lseek(picture_id,0x12,SEEK_SET);
    read(picture_id,data,4);
    int width = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    read(picture_id,data,4);
    int height = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    // 读取色深，只有知道色深才知道每个图片的像素点所占字节数
    lseek(picture_id,0x1c,SEEK_SET);
    read(picture_id,data,2);
    int depth = data[1] << 8 | data[0];

    // 计算图片的填充字节数
    int fills = 0; // 填充字节默认为零
    if((width*(depth/8))%4)
    {
        fills = 4 - ((width*(depth/8))%4);
    }

    // 求出每一行真实字节数
    int real_bytes =  (width*(depth/8)) + fills;

    // 搞一个动态数组用来保存像素数组的数据
    unsigned char *color_array = (unsigned char *)malloc(real_bytes*abs(height)*sizeof(char));
    unsigned char *color_point = color_array;

    lseek(picture_id, offset, SEEK_SET);
    read(picture_id, color_array, real_bytes*abs(height));

    //居中显示
    if (status == Mediate){
        posx = (800 - width)/2;
        posy = (480 - abs(height))/2;
    }

    // 循环遍历图片的像素点
    for(int h = 0;h < abs(height);h++)
    {
        for(int w = 0;w < width;w++)
        {
            unsigned char a,r,g,b;
            b = *color_point++;
            g = *color_point++;
            r = *color_point++;
            a = (depth == 24)?0:*color_point++;

            int color = a << 24 | r << 16 | g << 8 | b;

            // 将颜色画到屏幕上
            lcd_DrawPixel(w+posx, ((height>0)?height-1-h:abs(height))+posy, color);
        }
        // 跳过填充字节
        color_point+=fills;
    }
    
    free(color_array);
    close(picture_id);
}
