// 标准输入输出库
#include <stdio.h> 

// 系统IO库
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h> // 包含了输入的数据类型/数据结构

#include "touch_getV.h"


int touch_GetV()
{
    int touch_id = open("/dev/input/event0",O_RDONLY);
    if(touch_id == -1)
    {
        perror("open touch lcd failed");
        return -1;
    }

    struct input_event ev; // 定义一个结构体的变量，用于保存事件数据

    int posx = 0,posy = 0;
    while(1)
    {
        int size = read(touch_id,&ev,sizeof(ev));
        if(size != sizeof(ev))
        {
            // 如果数据大小都不一样，那就说明数据是有问题的，需要重读
            continue;
        }

        // 可以判定事件类型进行数据获取
        if(ev.type == EV_ABS && ev.code == ABS_X)
        {
            posx = ev.value;
        }
        if(ev.type == EV_ABS && ev.code == ABS_Y)
        {
            posy = ev.value;
        }

        // 按下触摸屏打印
        if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 1)
        {
            printf("x = %d, y = %d\n", posx, posy);
        }     

        // // 松开手指打印
        // if(ev.type == EV_KEY&&ev.code == BTN_TOUCH&&ev.value == 0)
        // {
        //     printf("x = %d,y = %d\n",posx,posy);
        // }
    }

    close(touch_id);

    return 0;
}